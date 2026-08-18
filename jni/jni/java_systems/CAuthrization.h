//
// Created by Error on 21.05.2025.
//

#ifndef CRIMINAL_MOSCOW_CAUTHRIZATION_H
#define CRIMINAL_MOSCOW_CAUTHRIZATION_H

#include <jni.h>

/**
 * Класс CAuthrization отвечает за показ/скрытие GUI авторизации.
 * Вызывается со стороны клиента (C++) при получении RPC от сервера.
 *
 * Взаимодействует с Kotlin-классом:
 *   com.criminal.moscow.gui.auth.Autorization(Boolean, Boolean, Int)
 */
class CAuthrization {
private:
    // Указатель на экземпляр Java-объекта Autorization
    static jobject thiz;

public:
    // Флаг состояния GUI
    static bool bIsShow;

    // Класс Autorization (глобальная ссылка)
    static jclass clazz;

    /**
     * Скрыть GUI авторизации.
     * Вызывает метод destroy() на стороне Kotlin.
     */
    static void hide();

    /**
     * Показать GUI авторизации.
     * @param isEmail    — режим email-входа (true/false)
     * @param isAutoAuth — автоавторизация включена (true/false)
     * @param skinId     — ID скина игрока, отображаемого в окне
     */
    static void show(bool isEmail, bool isAutoAuth, int skinId);
};

#endif // CRIMINAL_MOSCOW_CAUTHRIZATION_H
