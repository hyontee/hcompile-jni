//
// Created by Error on 03.01.2025.
//

#include "CShadows.h"
#include "patch.h"

RwTexture* gpShadowHeadLightsTex2 = nullptr;
RwTexture* gpShadowExplosionTex = nullptr;

bool CShadows::StoreStaticShadow(uint32 id, eShadowType type, RwTexture* texture, const CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistane, float scale, float drawDistance, bool temporaryShadow, float upDistance) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x005B8D38 + 1 : 0x6DD6A4), id, type, texture, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistane, scale, drawDistance, temporaryShadow, upDistance);
}
void CShadows::InjectHooks(){

    CHook::Write(g_libGTASA + (VER_x32 ? 0x677BE4 : 0x84D7F8), &asShadowsStored);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x679914 : 0x851250), &ShadowsStoredToBeRendered);
}
void CShadows::StoreShadowToBeRendered(uint8 type, RwTexture* texture, CVector* posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, uintptr_t * realTimeShadow, bool drawOnBuildings) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005B94A0 + 1 : 0x6DDD68), type, texture, posn, topX, topY, rightX, rightY, intensity, red, green, blue, zDistance, drawOnWater, scale, realTimeShadow, drawOnBuildings);
}
/*void CShadows::StoreShadowToBeRendered(uint8 type, RwTexture* texture, CVector* posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, uintptr_t * realTimeShadow, bool drawOnBuildings) {
    if (ShadowsStoredToBeRendered >= asShadowsStored.size())
        return;

    auto& shadow = asShadowsStored[ShadowsStoredToBeRendered];

    shadow.m_nType      = (eShadowType)type;
    shadow.m_pTexture   = texture;
    shadow.m_vecPosn    = posn;
    shadow.m_Front.x    = topX;
    shadow.m_Front.y    = topY;
    shadow.m_Side.x     = rightX;
    shadow.m_Side.y     = rightY;
    shadow.m_nIntensity = intensity;
    shadow.m_nRed       = red;
    shadow.m_nGreen     = green;
    shadow.m_nBlue      = blue;
    shadow.m_fZDistance = zDistance;
    shadow.m_bDrawOnWater     = drawOnWater;
    shadow.m_bDrawOnBuildings = drawOnBuildings;
    shadow.m_fScale     = scale;
    shadow.m_pRTShadow  = realTimeShadow;

    ShadowsStoredToBeRendered++;
}*/