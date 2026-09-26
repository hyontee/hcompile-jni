#pragma once
#include <string>
#include <stddef.h>

inline std::string decrypt(const std::string& value, const std::string& key) {
    if (key.empty()) return value;
    std::string out = value;
    for (size_t i=0;i<out.size();++i) out[i] = static_cast<char>(out[i] ^ key[i % key.size()]);
    return out;
}
