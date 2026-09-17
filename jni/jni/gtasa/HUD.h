#pragma once
#include <jni.h>

struct CGtaRect
{
public:
    float x1;
    float y1;
};

struct CRadarBrRect
{
public:
    float x1;
    float y1;

    float x2;
    float y2;
};

class CHUD
{
public:
    static bool m_bShow;
    static CGtaRect radarBgPos1;
    static CGtaRect radarBgPos2;

    static CRadarBrRect radar1;

    static void Disable()      { m_bShow = false; }
    static void Enable()       { m_bShow = true; };

    static bool IsEnabled()    { return m_bShow; };
    static void EditRadarBios(CRect* rect);

    static CSprite2d* radar;
    static CSprite2d* logo;

    static CGtaRect logoPos;
};