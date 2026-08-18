//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CWEAPONGTA_H
#define LIVERUSSIA_CWEAPONGTA_H

#include "game/Enums/WeaponType.h"
struct CWeaponGta
{
    eWeaponType dwType;
    uint32_t dwState;
    int32_t dwAmmoInClip;
    int32_t dwAmmo;
    uint32_t m_nTimer;
    bool m_bFirstPersonWeaponModeSelected;
    bool m_bDontPlaceInHand;
    uint8_t skip[2];
    uintptr_t *m_pWeaponFxSys;
};
#endif //LIVERUSSIA_CWEAPONGTA_H
