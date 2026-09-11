//
// Created by admin on 07.09.2023.
//

#pragma once

#include "../main.h"

class CInventoryTrade {
public:
    static void show(int slotcount);
    static void hide();
    static void updateRightSlotsInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info, char* amountText);
    static void updateLeftSlotsInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info, char* amountText);
    static void clearRightInventory();
    static void clearLeftInventory();
    static void setRightNullSlot(int id);
    static void setMaxPages(int page);
    static void setPlayerName(int player, char* name);
    static void setPlayerReadiness(int player, int status);
    static void setMoney(int money1, int money2, char* inventoryName);

    static bool isShow() {
        return bShow;
    }

    static bool bShow;
};