//
// Created by x1y2z on 16.09.2023.
//

#pragma once

#include "Automobile.h"
#include "game/common.h"

class CHeli : public CAutomobile {
#if VER_x32
    uint8 pad0[0x90];
#else
    uint8_t pad0[0x98];
#endif
};
