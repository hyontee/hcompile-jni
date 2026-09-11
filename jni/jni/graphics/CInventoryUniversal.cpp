//
// Created by admin on 07.09.2023.
//

#include <GLES2/gl2.h>
#include "CInventoryUniversal.h"
#include "../game/game.h"
#include "../util/CJavaWrapper.h"
#include "../game/snapshothelper.h"
#include "game/TextRasterizer/Color.h"
#include "../net/netgame.h"

extern CSnapShotHelper* pSnapShotHelper;
extern CNetGame* pNetGame;
extern CGame* pGame;

bool CInventoryUniversal::bShow = false;

int iBiasUniversal = -1;

void CInventoryUniversal::show(int playerslotcount, int universalslotcount, int biasUniversal) {
    CInventoryUniversal::bShow = true;
    iBiasUniversal = biasUniversal;
    int count_page_player;
    if(playerslotcount%20 == 0)
        count_page_player = playerslotcount/20;
    else
        count_page_player = playerslotcount/20+1;

    int count_page_universal;
    if(universalslotcount%20 == 0)
        count_page_universal = (universalslotcount-iBiasUniversal)/20;
    else
        count_page_universal = (universalslotcount-iBiasUniversal)/20+1;
    g_pJavaWrapper->ShowInventoryUniversal(count_page_player, playerslotcount,
                                           count_page_universal, universalslotcount);
    pGame->FindPlayerPed()->TogglePlayerControllable(false);
}

void CInventoryUniversal::hide() {
    CInventoryUniversal::bShow = false;
    g_pJavaWrapper->HideInventoryUniversal();
    pGame->FindPlayerPed()->TogglePlayerControllable(true);
}

void CInventoryUniversal::setInfo(int money, int donate, char* inventory1, char* inventory2)
{
    g_pJavaWrapper->SetUniversalInventoryInfo(money, donate, inventory1, inventory2);
}

void CInventoryUniversal::setNullSlotRight(int id) {

    Log("setNullSlotRight %d", id);
    if(isShow())
        g_pJavaWrapper->SetNullSlotRight(id);
}

void CInventoryUniversal::setNullSlotLeft(int id) {

    Log("setNullSlotLeft %d", id);
    if(isShow())
        g_pJavaWrapper->SetNullSlotLeft(id-iBiasUniversal);
}

void CInventoryUniversal::updateSlotsRightInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info) {
    Log("updateSlotsRightInfo %d,%d,%d,%d", id, type, amount, objectId);
    if (isShow()) {
        g_pJavaWrapper->UpdateSlotsRightInfo(id, type, amount, objectId, vecRotation.x,
                                                 vecRotation.y, vecRotation.z, zoom, caption, info);
    }
}

void CInventoryUniversal::updateSlotsLeftInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info, int price) {
    Log("updateSlotsLeftInfo %d,%d,%d,%d", id, type, amount, objectId);
    if (isShow()) {
        if(count == 0) {
            if (id != -1)
                g_pJavaWrapper->UpdateSlotsLeftInfo(count, id - iBiasUniversal, type, amount, objectId,
                                                    vecRotation.x, vecRotation.y, vecRotation.z,
                                                    zoom,
                                                    caption, info, 0);
            else {
                g_pJavaWrapper->UpdateSlotsLeftInfo(count, id, type, amount, objectId,
                                                    vecRotation.x, vecRotation.y, vecRotation.z,
                                                    zoom,
                                                    caption, info, 0);
            }
        }
        else if(count == 1)
        {
            g_pJavaWrapper->UpdateSlotsLeftInfo(count, id, type, amount, objectId,
                                                vecRotation.x, vecRotation.y, vecRotation.z,
                                                zoom,
                                                caption, info, price);
        }
    }
}

void CInventoryUniversal::clearRightInventory()
{
    Log("clearRightInventory");
    if(isShow())
        g_pJavaWrapper->ClearInventoryRight();
}

void CInventoryUniversal::clearLeftInventory()
{
    Log("clearLeftInventory");
    if(isShow())
        g_pJavaWrapper->ClearInventoryLeft();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stage_core_ui_inventory_InventoryUniversal_hideInventory(JNIEnv *env, jobject thiz) {
    CInventoryUniversal::hide();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_stage_core_ui_inventory_InventoryUniversal_sendInfoButtonActions(JNIEnv *env, jobject thiz,
                                                                         jint leftOrRight, jint action, jint slot_id,
                                                                         jint buttonId) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) 251);
    bsSend.Write((uint32_t) 0x94);
    bsSend.Write((uint16_t) action);
    if(leftOrRight == 1)
        bsSend.Write((uint16_t) slot_id);
    else if(leftOrRight == 2)
        bsSend.Write((uint16_t)(slot_id+iBiasUniversal));
    bsSend.Write((uint16_t)0);
    bsSend.Write((uint16_t)buttonId);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stage_core_ui_inventory_InventoryUniversal_sendInfoButtonActionsMarket(JNIEnv *env, jobject thiz,
                                                                         jint action, jint slot_id) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) 251);
    bsSend.Write((uint32_t) 0x94);
    bsSend.Write((uint16_t) action);
    bsSend.Write((uint16_t) slot_id);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}