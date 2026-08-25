/**
 * sc_editor_jni.cpp
 * JNI для редактора SC:
 * - nativeGetScInfo: извлекает PNG атлас + список спрайтов
 */
#include <jni.h>
#include <string>
#include <sstream>
#include <cstring>
#include <cmath>
#include <android/log.h>
#include "sc_core.h"
#include "stb/stb_image_write.h"
#include "astc_decoder.h"

#define TAG "ScEditor"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

#ifndef SCTOOL_JNI_CLASS
#define SCTOOL_JNI_CLASS Java_com_compose_sctool_ScProcessor
#endif
#define _CAT(a,b) a##b
#define CAT(a,b) _CAT(a,b)
#define FN(name) CAT(SCTOOL_JNI_CLASS, name)

static const uint8_t KTX_MAGIC[12] = {
    0xab,'K','T','X',' ','1','1',0xbb,0x0d,0x0a,0x1a,0x0a
};

struct ASTCFmt { uint32_t gl; int bw, bh; };
static const ASTCFmt ASTC_FMTS[] = {
    {0x93B0,4,4},{0x93B1,5,4},{0x93B2,5,5},{0x93B3,6,5},
    {0x93B4,6,6},{0x93B5,8,5},{0x93B6,8,6},{0x93B7,8,8},
    {0x93B8,10,5},{0x93B9,10,6},{0x93BA,10,8},{0x93BB,10,10},
    {0x93BC,12,10},{0x93BD,12,12},
    {0x93D0,4,4},{0x93D1,5,4},{0x93D2,5,5},{0x93D3,6,5},
    {0x93D4,6,6},{0x93D5,8,5},{0x93D6,8,6},{0x93D7,8,8},
    {0x93D8,10,5},{0x93D9,10,6},{0x93DA,10,8},{0x93DB,10,10},
    {0x93DC,12,10},{0x93DD,12,12},{0,0,0}
};

// Декодировать KTX блок в RGBA
static bool decode_ktx(const uint8_t* d, size_t sz,
                        int& w, int& h, std::vector<uint8_t>& rgba) {
    if (sz < 64 || memcmp(d, KTX_MAGIC, 12) != 0) return false;
    size_t p = 12;
    auto rd32 = [&]() -> uint32_t {
        if (p+4>sz) return 0; uint32_t v; memcpy(&v,d+p,4); p+=4; return v;
    };
    rd32(); // endianness
    uint32_t glType=rd32(); rd32(); uint32_t glFmt=rd32();
    uint32_t glInt=rd32(); rd32();
    w=(int)rd32(); h=(int)rd32();
    rd32();rd32();rd32();rd32();
    uint32_t kvd=rd32(); p+=kvd;
    uint32_t imgSize=rd32();
    if (p+imgSize>sz) imgSize=(uint32_t)(sz-p);

    if (glType==0 && glFmt==0) {
        int bw=8,bh=8;
        for (auto* f=ASTC_FMTS;f->gl;f++)
            if(f->gl==glInt){bw=f->bw;bh=f->bh;break;}
        bool srgb=(glInt>=0x93D0);
        return astc_decode(d+p, imgSize, w, h, bw, bh, rgba, srgb);
    }
    size_t needed=(size_t)w*h*4;
    if (p+needed>sz) return false;
    rgba.assign(d+p, d+p+needed);
    return true;
}

// Парсим SC тег Shape (18) → UV координаты спрайта
struct SpriteInfo {
    int id;
    float u0,v0,u1,v1; // UV в пикселях
    float w,h;          // размер в игровых единицах
};

static std::vector<SpriteInfo> parse_shapes(
    const std::vector<uint8_t>& raw, int atlas_w, int atlas_h)
{
    std::vector<SpriteInfo> result;
    size_t p = 0;

    // Skip SC header
    auto r16 = [&]() -> uint16_t {
        if (p+2>raw.size()) return 0;
        uint16_t v; memcpy(&v,raw.data()+p,2); p+=2; return v;
    };
    auto r8 = [&]() -> uint8_t {
        return p<raw.size() ? raw[p++] : 0;
    };
    auto r32 = [&]() -> uint32_t {
        if (p+4>raw.size()) return 0;
        uint32_t v; memcpy(&v,raw.data()+p,4); p+=4; return v;
    };
    auto rstr = [&]() -> std::string {
        uint8_t l=r8(); uint16_t sl=l;
        if (l==0xFF) sl=r16();
        if (p+sl>raw.size()) return "";
        std::string s((char*)raw.data()+p,sl); p+=sl; return s;
    };
    auto rs16 = [&]() -> int16_t {
        if (p+2>raw.size()) return 0;
        int16_t v; memcpy(&v,raw.data()+p,2); p+=2; return v;
    };

    uint16_t shape_c=r16(),mc_c=r16(),tex_c=r16();
    r16();r16();r16(); r32(); r8();
    uint16_t ec=r16();
    std::vector<uint16_t> ids(ec); for(auto& id:ids) id=r16();
    std::vector<std::string> names(ec); for(auto& n:names) n=rstr();

    // Build export name map: shape_id → name
    // MC binds shape, export names go with MC
    // We'll map by shape index for now

    // Walk tags
    while (p+5 <= raw.size()) {
        uint8_t tag = r8();
        uint32_t tlen = r32();
        if (tag == 0) break;
        size_t tag_start = p;

        if (tag == 18 && tlen >= 38) { // Shape tag
            uint16_t sh_id   = r16();
            uint16_t draw_c  = r16();
            uint16_t tex_idx = r16();

            // 8 UV int16 (×32767 normalized)
            int16_t uvs[8];
            for(int i=0;i<8;i++) uvs[i]=rs16();

            // 8 XY int16 (×20 pixels)
            int16_t xys[8];
            for(int i=0;i<8;i++) xys[i]=rs16();

            // UV → pixels
            // uvs are normalized 0-32767 → 0..atlas_size
            float u0 = (float)uvs[0]/32767.0f * atlas_w;
            float v0 = (float)uvs[1]/32767.0f * atlas_h;
            float u1 = (float)uvs[2]/32767.0f * atlas_w;
            float v1 = (float)uvs[5]/32767.0f * atlas_h;

            // XY → sprite size (×20 → pixels, take abs)
            float w = fabsf((float)(xys[2]-xys[0]))/20.0f;
            float h = fabsf((float)(xys[1]-xys[5]))/20.0f;

            SpriteInfo si;
            si.id = sh_id;
            si.u0 = u0; si.v0 = v0;
            si.u1 = u1; si.v1 = v1;
            si.w  = w;  si.h  = h;
            result.push_back(si);
        }

        p = tag_start + tlen;
    }

    return result;
}

