//
// Created by Error on 03.01.2025.
//

#ifndef LR64_CSHADOWS_H
#define LR64_CSHADOWS_H

#include "../main.h"
extern RwTexture* gpShadowHeadLightsTex2;
extern RwTexture* gpShadowExplosionTex;


enum eShadowType : uint8 {
    SHADOW_NONE     = 0,
    SHADOW_DEFAULT  = 1,
    SHADOW_ADDITIVE = 2,
    SHADOW_INVCOLOR = 3,
    SHADOW_OIL_1    = 4,
    SHADOW_OIL_2    = 5, // Oil on fire
    SHADOW_OIL_3    = 6,
    SHADOW_OIL_4    = 7,
    SHADOW_OIL_5    = 8
};

enum eShadowTextureType {
    SHADOW_TEX_CAR = 1,
    SHADOW_TEX_PED = 2,
    SHADOW_TEX_EXPLOSION = 3,
    SHADOW_TEX_HELI = 4,
    SHADOW_TEX_HEADLIGHTS = 5,
    SHADOW_TEX_BLOOD = 6
};

// bruh, OG name
enum VEH_SHD_TYPE {
    VEH_SHD_CAR = 0,
    VEH_SHD_BIKE = 1,
    VEH_SHD_HELI = 2,
    VEH_SHD_PLANE = 3,
    VEH_SHD_RC = 4,
    VEH_SHD_BIG_PLANE = 5 // AT400; ANDROM
};
class CRegisteredShadow {
public:
    CVector          m_vecPosn;
    CVector2D        m_Front;
    CVector2D        m_Side;
    float            m_fZDistance;
    float            m_fScale;
    RwTexture*       m_pTexture;
    uintptr_t * m_pRTShadow;
    uint16           m_nIntensity;
    eShadowType      m_nType; // TODO: Check if this is the correct type...
    uint8            m_nRed;
    uint8            m_nGreen;
    uint8            m_nBlue;

    uint8 m_bDrawOnWater : 1;
    uint8 m_bAlreadyRenderedInBatch : 1;          /// Whenever it has been rendered. Reset each frame, used for batching
    uint8 m_bDrawOnBuildings : 1;
};
constexpr uint32 MAX_STORED_SHADOWS      = 48;
class CShadows {
public:
    static inline uint16_t ShadowsStoredToBeRendered;
    static inline std::array<CRegisteredShadow, MAX_STORED_SHADOWS> asShadowsStored;
    static void InjectHooks();


    static bool StoreStaticShadow(uint32 id, eShadowType type, RwTexture *texture, const CVector *posn,
                           float frontX, float frontY, float sideX, float sideY, int16 intensity,
                           uint8 red, uint8 green, uint8 blue, float zDistane, float scale,
                           float drawDistance, bool temporaryShadow, float upDistance);

    static void
    StoreShadowToBeRendered(uint8 type, RwTexture *texture, CVector* posn, float topX,
                            float topY,
                            float rightX, float rightY, int16 intensity, uint8 red, uint8 green,
                            uint8 blue, float zDistance, bool drawOnWater, float scale,
                            uintptr_t *realTimeShadow, bool drawOnBuildings);
};


#endif //LR64_CSHADOWS_H
