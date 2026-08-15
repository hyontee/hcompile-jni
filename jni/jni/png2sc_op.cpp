/**
 * png2sc_op.cpp — PNG to SC
 * Заменяем только ПИКСЕЛИ внутри KTX (как Termux sc_tool),
 * обновляем MD5 в SC заголовке (bytes header[-16:] = MD5(raw)).
 */
#include "sc_core.h"
#include "stb/stb_image.h"
#include "astc_decoder.h"
#include <sstream>
#include <cstring>
#include <cmath>
#include <android/log.h>

#define TAG "Png2SC"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

static const uint8_t KTX_MAGIC[12] = {
    0xab,'K','T','X',' ','1','1',0xbb,0x0d,0x0a,0x1a,0x0a
};

// GL format → block size
struct GLBlock { uint32_t gl; int bw, bh; };
static const GLBlock GL_BLOCKS[] = {
    {0x93B0,4,4},{0x93B1,5,4},{0x93B2,5,5},{0x93B3,6,5},
    {0x93B4,6,6},{0x93B5,8,5},{0x93B6,8,6},{0x93B7,8,8},
    {0x93B8,10,5},{0x93B9,10,6},{0x93BA,10,8},{0x93BB,10,10},
    {0x93BC,12,10},{0x93BD,12,12},{0,0,0}
};
static size_t mip0_size(int w, int h, int bw, int bh) {
    return (size_t)((w+bw-1)/bw) * (size_t)((h+bh-1)/bh) * 16;
}
static bool get_block(uint32_t gl, int& bw, int& bh) {
    for (const auto* b = GL_BLOCKS; b->gl; b++)
        if (b->gl == gl) { bw=b->bw; bh=b->bh; return true; }
    return false;
}

// ── MD5 (RFC 1321, public domain) ────────────────────────────────────────────
static uint32_t md5_T[64];
static bool md5_init_done = false;
static void md5_init_T() {
    if (md5_init_done) return;
    for (int i=0;i<64;i++)
        md5_T[i] = (uint32_t)(4294967296.0 * fabs(sin(i+1.0)));
    md5_init_done = true;
}
#define ROL(x,n) (((x)<<(n))|((x)>>(32-(n))))
#define F(x,y,z) (((x)&(y))|(~(x)&(z)))
#define G(x,y,z) (((x)&(z))|((y)&~(z)))
#define H(x,y,z) ((x)^(y)^(z))
#define I(x,y,z) ((y)^((x)|~(z)))
#define STEP(f,a,b,c,d,k,s,i) a=b+ROL(a+f(b,c,d)+X[k]+md5_T[i-1],s)

