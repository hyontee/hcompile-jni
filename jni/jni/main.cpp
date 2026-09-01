#include <jni.h>
#include <thread>
#include <chrono>
#include "memory.h"
#include "aimbot.h"
#include "esp.h"

static Aimbot* aimbot = nullptr;
static ESP* esp = nullptr;
static bool running = true;

void cheatLoop() {
    Memory::init();
    LOGI("[ROCKET] Чит активирован");

    while (running) {
        uintptr_t local = Memory::read<uintptr_t>(Memory::getBase() + OFFSET_LOCAL_PLAYER);

        if (local) {
            if (aimbot) {
                aimbot->setLocal(local);
                aimbot->run();
            }

            if (esp) {
                esp->forceVisible();
                esp->noRecoil();
                esp->speedFire();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_nesqwik_NativeBridge_startCheat(JNIEnv* env, jobject thiz) {
    aimbot = new Aimbot();
    esp = new ESP();
    std::thread(cheatLoop).detach();
    LOGI("[ROCKET] Запущен через JNI");
}

extern "C" void __attribute__((constructor)) init() {
    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        aimbot = new Aimbot();
        esp = new ESP();
        cheatLoop();
    }).detach();
}