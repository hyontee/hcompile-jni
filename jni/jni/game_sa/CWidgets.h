//
// Created by x1y2z on 05.01.2024.
//

#pragma once

#include <cstdint>
#include "../game/RW/common.h"
#include "../game/sprite2d.h"
#include "HIDMapping.h"
#include "Rect.h"

#pragma pack(push, 1)
class CWidgetGta {
public:
    int vtable;
    HIDMapping m_HIDMapping;
    CSprite2d m_Sprite;
    float m_fOriginX;
    float m_fOriginY;
    float m_fScaleX;
    float m_fScaleY;
    float m_fFadeRate;
    CRect m_RectScreen;
    float m_fExtraPaddingScale;
    CRect m_RectPaddedScreen;
    float m_fTapHoldTime;
    bool m_bTaphold;
    CRGBA m_Color;
    bool m_bEnabled;
    bool m_bCachedEnabled;
    uint8_t pad0;
    float m_fTapHistory[10];
    int m_nTouchIndex;
    int m_nFrameCount;
    unsigned int m_nFlags;
    float m_fUserData;
    uintptr_t *m_pHoldEffect;
    float m_fHoldEffectPeriod;

};
VALIDATE_SIZE(CWidgetGta, 0x90);

enum {
    WIDGET_POSITION_ENTER_CAR,
    WIDGET_POSITION_CAR_SHOOT,
    WIDGET_POSITION_ACCELERATE,
    WIDGET_POSITION_BUTTON_SPRINT_AND_SWIM,
    WIDGET_POSITION_BRAKE,
    WIDGET_POSITION_VEHICLE_STEER_LEFT,
    WIDGET_POSITION_VEHICLE_STEER_RIGHT,
    WIDGET_POSITION_HORN,
    WIDGET_POSITION_HORN_ALT,
    WIDGET_POSITION_PHONE,
    WIDGET_POSITION_VEHICLE_SHOOT_LEFT,
    WIDGET_POSITION_VEHICLE_SHOOT_RIGHT,
    WIDGET_POSITION_HANDBRAKE_AND_AIR_SHOOT,
    WIDGET_POSITION_CAM_TOGGLE,
    WIDGET_POSITION_TARGETING_AND_ATTACK,
    WIDGET_POSITION_VC_SHOOT,
    WIDGET_POSITION_VC_SHOOT_ALT,
    WIDGET_POSITION_SWAP_WEAPONS,
    WIDGET_POSITION_PLACE_BET,
    WIDGET_POSITION_REMOVE_BET,
    WIDGET_POSITION_NEGATIVE,
    WIDGET_POSITION_POSITIVE,
    WIDGET_POSITION_POOL_ENGLISH
};

#pragma pack(pop)
