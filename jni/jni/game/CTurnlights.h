#pragma once

#include "../main.h"
#include "game.h"
#include <unordered_map>

class CTurnlights
{
private:
    /* data */
public:
    static void DrawTurnLightsLeft(int dummy);
    static void DrawTurnLightsRight(int dummy);

    static inline std::unordered_map<CVehicle*, bool> listRight;
    static inline std::unordered_map<CVehicle*, bool> listLeft;

    static void SetEnabledRight(CVehicle *pVehicle, bool toggle);

    static void SetEnabledLeft(CVehicle *pVehicle, bool toggle);

    static bool GetAtRight(CVehicle* pVehicle)
    {
        auto it = listRight.find(pVehicle);
        if(it != listRight.end())
            return listRight[pVehicle];

        return false;
    }
    static bool GetAtLeft(CVehicle* pVehicle)
    {
        auto it = listLeft.find(pVehicle);
        if(it != listLeft.end())
            return listLeft[pVehicle];

        return false;
    }
private:
    static int iDummyId;
    static uint32_t time;

};
