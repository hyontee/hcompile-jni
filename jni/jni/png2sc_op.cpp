/**
 * png2sc_op.cpp — PNG to SC (заменить KTX текстуру внутри SC)
 * Логика: читаем оригинальный SC, распаковываем, находим KTX[idx],
 * конвертируем новый PNG в KTX, заменяем блок, переупаковываем.
 */
#include "sc_core.h"
#include "stb/stb_image.h"
#include <sstream>
#include <cstring>
#include <android/log.h>

#define TAG "Png2SC"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

static const uint8_t KTX_MAGIC[12] = {
    0xab,'K','T','X',' ','1','1',0xbb,0x0d,0x0a,0x1a,0x0a
};

// Forward decl — из tex_ops.cpp
std::string png2ktx(const std::string&, const std::string&);

std::string png2sc(const std::string& png_path, const std::string& orig_sc,
                   int idx, const std::string& out_dir) {
    // 1. Конвертируем PNG в KTX во временную папку
    std::string tmp_dir = out_dir;
    std::string ktx_result = png2ktx(png_path, tmp_dir);
    if (ktx_result.substr(0, 2) == "Er") return ktx_result;

    std::string stem_png = sc_basename_no_ext(png_path);
    std::string ktx_path = out_dir + "/" + stem_png + ".ktx";

    std::vector<uint8_t> new_ktx;
    if (!sc_read_file(ktx_path, new_ktx)) return "Error: failed to read converted KTX";

    // 2. Читаем оригинальный SC
    std::vector<uint8_t> sc_data;
    if (!sc_read_file(orig_sc, sc_data)) return "Error: cannot read original SC: " + orig_sc;

    ScHeader hdr = sc_parse_header(sc_data);
    if (!hdr.valid) return "Error: not an SC file: " + orig_sc;

    std::vector<uint8_t> raw;
    try { raw = sc_decompress(sc_data, hdr.comp); }
    catch (std::exception& e) { return std::string("Error: decompress failed: ") + e.what(); }

    // 3. Находим KTX[idx]
    std::vector<size_t> ktx_offs;
    for (size_t i = 0; i + 12 <= raw.size(); i++) {
        if (memcmp(raw.data() + i, KTX_MAGIC, 12) == 0) ktx_offs.push_back(i);
    }
    if (ktx_offs.empty()) return "Error: no KTX blocks in SC";

    // Если idx < 0 — авто (первый блок)
    int target = (idx < 0 || idx >= (int)ktx_offs.size()) ? 0 : idx;
    LOGI("png2sc: replacing KTX[%d] of %zu total", target, ktx_offs.size());

    size_t start = ktx_offs[target];
    size_t end   = (target + 1 < (int)ktx_offs.size()) ? ktx_offs[target + 1] : raw.size();

    // 4. Заменяем блок
    std::vector<uint8_t> patched;
    patched.insert(patched.end(), raw.begin(), raw.begin() + start);
    patched.insert(patched.end(), new_ktx.begin(), new_ktx.end());
    patched.insert(patched.end(), raw.begin() + end, raw.end());

    // 5. Переупаковываем с той же компрессией
    std::vector<uint8_t> sc_header(sc_data.begin(), sc_data.begin() + hdr.comp.offset);
    std::vector<uint8_t> comp;
    try { comp = sc_compress(patched, hdr.comp.kind); }
    catch (std::exception& e) { return std::string("Error: compress failed: ") + e.what(); }

    sc_header.insert(sc_header.end(), comp.begin(), comp.end());

    std::string stem_sc  = sc_basename_no_ext(orig_sc);
    std::string out_path = out_dir + "/" + stem_sc + "_patched.sc";
    if (!sc_write_file(out_path, sc_header)) return "Error: write failed: " + out_path;

    std::ostringstream r;
    r << "OK: PNG to SC complete\n";
    r << "  Output: " << stem_sc << "_patched.sc\n";
    r << "  KTX index replaced: " << target << " of " << ktx_offs.size() << "\n";
    r << "  Dir: " << out_dir;
    return r.str();
}
