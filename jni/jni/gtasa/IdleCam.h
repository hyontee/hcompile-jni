//
// Created by weikton on 22.06.2025.
//

#pragma once
#define IDLE_ADDR 0x5D001C

#include "../main.h"
#include "../game/common.h"
#include "../game/entity.h"

class CIdleCam {
public:
    CEntity  *pTarget;
    VECTOR    positionToSlerpFrom;
    float       timeSlerpStarted;
    float       slerpDuration;
    VECTOR     lastIdlePos;
    float       slerpTime;
    float       timeControlsIdleForIdleToKickIn;
    float       timeIdleCamStarted;
    int32       lastFrameProcessed;
    float       timeLastTargetSelected;
    float       timeMinimumToLookAtSomething;
    float       timeTargetEntityWasLastVisible;
    float       timeToConsiderNonVisibleEntityAsOccluded;
    float       distTooClose;
    float       distStartFOVZoom;
    float       distTooFar;
    int32       targetLOSFramestoReject;
    int32       targetLOSCounter;
    int32       zoomState;
    float       zoomFrom;
    float       zoomTo;
    float       timeZoomStarted;
    float       zoomNearest;
    float       zoomFarthest;
    float       curFOV;
    float       durationFOVZoom;
    bool        bForceAZoomOut;
    bool        bHasZoomedIn;
    uint8       pad0[2];
    float       timeBeforeNewZoomIn;
    float       timeLastZoomIn;
    float       increaseMinimumTimeFactorforZoomedIn;
    float       degreeShakeIdleCam;
    float       shakeBuildUpTime;
    int32       lastTimePadTouched;
    int32       idleTickerFrames;
    uintptr     *pCam;

public:
    static void InjectHooks();
};

VALIDATE_SIZE(CIdleCam, 0x9C);
extern CIdleCam gIdleCam;