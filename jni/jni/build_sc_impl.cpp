/**
 * build_sc_impl.cpp
 * Build SC from PNGs — port of sc_build.py
 * Creates logic.sc + logic_tex.sc from list of PNG files
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
    d.push_back(v&0xFF); d.push_back(v>>8);
}
static void w32(std::vector<uint8_t>& d, uint32_t v) {
    d.push_back(v&0xFF); d.push_back((v>>8)&0xFF);
    d.push_back((v>>16)&0xFF); d.push_back(v>>24);
}
static void w32b(std::vector<uint8_t>& d, int32_t v) { w32(d, (uint32_t)v); }
static void ws16(std::vector<uint8_t>& d, int16_t v) {
    d.push_back((uint8_t)(v&0xFF)); d.push_back((uint8_t)(v>>8));
}
static void write_str(std::vector<uint8_t>& d, const std::string& s) {
    if (s.size() < 0xFF) d.push_back((uint8_t)s.size());
    else { d.push_back(0xFF); w16(d,(uint16_t)s.size()); }
    d.insert(d.end(), s.begin(), s.end());
}
static void write_tag(std::vector<uint8_t>& out, uint8_t tag,
                      const std::vector<uint8_t>& payload) {
    out.push_back(tag); w32(out,(uint32_t)payload.size());
    out.insert(out.end(), payload.begin(), payload.end());
}

static int next_pow2(int n) {
    int r = 1; while (r < n) r <<= 1; return r;
}
static int16_t clamp16(int v) {
    return (int16_t)std::max(-32768, std::min(32767, v));
}

// ── Identity matrix (Tag 8) ───────────────────────────────────────────────────
static std::vector<uint8_t> identity_matrix() {
    std::vector<uint8_t> d;
    // 6× i32 fixed-point ×1024: [1024,0,0,1024,0,0]
    w32b(d,1024); w32b(d,0); w32b(d,0);
    w32b(d,1024); w32b(d,0); w32b(d,0);
    return d;
}

// ── SC Header builder ─────────────────────────────────────────────────────────
static std::vector<uint8_t> build_header(
    uint16_t shape_c, uint16_t mc_c, uint16_t tex_c,
    const std::vector<std::pair<uint16_t,std::string>>& exports)
{
    std::vector<uint8_t> d;
    w16(d,shape_c); w16(d,mc_c); w16(d,tex_c);
    w16(d,0); w16(d,(uint16_t)exports.size()); w16(d,0); // tf,mat,ct
    w32(d,0); d.push_back(0); // unk_u32, unk_u8
    w16(d,(uint16_t)exports.size());
    for (auto& e : exports) w16(d,e.first);
    for (auto& e : exports) write_str(d,e.second);
    return d;
}

// ── Shape tag (Tag 18) ────────────────────────────────────────────────────────
static std::vector<uint8_t> build_shape(
    uint16_t sh_id, int x, int y, int w, int h, int aw, int ah)
{
    std::vector<uint8_t> d;
    w16(d,sh_id); w16(d,1); // id, draw_count
    w16(d,0);                // tex_idx

    // UV fixed-point ×32767
    int u0 = (int)round((double)x      /aw*32767);
    int u1 = (int)round((double)(x+w)  /aw*32767);
    int v0 = (int)round((double)y      /ah*32767);
    int v1 = (int)round((double)(y+h)  /ah*32767);
    int uvs[8] = {u0,v0, u1,v0, u1,v1, u0,v1};

    // XY pixels ×20, centred
    int hw=w/2, hh=h/2;
    int xys[8] = {-hw*20,hh*20, hw*20,hh*20, hw*20,-hh*20, -hw*20,-hh*20};

    for (int v : uvs) ws16(d,clamp16(v));
    for (int v : xys) ws16(d,clamp16(v));
    for (int i=0;i<10;i++) d.push_back(0); // tail
    return d;
}

// ── MC tag (Tag 49) ───────────────────────────────────────────────────────────
static std::vector<uint8_t> build_simple_mc(
    uint16_t mc_id, uint16_t sh_id, uint16_t mat_idx)
{
    std::vector<uint8_t> d;
    w16(d,mc_id); w16(d,1); // mc_id, frame_count=1
    w16(d,0);                // unk1
    w16(d,1);                // bind_count=1
    w32(d,0);                // unk2
    // bind: child_id, ct=0xFFFF, layer=1
    w16(d,sh_id); w16(d,0xFFFF); w16(d,1);
    // mat index
    w16(d,mat_idx);
    // unk_bytes[1]
    d.push_back(0);
    // name_flags[1]
    d.push_back(0xFF);
    // tail 13 bytes
    for (int i=0;i<13;i++) d.push_back(0);
    return d;
}

// ── Simple atlas packer ───────────────────────────────────────────────────────
struct Sprite { std::string name; int w, h; int x, y; std::vector<uint8_t> rgba; };

static bool pack_atlas(std::vector<Sprite>& sprites, int& aw, int& ah) {
    int total = 0;
    for (auto& s : sprites) total += s.w * s.h;
    int side = next_pow2((int)(sqrt((double)total)*1.3));
    aw = ah = side;

    for (int attempt=0; attempt<12; attempt++) {
        int x=0, y=0, row_h=0; bool ok=true;
        // Sort by height desc
        std::vector<int> order(sprites.size());
        std::iota(order.begin(),order.end(),0);
        std::stable_sort(order.begin(),order.end(),[&](int a,int b){
            return sprites[a].h > sprites[b].h;
        });
        std::vector<std::pair<int,int>> slots(sprites.size());
        for (int idx : order) {
            auto& s = sprites[idx];
            if (x+s.w > aw) { x=0; y+=row_h; row_h=0; }
            if (y+s.h > ah) { ok=false; break; }
            slots[idx] = {x,y};
            x += s.w; row_h = std::max(row_h, s.h);
        }
        if (ok) {
            for (int i=0;i<(int)sprites.size();i++) {
                sprites[i].x = slots[i].first;
                sprites[i].y = slots[i].second;
            }
            return true;
        }
        if (aw <= ah) aw = next_pow2(aw+1);
        else          ah = next_pow2(ah+1);
    }
    return false;
}

// ── Build KTX from RGBA ───────────────────────────────────────────────────────
static std::vector<uint8_t> build_ktx(
    const std::vector<uint8_t>& rgba, int w, int h, int bw, int bh)
{
    std::vector<uint8_t> astc;
    if (!astc_encode(rgba.data(), w, h, bw, bh, astc)) return {};

    // GL sRGB format
    struct GLFmt { uint32_t gl; int bw,bh; };
    static const GLFmt fmts[] = {
        {0x93D0,4,4},{0x93D4,6,6},{0x93D7,8,8},{0x93DB,10,10},{0x93DD,12,12},{0,0,0}
    };
    uint32_t gl = 0x93D7; // default 8x8 sRGB
    for (auto* f=fmts;f->gl;f++) if(f->bw==bw&&f->bh==bh){gl=f->gl;break;}

    std::vector<uint8_t> ktx;
    ktx.insert(ktx.end(), KTX_MAGIC, KTX_MAGIC+12);
    auto w32k = [&](uint32_t v){ uint8_t b[4]; memcpy(b,&v,4); ktx.insert(ktx.end(),b,b+4); };
    w32k(0x04030201); // endianness
    w32k(0);          // glType (compressed)
    w32k(1);          // glTypeSize
    w32k(0);          // glFormat
    w32k(gl);         // glInternalFormat
    w32k(0x1908);     // GL_RGBA
    w32k((uint32_t)w);
    w32k((uint32_t)h);
    w32k(0); w32k(0); w32k(1); w32k(1); w32k(0); // depth,array,faces,mips,kvd
    w32k((uint32_t)astc.size());
    ktx.insert(ktx.end(), astc.begin(), astc.end());
    return ktx;
}

// ── Parse block string ────────────────────────────────────────────────────────
static void parse_block(const std::string& s, int& bw, int& bh) {
    bw=8; bh=8;
    size_t pos = s.find_first_of("xXхХ");
    if (pos==std::string::npos) return;
    try { bw=std::stoi(s.substr(0,pos)); bh=std::stoi(s.substr(pos+1)); }
    catch(...) { bw=8; bh=8; }
}

// ── Parse names string ────────────────────────────────────────────────────────
static std::vector<std::string> parse_names(const std::string& s) {
    std::vector<std::string> names;
    std::string cur;
    for (char c : s) {
        if (c==','||c=='|'||c==';') {
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
    // Парсим входные данные
    auto png_paths = parse_names(png_paths_str);
    auto names     = parse_names(names_str);

    if (png_paths.empty()) return "Error: no PNG files provided";

    // Если имена не заданы — используем имена файлов
    if (names.empty()) {
        for (auto& p : png_paths) {
            std::string fname = p.substr(p.rfind('/')+1);
            size_t dot = fname.rfind('.');
            names.push_back(dot==std::string::npos ? fname : fname.substr(0,dot));
        }
    }
    if (names.size() < png_paths.size()) {
        while (names.size() < png_paths.size())
            names.push_back("export_" + std::to_string(names.size()));
    }

    int bw=8, bh=8;
    parse_block(block_str, bw, bh);
    LOGI("build_sc: %zu PNGs, block=%dx%d", png_paths.size(), bw, bh);

    // Загружаем PNG
    std::vector<Sprite> sprites;
    for (int i=0;i<(int)png_paths.size();i++) {
        int w,h,ch;
        uint8_t* img = stbi_load(png_paths[i].c_str(), &w, &h, &ch, 4);
        if (!img) return "Error: cannot load PNG: " + png_paths[i];
        Sprite s;
        s.name = names[i];
        s.w=w; s.h=h;
        s.rgba.assign(img, img+(size_t)w*h*4);
        stbi_image_free(img);
        sprites.push_back(std::move(s));
        LOGI("  Loaded: %s %dx%d", names[i].c_str(), w, h);
    }

    // Пакуем атлас
    int aw=0, ah=0;
    if (!pack_atlas(sprites, aw, ah))
        return "Error: atlas too large, reduce image count or sizes";
    LOGI("Atlas: %dx%d", aw, ah);

    // Строим RGBA атлас
    std::vector<uint8_t> atlas(aw*ah*4, 0);
    for (auto& s : sprites) {
        for (int y=0;y<s.h;y++) {
            memcpy(atlas.data()+((s.y+y)*aw+s.x)*4,
                   s.rgba.data()+y*s.w*4, s.w*4);
        }
    }

    // Кодируем KTX
    LOGI("Encoding ASTC %dx%d...", bw, bh);
    auto ktx = build_ktx(atlas, aw, ah, bw, bh);
    if (ktx.empty()) return "Error: ASTC encode failed";

    int n = (int)sprites.size();

    // Строим теги
    std::vector<uint8_t> logic_tags, mat_tags, shape_tags, mc_tags;
    std::vector<std::pair<uint16_t,std::string>> exports;

    for (int i=0;i<n;i++) {
        uint16_t sh_id = (uint16_t)i;
        uint16_t mc_id = (uint16_t)(n+i);

        auto sh = build_shape(sh_id, sprites[i].x, sprites[i].y,
                              sprites[i].w, sprites[i].h, aw, ah);
        auto mc = build_simple_mc(mc_id, sh_id, (uint16_t)i);
        auto mt = identity_matrix();

        write_tag(mat_tags,   8,  mt);
        write_tag(shape_tags, 18, sh);
        write_tag(mc_tags,    49, mc);
        exports.push_back({mc_id, sprites[i].name});
    }

    // Texture tag (Tag 45)
    std::vector<uint8_t> tex_payload;
    w32(tex_payload,(uint32_t)ktx.size()); w32(tex_payload,0);
    tex_payload.insert(tex_payload.end(), ktx.begin(), ktx.end());

    // Logic SC
    auto logic_hdr = build_header((uint16_t)n,(uint16_t)n,1,exports);
    std::vector<uint8_t> logic_raw = logic_hdr;
    logic_raw.insert(logic_raw.end(), mat_tags.begin(),   mat_tags.end());
    logic_raw.insert(logic_raw.end(), shape_tags.begin(), shape_tags.end());
    logic_raw.insert(logic_raw.end(), mc_tags.begin(),    mc_tags.end());
    // End tag
    logic_raw.push_back(0); w32(logic_raw,0);

    // Compress with zlib (SC v0 header: SC\x00\x00)
    std::vector<uint8_t> logic_data = {'S','C',0x00,0x00};
    try {
        auto comp = sc_compress(logic_raw, CompKind::ZLIB);
        logic_data.insert(logic_data.end(), comp.begin(), comp.end());
    } catch (std::exception& e) { return std::string("Error: compress: ")+e.what(); }

    // Texture SC
    std::vector<std::pair<uint16_t,std::string>> no_exports;
    auto tex_hdr = build_header(0,0,1,no_exports);
    std::vector<uint8_t> tex_raw = tex_hdr;
    write_tag(tex_raw, 45, tex_payload);
    tex_raw.push_back(0); w32(tex_raw,0);

    std::vector<uint8_t> tex_data = {'S','C',0x00,0x00};
    try {
        auto comp = sc_compress(tex_raw, CompKind::ZLIB);
        tex_data.insert(tex_data.end(), comp.begin(), comp.end());
    } catch (std::exception& e) { return std::string("Error: compress tex: ")+e.what(); }

    // Сохраняем
    std::string out_name = exports.empty() ? "output" : exports[0].second;
    std::string logic_path = out_dir + "/" + out_name + ".sc";
    std::string tex_path   = out_dir + "/" + out_name + "_tex.sc";

    if (!sc_write_file(logic_path, logic_data)) return "Error: write failed: " + logic_path;
    if (!sc_write_file(tex_path,   tex_data))   return "Error: write failed: " + tex_path;

    std::ostringstream r;
    r << "OK: Build SC complete\n";
    r << "  Sprites: " << n << "\n";
    r << "  Atlas:   " << aw << "x" << ah << "  ASTC " << bw << "x" << bh << "\n";
    r << "  Logic:   " << out_name << ".sc\n";
    r << "  Texture: " << out_name << "_tex.sc\n";
    r << "  Dir: " << out_dir;
    return r.str();
}
