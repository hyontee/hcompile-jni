//
// Created by Error on 25.05.2025.
//

#include "CGameActionMenu.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"

jobject CGameActionMenu::thiz = nullptr;
jclass CGameActionMenu::clazz = nullptr;
bool CGameActionMenu::bIsShow = false;


void CGameActionMenu::hide() {
    if(CGameActionMenu::thiz != nullptr)
    {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        jmethodID method = env->GetMethodID(CGameActionMenu::clazz, "destroy", "()V");
        env->CallVoidMethod(CGameActionMenu::thiz, method);

        env->DeleteGlobalRef(CGameActionMenu::thiz);
        CGameActionMenu::thiz = nullptr;
        CGameActionMenu::bIsShow = false;
    }
}

void CGameActionMenu::show() {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(CGameActionMenu::thiz == nullptr) {

        jmethodID constructor = env->GetMethodID(CGameActionMenu::clazz, "<init>",
                                                 "()V");
        CGameActionMenu::thiz = env->NewObject(CGameActionMenu::clazz, constructor);
        CGameActionMenu::thiz = env->NewGlobalRef(CGameActionMenu::thiz);
        CGameActionMenu::bIsShow = true;

    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_GameActionMenu_onClickGameAction(JNIEnv *env, jobject thiz,
        jint btnid) {
// TODO: implement onClickGameAction()
    switch (btnid) {
        case 0: {
            CGameActionMenu::hide();
            break;
        }
        case 1:{
            pNetGame->SendChatCommand("/lock");
            break;
        }
        case 2:{
            pNetGame->SendChatCommand("/en");
            break;
        }
        case 3:{
            pNetGame->SendChatCommand("/park");
            break;
        }
        case 4:{
            pNetGame->SendChatCommand("/put");
            break;
        }
    }
}