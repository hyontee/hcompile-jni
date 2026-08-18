//
// Created by plaka on 20.02.2023.
//

#include "CWorld.h"
#include "main.h"


void CWorld::Add(CEntityGta *entity) {
    ((void(*)(CEntityGta*))(g_libGTASA + (VER_x32 ? 0x00423418 + 1 : 0x507518)))(entity);
}
