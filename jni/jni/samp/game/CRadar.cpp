//
// Created by Error on 03.03.2025.
//

#include "CRadar.h"
#include "patch.h"

extern int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char *name);

void CRadar::ChangeBlipScale(int32 nIndex, int32 NewScale) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x442B11 : 0x527D74), nIndex, NewScale);
}

void CRadar::ChangeBlipColour(int32 nIndex, int32 nNewColour) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x442A59 : 0x527CD4), nIndex, nNewColour);
}

int CRadar::SetCoordBlip(int r0, CVector pos, int r4) {
    return CRadar__SetCoordBlip(r0, pos.x, pos.y, pos.z, 0, r4, "name");
}

void CRadar::SetBlipSprite(int32 nIndex, int32 BlipSprite) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x442BE1 : 0x527E1C), nIndex, BlipSprite);
}
