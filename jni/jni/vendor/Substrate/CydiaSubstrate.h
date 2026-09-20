#pragma once

// Compatibility shim for legacy MSHookFunction call sites.
// The project already ships Dobby, so use DobbyHook instead of requiring the
// separate Cydia Substrate SDK/header.
#include "../Dobby/include/dobby.h"

static inline void MSHookFunction(void* symbol, void* replace, void** result)
{
    if (!symbol || !replace)
        return;
    (void)DobbyHook(symbol, replace, result);
}
