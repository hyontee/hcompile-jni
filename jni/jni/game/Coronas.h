#pragma once

#include "common.h"
#include "RegisteredCorona.h"
#include "RW/rwcore.h"

class CCoronas {
public:
    static inline float SunScreenX;
    static inline float SunScreenY;
    // are there any obstacles between sun and camera
    static inline bool SunBlockedByClouds;
//    // change coronas brightness immediately
//    static bool& bChangeBrightnessImmediately;
//    // num of registered coronas in frame
//    static uint32& NumCoronas;
//    // coronas intensity multiplier
//    static float& LightsMult;
    // this is used to control moon size when you shooting it with sniper
    static inline uint MoonSize = 3;
    // Coronas array. count: MAX_NUM_CORONAS (default: 64)
    static constexpr int MAX_NUM_CORONAS = 64;
    static inline CRegisteredCorona aCoronas[MAX_NUM_CORONAS];
//
//    static uint16 (&ms_aEntityLightsOffsets)[8];
//
//    inline static struct { // NOTSA
//        bool DisableWetRoadReflections;
//        bool AlwaysRenderWetRoadReflections; // Ignored if if `DisableReflections == false`
//    } s_DebugSettings{};

public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void Update();
    static void Render();
    static void RenderReflections();
    static void RenderSunReflection();
    static void UpdateCoronaCoors(uint id, const CVector& posn, float farClip, float angle);
    static void DoSunAndMoon();

    static void RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha, const CVector* posn, float radius, float farClip, RwTexture* texture, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);

    void RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha, const CVector* posn, float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);
};

extern uint MAX_CORONAS;
constexpr int CORONA_TEXTURES_COUNT = 10;
//static inline RwTexture* gpCoronaTexture[CORONA_TEXTURES_COUNT];

static inline const char* aCoronaSpriteNames[] = {
        "coronastar",
        "coronamoon",
        "coronasun",
        "coronaboom",
        "coronaplace",
        "coronaarc",
        "coronastatus",
        "coronaright",
        "coronalef",
        "corona"
};