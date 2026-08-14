/**
 * Дополнение к sc_ops.cpp — sc_extract_ktx
 * Добавить этот файл в Android.mk -> LOCAL_SRC_FILES
 */
#include "sc_core.h"
#include <sstream>
#include <cstring>
#include <android/log.h>

#define TAG "SCOps2"

static const uint8_t KTX_MAGIC[12] = {
    0xab,'K','T','X',' ','1','1',0xbb,0x0d,0x0a,0x1a,0x0a
};

// Извлечь KTX по индексу из SC файла
std::string sc_extract_ktx(const std::string& sc_path, int idx, const std::string& out_dir) {
    std::vector<uint8_t> data;
    if (!sc_read_file(sc_path, data)) return "Error: cannot read " + sc_path;

    ScHeader hdr = sc_parse_header(data);
    if (!hdr.valid) return "Error: not an SC file";

    std::vector<uint8_t> raw;
    try { raw = sc_decompress(data, hdr.comp); }
    catch (std::exception& e) { return std::string("Error: ") + e.what(); }

    // Ищем все KTX блоки
    std::vector<size_t> ktx_offsets;
    for (size_t i = 0; i + 12 <= raw.size(); i++) {
        if (memcmp(raw.data() + i, KTX_MAGIC, 12) == 0) {
            ktx_offsets.push_back(i);
        }
    }

    if (ktx_offsets.empty()) return "Error: no KTX blocks found in SC";
    if (idx < 0 || idx >= (int)ktx_offsets.size())
        return "Error: index " + std::to_string(idx) + " out of range (found " +
               std::to_string(ktx_offsets.size()) + " KTX blocks)";

    size_t start = ktx_offsets[idx];
    size_t end   = (idx + 1 < (int)ktx_offsets.size()) ? ktx_offsets[idx + 1] : raw.size();

    std::string stem     = sc_basename_no_ext(sc_path);
    std::string out_path = out_dir + "/" + stem + "_" + std::to_string(idx) + ".ktx";
    std::vector<uint8_t> ktx_data(raw.begin() + start, raw.begin() + end);
    if (!sc_write_file(out_path, ktx_data)) return "Error: write failed " + out_path;

    std::ostringstream r;
    r << "OK: KTX extracted\n";
    r << "  File: " << stem << "_" << idx << ".ktx\n";
    r << "  Size: " << ktx_data.size() << " bytes\n";
    r << "  Dir:  " << out_dir;
    return r.str();
}
