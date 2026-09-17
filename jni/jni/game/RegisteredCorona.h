#pragma once

#include "common.h"
#include "rgba.h"
#include "CVector.h"


enum eCoronaType {
    CORONATYPE_SHINYSTAR,
    CORONATYPE_HEADLIGHT,
    CORONATYPE_MOON,
    CORONATYPE_REFLECTION,
    CORONATYPE_HEADLIGHTLINE,
    CORONATYPE_HEX,
    CORONATYPE_CIRCLE,
    CORONATYPE_RING,
    CORONATYPE_STREAK,
    CORONATYPE_TORUS,
    CORONATYPE_NONE
};

enum eCoronaFlareType : uint { FLARETYPE_NONE, FLARETYPE_SUN, FLARETYPE_HEADLIGHTS };

class CRegisteredCorona {
public:
    CVector          m_vPosn;
    uint          m_dwId{};     // Should be unique for each corona. Address or something
    RwTexture*       m_pTexture; // Pointer to the actual texture to be rendered
    float            m_fSize;
    float            m_fAngle;    // left from III&VC
    float            m_fFarClip;  // How far away this corona stays visible
    float            m_fNearClip; // How far away is the z value pulled towards camera.
    float            m_fHeightAboveGround;
    float            m_fFadeSpeed; // The speed the corona fades in and out
    CRGBA            m_Color;
    uint            m_FadedIntensity;           // Intensity that lags behind the given intenisty and fades out if the LOS is blocked
    uint            m_bRegisteredThisFrame; // Has this corona been registered by game code this frame
    eCoronaFlareType m_nFlareType;
    uint            m_bUsesReflection;
    uint            m_bCheckObstacles : 1;    // Do we check the LOS or do we render at the right Z value -> bLOSCheck
    uint            m_bOffScreen : 1;         // Set by the rendering code to be used by the update code
    uint            m_bJustCreated;           // If this corona has been created this frame we won't delete it (It hasn't had the time to get its OffScreen cleared)
    uint            m_bFlashWhileFading : 1;  // Does the corona fade out when closer to cam
    uint            m_bOnlyFromBelow : 1;     // This corona is only visible if the camera is below it
    uint            m_bHasValidHeightAboveGround : 1;   // this corona Has Valid Height Above Ground
    uint            m_bDrawWithWhiteCore : 1; // This corona rendered with a small white core.
    uint            m_bAttached : 1;          // This corona is attached to an entity.
    uintptr_t*         m_pAttachedTo;

    CRegisteredCorona() = default;

    void Update();
    auto GetPosition() const -> CVector;
    auto CalculateIntensity(float scrZ, float farClip) const -> float;
};