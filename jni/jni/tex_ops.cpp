/**
 * tex_ops.cpp — SCTX↔PNG, KTX↔PNG, PNG→SC
 * Использует sctx_parser.h, astc_decoder.h, stb уже подключены в qwsctx_jni.cpp
 * Здесь НЕ переопределяем STB — он уже в qwsctx_jni.cpp
 */
#include "sc_core.h"
#include "sctx_parser.h"
#include "astc_decoder.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <android/log.h>
#include <algorithm>
#include <sstream>
#include <cstring>

#define TAG "TexOps"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// KTX magic
static const uint8_t KTX_MAGIC[12] = {
    0xab,'K','T','X',' ','1','1',0xbb,0x0d,0x0a,0x1a,0x0a
};

// GL format → bytes per pixel (for supported formats)
static int gl_bpp(uint32_t glFmt) {
    switch(glFmt) {
        case 0x8058: // GL_RGBA8
        case 0x8C43: // GL_SRGB8_ALPHA8
            return 4;
        case 0x8D62: // GL_RGB565
            return 2;
        case 0x8055: // GL_R8
            return 1;
        default:
            return 0; // compressed / unknown
    }
}

// ── SCTX → PNG ────────────────────────────────────────────────────────────────
std::string sctx2png(const std::string& sctx_path, const std::string& out_dir) {
    std::vector<uint8_t> data;
    if (!sc_read_file(sctx_path, data))
        return "❌ Не удалось открыть: " + sctx_path;

    SCTXFile sctx;
    std::string err;
    if (!parse_sctx(data.data(), data.size(), sctx, err))
        return "❌ Ошибка парсинга SCTX: " + err;

    int w = sctx.width, h = sctx.height;
    LOGI("sctx2png: %dx%d type=%s", w, h, pixel_type_name(sctx.pixel_type).c_str());

    // Decode mip 0
    uint32_t offset = sctx.levels.empty() ? 0 : sctx.levels[0].offset;
    if (offset >= sctx.texture_data.size())
        return "❌ Некорректный offset mip-уровня";

    const uint8_t* mip = sctx.texture_data.data() + offset;
    size_t mip_avail   = sctx.texture_data.size() - offset;

    std::vector<uint8_t> rgba;
    if (pixel_is_astc(sctx.pixel_type)) {
        int bw, bh;
        pixel_astc_blocks(sctx.pixel_type, bw, bh);
        size_t sz = astc_level_size(w, h, bw, bh);
        if (sz > mip_avail) sz = mip_avail;
        if (!astc_decode(mip, sz, w, h, bw, bh, rgba, pixel_is_astc_srgb(sctx.pixel_type)))
            return "❌ Ошибка ASTC декодирования";
    } else if (pixel_is_raw(sctx.pixel_type)) {
        size_t needed = (size_t)w * h * 4;
        if (needed > mip_avail) return "❌ Недостаточно данных RAW";
        rgba.assign(mip, mip + needed);
        if (sctx.pixel_type == PT_BGRA8Unorm || sctx.pixel_type == 81)
            for (size_t i = 0; i < rgba.size(); i += 4) std::swap(rgba[i], rgba[i+2]);
    } else {
        return "❌ Неподдерживаемый pixel_type: " + pixel_type_name(sctx.pixel_type);
    }

    std::string stem     = sc_basename_no_ext(sctx_path);
    std::string png_path = out_dir + "/" + stem + ".png";
    if (!stbi_write_png(png_path.c_str(), w, h, 4, rgba.data(), w * 4))
        return "❌ Ошибка сохранения PNG: " + png_path;

    std::ostringstream r;
    r << "✅ SCTX → PNG\n";
    r << "  " << stem << ".png\n";
    r << "  " << w << "×" << h << "  " << pixel_type_name(sctx.pixel_type) << "\n";
    r << "  " << out_dir;
    return r.str();
}

