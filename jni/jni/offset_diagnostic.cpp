#include <android/log.h>
#include <cstdint>

#define TAG "PLUGIN_OFFSETS"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

extern "C" __attribute__((constructor)) void init_offset_diagnostic(){
#if defined(__aarch64__)
    LOGI("ABI arm64-v8a");
    LOGI("0x00110007 is not a file offset for the supplied 64-bit libplugin.so");
#elif defined(__arm__)
    LOGI("ABI armeabi-v7a");
    LOGI("0x00110007 may belong to a 32-bit target/version; no unsafe direct hook is installed");
#else
    LOGI("unsupported ABI");
#endif
}
