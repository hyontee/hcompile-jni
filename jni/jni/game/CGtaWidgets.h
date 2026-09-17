//
// Created by plaka on 03.02.2023.
//

#ifndef LIVERUSSIA_CGTAWIDGETS_H
#define LIVERUSSIA_CGTAWIDGETS_H

#include <cstdint>
#include "playerped.h"

class CGtaWidgets {
public:
    static uintptr_t* pWidgets;

    static void setEnabled(int thiz, bool bEnabled);

    static void init();

    CPlayerPed * GetPlayerPed() { return m_pPlayerPed; };
    CPlayerPed			*m_pPlayerPed;

    static bool isSprint;
};

enum {
    WIDGET_ENTER_CAR,
    WIDGET_SHOOT,
    WIDGET_ACCELERATE,
    WIDGET_BRAKE,
    WIDGET_HANDBRAKE,
    CRASH0,
    CRASH1,
    WIDGET_HORN,
    WIDGET_HORN_ALT,
    WIDGET_BUTTERFLY,
    WIDGET_KISS,
    WIDGET_SHOOT1,
    CRASH2,
    WIDGET_UP,
    WIDGET_VEH_UP,
    WIDGET_DOWN,
    WIDGET_VEH_SHOOT_LEFT,
    WIDGET_VEH_SHOOT_RIGHT,
    WIDGET_CAM_TOGGLE,
    WIDGET_TARGET,
    WIDGET_TARGET2,
    WIDGET_SHOOT_LEFT,
    WIDGET_SHOOT_RIGHT,
    WIDGET_SHOOT_RIGHT1,
    WIDGET_ROCKET_SHOOT,
    WIDGET_EXPLODE,
    WIDGET_ROTATE_LEFT,
    WIDGET_ROTATE_RIGHT,
    WIDGET_MONEY,
    WIDGET_CHANGE_GUN,
    WIDGET_NITRO,
    WIDGET_SPRINT,
    CRASH3,
    WIDGET_SWIM_LEFT,
    WIDGET_SWIM_RIGHT,
};

#endif //LIVERUSSIA_CGTAWIDGETS_H
