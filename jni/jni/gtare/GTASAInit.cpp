#include "../main.h"

#include "CHud.h"

void GTASAInit()
{
    //CModelInfo::Initialise();
    CHud::Sprites = (CSprite2d**)(g_libGTASA + 0x8F0704);
}