#ifndef MEMORY_H
#define MEMORY_H

#include <jni.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <android/log.h>

#define LOG_TAG "ROCKET"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class Memory {
private:
    static uintptr_t base_address;

public:
    static void init() {
        FILE* maps = fopen("/proc/self/maps", "r");
        char line[512];
        while (fgets(line, sizeof(line), maps)) {
            if (strstr(line, "libil2cpp.so")) {
                sscanf(line, "%lx-%*lx", &base_address);
                break;
            }
        }
        fclose(maps);
        LOGI("[ROCKET] Base: 0x%lx", base_address);
    }

    static uintptr_t getBase() { return base_address; }

    template<typename T>
    static T read(uintptr_t address) {
        return *reinterpret_cast<T*>(address);
    }

    template<typename T>
    static void write(uintptr_t address, T value) {
        mprotect((void*)(address & ~0xFFF), 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
        *reinterpret_cast<T*>(address) = value;
    }
};

uintptr_t Memory::base_address = 0;

#endif