// ── nativeGetScInfo ───────────────────────────────────────────────────────────
extern "C"
JNIEXPORT jstring JNICALL
Java_com_compose_sctool_ScProcessor_nativeGetScInfo(
        JNIEnv* env, jclass, jstring sc_path_j, jstring out_dir_j) {

    auto jstr = [&](jstring s) -> std::string {
        const char* c = env->GetStringUTFChars(s, nullptr);
        std::string r(c); env->ReleaseStringUTFChars(s, c); return r;
    };

    std::string sc_path = jstr(sc_path_j);
    std::string out_dir = jstr(out_dir_j);

    std::vector<uint8_t> data;
    if (!sc_read_file(sc_path, data))
        return env->NewStringUTF("Error: cannot read SC");

    ScHeader hdr = sc_parse_header(data);
    if (!hdr.valid)
        return env->NewStringUTF("Error: not an SC file");

    std::vector<uint8_t> raw;
    try { raw = sc_decompress(data, hdr.comp); }
    catch (std::exception& e) {
        return env->NewStringUTF((std::string("Error: ") + e.what()).c_str());
    }

    // Найти KTX
    size_t ktx_off = std::string::npos;
    for (size_t i = 0; i+12 <= raw.size(); i++) {
        if (memcmp(raw.data()+i, KTX_MAGIC, 12) == 0) { ktx_off=i; break; }
    }

    // Если нет KTX — попробуем tex файл рядом
    if (ktx_off == std::string::npos) {
        std::string stem = sc_basename_no_ext(sc_path);
        std::string dir  = sc_dirname(sc_path);
        std::string tex_path = dir + "/" + stem + "_tex.sc";
        std::vector<uint8_t> tex_data;
        if (sc_read_file(tex_path, tex_data)) {
            ScHeader tex_hdr = sc_parse_header(tex_data);
            if (tex_hdr.valid) {
                try {
                    std::vector<uint8_t> tex_raw = sc_decompress(tex_data, tex_hdr.comp);
                    // Декомпрессированный tex SC — ищем KTX внутри
                    for (size_t i = 0; i+12 <= tex_raw.size(); i++) {
                        if (memcmp(tex_raw.data()+i, KTX_MAGIC, 12) == 0) {
                            // Декодируем из tex_raw
                            int aw=0, ah=0;
                            std::vector<uint8_t> rgba;
                            size_t blk_end = tex_raw.size();
                            if (!decode_ktx(tex_raw.data()+i, blk_end-i, aw, ah, rgba)) {
                                return env->NewStringUTF("Error: KTX decode failed (tex)");
                            }
                            std::string png_path = out_dir + "/" + stem + "_atlas.png";
                            stbi_write_png(png_path.c_str(), aw, ah, 4, rgba.data(), aw*4);

                            auto sprites = parse_shapes(raw, aw, ah);
                            std::ostringstream out;
                            for (auto& s : sprites) {
                                out << s.id << "|" << "sprite_" << s.id << "|"
                                    << s.u0 << "|" << s.v0 << "|"
                                    << s.u1 << "|" << s.v1 << "|"
                                    << s.w  << "|" << s.h  << "\n";
                            }
                            return env->NewStringUTF(out.str().c_str());
                        }
                    }
                } catch (...) {}
            }
        }
        return env->NewStringUTF("Error: no KTX found (load _tex.sc too)");
    }

    // Декодируем KTX из основного SC
    size_t blk_end = raw.size();
    for (size_t i = ktx_off+12; i+12 <= raw.size(); i++) {
        if (memcmp(raw.data()+i, KTX_MAGIC, 12) == 0) { blk_end=i; break; }
    }

    int aw=0, ah=0;
    std::vector<uint8_t> rgba;
    if (!decode_ktx(raw.data()+ktx_off, blk_end-ktx_off, aw, ah, rgba))
        return env->NewStringUTF("Error: KTX decode failed");

    // Сохраняем PNG атлас
    std::string stem     = sc_basename_no_ext(sc_path);
    std::string png_path = out_dir + "/" + stem + "_atlas.png";
    if (!stbi_write_png(png_path.c_str(), aw, ah, 4, rgba.data(), aw*4))
        return env->NewStringUTF("Error: PNG write failed");

    // Парсим Shape теги
    auto sprites = parse_shapes(raw, aw, ah);

    // Возвращаем список спрайтов в формате id|name|u0|v0|u1|v1|w|h
    std::ostringstream out;
    for (auto& s : sprites) {
        out << s.id << "|" << "sprite_" << s.id << "|"
            << s.u0 << "|" << s.v0 << "|"
            << s.u1 << "|" << s.v1 << "|"
            << s.w  << "|" << s.h  << "\n";
    }
    return env->NewStringUTF(out.str().c_str());
}
