#include "../main.h"
#include "CModelInfo.h"
#include "CHud.h"

void GTASAInit()
{
    CModelInfo::ms_modelInfoPtrs = (CBaseModelInfo**)(g_libGTASA + 0x87BF48);
    CHud::Sprites = (CSprite2d**)(g_libGTASA + 0x8F0704);
}