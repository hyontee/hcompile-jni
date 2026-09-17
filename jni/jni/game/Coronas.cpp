#include "Coronas.h"
#include "../util/patch.h"

void CCoronas::RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha, const CVector* posn, float radius, float farClip, RwTexture* texture, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay) {
    CHook::CallFunction<void>(g_libGTASA + 0x52EA74 + 1, id, attachTo, red, green, blue, alpha, posn, radius, farClip, texture, flareType, enableReflection, checkObstacles, _param_not_used, angle, longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

void CCoronas::RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha, const CVector* posn, float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay) {
    CHook::CallFunction<void>(g_libGTASA + 0x52EDD0 + 1, id, attachTo, red, green, blue, alpha, posn, radius, farClip, coronaType, flareType, enableReflection, checkObstacles, _param_not_used, angle, longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

void CCoronas::InjectHooks() {
    CHook::Write(g_libGTASA + 0x5D1A34, &CCoronas::SunScreenX);
    CHook::Write(g_libGTASA + 0x5D1704, &CCoronas::SunScreenY);

    CHook::Write(g_libGTASA + 0x5D1F40, &CCoronas::SunBlockedByClouds);
    CHook::Write(g_libGTASA + 0x5CDC50, &CCoronas::MoonSize);
}

void CCoronas::Render() {
    CHook::CallFunction<void>(g_libGTASA + 0x52D528 + 1);
}
