//
// Created by plaka on 11.02.2023.
//

#include "CSpeedometr.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CGameActionMenu.h"

jclass  CSpeedometr::clazz = nullptr;
jobject CSpeedometr::thiz = nullptr;
bool    CSpeedometr::bIsShow = false;
bool    CSpeedometr::bIsNoNeedDraw = false;
float   CSpeedometr::fFuel = 0.0f;
int     CSpeedometr::iMilliage = 0;

void CSpeedometr::show()
{
    if( CSpeedometr::bIsShow ) return;
    Log("CSpeedometr::show()");
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    // construct

    jmethodID constructor = env->GetMethodID(CSpeedometr::clazz, "<init>", "()V");
    CSpeedometr::thiz = env->NewObject(CSpeedometr::clazz, constructor, g_pJavaWrapper->activity);
    CSpeedometr::thiz = env->NewGlobalRef(CSpeedometr::thiz);

    CSpeedometr::bIsShow = true;
}

void CSpeedometr::tempToggle(bool toggle)
{
    if( !CSpeedometr::bIsShow ) return;
    Log("CSpeedometr::tempToggle(%d)", toggle);

    bIsNoNeedDraw = !toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetMethodID(CSpeedometr::clazz, "tempToggle", "(Z)V");
    env->CallVoidMethod(CSpeedometr::thiz, method, toggle);
}

void CSpeedometr::hide()
{
    if( !CSpeedometr::bIsShow ) return;
    Log("CSpeedometr::hide()");
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetMethodID(CSpeedometr::clazz, "destroy", "()V");
    env->CallVoidMethod(CSpeedometr::thiz, method);

    env->DeleteGlobalRef(CSpeedometr::thiz);
    CSpeedometr::thiz = nullptr;

    CSpeedometr::bIsShow = false;
}

void CSpeedometr::update()
{
    if( !CSpeedometr::bIsShow ) return;
    Log("CSpeedometr::update()");
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(!pPed->IsInVehicle()) return;

    CVehicle* pVehicle = pPed->GetCurrentVehicle();

    jmethodID method = env->GetMethodID(clazz, "updateSpeed", "(IIIIII)V");
    env->CallVoidMethod(
            CSpeedometr::thiz,
            method,
            (int) (pVehicle->m_pVehicle->pHandling->m_transmissionData.m_fCurrentSpeed * 175.0f),
            (int) pVehicle->GetHealth(),
            pVehicle->m_bEngineOn,
            (int) pVehicle->GetLightsState(),
            (int) pVehicle->m_bIsLocked,
            pVehicle->m_iTurnState
            );
}

void CSpeedometr::updateFuel() {
    if( !CSpeedometr::bIsShow ) return;
    Log("CSpeedometr::updateFuel()");
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(!pPed->IsInVehicle()) return;

    jmethodID method = env->GetMethodID(clazz, "updateFuel", "(I)V");
    env->CallVoidMethod(
            CSpeedometr::thiz,
            method,
            (int)CSpeedometr::fFuel
    );
}
void CSpeedometr::setMaxCarValues() {
    if( !CSpeedometr::bIsShow ) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    Log("CSpeedometr::setMaxCarValues()");
    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(!pPed->IsInVehicle()) return;

    jmethodID method = env->GetMethodID(clazz, "setMaxCarValues", "(II)V");
    env->CallVoidMethod(
            CSpeedometr::thiz,
            method,
            (int)CSpeedometr::fFuel
    );
}

void CSpeedometr::updateTurn(bool isTurn){
    if(CSpeedometr::thiz == nullptr) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(!pPed->IsInVehicle()) return;

    CVehicle* pVehicle = pPed->GetCurrentVehicle();

    jmethodID method = env->GetMethodID(clazz, "updateTurn", "(ZI)V");
    env->CallVoidMethod(
            CSpeedometr::thiz,
            method,
            isTurn,
            pVehicle->m_iTurnState
    );
}


extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_SpeedometerNew_sendClick(JNIEnv *env, jobject thiz, jint click_id) {
    switch(click_id)
    {
        case CSpeedometr::BUTTON_ENGINE:
        {
            pNetGame->SendChatCommand("/engine");
            break;
        }
        case CSpeedometr::BUTTON_LIGHT:
        {
            pNetGame->SendChatCommand("/lights");
            break;
        }
        case CSpeedometr::BUTTON_TURN_LEFT:
        {
            CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
            CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_LEFT) {
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
                CSpeedometr::updateTurn(false);
            }
            else {
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_LEFT;
            }

            CSpeedometr::update();

            break;
        }
        case CSpeedometr::BUTTON_TURN_RIGHT:
        {
            CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
            CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_RIGHT) {
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
                CSpeedometr::updateTurn(false);
            }
            else {
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_RIGHT;
            }
            CSpeedometr::update();
            break;
        }
        case CSpeedometr::BUTTON_TURN_ALL:
        {
            CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
            CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_ALL) {
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
                CSpeedometr::updateTurn(false);
            }
            else {
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_ALL;
            }
            CSpeedometr::update();
            break;
        }
        case 6: {
            CGameActionMenu::show();
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tunings_Colors_color_1change(JNIEnv *env, jobject thiz, jint type,
                                                        jint r, jint g, jint b) {
    if(!pNetGame)
        return;

    if(!pNetGame->GetPlayerPool())
        return;

    if(!pNetGame->GetPlayerPool()->GetLocalPlayer())
        return;

    CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
    CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

    if(pVehicle) {
        pVehicle->color1.R = r;
        pVehicle->color1.G = g;
        pVehicle->color1.B = b;
    }

}
extern "C"
JNIEXPORT jint JNICALL
Java_com_criminal_moscow_gui_SpeedometerNew_nativeGetMaxSpeed(JNIEnv *env, jobject thiz) {
    return 300;
}