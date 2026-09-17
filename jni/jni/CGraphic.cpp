#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>
#include "CGraphic.h"
#include "../util/armhook.h"
#include "..//main.h"
#include "..//settings.h"

extern CSettings* pSettings;

void SetMemoryValue(uintptr_t address, int value) {
    *(int*)(address) = value;
}

void CGraphic::ApplyReflections(int reflection) {
    SetMemoryValue(g_libGTASA + 0x63E464, reflection);
}

void CGraphic::ApplyResolution(int resolution) {
    SetMemoryValue(g_libGTASA + 0x63E3A4, resolution);
}

void CGraphic::ApplyEffects(int effects) {
    SetMemoryValue(g_libGTASA + 0x63E384, effects);
}

        // GRAPHIC SETTINGS
            //if(*(int*)(g_libGTASA + 0x985260) != 0x87F40000){ *(int*)(g_libGTASA + 0x985260) = 0x985260) = 0x87F40000; } // LOD DISTANCE
            //if (*(int*)(g_libGTASA + 0x63E3A4) != 100) { *(int*)(g_libGTASA + 0x63E3A4) = 100; } // RES
            //if (*(int*)(g_libGTASA + 0x63E464) != 4) { *(int*)(g_libGTASA + 0x63E464) = 3; } // REFLECTION
            //if (*(int*)(g_libGTASA + 0x63E3C4) != 100) { *(int*)(g_libGTASA + 0x63E3C4) = 100; } // DRAW DISTANCE
            //if (*(int*)(g_libGTASA + 0x63E324) != 2) { *(int*)(g_libGTASA + 0x63E324) = 2; } // SHADOWS
            //if (*(int*)(g_libGTASA + 0x63E384) != 3) { *(int*)(g_libGTASA + 0x63E384) = 3; } // EFFECTS