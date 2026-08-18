#include "../main.h"
#include "game.h"

#include "sprite2d.h"

//2_1
void CSprite2d::Draw(float x, float y, float width, float height, CRGBA& color)
{
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005C8F20 + 1 : 0x6ED440), this, x, y, width, height, color);
}


CSprite2d::~CSprite2d()
{
	((int (*)(CSprite2d*))(g_libGTASA + 0x005C8856 + 1))(this);
}
