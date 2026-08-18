//
// Created by plaka on 20.02.2023.
//

#include "CChooseSpawn.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"

jobject CChooseSpawn::thiz = nullptr;
jclass CChooseSpawn::clazz = nullptr;

void CChooseSpawn::show(int organization, int home, int family, int pos)
{
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(CChooseSpawn::thiz == nullptr) {

        jmethodID constructor = env->GetMethodID(CChooseSpawn::clazz, "<init>",
                                                 "(IIII)V");
        CChooseSpawn::thiz = env->NewObject(CChooseSpawn::clazz, constructor,
                                       organization, home, family, pos);
        CChooseSpawn::thiz = env->NewGlobalRef(CChooseSpawn::thiz);
    }
}

void CChooseSpawn::hide()
{
    if(thiz != nullptr)
    {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        jmethodID method = env->GetMethodID(clazz, "destroy", "()V");
        env->CallVoidMethod(thiz, method);

        env->DeleteGlobalRef(thiz);
        thiz = nullptr;
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_SpawnSelect_SendSpawn(JNIEnv *env, jobject thiz, jint spawnid) {
    pNetGame->SendCustomPacket(251, 60, spawnid);
}

