#include "main.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "dialog.h"
#include "scoreboard.h"
#include "keyboard.h"
#include "buttons.h"
#include "../CSettings.h"
#include "../util/CJavaWrapper.h"
#include "../util/interfaces/CHud.h"

extern CKeyBoard *pKeyBoard;
extern CDialogWindow *pDialogWindow;
extern CGame *pGame;
extern CScoreBoard *pScoreBoard;
extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CSettings *pSettings;

/*void* ButtonsPressed(void* g_button)
{
    for (int i = 0; i < 50; i++)
    {   
    	usleep(5000);
    	*(bool*)g_button = true;  
    }
	pthread_exit(0);
	return (void*)0;
}
pthread_t threadButtons;*/
	
bool CButtons::m_bIsActive = false;
bool CButtons::m_bIsShow = false;
int8_t CButtons::m_iKey[7];

CButtons::CButtons() 
{
    m_bIsItemShow = false;
    m_bIsShow = false;
}

void CButtons::Render()
{
	if (!pGame) return;
	if (!pGame->FindPlayerPed()) return;
	
	if (m_bIsActive == true)
	{
		if (!pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS))
		{
			if (m_bIsShow == true)
			{
				g_pJavaWrapper->HideButton();
				CHud::hudButton(0, 0, 0, 0, 0, 0, 0);
				m_bIsShow = false;
			}
		}
		else
		{
			if (m_bIsShow == false)
			{
				g_pJavaWrapper->HideButton();
				CHud::hudButton(m_iKey[0], m_iKey[1], m_iKey[2], m_iKey[3], m_iKey[4], m_iKey[5], m_iKey[6]);
				m_bIsShow = true;
			}
		}
		return;
	}
	
	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS)) 
	{
		if (m_bIsShow == true) 
		{
			g_pJavaWrapper->HideButton();
			m_bIsShow = false;
		}
		return;
    }
	else
	{
		if (m_bIsShow == false) 
		{
			g_pJavaWrapper->ShowButton();
			m_bIsShow = true;
		}
	}
}