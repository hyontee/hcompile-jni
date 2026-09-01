#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <link.h>
#include "vendor/Dobby/include/dobby.h"

typedef bool (*tConnect)(void*, const char*, uint16_t, uint16_t, uint32_t, int);
tConnect orig_Connect = nullptr;

// ип вводите тут.
bool hook_Connect(void* _this, const char* host, uint16_t port, uint16_t cPort, uint32_t dep, int sleep) {
    return orig_Connect(_this, "188.127.241.74", 3256, cPort, dep, sleep);
}

void InitializeHooks() {
    uintptr_t base = 0;
    
    while (!base) {
        dl_iterate_phdr([](dl_phdr_info* info, size_t, void* data) {
            if (info->dlpi_name && strstr(info->dlpi_name, "libbrbtestt.so")) {
                *(uintptr_t*)data = info->dlpi_addr;
                return 1;
            }
            return 0;
        }, &base);
        if (!base) usleep(100000);
    }

    uintptr_t rakClient = 0;
    while (!(rakClient = *(uintptr_t*)(base + 0x12DB30))) {
        usleep(100000);
    }

    uintptr_t vtable = *(uintptr_t*)rakClient;
    void* target = (void*)*(uintptr_t*)(vtable + 16);

    DobbyHook(target, (void*)hook_Connect, (void**)&orig_Connect);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    std::thread(InitializeHooks).detach();
    return JNI_VERSION_1_6;
}