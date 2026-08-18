//////////////////////////////////////
/// Created by Cross on 12.09.2025////
//////////////////////////////////////

#pragma once
#include <jni.h>

class CTabletMusic {
public:
    // Инициализация (кеширование класса и методов)
    static void Init(JNIEnv* env = nullptr);

    // Обычные действия UI
    static void Show();
    static void Hide();
    static void Toggle();

    // Принудительное открытие из RPC (без проверок режима)
    static void ForceShowFromRpc();

    // Локальный флаг показа (нативный)
    static bool IsShown();

private:
    static JNIEnv* Jni();
    static bool JavaIsVisible();

    // Кешированные JNI-ссылки/состояние
    static jobject   thiz;
    static jclass    clazz;
    static bool      bIsShow;

    // Кешированные ID методов Java
    static jmethodID mCtor;
    static jmethodID mShow;
    static jmethodID mHide;
    static jmethodID mIsVisibleStatic;
};

//////////////////////////////////////
/// Created by Cross on 12.09.2025////
//////////////////////////////////////
