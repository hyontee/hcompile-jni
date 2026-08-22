/**
 * build_sc_impl.cpp
 * Build SC from PNGs — port of sc_build.py
 * Produces: name.sc (logic-only v5) + name_tex.sc (tex-only v5) + name_combined.sc (v5 joined)
 *
 * Формат v5:  SC\x05\x00\x00\x00 + u32(id_size=0) + zstd_payload
 * combined:   один zstd поток = raw_logic + raw_tex (как делает sc_join)
 */
#include "sc_core.h"
#include "stb/stb_image.h"
#include "astc_decoder.h"
#include <sstream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <android/log.h>

#define TAG "BuildSC"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

static const uint8_t KTX_MAGIC[12] = {
    0xab,'K','T','X',' ','1','1',0xbb,0x0d,0x0a,0x1a,0x0a
};

// ── Helpers ───────────────────────────────────────────────────────────────────
static void w16(std::vector<uint8_t>& d, uint16_t v) {
    d.push_back(v & 0xFF); d.push_back(v >> 8);
}
static void w32(std::vector<uint8_t>& d, uint32_t v) {
    d.push_back(v & 0xFF); d.push_back((v >> 8) & 0xFF);
    d.push_back((v >> 16) & 0xFF); d.push_back(v >> 24);
}
static void w32b(std::vector<uint8_t>& d, int32_t v) { w32(d, (uint32_t)v); }
static void ws16(std::vector<uint8_t>& d, int16_t v) {
    d.push_back((uint8_t)(v & 0xFF)); d.push_back((uint8_t)(v >> 8));
}
static void write_str(std::vector<uint8_t>& d, const std::string& s) {
    if (s.size() < 0xFF) d.push_back((uint8_t)s.size());
    else { d.push_back(0xFF); w16(d, (uint16_t)s.size()); }
    d.insert(d.end(), s.begin(), s.end());
}
static void write_tag(std::vector<uint8_t>& out, uint8_t tag,
                      const std::vector<uint8_t>& payload) {
    out.push_back(tag);
    w32(out, (uint32_t)payload.size());
    out.insert(out.end(), payload.begin(), payload.end());
}
static int next_pow2(int n) {
    int r = 1; while (r < n) r <<= 1; return r;
}
static int16_t clamp16(int v) {
    return (int16_t)std::max(-32768, std::min(32767, v));
}

// ── SC v1 file wrapper ────────────────────────────────────────────────────────
// Real SC v1 format (from hero_portraits.sc analysis):
// SC(2) + ver u16=0 + flags u16=0x0300 + md5 zeros(16) + u32=0 + zstd(raw)
// Total header = 26 bytes before zstd magic
static std::vector<uint8_t> make_sc_v1(const std::vector<uint8_t>& raw) {
    auto comp = sc_compress(raw, CompKind::ZSTD);
    std::vector<uint8_t> out;
    out.push_back('S'); out.push_back('C');
    // version u16 = 0
    out.push_back(0x00); out.push_back(0x00);
    // flags u16 = 0x0300
    out.push_back(0x00); out.push_back(0x03);
    // md5 placeholder (16 bytes zeros)
    for (int i = 0; i < 16; i++) out.push_back(0x00);
    // u32 = 0
    out.push_back(0x00); out.push_back(0x00);
    out.push_back(0x00); out.push_back(0x00);
    out.insert(out.end(), comp.begin(), comp.end());
    return out;
}

// ── Identity matrix (Tag 8) ───────────────────────────────────────────────────
static std::vector<uint8_t> identity_matrix() {
    std::vector<uint8_t> d;
    w32b(d, 1024); w32b(d, 0); w32b(d, 0);
    w32b(d, 1024); w32b(d, 0); w32b(d, 0);
    return d;
}

// ── SC raw header ─────────────────────────────────────────────────────────────
// Из hero_orig: shape_c, mc_c, tex_c, tf_c, mat_c, ct_c (все u16)
//               unk_u32, unk_u8
//               exp_c u16
//               [exp_ids u16 * exp_c]
//               [exp_names str * exp_c]
static std::vector<uint8_t> build_header(
    uint16_t shape_c, uint16_t mc_c, uint16_t tex_c,
    const std::vector<std::pair<uint16_t, std::string>>& exports)
{
    std::vector<uint8_t> d;
    w16(d, shape_c); w16(d, mc_c); w16(d, tex_c);
    w16(d, 0);  // tf_count
    w16(d, 0);  // mat_count (матрицы идут как теги Tag 8, не в счётчик)
    w16(d, 0);  // ct_count
    w32(d, 0);  // unk_u32
    d.push_back(0); // unk_u8
    w16(d, (uint16_t)exports.size()); // exp_c — один раз!
    for (auto& e : exports) w16(d, e.first);
    for (auto& e : exports) write_str(d, e.second);
    return d;
}

