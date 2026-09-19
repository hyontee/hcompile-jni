#include "CDebugInfo.h"

#include "main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/RW/RenderWare.h"
#include "CChatWindow.h"
#include "playertags.h"
#include "CDialog.h"
#include "CKeyboard.h"
#include "CSettings.h"
#include "util/armhook.h"

extern CGUI* pGUI;
extern CGame* pGame;
extern CChatWindow* pChatWindow;
extern CKeyBoard* pKeyBoard;
extern CSettings* pSettings;

uint32_t CDebugInfo::uiStreamedPeds = 0;
uint32_t CDebugInfo::uiStreamedVehicles = 0;
uint32_t CDebugInfo::m_uiDrawDebug = 0;
uint32_t CDebugInfo::m_uiDrawFPS = 0;
uint32_t CDebugInfo::m_dwSpeedMode = 0;
uint32_t CDebugInfo::m_dwSpeedStart = 0;

void CDebugInfo::ToggleDebugDraw()
{
	m_uiDrawDebug ^= 1;
}

void CDebugInfo::SetDrawFPS(uint32_t bDraw)
{
	m_uiDrawFPS = bDraw;
}

extern uint8_t g_maxFPS;
void CDebugInfo::Draw()
{
	char szStr[30];
	if (!pKeyBoard->IsOpen() && pSettings->GetReadOnly().iFPSCounter)
	{
        float* pFPS = reinterpret_cast<float*>(g_libGTASA + 0x608E00);
        float currentFPS = *pFPS;
        if (currentFPS > static_cast<uint8_t>(g_maxFPS)) {
            *pFPS = static_cast<uint8_t>(g_maxFPS);
        } else {
            *pFPS = currentFPS;
        }

		snprintf(&szStr[0], 30, "FPS: %.0f", *pFPS);
        ImVec2 pos = ImVec2(pGUI->ScaleX(40.0f), pGUI->ScaleY(1080.0f - pGUI->GetFontSize() * 5));

		pGUI->RenderText(pos, (ImU32)0xFFFFFFFF, true, &szStr[0]);
	}
}

void CDebugInfo::ApplyDebugPatches()
{
#ifdef DEBUG_INFO_ENABLED

	UnFuck(g_libGTASA + 0x008B8018);
	*(uint8_t*)(g_libGTASA + 0x008B8018) = 1;
	NOP(g_libGTASA + 0x00399EDA, 2);
	NOP(g_libGTASA + 0x00399F46, 2);
	NOP(g_libGTASA + 0x00399F92, 2);

#endif
}

void CDebugInfo::ToggleSpeedMode()
{
	m_dwSpeedMode ^= 1;
	if (m_dwSpeedMode)
	{
		pChatWindow->AddDebugMessage("Speed mode enabled");
	}
	else
	{
		pChatWindow->AddDebugMessage("Speed mode disabled");
	}
}

void CDebugInfo::ProcessSpeedMode(VECTOR* pVecSpeed)
{
	if (!m_dwSpeedMode)
	{
		return;
	}
	static uint32_t m_dwState = 0;
	float speed = sqrt((pVecSpeed->X * pVecSpeed->X) + (pVecSpeed->Y * pVecSpeed->Y) + (pVecSpeed->Z * pVecSpeed->Z)) * 2.0f * 100.0f;
	if (speed >= 1.0f)
	{
		if (!m_dwSpeedStart)
		{
			m_dwSpeedStart = GetTickCount();
			m_dwState = 0;
			pChatWindow->AddDebugMessage("Start");
		}
		if ((speed >= 119.0f) && (speed <= 121.0f) && (m_dwState == 0))
		{
			pChatWindow->AddDebugMessage("1 to 100: %d", GetTickCount() - m_dwSpeedStart);
			m_dwSpeedStart = GetTickCount();
			m_dwState = 1;
		}
		if ((speed >= 230.0f) && (speed <= 235.0f) && (m_dwState == 1))
		{
			pChatWindow->AddDebugMessage("100 to 200: %d", GetTickCount() - m_dwSpeedStart);
			m_dwSpeedStart = 0;
			m_dwState = 0;
		}
		// process for 100 and 200
	}
	else
	{
		if (m_dwSpeedStart)
		{
			m_dwSpeedStart = 0;
			m_dwState = 0;
			pChatWindow->AddDebugMessage("Reseted");
			return;
		}
	}
}
