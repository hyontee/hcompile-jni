//
// Created by admin on 07.09.2023.
//

#pragma once

#include "../main.h"

class CInventory {
public:
    static void show(int slotcount);
    static void hide();
    static void setSkin(int skin);
    static void updateSlotsInfo(int count, int id, int type, int amount, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info);
    static void updateAcsSlotsInfo(int acc_slot, uint16_t objectId, CVector vecRotation, float zoom, char* caption, char* info);
    static void setInfo(int health, int thirst, int hunger, int money, int donate);
    static void clearInventory();
    static void setNullSlot(int id);

    static bool isShow() {
        return bShow;
    }

    static bool bShow;
};