// ── Shape tag (Tag 18) ────────────────────────────────────────────────────────
static std::vector<uint8_t> build_shape(
    uint16_t sh_id, int x, int y, int w, int h, int aw, int ah)
{
    std::vector<uint8_t> d;
    w16(d, sh_id); w16(d, 1); // id, draw_count=1
    w16(d, 0);                 // tex_idx=0

    int u0 = (int)round((double)x       / aw * 32767);
    int u1 = (int)round((double)(x + w) / aw * 32767);
    int v0 = (int)round((double)y       / ah * 32767);
    int v1 = (int)round((double)(y + h) / ah * 32767);
    int uvs[8] = {u0, v0,  u1, v0,  u1, v1,  u0, v1};

    int hw = w / 2, hh = h / 2;
    int xys[8] = {-hw*20, -hh*20,  hw*20, -hh*20,  hw*20, hh*20,  -hw*20, hh*20};

    for (int v : uvs) ws16(d, clamp16(v));
    for (int v : xys) ws16(d, clamp16(v));
    for (int i = 0; i < 10; i++) d.push_back(0); // tail
    return d;
}

// ── MC tag (Tag 49) ───────────────────────────────────────────────────────────
// Структура по hero_orig (разобрана из 279 байт raw qw.sc):
// mc_id u16, frame_count u16=1, unk u16=0, bind_count u16=1, unk u32=0
// bind: sh_id u16, mat_idx u16, ct u16=0xFFFF, layer u16=1
// unk_u8=0, name_flags u8=0xFF, tail[13]
static std::vector<uint8_t> build_simple_mc(
    uint16_t mc_id, uint16_t sh_id, uint16_t mat_idx)
{
    std::vector<uint8_t> d;
    w16(d, mc_id); w16(d, 1);  // mc_id, frame_count=1
    w16(d, 0);                  // unk1
    w16(d, 1);                  // bind_count=1
    w32(d, 0);                  // unk2
    // bind entry: child_id, ct_idx, layer
    w16(d, sh_id); w16(d, 0xFFFF); w16(d, 1);
    // mat indices (separate block, bind_count × u16)
    w16(d, mat_idx);
    d.push_back(0);    // unk_bytes[0]
    d.push_back(0xFF); // name_flags[0]
    for (int i = 0; i < 13; i++) d.push_back(0); // tail
    return d;
}

// ── Simple atlas packer ───────────────────────────────────────────────────────
struct Sprite { std::string name; int w, h; int x, y; std::vector<uint8_t> rgba; };

static bool pack_atlas(std::vector<Sprite>& sprites, int& aw, int& ah) {
    int total = 0;
    for (auto& s : sprites) total += s.w * s.h;
    int side = next_pow2((int)(sqrt((double)total) * 1.3));
    aw = ah = side;

    for (int attempt = 0; attempt < 12; attempt++) {
        int x = 0, y = 0, row_h = 0; bool ok = true;
        std::vector<int> order(sprites.size());
        std::iota(order.begin(), order.end(), 0);
        std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
            return sprites[a].h > sprites[b].h;
        });
        std::vector<std::pair<int,int>> slots(sprites.size());
        for (int idx : order) {
            auto& s = sprites[idx];
            if (x + s.w > aw) { x = 0; y += row_h; row_h = 0; }
            if (y + s.h > ah) { ok = false; break; }
            slots[idx] = {x, y};
            x += s.w; row_h = std::max(row_h, s.h);
        }
        if (ok) {
            for (int i = 0; i < (int)sprites.size(); i++) {
                sprites[i].x = slots[i].first;
                sprites[i].y = slots[i].second;
            }
            return true;
        }
        if (aw <= ah) aw = next_pow2(aw + 1);
        else          ah = next_pow2(ah + 1);
    }
    return false;
}

