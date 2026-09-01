#ifndef MISC_H
#define MISC_H

#include <android/log.h>
#include <fstream>

#define LOG_TAG "ROCKET_CHEAT"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Проверка нажатия кнопки (через виртуальный джойстик или тач)
bool isAimKeyPressed() {
    // Читаем статус из shared memory или файла
    std::ifstream file("/sdcard/aim_toggle.txt");
    char c;
    file >> c;
    return c == '1';
}

// Быстрый скриншот (для OBS или стрима)
void screenshot() {
    system("screencap -p /sdcard/screenshot.png");
}

// Трансляция координат в чат (для смеха)
void sendCoordsToChat(float x, float y, float z) {
    // Патчим функцию отправки сообщения (0x1D8000)
    uintptr_t sendFunc = Memory::getBase() + 0x1D8000;
    char msg[256];
    sprintf(msg, "[%f, %f, %f]", x, y, z);
    // Вызываем напрямую
    void (*send)(const char*) = (void(*)(const char*))sendFunc;
    send(msg);
}

#endif