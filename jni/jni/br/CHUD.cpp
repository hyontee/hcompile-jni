#include "../main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "../chatwindow.h"

#include "CHUD.h"
#include "../util/patch.h"
#include <string>

// GTASA HOOK
#include "../gtare/CHud.h"

// JAVA
#include "../util/CJavaWrapper.h"
extern CJavaWrapper* g_pJavaWrapper;

extern CGame* pGame;
extern CChatWindow *pChatWindow;
extern CNetGame *pNetGame;
extern CGUI *pGUI;

bool CHUD::m_bShow = false;
bool CHUD::m_isDialog = false;
bool CHUD::m_bPerson = false;
bool CHUD::m_bRendered = false;
bool CHUD::m_bCreatedCar = false;
bool CHUD::m_bShowDialog = false;
bool CHUD::m_isAutoShop = false;

// FIX NETGAME (RECONNECTING)
bool CHUD::m_bHidenNetGame = false;
bool CHUD::m_bNewGame = false;

CVector2D CHUD::radarBgPos1; // X Y
CVector2D CHUD::radarBgPos2; // X Y
CVector2D CHUD::radarPos;        // X Y
float       CHUD::radarSize;
CRadarBrRect CHUD::radar1; // x y x2 y2
CSprite2d *CHUD::m_pSprite;

RwTexture *CHUD::hud_radar;
RwTexture *CHUD::hud_radar2;

void CHUD::Initialise()
{   
    Log(OBFUSCATE("CHUD: Initialise"));
    m_pSprite = new CSprite2d();
    m_pSprite->m_pRwTexture = (RwTexture *) LoadTextureFromDB("samp", "hud_back");

    Log(OBFUSCATE("CHUD: Loading.."));
}
void CHUD::DrawBackGound()
{
    if(m_bShow) {
        CHUD::CreateRadarBg(CHUD::radarBgPos1.X,
                            CHUD::radarBgPos1.Y,
                            CHUD::radarBgPos2.X,
                            CHUD::radarBgPos2.Y);
        /*Hud::CreateRadarBg(pSettings->GetReadOnly().radar_x - 1,
                      pSettings->GetReadOnly().radar_y - 13,
                      3.35f * pSettings->GetReadOnly().radar_scale,
                      3.35f * pSettings->GetReadOnly().radar_scale, "hud_radar");*/
    }
}
void CHUD::CreateRadarBg(float x, float y, float width, float height) {
    CRGBA color_white;
    color_white.A = 255;
    color_white.B = 255;
    color_white.G = 255;
    color_white.R = 255;

    m_pSprite->Draw(x, y, width, height, color_white);

}

void CHUD::ShowHud(bool isShow)
{
        if(isShow)
        {
                  if(pNetGame->GetGameState() != GAMESTATE_CONNECTED) 
                                                 g_pJavaWrapper->ShowHud();
        }
        else
        {
	g_pJavaWrapper->HideHud();
        }
}

void CHUD::Render()
{
                      if(CHUD::IsEnabled())
                                    Log(OBFUSCATE("CHUD: Render"));
                      else 
                                    Log(OBFUSCATE("CHUD: NoRender"));
}