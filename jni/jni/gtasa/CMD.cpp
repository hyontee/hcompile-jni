#include <string.h>
#include "../main.h"
#include "CMD.h"
#include "../CDebugInfo.h"
#include "../util/CJavaWrapper.h"
#include "../chatwindow.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../CSettings.h"
#include "../keyboard.h"
#include "../scoreboard.h"
#include "../game/CCustomPlateManager.h"

extern CGUI *pGUI;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CNetGame *pNetGame;
extern CGame * pGame;
extern CChatWindow* pChatWindow;
extern CScoreBoard* pScoreBoard;

bool CMD::ProcessCommands(const char* str)
{
    if (strstr(str, "/debug")) {
        pGame->m_bDl_enabled = !pGame->m_bDl_enabled;
        CDebugInfo::ToggleDebugDraw(); // доделать меню дебага пакетов и коллизий
        return true;
    }
    if (strstr(str, "/chat")) {
        pChatWindow->g_bChatVisible = !pChatWindow->g_bChatVisible;
        return true;
    }
    if (strstr(str, "/cameditgui")) {
        if (!pChatWindow->isHudHidden) {
            g_pJavaWrapper->HideHud();
            g_pJavaWrapper->ChatStatus(3);
            pChatWindow->isHudHidden = true;
            pChatWindow->g_bChatVisible = false;
        }
        else {
            g_pJavaWrapper->ShowHud();
            g_pJavaWrapper->ChatStatus(1);
            pChatWindow->isHudHidden = false;
            pChatWindow->g_bChatVisible = true;
        }
        return true;
    }
    return false;
}