#pragma once
#include "font.h"
#include <string>

#include "../vendor/imgui/imgui.h"

class CInfoBarText
{
	static bool m_bIsEnabled;
public:
	static void DrawHealth(int fHP);
	static void DrawArmour(int fArmour);

	static void SetEnabled(bool bEnable);
};