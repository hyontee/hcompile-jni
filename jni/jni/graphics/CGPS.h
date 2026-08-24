//
// Created by admin on 30.12.2023.
//

#pragma once
#include "../main.h"
#include "game_sa/Radar.h"

class CGPS {
public:
    static void InjectHooks();
    static RwUInt32 GetTraceColor(eBlipColour clr, bool friendly = false);
    static CRGBA& GetTraceTextColor(eBlipColour clr, bool friendly = false);
    static void Initialise();

    inline static void Setup2DVertex(RwOpenGLVertex &vertex, float x, float y, RwUInt32 color);

};
