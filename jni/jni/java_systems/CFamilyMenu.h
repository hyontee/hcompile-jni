//
// Created by August on 26.10.2025.
//

#pragma once
#ifndef GTECH_BY_CROSS_CFAMILYMENU_H
#define GTECH_BY_CROSS_CFAMILYMENU_H

#include <jni.h>
#include <string>

/**
 * CFamilyMenu — JNI-обёртка для Java-класса com.criminal.moscow.gui.family.FamilyMenu.
 *
 * Используется для управления экраном семьи (показ, скрытие, обновление данных)
 * напрямую из нативного кода (C++).
 */
class CFamilyMenu {
public:
    // === Управление ===
    static void show(const char* name, int balance, int reputation);             // Упрощённый показ без скина
    static void show(const char* name, int balance, int reputation, int skinId); // Показ меню семьи с поддержкой skinId
    static void hide();                                                          // Скрыть меню (с анимацией)
    static void destroy();                                                       // Очистить JNI-ссылки

    // === JNI ссылки ===
    static jobject j_family_menu;   // Глобальная ссылка на Java-объект FamilyMenu
    static jclass  j_class;         // Глобальная ссылка на Java-класс FamilyMenu

    // === Статус ===
    static bool bIsShow;            // true — меню открыто

    // === Последние данные семьи (для повторного вызова) ===
    static std::string sFamilyName; // Название семьи
    static int iFamilyBalance;      // Баланс семьи
    static int iFamilyReputation;   // Репутация семьи
    static int iFamilySkin;         // Skin ID игрока (для передачи в меню взаимодействия)
};

// ===== JNI функции =====

// 🔹 Вызывается из Java при инициализации FamilyMenu
// Привязывает активный экземпляр Java-класса к C++ слою
extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_family_FamilyMenu_init(JNIEnv* env, jobject /*unused*/, jobject instance);

// 🔹 Вызывается из Java при нажатии кнопки "Выйти"
// Отправляет RPC закрытия меню на сервер
extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_family_FamilyMenu_nativeHide(JNIEnv* env, jobject thiz);

#endif // GTECH_BY_CROSS_CFAMILYMENU_H
