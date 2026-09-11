//
// Created by admin on 29.12.2023.
//

#pragma once


#include "Vector2D.h"
#include "game/RW/common.h"
#include "Radar.h"

#pragma pack(push, 1)
class MobileMenu {
public:
    CVector2D bgUVSize;
    CVector2D bgTargetCoords;
    CVector2D bgCurCoords;
    CVector2D bgStartCoords;
    uint32_t screenStack[3]; // BAD!!!!!!!!!!!!!!!!!
    float pendingScreen;
    RwTexture* bgTex;
    RwTexture* sliderEmpty;
    RwTexture* sliderFull;
    RwTexture* sliderNub;
    uint32_t controlsBack;
    uint32_t controlsBack2;
    tBlipHandle waypoint_blip;
    bool m_WantsToRestartGame;
    bool WantsToLoad;
    char unused1[2];
    uint32_t SelectedSlot;
    bool CurrentGameNotResumable;
    bool InitializedForSignOut;
    char unused2[2];
    float NEW_MAP_SCALE;
    float MAP_OFFSET_X;
    float MAP_OFFSET_Y;
    float MAP_AREA_X;
    float MAP_AREA_Y;
    bool DisplayingMap;
    bool isMapMode;
    bool pointerMode;
    bool isMouse;
    CVector2D pointerCoords[4];
    uint32_t pointerState[4];
    uint32_t pointerPress[4];
};
#pragma pack(push)

VALIDATE_SIZE(MobileMenu, 0xB0);