// ── PNG → SCTX ────────────────────────────────────────────────────────────────
std::string png2sctx(const std::string& png_path, const std::string& out_dir) {
    int w, h, ch;
    uint8_t* img = stbi_load(png_path.c_str(), &w, &h, &ch, 4);
    if (!img) return "❌ Не удалось загрузить PNG: " + png_path;

    int bw = 8, bh = 8;
    uint32_t ptype = PT_ASTC_8x8;

    // Mip levels
    int levels = 1;
    int mw = w, mh = h;
    while ((mw > 1 || mh > 1) && levels < 11) {
        mw = std::max(1, mw/2); mh = std::max(1, mh/2); levels++;
    }

    SCTXFile sctx;
    sctx.pixel_type   = ptype;
    sctx.width        = (uint16_t)w;
    sctx.height       = (uint16_t)h;
    sctx.levels_count = (uint8_t)levels;
    sctx.flags        = FLAG_COMPRESSION | FLAG_UNKNOWN2;

    std::vector<uint8_t> tex_data;
    for (int lvl = 0; lvl < levels; lvl++) {
        int lw = std::max(1, w >> lvl);
        int lh = std::max(1, h >> lvl);
        std::vector<uint8_t> scaled(lw * lh * 4);
        if (lvl == 0) {
            scaled.assign(img, img + (size_t)w*h*4);
        } else {
            for (int y = 0; y < lh; y++) for (int x = 0; x < lw; x++) {
                int sx = std::min((int)((float)x*w/lw), w-1);
                int sy = std::min((int)((float)y*h/lh), h-1);
                int src = (sy*w+sx)*4, dst = (y*lw+x)*4;
                scaled[dst]=img[src]; scaled[dst+1]=img[src+1];
                scaled[dst+2]=img[src+2]; scaled[dst+3]=img[src+3];
            }
        }
        MipLevel ml; ml.width=(uint16_t)lw; ml.height=(uint16_t)lh;
        ml.offset=(uint32_t)tex_data.size();

        std::vector<uint8_t> enc;
        if (!astc_encode(scaled.data(), lw, lh, bw, bh, enc)) {
            stbi_image_free(img);
            return "❌ Ошибка ASTC кодирования (mip " + std::to_string(lvl) + ")";
        }
        tex_data.insert(tex_data.end(), enc.begin(), enc.end());
        sctx.levels.push_back(ml);
    }
    stbi_image_free(img);

    std::vector<uint8_t> out_data;
    std::string build_err;
    if (!build_sctx(sctx, tex_data, out_data, build_err))
        return "❌ Ошибка сборки SCTX: " + build_err;

    std::string stem      = sc_basename_no_ext(png_path);
    std::string out_path  = out_dir + "/" + stem + ".sctx";
    if (!sc_write_file(out_path, out_data))
        return "❌ Ошибка записи: " + out_path;

    std::ostringstream r;
    r << "✅ PNG → SCTX\n";
    r << "  " << stem << ".sctx\n";
    r << "  " << w << "×" << h << "  ASTC_8x8  " << levels << " mip\n";
    r << "  " << out_dir;
    return r.str();
}

