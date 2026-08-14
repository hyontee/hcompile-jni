#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "CHUD.h"
#include "util/patch.h"
#include <string>

// GTASA HOOK
#include "clientlogic/CHud.h"

extern CGame* pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;

bool CHUD::m_bShow = false;
bool CHUD::m_bShowDialog = false;

CGtaRect CHUD::radarBgPos1; // X Y
CGtaRect CHUD::radarBgPos2; // X Y
CGtaRect CHUD::radarPos;        // X Y

RwTexture *CHUD::hud_radar;

void CHUD::Initialise()
{   
    Log("CHUD: Initialise");

    hud_radar = nullptr;
    hud_radar = (RwTexture*)LoadTextureFromDB("samp", "hud_back"); //WARNING: твою текстуру сзади радара закинь в кеш в папку  samp с названием hud_back
    Log("CHUD: Loading..");
}

void CHUD::EditRadar(CRect* rect)
{

        rect->left      = CHUD::radarPos.x1; // posX
        rect->bottom    = CHUD::radarPos.y1; // posY
		
		float mult = 0.310f; //это размер, изменяй его под свой худ (на телефонах он будет ровный везде, главное у себя хотя бы подровняй размер)

        rect->right     = CHUD::radarPos.x1 * mult; // ScaleX
        rect->top       = CHUD::radarPos.x1 * mult; //ScaleY

}

void CHUD::Render()
{
                      if(CHUD::IsEnabled())
                      {
                                    Log("CHUD: Render");
                     } else Log("CHUD: NoRender");
}