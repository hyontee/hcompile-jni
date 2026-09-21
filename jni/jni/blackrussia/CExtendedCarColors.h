#pragma once

#include "../game/font.h"

class CExtendedCarColors
{
	static CRGBA ms_vehicleColourTable[256];
public:
	static void ApplyPatches_level0();
	static CRGBA& GetCarColorByID(uint8_t id);
};