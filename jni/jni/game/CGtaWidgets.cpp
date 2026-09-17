#include "CGtaWidgets.h"
#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../util/patch.h"
#include "../chatwindow.h"
#include "../util/armhook.h"
#include "../net/netgame.h"
#include "../chatwindow.h"
#include "game.h"

uintptr_t* CGtaWidgets::pWidgets = nullptr;
extern CNetGame *pNetGame;
extern CChatWindow* pChatWindow;
extern CGame *pGame;
CLocalPlayer* pLocalPlayer;
bool CGtaWidgets::isSprint = true;

void CGtaWidgets::setEnabled(int thiz, bool bEnabled)
{
    *(BYTE *)(thiz + 0x4D) = bEnabled; // this->m_bEnabled
}
void (*CWidgetButton__Update)(int result, int a2, int a3, int a4);
void CWidgetButton__Update_hook(int result, int a2, int a3, int a4)
{
    if (!result)
        return;

    if (pChatWindow->isHudHidden)
    {
        static const int widgetsToDisable[] = {
                WIDGET_SPRINT,
                WIDGET_ENTER_CAR,
                WIDGET_SHOOT,
                WIDGET_ACCELERATE,
                WIDGET_BRAKE,
                WIDGET_HANDBRAKE,
                WIDGET_HORN,
                WIDGET_TARGET,
                WIDGET_TARGET2,
                WIDGET_SHOOT_LEFT
        };
        for (int widget : widgetsToDisable)
        {
            CGtaWidgets::setEnabled(CGtaWidgets::pWidgets[widget], false);
        }
    }

    if (!CGtaWidgets::isSprint)
    {
        CGtaWidgets::setEnabled(CGtaWidgets::pWidgets[WIDGET_SPRINT], false);
        //Log("WORKAEM");
    }

    CWidgetButton__Update(result, a2, a3, a4);
}
void CGtaWidgets::init()
{
    CGtaWidgets::pWidgets = (uintptr_t*)(g_libGTASA + 0x00657E48);

    CHook::InlineHook(g_libGTASA, 0x00274AB4, &CWidgetButton__Update_hook, &CWidgetButton__Update);
}