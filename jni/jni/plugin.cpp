#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <link.h>
#include "vendor/Dobby/include/dobby.h"

// -------------------------------------------------------------
// 1. Определение типов и оригнальных указателей
// -------------------------------------------------------------

// Хук подключения (подмена IP)
typedef bool (*tConnect)(void*, const char*, uint16_t, uint16_t, uint32_t, int);
tConnect orig_Connect = nullptr;

// Хук установки номера (фикс вылета)
typedef void (*tSetPlateText)(void* vehicle, const char* text);
tSetPlateText orig_SetPlateText = nullptr;


// -------------------------------------------------------------
// 2. Функции-перехватчики (Hooks)
// -------------------------------------------------------------

// Подмена IP и порта
bool hook_Connect(void* _this, const char* host, uint16_t port, uint16_t cPort, uint32_t dep, int sleep) {
    return orig_Connect(_this, "185.207.214.14", 3233, cPort, dep, sleep);
}

// Защита от вылета при рендере номеров
void hook_SetPlateText(void* vehicle, const char* text) {
    // Если объект машины равен nullptr — игнорируем вызов, чтобы клиент не падал
    if (!vehicle) return; 
    
    if (orig_SetPlateText) {
        orig_SetPlateText(vehicle, text);
    }
}


// -------------------------------------------------------------
// 3. Инициализация и установка хуков
// -------------------------------------------------------------

void InitializeHooks() {
    uintptr_t base = 0;
    
    // Ожидаем загрузку libbrbonus.so в память
    while (!base) {
        dl_iterate_phdr([](dl_phdr_info* info, size_t, void* data) {
            if (info->dlpi_name && strstr(info->dlpi_name, "libbrbonus.so")) {
                *(uintptr_t*)data = info->dlpi_addr;
                return 1;
            }
            return 0;
        }, &base);
        if (!base) usleep(100000);
    }

    // Безопасное ожидание полной инициализации rakClient и vtable
    uintptr_t rakClient = 0;
    uintptr_t vtable = 0;

    while (true) {
        rakClient = *(uintptr_t*)(base + 0x12BFD0);
        if (rakClient != 0) {
            vtable = *(uintptr_t*)rakClient;
            if (vtable != 0) break;
        }
        usleep(100000);
    }

    // 1. Установка хука подключения
    void* targetConnect = (void*)*(uintptr_t*)(vtable + 16);
    if (targetConnect) {
        DobbyHook(targetConnect, (void*)hook_Connect, (void**)&orig_Connect);
    }

    // 2. Установка хука номера (Фикс краша при спавне авто)
    uintptr_t setPlateAddr = base + 0x000c8f1a;
    DobbyHook((void*)setPlateAddr, (void*)hook_SetPlateText, (void**)&orig_SetPlateText);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    std::thread(InitializeHooks).detach();
    return JNI_VERSION_1_6;
}