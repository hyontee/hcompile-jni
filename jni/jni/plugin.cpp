#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <unistd.h>
#include <thread>
#include <link.h>
#include "vendor/Dobby/include/dobby.h"

#define LOG_TAG "BLINE_JNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ============================================================
// НАСТРОЙКИ
// ============================================================

// Название, которое будет показываться вместо "BLACK RUSSIA"
// в сообщении "Соединение к BLACK RUSSIA..."
static const char* kServerName = "BLINE MOBILE";

// Оставляем существующее перенаправление подключения из исходного jni.
// Если оно не нужно, поставьте false.
static const bool kRedirectConnection = true;
static const char* kRedirectHost = "185.207.214.14";
static const uint16_t kRedirectPort = 2381;

// Offset CChat::AddDebugMessage для ARM64 из системы архива.
static constexpr uintptr_t kChatAddDebugMessageOffsetArm64 = 0x569900;

// Имена возможной библиотеки игры.
static const char* kGameLibraries[] = {
    "libblackrussia-client.so",
    "libbrbcbtodinnnnnnn.so"
};

typedef bool (*tConnect)(void*, const char*, uint16_t, uint16_t, uint32_t, int);
static tConnect orig_Connect = nullptr;

typedef int64_t (*tAddDebugMessage)(const char*, ...);
static tAddDebugMessage orig_AddDebugMessage = nullptr;

static uintptr_t FindGameBase()
{
    uintptr_t base = 0;

    dl_iterate_phdr([](dl_phdr_info* info, size_t, void* data) {
        if (!info || !info->dlpi_name || !data) {
            return 0;
        }

        for (const char* lib : kGameLibraries) {
            if (strstr(info->dlpi_name, lib)) {
                *(uintptr_t*)data = static_cast<uintptr_t>(info->dlpi_addr);
                return 1;
            }
        }

        return 0;
    }, &base);

    return base;
}

// Hook подключения. Сохраняем оригинальные host/port, если перенаправление выключено.
static bool hook_Connect(
    void* thiz,
    const char* host,
    uint16_t port,
    uint16_t cPort,
    uint32_t dep,
    int sleep)
{
    if (!orig_Connect) {
        return false;
    }

    if (kRedirectConnection) {
        return orig_Connect(thiz, kRedirectHost, kRedirectPort, cPort, dep, sleep);
    }

    return orig_Connect(thiz, host, port, cPort, dep, sleep);
}

// Меняем только отображаемый текст, не ломая остальные сообщения.
static int64_t hook_AddDebugMessage(const char* format, ...)
{
    if (!orig_AddDebugMessage) {
        return 0;
    }

    if (!format) {
        return orig_AddDebugMessage("%s", "");
    }

    char formatted[2048];

    va_list args;
    va_start(args, format);
    vsnprintf(formatted, sizeof(formatted), format, args);
    va_end(args);

    std::string text(formatted);
    const std::string from = "BLACK RUSSIA";
    const std::string to = kServerName;

    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos) {
        text.replace(pos, from.length(), to);
        pos += to.length();
    }

    return orig_AddDebugMessage("%s", text.c_str());
}

static void InitializeHooks()
{
    uintptr_t base = 0;

    // Ждём загрузки библиотеки игры.
    while (!base) {
        base = FindGameBase();
        if (!base) {
            usleep(100000);
        }
    }

    LOGI("Game base: %p", reinterpret_cast<void*>(base));

#if defined(__aarch64__)
    // CChat::AddDebugMessage
    void* chatTarget = reinterpret_cast<void*>(
        base + kChatAddDebugMessageOffsetArm64
    );

    if (DobbyHook(
            chatTarget,
            reinterpret_cast<void*>(hook_AddDebugMessage),
            reinterpret_cast<void**>(&orig_AddDebugMessage)) != RS_SUCCESS) {
        LOGE("DobbyHook CChat::AddDebugMessage failed");
    } else {
        LOGI("Server name hook installed: %s", kServerName);
    }

    // RakClient::Connect из существующего jni: ищем объект через прежний offset.
    uintptr_t rakClient = 0;
    while (!rakClient) {
        rakClient = *(uintptr_t*)(base + 0x131200);
        if (!rakClient) {
            usleep(100000);
        }
    }

    uintptr_t vtable = *(uintptr_t*)rakClient;
    if (vtable) {
        void* connectTarget = (void*)*(uintptr_t*)(vtable + 16);

        if (DobbyHook(
                connectTarget,
                reinterpret_cast<void*>(hook_Connect),
                reinterpret_cast<void**>(&orig_Connect)) != RS_SUCCESS) {
            LOGE("DobbyHook RakClient::Connect failed");
        } else {
            LOGI("Connect hook installed");
        }
    }
#else
    LOGE("This build is configured for ARM64 only.");
#endif
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)vm;
    (void)reserved;

    std::thread(InitializeHooks).detach();
    return JNI_VERSION_1_6;
}
