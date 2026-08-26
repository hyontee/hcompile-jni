#pragma once
#include "vendor/Dobby/include/dobby.h"

void hook_Gui(unsigned int pkt, int ll, const char* buffGUI);
extern void (*orig_Gui)(unsigned int pkt, int ll, const char* buffGUI);
void SetupHook();