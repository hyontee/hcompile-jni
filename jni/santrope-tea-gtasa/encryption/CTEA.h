#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

class CTEA {
    uint32_t k_[4]{};
    static void enc(uint32_t &v0, uint32_t &v1, const uint32_t k[4]) {
        uint32_t sum = 0, delta = 0x9e3779b9u;
        for (unsigned i=0;i<32;i++) {
            v0 += (((v1<<4) + k[0]) ^ (v1 + sum) ^ ((v1>>5) + k[1]));
            sum += delta;
            v1 += (((v0<<4) + k[2]) ^ (v0 + sum) ^ ((v0>>5) + k[3]));
        }
    }
public:
    void SetKey(const uint32_t key[4]) { if (key) memcpy(k_, key, sizeof(k_)); }
    void EncryptData(void *data, size_t size, int /*bits*/ = 32) {
        if (!data) return;
        unsigned char *p = static_cast<unsigned char*>(data);
        for (size_t off=0; off+8<=size; off+=8) {
            uint32_t v0,v1; memcpy(&v0,p+off,4); memcpy(&v1,p+off+4,4);
            enc(v0,v1,k_); memcpy(p+off,&v0,4); memcpy(p+off+4,&v1,4);
        }
    }
};
