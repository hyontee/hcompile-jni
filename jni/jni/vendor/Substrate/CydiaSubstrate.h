#pragma once
// CydiaSubstrate compatibility stub — переадресует MSHookFunction на DobbyHook
// Оригинал: https://github.com/nicowillis/android-substrate
#include "../Dobby/include/dobby.h"

static inline void MSHookFunction(void* symbol, void* replace, void** result) {
    DobbyHook(symbol, replace, result);
}