// ── KTX → PNG ────────────────────────────────────────────────────────────────
std::string ktx2png(const std::string& ktx_path, const std::string& out_dir) {
    std::vector<uint8_t> data;
    if (!sc_read_file(ktx_path, data)) return "❌ Не удалось открыть: " + ktx_path;

    if (data.size() < 64 || memcmp(data.data(), KTX_MAGIC, 12) != 0)
        return "❌ Не KTX файл";

    // KTX header layout
    uint32_t glType, glFormat, glInternalFormat, glBaseInternalFormat;
    uint32_t pixelW, pixelH, pixelD;
    uint32_t numFaces, numMips, bytesOfKeyValueData;

    size_t p = 12;
    auto rd32 = [&]() -> uint32_t {
        if (p + 4 > data.size()) return 0;
        uint32_t v; memcpy(&v, data.data()+p, 4); p+=4; return v;
    };
    rd32(); // endianness
    glType              = rd32();
    rd32(); // glTypeSize
    glFormat            = rd32();
    glInternalFormat    = rd32();
    glBaseInternalFormat= rd32();
    pixelW              = rd32();
    pixelH              = rd32();
    pixelD              = rd32();
    rd32(); // numArrayElements
    numFaces            = rd32();
    numMips             = rd32();
    bytesOfKeyValueData = rd32();
    p += bytesOfKeyValueData;

    int w = (int)pixelW, h = (int)pixelH;
    if (p + 4 > data.size()) return "❌ KTX: нет данных изображения";

    uint32_t imgSize = rd32(); // imageSize for mip 0

    LOGI("ktx: %dx%d glType=%u glFmt=%u intFmt=%u", w, h, glType, glFormat, glInternalFormat);

    std::vector<uint8_t> rgba;

    // Check if ASTC compressed
    bool is_astc = (glType == 0 && glFormat == 0);
    if (is_astc) {
        // glInternalFormat: 0x93B0..0x93BD = ASTC 4x4..12x12
        struct ASTCFmt { uint32_t gl; int bw, bh; };
        static const ASTCFmt fmts[] = {
            {0x93B0,4,4},{0x93B1,5,4},{0x93B2,5,5},{0x93B3,6,5},
            {0x93B4,6,6},{0x93B5,8,5},{0x93B6,8,6},{0x93B7,8,8},
            {0x93B8,10,5},{0x93B9,10,6},{0x93BA,10,8},{0x93BB,10,10},
            {0x93BC,12,10},{0x93BD,12,12},
            // sRGB
            {0x93D0,4,4},{0x93D1,5,4},{0x93D2,5,5},{0x93D3,6,5},
            {0x93D4,6,6},{0x93D5,8,5},{0x93D6,8,6},{0x93D7,8,8},
            {0x93D8,10,5},{0x93D9,10,6},{0x93DA,10,8},{0x93DB,10,10},
            {0x93DC,12,10},{0x93DD,12,12},
            {0,0,0}
        };
        int bw = 0, bh = 0;
        for (auto* f = fmts; f->gl; f++) {
            if (f->gl == glInternalFormat) { bw = f->bw; bh = f->bh; break; }
        }
        if (!bw) return "❌ KTX: неизвестный ASTC формат: 0x" +
                        [&]{ std::ostringstream s; s<<std::hex<<glInternalFormat; return s.str(); }();

        bool srgb = (glInternalFormat >= 0x93D0);
        size_t ast_size = std::min((size_t)imgSize, data.size()-p);
        if (!astc_decode(data.data()+p, ast_size, w, h, bw, bh, rgba, srgb))
            return "❌ KTX: ошибка ASTC декодирования";
    } else {
        // Uncompressed
        int bpp = gl_bpp(glInternalFormat != 0 ? glInternalFormat : glFormat);
        if (bpp == 4) {
            size_t needed = (size_t)w*h*4;
            if (p + needed > data.size()) return "❌ KTX: недостаточно данных";
            rgba.assign(data.begin()+p, data.begin()+p+needed);
        } else if (bpp == 2) {
            // RGB565 → RGBA8
            size_t needed = (size_t)w*h*2;
            if (p + needed > data.size()) return "❌ KTX: недостаточно данных";
            rgba.resize(w*h*4);
            for (int i = 0; i < w*h; i++) {
                uint16_t px; memcpy(&px, data.data()+p+i*2, 2);
                rgba[i*4+0] = ((px>>11)&0x1F)<<3;
                rgba[i*4+1] = ((px>>5)&0x3F)<<2;
                rgba[i*4+2] = (px&0x1F)<<3;
                rgba[i*4+3] = 255;
            }
        } else {
            return "❌ KTX: неподдерживаемый формат GL: 0x" +
                   [&]{ std::ostringstream s; s<<std::hex<<glInternalFormat; return s.str(); }();
        }
    }

    std::string stem     = sc_basename_no_ext(ktx_path);
    std::string png_path = out_dir + "/" + stem + ".png";
    if (!stbi_write_png(png_path.c_str(), w, h, 4, rgba.data(), w*4))
        return "❌ Ошибка сохранения PNG: " + png_path;

    std::ostringstream r;
    r << "✅ KTX → PNG\n  " << stem << ".png\n  " << w << "×" << h << "\n  " << out_dir;
    return r.str();
}