// ── Build KTX from RGBA ───────────────────────────────────────────────────────
static std::vector<uint8_t> build_ktx(
    const std::vector<uint8_t>& rgba, int w, int h, int bw, int bh)
{
    std::vector<uint8_t> astc;
    if (!astc_encode(rgba.data(), w, h, bw, bh, astc)) return {};

    struct GLFmt { uint32_t gl; int bw, bh; };
    static const GLFmt fmts[] = {
        {0x93D0,4,4},{0x93D4,6,6},{0x93D7,8,8},{0x93DB,10,10},{0x93DD,12,12},{0,0,0}
    };
    uint32_t gl = 0x93D7;
    for (auto* f = fmts; f->gl; f++) if (f->bw == bw && f->bh == bh) { gl = f->gl; break; }

    std::vector<uint8_t> ktx;
    ktx.insert(ktx.end(), KTX_MAGIC, KTX_MAGIC + 12);
    auto w32k = [&](uint32_t v) { uint8_t b[4]; memcpy(b, &v, 4); ktx.insert(ktx.end(), b, b+4); };
    w32k(0x04030201); w32k(0); w32k(1); w32k(0); w32k(gl); w32k(0x1908);
    w32k((uint32_t)w); w32k((uint32_t)h);
    w32k(0); w32k(0); w32k(1); w32k(1); w32k(0); // depth,array,faces,mips,kvd
    w32k((uint32_t)astc.size());
    ktx.insert(ktx.end(), astc.begin(), astc.end());
    return ktx;
}

// ── Helpers ───────────────────────────────────────────────────────────────────
static void parse_block(const std::string& s, int& bw, int& bh) {
    bw = 8; bh = 8;
    size_t pos = s.find_first_of("xXхХ");
    if (pos == std::string::npos) return;
    try { bw = std::stoi(s.substr(0, pos)); bh = std::stoi(s.substr(pos + 1)); }
    catch(...) { bw = 8; bh = 8; }
}
static std::vector<std::string> parse_names(const std::string& s) {
    std::vector<std::string> names;
    std::string cur;
    for (char c : s) {
        if (c == ',' || c == '|' || c == ';') {
            if (!cur.empty()) { names.push_back(cur); cur.clear(); }
        } else cur += c;
    }
    if (!cur.empty()) names.push_back(cur);
    return names;
}

