//////////////////////////////////////
/// Created by Cross on 12.09.2025////
//////////////////////////////////////

#include "CTabletMusic.h"
#include "util/CJavaWrapper.h"
#include "main.h"

#ifndef LOGI
#include <android/log.h>
#define LOG_TAG "Tablet"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

// ---------- Статические поля ----------
jobject   CTabletMusic::thiz   = nullptr;
jclass    CTabletMusic::clazz  = nullptr;
bool      CTabletMusic::bIsShow = false;

jmethodID CTabletMusic::mCtor            = nullptr;
jmethodID CTabletMusic::mShow            = nullptr;
jmethodID CTabletMusic::mHide            = nullptr;
jmethodID CTabletMusic::mIsVisibleStatic = nullptr;

// ---------- Вспомогалки ----------
static inline void ClearPendingExc(JNIEnv* env, const char* where) {
    if (env && env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOGW("JNI exception cleared at %s", where);
    }
}

JNIEnv* CTabletMusic::Jni() {
    return g_pJavaWrapper ? g_pJavaWrapper->GetEnv() : nullptr;
}

// ---------- Инициализация ----------
void CTabletMusic::Init(JNIEnv* env) {
    if (CTabletMusic::clazz) return;
    if (!env) env = Jni();
    if (!env) { LOGE("Init: env == null"); return; }

    jclass local = env->FindClass("com/criminal/moscow/gui/tablet/GameTabletMusic");
    if (!local) {
        ClearPendingExc(env, "FindClass(GameTabletMusic)");
        LOGE("Init: class GameTabletMusic not found");
        return;
    }
    CTabletMusic::clazz = (jclass)env->NewGlobalRef(local);
    env->DeleteLocalRef(local);

    CTabletMusic::mCtor  = env->GetMethodID(CTabletMusic::clazz, "<init>", "()V");
    CTabletMusic::mShow  = env->GetMethodID(CTabletMusic::clazz, "show",  "()V");
    CTabletMusic::mHide  = env->GetMethodID(CTabletMusic::clazz, "hide",  "()V");
    CTabletMusic::mIsVisibleStatic =
            env->GetStaticMethodID(CTabletMusic::clazz, "isVisible", "()Z");

    ClearPendingExc(env, "GetMethodIDs");
    LOGI("Init: cached ctor=%p show=%p hide=%p isVisible=%p",
         mCtor, mShow, mHide, mIsVisibleStatic);
}

// ---------- Запрос состояния из Java (если есть) ----------
bool CTabletMusic::JavaIsVisible() {
    JNIEnv* env = Jni();
    if (!env) return false;
    if (!CTabletMusic::clazz || !CTabletMusic::mIsVisibleStatic) CTabletMusic::Init(env);
    if (!CTabletMusic::clazz || !CTabletMusic::mIsVisibleStatic) return false;

    jboolean vis = env->CallStaticBooleanMethod(CTabletMusic::clazz, CTabletMusic::mIsVisibleStatic);
    ClearPendingExc(env, "CallStaticBooleanMethod(isVisible)");
    return vis == JNI_TRUE;
}

// ---------- Показ ----------
void CTabletMusic::Show() {
    JNIEnv* env = Jni();
    if (!env) { LOGE("Show: env == null"); return; }
    if (!CTabletMusic::clazz) Init(env);
    if (!CTabletMusic::clazz) { LOGE("Show: clazz == null"); return; }

    if (!CTabletMusic::thiz) {
        if (!CTabletMusic::mCtor) { LOGE("Show: mCtor == null"); return; }
        jobject obj = env->NewObject(CTabletMusic::clazz, CTabletMusic::mCtor);
        if (!obj) { ClearPendingExc(env, "NewObject(GameTabletMusic)"); LOGE("Show: NewObject failed"); return; }
        CTabletMusic::thiz = env->NewGlobalRef(obj);
        env->DeleteLocalRef(obj);
    }

    if (CTabletMusic::mShow) {
        LOGI("Show: calling GameTabletMusic.show()");
        env->CallVoidMethod(CTabletMusic::thiz, CTabletMusic::mShow);
        ClearPendingExc(env, "CallVoidMethod(show)");
        CTabletMusic::bIsShow = true;
    } else {
        LOGE("Show: mShow == null");
    }
}

