#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "CHUD.h"
//#include "util/patch.h"
#include <string>
#include <jni.h>

// GTASA HOOK
#include "game/GTASA/Models/ModelInfo.h"
#include "game/GTASA/Models/CHud.h"
#include <game/RW/rwcore.h>
#include <game/RW/rpworld.h>
#include "game/RW/RenderWare.h"
#include "../interface/hud.h"

extern CGame* pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;

bool CHUD::m_bShow = false;
bool CHUD::m_bShowDialog = false;

CGtaRect CHUD::radarBgPos1; // X Y
CGtaRect CHUD::radarBgPos2; // X Y
CGtaRect CHUD::radarPos; // X Y
CRadarBrRect CHUD::radar1; // x y x2 y2
CGtaSizePosition CHUD::radarBgPosSize;

RwTexture *CHUD::hud_radar;

void CHUD::Initialise()
{   
    Log("CHUD: Initialise");

    hud_radar = nullptr;
    hud_radar = (RwTexture*)LoadTextureFromDB("radar", "bg_hud_map");
    Log("CHUD: Loading..");
}

void CHUD::EditRadar(CRect* rect)
{
//        posX posY ScaleX ScaleY

    rect->left      = CHUD::radarPos.x1; // posX
    rect->bottom    = CHUD::radarPos.y1; // posY

    rect->right     = 34.6f; // ScaleX
    rect->top       = 34.6f; // ScaleY
    rect->right     = 42.0f; // ScaleX размеры
    rect->top       = 42.0f; // ScaleY размеры
}

void CHUD::EditRadarBios(CRect* rect)
{
//        posX posY ScaleX ScaleY

    rect->left      = CHUD::radar1.x1; // posX
    rect->bottom    = CHUD::radar1.y1; // posY

    rect->right = CHUD::radar1.x2;
    rect->top = CHUD::radar1.y2;
}

/*void CHUD::EditRadar(CRect* rect)
{*/
//        posX posY ScaleX ScaleY

//        rect->left      = 60.0f; // posX
//        rect->bottom    = 70.0f; // posY
//        -----------------------------------------
//        ((void(*)())(g_libGTASA + 0x003D4ED8 + 1))(); // CHud::DrawRadar(void)
//        -----------------------------------------
//        rect->right     = 45.0f; // ScaleX
//        rect->top       = 45.0f; // ScaleY

        //rect->left      = 51.2f; // posX
        //rect->bottom    = 76.6f; // posY в низ (от вверха)
//        -----------------------------------------
//        ((void(*)())(g_libGTASA + 0x003D4ED8 + 1))(); // CHud::DrawRadar(void)
//        -----------------------------------------
        /*rect->right     = 42.0f; // ScaleX размеры
        rect->top       = 42.0f; // ScaleY размеры

}*/
extern "C"
{
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_SetRadarBgPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1, jfloat x2, jfloat y2)
    {
        // -- обложка
        CHUD::radarBgPos1.x1 = x1;
        CHUD::radarBgPos1.y1 = y1;

        CHUD::radarBgPos2.x1 = x2;
        CHUD::radarBgPos2.y1 = y2;

        // -- радар
        //                                    CHUD::radar1.x1 = x1;
        //		CHUD::radar1.y1 = y1;
    }

    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_SetRadarPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1, jfloat x2, jfloat y2)
    {
        CHUD::radar1.x1 = x1;
        CHUD::radar1.y1 = y1;

        CHUD::radar1.x2 = x2;
        CHUD::radar1.y2 = y2;
    }

    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_SetRadarEnabled(JNIEnv *env, jobject thiz, jboolean tf)
    {
        if(tf)
        {
            CHUD::Enable();
        }
        else
        {
            CHUD::Disable();
        }
    }
}

void CHUD::Render()
{
    if(CHUD::IsEnabled())
    {
        Log("CHUD: Render");

    } else Log("CHUD: NoRender");
}