#include "../main.h"
#include "../game/RW/RenderWare.h"
#include "../game/game.h"

#include "CSnow.h"

uint32_t CSnow::m_dwParticeHandle1 = 0;
uint32_t CSnow::m_dwParticeHandle2 = 0;
uint32_t CSnow::m_dwLastTickCreated = 0;
int CSnow::m_iCurrentSnow = 0;

static const char* g_iSnows[] = { "SNOWFXII", "SNOWFXIII", "SNOWFXIV" };

void CSnow::Initialise()
{
	m_dwParticeHandle1 = 0;
	m_dwParticeHandle2 = 0;
	m_dwLastTickCreated = GetTickCount();
}
#include "..//chatwindow.h"
extern CChatWindow* pChatWindow;
void CSnow::Process(CPlayerPed* pPed, int iInterior)
{
	// -- люблю 60 фпс)
}

void CSnow::SetCurrentSnow(int iSnow)
{
	m_iCurrentSnow = iSnow;
}

int CSnow::GetCurrentSnow()
{
	return m_iCurrentSnow;
}
