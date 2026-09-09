#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "CHUD.h"
#include <string>

extern CGame* pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;

bool CHUD::m_bShow = false;
bool CHUD::m_bShowDialog = false;

CGtaRect CHUD::radarBgPos1; // X Y
CGtaRect CHUD::radarBgPos2; // X Y
CGtaRect CHUD::radarPos;    // X Y
float       CHUD::radarSize;
bool CHUD::m_bShowAll = true; // инициализация


RwTexture *CHUD::hud_radar;

void CHUD::Initialise()
{   
    Log("CHUD: Initialise");

    hud_radar = nullptr;
    
    hud_radar = (RwTexture*)LoadTextureFromDB("samp", "hud_back");
    Log("CHUD: Loading..");
}
void CHUD::toggleAll(bool show)
{
    CHUD::m_bShowAll = show;
    // Здесь можно добавить логику скрытия других элементов интерфейса,
    // если они управляются через C++
}
void CHUD::EditRadar(CRect* rect)
{
//        posX posY ScaleX ScaleY

//        rect->left      = 60.0f; // posX
//        rect->bottom    = 70.0f; // posY
//        -----------------------------------------
//        ((void(*)())(g_libGTASA + 0x003D4ED8 + 1))(); // CHud::DrawRadar(void)
//        -----------------------------------------
//        rect->right     = 45.0f; // ScaleX
//        rect->top       = 45.0f; //ScaleY

        rect->left      = CHUD::radarPos.x1; // posX
        rect->bottom    = CHUD::radarPos.y1; // posY

        rect->right     = CHUD::radarSize; // ScaleX
        rect->top       = CHUD::radarSize; //ScaleY

}

void CHUD::Render()
{
                      if(CHUD::IsEnabled())
                      {
                                    Log("CHUD: Render");
                     } else {
                        Log("CHUD: NoRender");
                     }
}