// ── Main build_sc ─────────────────────────────────────────────────────────────
std::string build_sc(const std::string& png_paths_str,
                     const std::string& names_str,
                     const std::string& block_str,
                     const std::string& out_dir) {
    auto png_paths = parse_names(png_paths_str);
    auto names     = parse_names(names_str);

    if (png_paths.empty()) return "Error: no PNG files provided";

    if (names.empty()) {
        for (auto& p : png_paths) {
            std::string fname = p.substr(p.rfind('/') + 1);
            size_t dot = fname.rfind('.');
            names.push_back(dot == std::string::npos ? fname : fname.substr(0, dot));
        }
    }
    while (names.size() < png_paths.size())
        names.push_back("export_" + std::to_string(names.size()));

    int bw = 8, bh = 8;
    parse_block(block_str, bw, bh);
    LOGI("build_sc: %zu PNGs, block=%dx%d", png_paths.size(), bw, bh);

    // Загружаем PNG
    std::vector<Sprite> sprites;
    for (int i = 0; i < (int)png_paths.size(); i++) {
        int w, h, ch;
        uint8_t* img = stbi_load(png_paths[i].c_str(), &w, &h, &ch, 4);
        if (!img) return "Error: cannot load PNG: " + png_paths[i];
        Sprite s;
        s.name = names[i]; s.w = w; s.h = h;
        s.rgba.assign(img, img + (size_t)w * h * 4);
        stbi_image_free(img);
        sprites.push_back(std::move(s));
        LOGI("  Loaded: %s %dx%d", names[i].c_str(), w, h);
    }

    // Пакуем атлас
    int aw = 0, ah = 0;
    if (!pack_atlas(sprites, aw, ah))
        return "Error: atlas too large";
    LOGI("Atlas: %dx%d", aw, ah);

    // RGBA атлас
    std::vector<uint8_t> atlas(aw * ah * 4, 0);
    for (auto& s : sprites)
        for (int y = 0; y < s.h; y++)
            memcpy(atlas.data() + ((s.y + y) * aw + s.x) * 4,
                   s.rgba.data() + y * s.w * 4, s.w * 4);

    // KTX
    LOGI("Encoding ASTC %dx%d...", bw, bh);
    auto ktx = build_ktx(atlas, aw, ah, bw, bh);
    if (ktx.empty()) return "Error: ASTC encode failed";

    int n = (int)sprites.size();

    // Строим теги
    std::vector<uint8_t> mat_tags, shape_tags, mc_tags;
    std::vector<std::pair<uint16_t, std::string>> exports;

    for (int i = 0; i < n; i++) {
        uint16_t sh_id = (uint16_t)i;
        uint16_t mc_id = (uint16_t)(n + i);
        write_tag(mat_tags,   8,  identity_matrix());
        write_tag(shape_tags, 18, build_shape(sh_id,
            sprites[i].x, sprites[i].y, sprites[i].w, sprites[i].h, aw, ah));
        write_tag(mc_tags,    49, build_simple_mc(mc_id, sh_id, (uint16_t)i));
        exports.push_back({mc_id, sprites[i].name});
    }

    // Texture tag (Tag 45): u32 ktx_size + u32 0 + KTX data
    std::vector<uint8_t> tex_tag_payload;
    w32(tex_tag_payload, (uint32_t)ktx.size());
    w32(tex_tag_payload, 0);
    tex_tag_payload.insert(tex_tag_payload.end(), ktx.begin(), ktx.end());

    // ── raw_logic ────────────────────────────────────────────────────────────
    auto logic_hdr = build_header((uint16_t)n, (uint16_t)n, 1, exports);
    std::vector<uint8_t> raw_logic = logic_hdr;
    raw_logic.insert(raw_logic.end(), mat_tags.begin(),   mat_tags.end());
    raw_logic.insert(raw_logic.end(), shape_tags.begin(), shape_tags.end());
    raw_logic.insert(raw_logic.end(), mc_tags.begin(),    mc_tags.end());
    raw_logic.push_back(0); w32(raw_logic, 0); // End tag

    // ── raw_tex ──────────────────────────────────────────────────────────────
    std::vector<std::pair<uint16_t, std::string>> no_exp;
    auto tex_hdr = build_header(0, 0, 1, no_exp);
    std::vector<uint8_t> raw_tex = tex_hdr;
    write_tag(raw_tex, 45, tex_tag_payload);
    raw_tex.push_back(0); w32(raw_tex, 0); // End tag

    // ── Собираем файлы ───────────────────────────────────────────────────────
    // combined = один zstd поток из raw_logic + raw_tex (как sc_join)
    std::vector<uint8_t> raw_combined = raw_logic;
    raw_combined.insert(raw_combined.end(), raw_tex.begin(), raw_tex.end());

    std::vector<uint8_t> logic_file, tex_file, combined_file;
    try {
        logic_file    = make_sc_v1(raw_logic);
        tex_file      = make_sc_v1(raw_tex);
        combined_file = make_sc_v1(raw_combined);
    } catch (std::exception& e) {
        return std::string("Error: compress: ") + e.what();
    }

    std::string out_name = exports.empty() ? "output" : exports[0].second;
    std::string logic_path    = out_dir + "/" + out_name + ".sc";
    std::string tex_path      = out_dir + "/" + out_name + "_tex.sc";
    std::string combined_path = out_dir + "/" + out_name + "_combined.sc";

    if (!sc_write_file(logic_path,    logic_file))    return "Error: write: " + logic_path;
    if (!sc_write_file(tex_path,      tex_file))      return "Error: write: " + tex_path;
    if (!sc_write_file(combined_path, combined_file)) return "Error: write: " + combined_path;

    std::ostringstream r;
    r << "OK: Build SC complete\n";
    r << "  Sprites: " << n << "\n";
    r << "  Atlas:   " << aw << "x" << ah << "  ASTC " << bw << "x" << bh << "\n";
    r << "  Logic:   " << out_name << ".sc  (" << logic_file.size() << " bytes)\n";
    r << "  Texture: " << out_name << "_tex.sc  (" << tex_file.size() << " bytes)\n";
    r << "  Combined:" << out_name << "_combined.sc  (" << combined_file.size() << " bytes)\n";
    r << "  Dir: " << out_dir;
    return r.str();
}