static void md5_block(uint32_t s[4], const uint8_t* blk) {
    uint32_t X[16], a=s[0],b=s[1],c=s[2],d=s[3];
    for(int i=0;i<16;i++) memcpy(&X[i],blk+i*4,4);
    STEP(F,a,b,c,d, 0, 7, 1); STEP(F,d,a,b,c, 1,12, 2);
    STEP(F,c,d,a,b, 2,17, 3); STEP(F,b,c,d,a, 3,22, 4);
    STEP(F,a,b,c,d, 4, 7, 5); STEP(F,d,a,b,c, 5,12, 6);
    STEP(F,c,d,a,b, 6,17, 7); STEP(F,b,c,d,a, 7,22, 8);
    STEP(F,a,b,c,d, 8, 7, 9); STEP(F,d,a,b,c, 9,12,10);
    STEP(F,c,d,a,b,10,17,11); STEP(F,b,c,d,a,11,22,12);
    STEP(F,a,b,c,d,12, 7,13); STEP(F,d,a,b,c,13,12,14);
    STEP(F,c,d,a,b,14,17,15); STEP(F,b,c,d,a,15,22,16);
    STEP(G,a,b,c,d, 1, 5,17); STEP(G,d,a,b,c, 6, 9,18);
    STEP(G,c,d,a,b,11,14,19); STEP(G,b,c,d,a, 0,20,20);
    STEP(G,a,b,c,d, 5, 5,21); STEP(G,d,a,b,c,10, 9,22);
    STEP(G,c,d,a,b,15,14,23); STEP(G,b,c,d,a, 4,20,24);
    STEP(G,a,b,c,d, 9, 5,25); STEP(G,d,a,b,c,14, 9,26);
    STEP(G,c,d,a,b, 3,14,27); STEP(G,b,c,d,a, 8,20,28);
    STEP(G,a,b,c,d,13, 5,29); STEP(G,d,a,b,c, 2, 9,30);
    STEP(G,c,d,a,b, 7,14,31); STEP(G,b,c,d,a,12,20,32);
    STEP(H,a,b,c,d, 5, 4,33); STEP(H,d,a,b,c, 8,11,34);
    STEP(H,c,d,a,b,11,16,35); STEP(H,b,c,d,a,14,23,36);
    STEP(H,a,b,c,d, 1, 4,37); STEP(H,d,a,b,c, 4,11,38);
    STEP(H,c,d,a,b, 7,16,39); STEP(H,b,c,d,a,10,23,40);
    STEP(H,a,b,c,d,13, 4,41); STEP(H,d,a,b,c, 0,11,42);
    STEP(H,c,d,a,b, 3,16,43); STEP(H,b,c,d,a, 6,23,44);
    STEP(H,a,b,c,d, 9, 4,45); STEP(H,d,a,b,c,12,11,46);
    STEP(H,c,d,a,b,15,16,47); STEP(H,b,c,d,a, 2,23,48);
    STEP(I,a,b,c,d, 0, 6,49); STEP(I,d,a,b,c, 7,10,50);
    STEP(I,c,d,a,b,14,15,51); STEP(I,b,c,d,a, 5,21,52);
    STEP(I,a,b,c,d,12, 6,53); STEP(I,d,a,b,c, 3,10,54);
    STEP(I,c,d,a,b,10,15,55); STEP(I,b,c,d,a, 1,21,56);
    STEP(I,a,b,c,d, 8, 6,57); STEP(I,d,a,b,c,15,10,58);
    STEP(I,c,d,a,b, 6,15,59); STEP(I,b,c,d,a,13,21,60);
    STEP(I,a,b,c,d, 4, 6,61); STEP(I,d,a,b,c,11,10,62);
    STEP(I,c,d,a,b, 2,15,63); STEP(I,b,c,d,a, 9,21,64);
    s[0]+=a; s[1]+=b; s[2]+=c; s[3]+=d;
}
static void compute_md5(const uint8_t* data, size_t len, uint8_t out[16]) {
    md5_init_T();
    uint32_t s[4] = {0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476};
    uint64_t bit_len = (uint64_t)len * 8;
    size_t padded = ((len+8)/64+1)*64;
    std::vector<uint8_t> msg(padded, 0);
    memcpy(msg.data(), data, len);
    msg[len] = 0x80;
    memcpy(msg.data()+padded-8, &bit_len, 8);
    for (size_t i=0;i<padded;i+=64) md5_block(s, msg.data()+i);
    for (int i=0;i<4;i++) memcpy(out+i*4, &s[i], 4);
}

// Forward decl — из tex_ops.cpp
std::string png2ktx(const std::string&, const std::string&, const std::string&);

