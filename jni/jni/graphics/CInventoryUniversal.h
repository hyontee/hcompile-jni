//
// Created by admin on 07.09.2023.
//

#pragma once

#include "../main.h"

class CInventoryUniversal {
public:
    static void show(int playerslotcount, int universalslotcount, int biasUniversal);
    static void hide();
    static void updateSlotsRightInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info);
    static void updateSlotsLeftInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info, int price);
    static void setInfo(int money, int donate, char* inventory1, char* inventory2);
    static void clearRightInventory();
    static void clearLeftInventory();
    static void setNullSlotRight(int id);
    static void setNullSlotLeft(int id);

    static bool isShow() {
        return bShow;
    }

    static bool bShow;
};