// ---------- Принудительное открытие из RPC (без проверок режима) ----------
void CTabletMusic::ForceShowFromRpc() {
    // ВАЖНО: никаких проверок «режима бумбокса» — всегда открываем UI.
    // Используется в обработчике PACKET_CUSTOMRPC для sub-id (например) 100.
    JNIEnv* env = Jni();
    if (!env) { LOGE("ForceShowFromRpc: env == null"); return; }
    if (!CTabletMusic::clazz) Init(env);
    if (!CTabletMusic::clazz) { LOGE("ForceShowFromRpc: clazz == null"); return; }

    if (!CTabletMusic::thiz) {
        if (!CTabletMusic::mCtor) { LOGE("ForceShowFromRpc: mCtor == null"); return; }
        jobject obj = env->NewObject(CTabletMusic::clazz, CTabletMusic::mCtor);
        if (!obj) { ClearPendingExc(env, "NewObject(GameTabletMusic)"); LOGE("ForceShowFromRpc: NewObject failed"); return; }
        CTabletMusic::thiz = env->NewGlobalRef(obj);
        env->DeleteLocalRef(obj);
    }

    if (CTabletMusic::mShow) {
        LOGI("ForceShowFromRpc: calling GameTabletMusic.show()");
        env->CallVoidMethod(CTabletMusic::thiz, CTabletMusic::mShow);
        ClearPendingExc(env, "CallVoidMethod(show)");
        CTabletMusic::bIsShow = true;
    } else {
        LOGE("ForceShowFromRpc: mShow == null");
    }
}

// ---------- Скрытие ----------
void CTabletMusic::Hide() {
    JNIEnv* env = Jni();
    if (!env) { LOGE("Hide: env == null"); CTabletMusic::bIsShow = false; return; }
    if (!CTabletMusic::clazz) Init(env);
    if (!CTabletMusic::clazz) { LOGE("Hide: clazz == null"); CTabletMusic::bIsShow = false; return; }

    // Если объект не создан (например, Java показывал через статик), создадим его
    if (!CTabletMusic::thiz && CTabletMusic::mCtor) {
        jobject obj = env->NewObject(CTabletMusic::clazz, CTabletMusic::mCtor);
        if (obj) {
            CTabletMusic::thiz = env->NewGlobalRef(obj);
            env->DeleteLocalRef(obj);
        }
        ClearPendingExc(env, "NewObject(GameTabletMusic) for hide");
    }

    if (CTabletMusic::thiz && CTabletMusic::mHide) {
        LOGI("Hide: calling GameTabletMusic.hide()");
        env->CallVoidMethod(CTabletMusic::thiz, CTabletMusic::mHide);
        ClearPendingExc(env, "CallVoidMethod(hide)");
    }

    if (CTabletMusic::thiz) {
        env->DeleteGlobalRef(CTabletMusic::thiz);
        CTabletMusic::thiz = nullptr;
    }
    CTabletMusic::bIsShow = false;
}

// ---------- Тоггл ----------
void CTabletMusic::Toggle() {
    const bool javaVisible = CTabletMusic::JavaIsVisible();
    LOGI("Toggle: javaVisible=%d, native(bIsShow)=%d", (int)javaVisible, (int)bIsShow);
    if (javaVisible) Hide(); else Show();
}

// ---------- Локальный флаг ----------
bool CTabletMusic::IsShown() {
    // Возвращаем локальное знание; для точной правды можно сверить с JavaIsVisible()
    return CTabletMusic::bIsShow;
}

//////////////////////////////////////
/// Created by Cross on 12.09.2025////
//////////////////////////////////////