std::string png2sc(const std::string& png_path, const std::string& orig_sc,
                   int idx, const std::string& out_dir) {
    // 1. Читаем PNG
    int pw=0, ph=0, pc=0;
    uint8_t* rgba = stbi_load(png_path.c_str(), &pw, &ph, &pc, 4);
    if (!rgba) return "Error: cannot load PNG: " + png_path;

    // 2. Читаем и декомпрессируем SC
    std::vector<uint8_t> sc_data;
    if (!sc_read_file(orig_sc, sc_data)) {
        stbi_image_free(rgba);
        return "Error: cannot read SC: " + orig_sc;
    }
    ScHeader hdr = sc_parse_header(sc_data);
    if (!hdr.valid) { stbi_image_free(rgba); return "Error: not an SC file"; }

    std::vector<uint8_t> raw;
    try { raw = sc_decompress(sc_data, hdr.comp); }
    catch (std::exception& e) {
        stbi_image_free(rgba);
        return std::string("Error: decompress: ") + e.what();
    }

    // 3. Найти все KTX блоки
    struct KTXInfo { size_t offset; int w, h; uint32_t gl; int bw, bh; size_t px_start; };
    std::vector<KTXInfo> ktxs;
    for (size_t i = 0; i+64 <= raw.size(); i++) {
        if (memcmp(raw.data()+i, KTX_MAGIC, 12) != 0) continue;
        uint32_t gl; memcpy(&gl, raw.data()+i+28, 4);
        uint32_t w;  memcpy(&w,  raw.data()+i+36, 4);
        uint32_t h;  memcpy(&h,  raw.data()+i+40, 4);
        uint32_t kv; memcpy(&kv, raw.data()+i+60, 4);
        size_t px = i + 64 + kv + 4;
        int bw=8, bh_=8; get_block(gl, bw, bh_);
        ktxs.push_back({i, (int)w, (int)h, gl, bw, bh_, px});
    }
    if (ktxs.empty()) { stbi_image_free(rgba); return "Error: no KTX blocks in SC"; }

    // 4. Выбрать target KTX
    int target = -1;
    if (idx >= 0 && idx < (int)ktxs.size()) {
        target = idx;
    } else {
        for (int i=0;i<(int)ktxs.size();i++)
            if (ktxs[i].w == pw && ktxs[i].h == ph) { target=i; break; }
        if (target < 0) target = 0;
    }
    auto& k = ktxs[target];
    LOGI("png2sc: KTX[%d] %dx%d ASTC %dx%d px_start=%zu",
         target, k.w, k.h, k.bw, k.bh, k.px_start);

    // 5. Кодируем PNG → ASTC через tex_ops png2ktx (в tmp файл)
    //    Используем уже существующую функцию, не дублируем encode
    std::string tmp_ktx_result = png2ktx(png_path,
        std::to_string(k.bw)+"x"+std::to_string(k.bh), out_dir);
    if (tmp_ktx_result.substr(0,2) == "Er") {
        stbi_image_free(rgba);
        return tmp_ktx_result;
    }

    // Читаем сгенерированный KTX файл
    std::string stem = sc_basename_no_ext(png_path);
    std::string ktx_path = out_dir + "/" + stem + ".ktx";
    std::vector<uint8_t> new_ktx;
    if (!sc_read_file(ktx_path, new_ktx)) {
        stbi_image_free(rgba);
        return "Error: cannot read temp KTX: " + ktx_path;
    }
    stbi_image_free(rgba);

    // Извлекаем пиксели из KTX (offset = 64 + kv + 4)
    if (new_ktx.size() < 68) return "Error: KTX too small";
    uint32_t new_kv; memcpy(&new_kv, new_ktx.data()+60, 4);
    size_t new_px_start = 64 + new_kv + 4;
    if (new_px_start >= new_ktx.size()) return "Error: KTX px_start out of range";
    std::vector<uint8_t> new_pixels(new_ktx.begin()+new_px_start, new_ktx.end());

    size_t orig_mip0 = mip0_size(k.w, k.h, k.bw, k.bh);
    LOGI("png2sc: orig_mip0=%zu new_pixels=%zu", orig_mip0, new_pixels.size());

    // 6. Заменяем только пиксели (как Termux)
    std::vector<uint8_t> new_raw;
    new_raw.reserve(raw.size() - orig_mip0 + new_pixels.size());
    new_raw.insert(new_raw.end(), raw.begin(), raw.begin()+k.px_start);
    new_raw.insert(new_raw.end(), new_pixels.begin(), new_pixels.end());
    new_raw.insert(new_raw.end(), raw.begin()+k.px_start+orig_mip0, raw.end());

    // 7. Обновляем MD5 в SC заголовке (последние 16 байт перед zstd)
    std::vector<uint8_t> sc_hdr(sc_data.begin(), sc_data.begin()+hdr.comp.offset);
    if (sc_hdr.size() >= 16) {
        uint8_t digest[16];
        compute_md5(new_raw.data(), new_raw.size(), digest);
        memcpy(sc_hdr.data() + sc_hdr.size() - 16, digest, 16);
    }

    // 8. Компрессируем и записываем
    std::vector<uint8_t> comp;
    try { comp = sc_compress(new_raw, hdr.comp.kind); }
    catch (std::exception& e) { return std::string("Error: compress: ") + e.what(); }

    sc_hdr.insert(sc_hdr.end(), comp.begin(), comp.end());
    std::string out_path = out_dir+"/"+sc_basename_no_ext(orig_sc)+"_patched.sc";
    if (!sc_write_file(out_path, sc_hdr)) return "Error: write failed: "+out_path;

    std::ostringstream r;
    r << "OK: PNG to SC\n";
    r << "  KTX[" << target << "]: " << k.w << "x" << k.h
      << " ASTC " << k.bw << "x" << k.bh << "\n";
    r << "  Raw: " << raw.size() << " -> " << new_raw.size() << "\n";
    r << "  Output: " << sc_basename_no_ext(orig_sc) << "_patched.sc\n";
    r << "  Dir: " << out_dir;
    return r.str();
}
