//
// Created by admin on 26.10.2023.
//

#include "CTuning.h"
#include "../util/CJavaWrapper.h"
#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"

extern CNetGame *pNetGame;
extern CGame* pGame;

bool CTuning::bShow = false;

void CTuning::show() {
    CTuning::bShow = true;
    g_pJavaWrapper->ShowTuning();
}

void CTuning::hide() {
    CTuning::bShow = false;
    g_pJavaWrapper->HideTuning();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stage_core_ui_tune_Tune_sendInfoButtonActions(JNIEnv *env, jobject thiz, jint action, jint r,jint g, jint b, jint a) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) 251);
    bsSend.Write((uint32_t) 0x97);
    bsSend.Write((uint16_t) action);
    bsSend.Write((uint32_t) r);
    bsSend.Write((uint32_t) g);
    bsSend.Write((uint32_t) b);
    bsSend.Write((uint32_t) a);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_stage_core_ui_tune_Tune_hideTuning(JNIEnv *env, jobject thiz) {
    CTuning::hide();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_sendTuneUpdateToClient(JNIEnv *env, jobject thiz,
                                                                    jint id, jint r, jint g,
                                                                    jint b, jint a) {
    CPlayerPed* pPed = pGame->FindPlayerPed();
    if(!pPed->IsInVehicle()) return;

    CVehicle* pVehicle = pPed->GetCurrentVehicle();
    if(!pVehicle) return;

    switch(id)
    {
        case 0:
        {
            pVehicle->mVehicleColor.Set(r, g, b, 255);
            break;
        }
        case 1:
        {
            // toner
            break;
        }
        case 2:
        {
            pVehicle->SetHeadlightsColor(r,g,b);
            break;
        }
        case 3:
        {
            pVehicle->SetCustomShadow(r,g,b,0.7f,0.9f, "neonstyn");
            break;
        }
    }
}