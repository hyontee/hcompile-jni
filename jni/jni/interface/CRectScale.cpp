#include "../main.h"
#include "../game/game.h"
#include "CRectScale.h"
#include "..//chatwindow.h"

#include <sstream>
#include <iomanip>
extern CChatWindow* pChatWindow;
CVector2D CRectScale::m_aScale[E_HUD_ELEMENT::HUD_SIZE];

CVector2D CRectScale::GetElementScale(E_HUD_ELEMENT id)
{
	if (id < E_HUD_ELEMENT::HUD_HP || id >= E_HUD_ELEMENT::HUD_SIZE)
	{
		return CVector2D();
	}
	return m_aScale[id];
}

void CRectScale::SetElementScale(E_HUD_ELEMENT id, int x, int y)
{
	if (id < E_HUD_ELEMENT::HUD_HP || id >= E_HUD_ELEMENT::HUD_SIZE)
	{
		return;
	}

	m_aScale[id].X = x;
	m_aScale[id].Y = y;

	if (x <= 5)
	{
		m_aScale[id].X = -1;
	}
	if (y <= 5)
	{
		m_aScale[id].Y = -1;
	}
}
