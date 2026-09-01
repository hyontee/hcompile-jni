#ifndef MEMORY_H
#define MEMORY_H

#include <jni.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

class Memory {
private:
    static int pid;
    static uintptr_t base_address;

public:
    static void init() {
        pid = getpid();
        // Получаем базовый адрес libil2cpp.so
        FILE* maps = fopen("/proc/self/maps", "r");
        char line[512];
        while (fgets(line, sizeof(line), maps)) {
            if (strstr(line, "libil2cpp.so")) {
                sscanf(line, "%lx-%*lx", &base_address);
                break;
            }
        }
        fclose(maps);
    }

    static uintptr_t getBase() { return base_address; }

    template<typename T>
    static T read(uintptr_t address) {
        return *reinterpret_cast<T*>(address);
    }

    template<typename T>
    static void write(uintptr_t address, T value) {
        // Делаем память доступной для записи
        mprotect((void*)(address & ~0xFFF), 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
        *reinterpret_cast<T*>(address) = value;
    }

    static void writeBytes(uintptr_t address, unsigned char* bytes, size_t len) {
        mprotect((void*)(address & ~0xFFF), 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
        memcpy((void*)address, bytes, len);
    }

    // Обход античита через /dev/mem (если доступен)
    static bool kernelWrite(uintptr_t phys_addr, void* data, size_t len) {
        int fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd < 0) return false;
        lseek(fd, phys_addr, SEEK_SET);
        write(fd, data, len);
        close(fd);
        return true;
    }
};

uintptr_t Memory::base_address = 0;
int Memory::pid = 0;

#endif