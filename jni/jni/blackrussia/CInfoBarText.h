#pragma once
#include "../game/font.h"
#include <string>

class CInfoBarText
{
	static bool m_bIsEnabled;
public:
	static void Draw(int fHP, int fArmour);

	static void SetEnabled(bool bEnable);
};