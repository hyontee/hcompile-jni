#include "Coronas.h"
#include "../util/armhook.h"

void CCoronas::RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha, const CVector* posn, float radius, float farClip, RwTexture* texture, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay) {
    using RegisterCoronaFn = void (*)(int, uintptr_t, int, int, int, int, const CVector*, float, float, RwTexture*, eCoronaFlareType, bool, bool, int, float, bool, float, int, float, bool, bool);
    auto fn = reinterpret_cast<RegisterCoronaFn>(g_libGTASA + 0x52EA74 + 1);
    fn(id, attachTo, red, green, blue, alpha, posn, radius, farClip, texture, flareType, enableReflection, checkObstacles, _param_not_used, angle, longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

void CCoronas::RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha, const CVector* posn, float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay) {
    using RegisterCoronaFn = void (*)(int, uintptr_t, int, int, int, int, const CVector*, float, float, eCoronaType, eCoronaFlareType, bool, bool, int, float, bool, float, int, float, bool, bool);
    auto fn = reinterpret_cast<RegisterCoronaFn>(g_libGTASA + 0x52EDD0 + 1);
    fn(id, attachTo, red, green, blue, alpha, posn, radius, farClip, coronaType, flareType, enableReflection, checkObstacles, _param_not_used, angle, longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

void CCoronas::InjectHooks() {
    // Equivalent of the original CHook::Write() calls, using DAG STYLE's armhook API.
    WriteMemory(g_libGTASA + 0x5D1A34, (uintptr_t)&CCoronas::SunScreenX, sizeof(CCoronas::SunScreenX));
    WriteMemory(g_libGTASA + 0x5D1704, (uintptr_t)&CCoronas::SunScreenY, sizeof(CCoronas::SunScreenY));
    WriteMemory(g_libGTASA + 0x5D1F40, (uintptr_t)&CCoronas::SunBlockedByClouds, sizeof(CCoronas::SunBlockedByClouds));
    WriteMemory(g_libGTASA + 0x5CDC50, (uintptr_t)&CCoronas::MoonSize, sizeof(CCoronas::MoonSize));
}

void CCoronas::Render() {
    using RenderFn = void (*)();
    auto fn = reinterpret_cast<RenderFn>(g_libGTASA + 0x52D528 + 1);
    fn();
}
