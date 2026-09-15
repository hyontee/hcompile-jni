#include "../main.h"
#include "../CSettings.h"
#include "../dialog.h"
#include "../chatwindow.h"
#include "../keyboard.h"

#include "../game/game.h"
#include "../game/CRadarRect.h"
#include "../game/CInfoBarText.h"

#include "../net/netgame.h"

#include "../util/armhook.h"

#include "../gui/gui.h"
#include "CInterface.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CDialogWindow *pDialogWindow;
extern CChatWindow *pChatWindow;

uint8_t CInterface::m_iSatiety = 0;
uint32_t CInterface::m_iEventCoins = 0;
uint32_t CInterface::m_iDonate = 0;
bool CInterface::m_bUsedEvent = false;
bool CInterface::m_bGZ = false;
bool CInterface::m_bE2y = false;
bool CInterface::LoadedHudTexture = false;

#include "../util/interfaces/CHud.h"
void CInterface::RenderHud()
{
	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_HUD) || pDialogWindow->m_bIsActive)
	{
		if (LoadedHudTexture == true)
		{
			CHud::HideHud();
			LoadedHudTexture = false;
		}
		return;
	}
	
	if (LoadedHudTexture == false)
	{
		if (pSettings && pSettings->GetReadOnly().iHUD)
		{
			CHud::ShowHud();
			LoadedHudTexture = true;
		}
	}
	else
	{
		if (pSettings && !pSettings->GetReadOnly().iHUD)
		{
			CHud::HideHud();
			LoadedHudTexture = false;
		}
	}
}