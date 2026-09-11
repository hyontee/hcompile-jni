//
// Created by admin on 29.12.2023.
//

#pragma once

#include "../main.h"

class CColourSet {
public:
    float  m_fAmbientRed;
    float  m_fAmbientGreen;
    float  m_fAmbientBlue;

    float  m_fAmbientRed_Obj;
    float  m_fAmbientGreen_Obj;
    float  m_fAmbientBlue_Obj;

    float m_fAmbientBeforeBrightnessRed; // m_fDirectional
    float m_fAmbientBeforeBrightnessGreen;
    float m_fAmbientBeforeBrightnessBlue;

    uint16 m_nSkyTopRed;
    uint16 m_nSkyTopGreen;
    uint16 m_nSkyTopBlue;

    uint16 m_nSkyBottomRed;
    uint16 m_nSkyBottomGreen;
    uint16 m_nSkyBottomBlue;

    uint16 m_nSunCoreRed;
    uint16 m_nSunCoreGreen;
    uint16 m_nSunCoreBlue;

    uint16 m_nSunCoronaRed;
    uint16 m_nSunCoronaGreen;
    uint16 m_nSunCoronaBlue;

    float  m_fSunSize;
    float  m_fSpriteSize;
    float  m_fSpriteBrightness;
    uint16 m_nShadowStrength;
    uint16 m_nLightShadowStrength;
    uint16 m_nPoleShadowStrength;
    float  m_fFarClip;
    float  m_fFogStart;
    float  m_fLightsOnGroundBrightness;

    uint16 m_nLowCloudsRed;
    uint16 m_nLowCloudsGreen;
    uint16 m_nLowCloudsBlue;

    uint16 m_nFluffyCloudsBottomRed;
    uint16 m_nFluffyCloudsBottomGreen;
    uint16 m_nFluffyCloudsBottomBlue;

    float  m_fWaterRed;
    float  m_fWaterGreen;
    float  m_fWaterBlue;
    float  m_fWaterAlpha;

    float  m_fPostFx1Red;
    float  m_fPostFx1Green;
    float  m_fPostFx1Blue;
    float  m_fPostFx1Alpha;

    float  m_fPostFx2Red;
    float  m_fPostFx2Green;
    float  m_fPostFx2Blue;
    float  m_fPostFx2Alpha;

    float  m_fCloudAlpha;
    int32 m_nHighLightMinIntensity;
    uint16 m_nWaterFogAlpha;
    float  m_fIllumination;
    float  m_fLodDistMult;


};