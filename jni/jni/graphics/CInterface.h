#pragma once

#include "../vendor/imgui/imgui.h"
#include "../game/RW/RenderWare.h"

class CInterface 
{
public:
    static void RenderHud();

	static uint8_t m_Level;
	static uint16_t m_CurentEXP;
	static uint16_t m_NextLvlEXP;
	static uint8_t m_iSatiety;
	static uint32_t m_iEventCoins;
	static uint32_t m_iDonate;
	static bool m_bUsedEvent;
	static bool m_bGZ;
	static bool m_bE2y;
	
private:
	static bool LoadedHudTexture;
};
