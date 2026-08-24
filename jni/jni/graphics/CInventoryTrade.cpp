//
// Created by admin on 07.09.2023.
//

#include <GLES2/gl2.h>
#include "CInventoryTrade.h"
#include "../game/game.h"
#include "../util/CJavaWrapper.h"
#include "../game/snapshothelper.h"
#include "game/TextRasterizer/Color.h"
#include "../net/netgame.h"

extern CSnapShotHelper* pSnapShotHelper;
extern CNetGame* pNetGame;
extern CGame* pGame;

bool CInventoryTrade::bShow = false;

void CInventoryTrade::show(int slotcount) {
    CInventoryTrade::bShow = true;
    int count_page;
    if(slotcount%20 == 0)
        count_page = slotcount/20;
    else
        count_page = slotcount/20+1;
    g_pJavaWrapper->ShowTradeInventory(count_page, slotcount);
    pGame->FindPlayerPed()->TogglePlayerControllable(false);
}

void CInventoryTrade::hide() {
    CInventoryTrade::bShow = false;
    g_pJavaWrapper->HideTradeInventory();
    pGame->FindPlayerPed()->TogglePlayerControllable(true);
}

void CInventoryTrade::setRightNullSlot(int id) {

    if(isShow())
        g_pJavaWrapper->SetTradeNullRightSlot(id);
}

void CInventoryTrade::updateRightSlotsInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info, char* amountText) {

    if(isShow())
    {
        g_pJavaWrapper->UpdateTradeRightSlotsInfo(id, type, amount, objectId, vecRotation.x, vecRotation.y, vecRotation.z, zoom, caption,
                                        info, amountText);
    }
}

void CInventoryTrade::updateLeftSlotsInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info, char* amountText) {

    if(isShow())
    {
        g_pJavaWrapper->UpdateTradeLeftSlotsInfo(id, type, amount, objectId, vecRotation.x, vecRotation.y, vecRotation.z, zoom, caption,
                                                  info, amountText);
    }
}

void CInventoryTrade::clearRightInventory()
{
    if(isShow())
        g_pJavaWrapper->ClearTradeRightInventory();
}

void CInventoryTrade::clearLeftInventory()
{
    if(isShow())
        g_pJavaWrapper->ClearTradeLeftInventory();
}

void CInventoryTrade::setMaxPages(int page) {

    if(isShow())
    {
        int count_page;
        if(page%20 == 0)
            count_page = page/20;
        else
            count_page = page/20+1;
        g_pJavaWrapper->SetTradeMaxPages(count_page);
    }
}

void CInventoryTrade::setPlayerName(int player, char* name)
{
    if(isShow())
        g_pJavaWrapper->SetTradePlayerName(player, name);
}

void CInventoryTrade::setPlayerReadiness(int player, int status)
{
    if(isShow())
        g_pJavaWrapper->SetTradePlayerReadiness(player, status);
}

void CInventoryTrade::setMoney(int money1, int money2, char* inventoryName)
{
    if(isShow())
        g_pJavaWrapper->SetTradeMoney(money1, money2, inventoryName);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stage_core_ui_inventory_InventoryTrade_hideInventoryTrade(JNIEnv *env, jobject thiz) {
    CInventoryTrade::hide();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_stage_core_ui_inventory_InventoryTrade_sendInfoButtonActions(JNIEnv *env, jobject thiz,
                                                                     jint actionid, jint key, jint button) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) 251);
    bsSend.Write((uint32_t) 0x100);
    bsSend.Write((uint16_t) actionid);
    bsSend.Write((uint32_t) key);
    bsSend.Write((uint32_t) button);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}