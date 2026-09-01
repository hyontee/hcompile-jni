#include <jni.h>
#include <thread>
#include <chrono>
#include "offsets.h"
#include "memory.h"
#include "aimbot.h"
#include "esp.h"
#include "misc.h"
#include "anti_ban.h"

// Глобальные объекты
Aimbot* aimbot = nullptr;
ESP* esp = nullptr;
bool running = true;

// Основной поток чита
void cheatLoop() {
    Memory::init();
    AntiBan::patchAntiCheat();
    AntiBan::spoofIDs();
    AntiBan::clearLogs();

    uintptr_t base = Memory::getBase();
    uintptr_t localPlayer = Memory::read<uintptr_t>(base + OFFSET_LOCAL_PLAYER);
    
    aimbot = new Aimbot();
    aimbot->setLocal(localPlayer);
    esp = new ESP();

    // Бесконечный цикл с частотой 60 FPS
    while (running) {
        // Обновляем localPlayer (на случай перезагрузки)
        localPlayer = Memory::read<uintptr_t>(base + OFFSET_LOCAL_PLAYER);
        aimbot->setLocal(localPlayer);

        // ESP и валлхак
        esp->forceVisibility();
        
        // Aimbot (активируем по тапу/кнопке)
        if (isAimKeyPressed()) {  // функция из misc.h
            aimbot->run();
        }

        // NoRecoil + Spread
        Memory::write<float>(localPlayer + OFF_RECOIL_X, 0.0f);
        Memory::write<float>(localPlayer + OFF_RECOIL_Y, 0.0f);
        Memory::write<float>(localPlayer + OFF_SPREAD, 0.001f);

        // Скорострельность (ускоряем в 2x)
        float fireRate = Memory::read<float>(localPlayer + OFF_FIRE_RATE);
        if (fireRate > 0.05f) {
            Memory::write<float>(localPlayer + OFF_FIRE_RATE, fireRate * 0.5f);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

// JNI-хук для инжекта
extern "C" JNIEXPORT void JNICALL
Java_com_standoff2_NativeBridge_startCheat(JNIEnv* env, jobject thiz) {
    std::thread cheatThread(cheatLoop);
    cheatThread.detach();
}

// Точка входа для инжекта через .so
extern "C" void __attribute__((constructor)) init() {
    // Создаём фоновый поток при загрузке библиотеки
    std::thread([]() {
        // Ждём полной загрузки игры
        std::this_thread::sleep_for(std::chrono::seconds(5));
        cheatLoop();
    }).detach();
}