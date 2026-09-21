#pragma once
#include <jni.h>

struct CGtaRect
{
public:
	float x1;           // x1
	float y1;    // y1

//	float x2;        // x2
//	float y2;          // y2
};

struct CRadarBrRect
{
public:
	float x1;           // x1
	float y1;    // y1

	float x2;        // x2
	float y2;          // y2
};

class CHUD
{
public:
    static CSprite2d* m_pSprite;
    static bool m_bCreatedCar;
    static bool m_bNewGame;
    static bool m_bShow;
    static bool m_bPerson;
    static bool m_isDialog;
    static bool m_isAutoShop;
    static bool m_bRendered;
    static bool m_bShowDialog;
    static bool m_bHidenNetGame;
    static RwTexture* hud_radar;
    static RwTexture* hud_radar2;

public:
    static void Initialise();

    static CVector2D radarBgPos1; // x y
    static CVector2D radarBgPos2; // x y
    static CVector2D radarPos; // x y
    static float   radarSize;
    static CRadarBrRect radar1; // x y

    static void Disable()      { m_bShow = false; }
    static void Enable()       { m_bShow = true; };

    static void DisableD()      { m_bShow = false; }
    static void EnableD()       { m_bShow = true; };

    static bool IsEnabled()    { return m_bShow; };
    static bool IsEnabledDialog()    { return m_bShowDialog; };
    static void DrawBackGound();
    static void CreateRadarBg(float x, float y, float width, float height);

    static void Render();
    static void ShowHud(bool isShow);
};