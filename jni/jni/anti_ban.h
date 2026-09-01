#ifndef ANTI_BAN_H
#define ANTI_BAN_H

#include "memory.h"
#include <string>
#include <fstream>

class AntiBan {
private:
    // Хуки на проверки
    static uintptr_t hookCheckIntegrity(uintptr_t original) {
        // Возвращаем всегда 0 (успех)
        return 0;
    }

    static uintptr_t hookPacketSniff(uintptr_t original) {
        // Меняем контрольные суммы в пакетах
        uint32_t* checksum = (uint32_t*)original;
        *checksum = 0xDEADBEEF; // фиктивная сумма
        return original;
    }

public:
    static void patchAntiCheat() {
        // Отключаем флаг античита
        Memory::write<byte>(Memory::getBase() + OFF_ANTICHEAT_FLAG, 0);
        
        // Патчим проверку целостности (инлайн-хук)
        uintptr_t integrityCheck = Memory::getBase() + 0x1C3F00; // смещение функции
        unsigned char hook[] = { 
            0x00, 0x00, 0x80, 0xD2, // MOV X0, #0
            0xC0, 0x03, 0x5F, 0xD6  // RET
        };
        Memory::writeBytes(integrityCheck, hook, sizeof(hook));

        // Патчим проверку пакетов
        uintptr_t packetCheck = Memory::getBase() + 0x1C5000;
        unsigned char packetHook[] = {
            0x20, 0x00, 0x80, 0xD2, // MOV X0, #1 (всегда успешно)
            0xC0, 0x03, 0x5F, 0xD6
        };
        Memory::writeBytes(packetCheck, packetHook, sizeof(packetHook));

        // Скрываем открытые файлы /proc/self/maps
        std::ofstream("/proc/self/fd", std::ios::app);
        // Удаляем себя из списка процессов (для рута)
        if (getuid() == 0) {
            system("mount -o bind /dev/null /proc/self/maps");
        }
    }

    // Изменяем Device ID и Android ID
    static void spoofIDs() {
        // Прямая запись в проперти (требует root)
        system("setprop ro.product.device 'Samsung SM-G998B'");
        system("setprop ro.build.fingerprint 'samsung/beyond2/beyond2:12/SP1A.210812.016/G998BXXU1AVBF:user/release-keys'");
        system("setprop persist.sys.android_id 'DEADBEEFCAFEBABE'");
    }

    // Чистим логи
    static void clearLogs() {
        system("logcat -c");
        system("rm -rf /data/local/tmp/*");
        system("rm -rf /sdcard/Android/data/com.standoff2/files/*.log");
    }
};

#endif