#pragma once

struct CGtaRect
{
public:
	float x1;           // x1
	float y1;    // y1

//	float x2;        // x2
//	float y2;          // y2
};

class CHUD
{
public:
    static bool m_bShow;
    static bool m_bShowDialog;
    static RwTexture* hud_radar;
    static bool m_bShowAll; // переменная для хранения общего состояния

public:
    static void Initialise();
    static void toggleAll(bool show);
    static CGtaRect radarBgPos1; // x y
    static CGtaRect radarBgPos2; // x y
    static CGtaRect radarPos; // x y
    static float radarSize;

    static void Disable()      { m_bShow = false; }
    static void Enable()       { m_bShow = true; };

    static void DisableD()      { m_bShow = false; }
    static void EnableD()       { m_bShow = true; };

    static bool IsEnabled()    { return m_bShow; };
    static bool IsEnabledDialog()    { return m_bShowDialog; };

    static void EditRadar(CRect* rect);
    static void Render();
};
