// ASTC decoder/encoder using ARM astcenc library (v5.3.0)
#include "astc_decoder.h"
#include "sctx_parser.h"   // для pixel_is_astc_srgb
#include <cstring>
#include <vector>
#include <android/log.h>
#include <astcenc.h>

#define TAG "QwSCTX_ASTC"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

bool astc_decode(const uint8_t* data, size_t data_size,
                 int width, int height,
                 int block_w, int block_h,
                 std::vector<uint8_t>& output,
                 bool srgb)
{
    output.assign((size_t)width * height * 4, 0);

    astcenc_config config;
    astcenc_profile profile = srgb ? ASTCENC_PRF_LDR_SRGB : ASTCENC_PRF_LDR;

    astcenc_error status = astcenc_config_init(
        profile,
        (unsigned int)block_w, (unsigned int)block_h, 1,
        ASTCENC_PRE_MEDIUM,
        ASTCENC_FLG_DECOMPRESS_ONLY,
        &config
    );
    if(status != ASTCENC_SUCCESS) {
        LOGE("astcenc_config_init failed: %s", astcenc_get_error_string(status));
        return false;
    }

    astcenc_context* ctx = nullptr;
    status = astcenc_context_alloc(&config, 1, &ctx);
    if(status != ASTCENC_SUCCESS) {
        LOGE("astcenc_context_alloc failed: %s", astcenc_get_error_string(status));
        return false;
    }

    astcenc_swizzle swizzle = { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A };

    uint8_t* out_ptr = output.data();
    astcenc_image image;
    image.dim_x     = (unsigned int)width;
    image.dim_y     = (unsigned int)height;
    image.dim_z     = 1;
    image.data_type = ASTCENC_TYPE_U8;
    image.data      = reinterpret_cast<void**>(&out_ptr);

    status = astcenc_decompress_image(ctx, data, data_size, &image, &swizzle, 0);
    if(status != ASTCENC_SUCCESS) {
        LOGE("astcenc_decompress_image failed: %s (block=%dx%d img=%dx%d size=%zu)",
             astcenc_get_error_string(status), block_w, block_h, width, height, data_size);
    }

    astcenc_context_free(ctx);
    return status == ASTCENC_SUCCESS;
}

// Перегрузка без srgb (для совместимости)
bool astc_decode(const uint8_t* data, size_t data_size,
                 int width, int height,
                 int block_w, int block_h,
                 std::vector<uint8_t>& output)
{
    return astc_decode(data, data_size, width, height, block_w, block_h, output, false);
}

bool astc_encode(const uint8_t* rgba, int width, int height,
                 int block_w, int block_h,
                 std::vector<uint8_t>& output)
{
    int bx = (width  + block_w - 1) / block_w;
    int by = (height + block_h - 1) / block_h;
    output.resize((size_t)bx * by * 16, 0);

    astcenc_config config;
    astcenc_error status = astcenc_config_init(
        ASTCENC_PRF_LDR,
        (unsigned int)block_w, (unsigned int)block_h, 1,
        ASTCENC_PRE_MEDIUM, 0, &config
    );
    if(status != ASTCENC_SUCCESS) {
        LOGE("astcenc_config_init (encode) failed: %s", astcenc_get_error_string(status));
        return false;
    }

    astcenc_context* ctx = nullptr;
    status = astcenc_context_alloc(&config, 1, &ctx);
    if(status != ASTCENC_SUCCESS) {
        LOGE("astcenc_context_alloc (encode) failed: %s", astcenc_get_error_string(status));
        return false;
    }

    astcenc_swizzle swizzle = { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A };

    const uint8_t* in_ptr = rgba;
    astcenc_image image;
    image.dim_x     = (unsigned int)width;
    image.dim_y     = (unsigned int)height;
    image.dim_z     = 1;
    image.data_type = ASTCENC_TYPE_U8;
    image.data      = reinterpret_cast<void**>(const_cast<uint8_t**>(&in_ptr));

    status = astcenc_compress_image(ctx, &image, &swizzle,
                                    output.data(), output.size(), 0);
    if(status != ASTCENC_SUCCESS) {
        LOGE("astcenc_compress_image failed: %s", astcenc_get_error_string(status));
    }

    astcenc_context_free(ctx);
    return status == ASTCENC_SUCCESS;
}
