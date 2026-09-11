//
// Created by admin on 07.09.2023.
//

#include <GLES2/gl2.h>
#include "CInventory.h"
#include "../game/game.h"
#include "../util/CJavaWrapper.h"
#include "../game/snapshothelper.h"
#include "game/TextRasterizer/Color.h"
#include "../net/netgame.h"

extern CSnapShotHelper* pSnapShotHelper;
extern CNetGame* pNetGame;
extern CGame* pGame;

bool CInventory::bShow = false;

void CInventory::show(int slotcount) {
    CInventory::bShow = true;
    int count_page;
    if(slotcount%20 == 0)
        count_page = slotcount/20;
    else
        count_page = slotcount/20+1;
    g_pJavaWrapper->ShowInventory(count_page, slotcount);
    pGame->FindPlayerPed()->TogglePlayerControllable(false);
}

void CInventory::hide() {
    CInventory::bShow = false;
    g_pJavaWrapper->HideInventory();
    pGame->FindPlayerPed()->TogglePlayerControllable(true);
}

void CInventory::setInfo(int health, int thirst, int hunger, int money, int donate)
{
    if(isShow())
        g_pJavaWrapper->SetInventoryInfo(health, thirst, hunger, money, donate);
}

void CInventory::setSkin(int skin) {
    if(isShow())
        g_pJavaWrapper->SetInventorySkin(skin, 0.0f, 180.0f, -20.0f, 1.0f);
}

void CInventory::setNullSlot(int id) {

    if(isShow())
        g_pJavaWrapper->SetNullSlot(id);
}

void CInventory::updateSlotsInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info) {

    if(isShow())
    {
        Log("updateSlotsInfo cinventory caption %s info %s", caption, info);
        g_pJavaWrapper->UpdateSlotsInfo(id, type, amount, objectId, vecRotation.x, vecRotation.y, vecRotation.z, zoom, caption,
                                        info);
    }
}

void CInventory::updateAcsSlotsInfo(int acc_slot, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info) {

    Log("updateAcsSlotsInfo %d %d %s %s", acc_slot, objectId, caption, info);
    if(isShow() && (acc_slot >= -1 && acc_slot <= 9))
        g_pJavaWrapper->UpdateSlotsAcsInfo(acc_slot,objectId, vecRotation.x,vecRotation.y,vecRotation.z, zoom, caption, info);
}

void CInventory::clearInventory()
{
    if(isShow())
        g_pJavaWrapper->ClearInventory();
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_stage_core_ui_inventory_Inventory_hideInventory(JNIEnv *env, jobject thiz) {
    CInventory::hide();
}

JNIEXPORT void JNICALL
Java_com_stage_core_ui_inventory_Inventory_sendInfoButtonActions(JNIEnv *env, jobject thiz,
                                                                jint action, jint slot_id,
                                                                jint second_slot_id) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) 251);
    bsSend.Write((uint32_t) 0x94);
    bsSend.Write((uint16_t) action);
    bsSend.Write((uint16_t) slot_id);
    bsSend.Write((uint16_t) second_slot_id);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}
}