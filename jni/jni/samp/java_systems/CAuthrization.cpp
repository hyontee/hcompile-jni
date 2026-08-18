//
// Created by Error on 21.05.2025.
//

#include "CAuthrization.h"
#include "main.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"

jobject CAuthrization::thiz = nullptr;
jclass  CAuthrization::clazz = nullptr;
bool    CAuthrization::bIsShow = false;

void CAuthrization::hide() {
    if (CAuthrization::thiz != nullptr) {
        JNIEnv* env = g_pJavaWrapper->GetEnv();
        if (!env || !CAuthrization::clazz) return;

        jmethodID method = env->GetMethodID(CAuthrization::clazz, "destroy", "()V");
        if (method) {
            env->CallVoidMethod(CAuthrization::thiz, method);
        } else {
            Log("CAuthrization::hide -> destroy() method not found!");
        }

        env->DeleteGlobalRef(CAuthrization::thiz);
        CAuthrization::thiz = nullptr;
        CAuthrization::bIsShow = false;
    }
}

/**
 * Показывает GUI авторизации
 * @param isEmail - есть ли email-режим
 * @param isAutoAuth - включена ли автоавторизация
 * @param skinId - id скина (передаётся из сервера)
 */
void CAuthrization::show(bool isEmail, bool isAutoAuth, int skinId) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if (!env) return;

    // Убедимся, что класс загружен
    if (!CAuthrization::clazz) {
        jclass localClazz = env->FindClass("com/criminal/moscow/gui/auth/Autorization");
        if (!localClazz) {
            Log("CAuthrization::show -> class Autorization not found!");
            return;
        }
        CAuthrization::clazz = (jclass)env->NewGlobalRef(localClazz);
        env->DeleteLocalRef(localClazz);
    }

    if (CAuthrization::thiz == nullptr) {
        Log("Creating Autorization GUI (email=%d, auto=%d, skin=%d)", isEmail, isAutoAuth, skinId);

        jmethodID constructor = env->GetMethodID(CAuthrization::clazz, "<init>", "(ZZI)V");
        if (!constructor) {
            Log("JNI ERROR: Constructor (ZZI)V not found in Autorization!");
            return;
        }

        jobject localObj = env->NewObject(CAuthrization::clazz, constructor,
                                          (jboolean)isEmail, (jboolean)isAutoAuth, (jint)skinId);
        if (!localObj) {
            Log("JNI ERROR: Failed to instantiate Autorization object!");
            return;
        }

        CAuthrization::thiz = env->NewGlobalRef(localObj);
        env->DeleteLocalRef(localObj);
        CAuthrization::bIsShow = true;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Autorization_ToggleAutoLogin(JNIEnv* env, jobject thiz, jboolean toggle) {
    if (!pNetGame) return;

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)PACKET_AUTH);
    bsSend.Write((uint8_t)6);
    bsSend.Write((uint8_t)toggle);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);

    CSettings::m_Settings.szAutoLogin = toggle;
    CSettings::save();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Autorization_onLoginClick(JNIEnv* env, jobject thiz, jstring pass) {
    if (!pNetGame) return;

    const char* inputPassword = env->GetStringUTFChars(pass, nullptr);
    pNetGame->SendLoginPacket(inputPassword);
    env->ReleaseStringUTFChars(pass, inputPassword);
}
