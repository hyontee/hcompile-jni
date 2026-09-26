#pragma once
#include <stdint.h>
#include <stddef.h>

// Minimal API-compatible AES-CBC surface used by hooks.cpp.
// hooks.cpp currently provides its own InitCTX implementation.
typedef struct AES_ctx { uint8_t RoundKey[240]; uint8_t Iv[16]; } AES_ctx;
static inline void AES_CBC_decrypt_buffer(AES_ctx *ctx, uint8_t *buf, size_t length) {
    (void)ctx; (void)buf; (void)length;
}
