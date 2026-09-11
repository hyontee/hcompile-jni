//
// Created by admin on 25.09.2023.
//

#include <GLES2/gl2.h>
#include "CBuyAuto.h"
#include "../util/CJavaWrapper.h"
#include "../game/game.h"
#include "../net/netgame.h"

extern CNetGame *pNetGame;
extern CSnapShotHelper *pSnapShotHelper;
extern CGame *pGame;

bool CBuyAuto::bShow = false;

void CBuyAuto::show() {
    CBuyAuto::bShow = true;
    g_pJavaWrapper->ShowBuyAuto();
    pGame->FindPlayerPed()->TogglePlayerControllable(false);
}

void CBuyAuto::hide() {
    CBuyAuto::bShow = false;
    g_pJavaWrapper->HideBuyAuto();
    pGame->FindPlayerPed()->TogglePlayerControllable(true);
}

void CBuyAuto::addCarToRecycler(uint16_t vehicleModelId, int price, int maxSpeed, int maxFuel,
                                float timeTo100, int availabilityInStock, char* name) {
    g_pJavaWrapper->AddCarToRecycler(vehicleModelId, 20.0f,180.0f,45.0f,0.78f, price, maxSpeed,
                                     maxFuel, timeTo100, availabilityInStock, name);
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_stage_core_ui_buyauto_BuyAuto_hideBuyAuto(JNIEnv *env, jobject thiz) {
    CBuyAuto::hide();
}
JNIEXPORT void JNICALL
Java_com_stage_core_ui_buyauto_BuyAuto_sendAction(JNIEnv *env, jobject thiz, jint action,
jint value) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) 251);
    bsSend.Write((uint32_t) 0x96);
    bsSend.Write((uint8_t) action);
    bsSend.Write((uint32_t) value);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}
}