// ── PNG → KTX ────────────────────────────────────────────────────────────────
std::string png2ktx(const std::string& png_path, const std::string& out_dir) {
    int w, h, ch;
    uint8_t* img = stbi_load(png_path.c_str(), &w, &h, &ch, 4);
    if (!img) return "❌ Не удалось загрузить PNG: " + png_path;

    // Encode ASTC 8x8
    std::vector<uint8_t> astc;
    bool ok = astc_encode(img, w, h, 8, 8, astc);
    stbi_image_free(img);
    if (!ok) return "❌ Ошибка ASTC кодирования";

    // Build KTX file
    std::vector<uint8_t> out_data;
    // Magic + header
    uint8_t header[64] = {};
    memcpy(header, KTX_MAGIC, 12);
    auto w32 = [&](int off, uint32_t v) { memcpy(header+off, &v, 4); };
    w32(12, 0x04030201); // endianness
    w32(16, 0);          // glType (compressed)
    w32(20, 1);          // glTypeSize
    w32(24, 0);          // glFormat
    w32(28, 0x93D7);     // glInternalFormat ASTC_8x8_SRGB
    w32(32, 0x1908);     // glBaseInternalFormat RGBA
    w32(36, (uint32_t)w);
    w32(40, (uint32_t)h);
    w32(44, 0);          // depth
    w32(48, 0);          // numArrayElements
    w32(52, 1);          // numFaces
    w32(56, 1);          // numMips
    w32(60, 0);          // keyValueData

    out_data.insert(out_data.end(), header, header+64);
    uint32_t imgSize = (uint32_t)astc.size();
    uint8_t sz[4]; memcpy(sz, &imgSize, 4);
    out_data.insert(out_data.end(), sz, sz+4);
    out_data.insert(out_data.end(), astc.begin(), astc.end());

    std::string stem     = sc_basename_no_ext(png_path);
    std::string out_path = out_dir + "/" + stem + ".ktx";
    if (!sc_write_file(out_path, out_data))
        return "❌ Ошибка записи: " + out_path;

    std::ostringstream r;
    r << "✅ PNG → KTX\n  " << stem << ".ktx\n  " << w << "×" << h << "  ASTC_8x8\n  " << out_dir;
    return r.str();
}

// ── PNG → SC (текстурный блок) ────────────────────────────────────────────────
std::string png2sc(const std::string& png_path, const std::string& out_dir) {
    // Конвертируем PNG → KTX во временный файл, потом оборачиваем в SC
    std::string tmp_ktx = out_dir + "/_tmp_tex.ktx";
    std::string r = png2ktx(png_path, out_dir);
    if (r.substr(0,2) == "❌") return r;

    std::string stem     = sc_basename_no_ext(png_path);
    std::string ktx_path = out_dir + "/" + stem + ".ktx";

    std::vector<uint8_t> ktx_data;
    if (!sc_read_file(ktx_path, ktx_data)) return "❌ Не удалось прочитать KTX";

    // Оборачиваем в SC v1 с zstd
    std::vector<uint8_t> header = {'S','C', 0x01, 0x00, 0x00, 0x00};
    std::vector<uint8_t> comp;
    try { comp = sc_compress(ktx_data, CompKind::ZSTD); }
    catch (std::exception& e) { return std::string("❌ ") + e.what(); }

    header.insert(header.end(), comp.begin(), comp.end());
    std::string out_path = out_dir + "/" + stem + "_tex.sc";
    if (!sc_write_file(out_path, header)) return "❌ Ошибка записи: " + out_path;

    std::ostringstream res;
    res << "✅ PNG → SC\n  " << stem << "_tex.sc\n  " << out_dir;
    return res.str();
}
