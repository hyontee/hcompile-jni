#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

class CTinyEncrypt {
    uint8_t key_[16]{};
public:
    void SetKey(const uint8_t *key) { if (key) memcpy(key_, key, sizeof(key_)); }
    void DecryptData(void *data, size_t size, int block = 16) {
        if (!data || block <= 0) return;
        uint8_t *p = static_cast<uint8_t*>(data);
        for (size_t i=0;i<size;i++) p[i] ^= key_[i % sizeof(key_)];
    }
};
