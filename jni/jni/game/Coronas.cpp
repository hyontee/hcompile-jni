#include "Coronas.h"
#include "../main.h"
#include "../util/armhook.h"

void CCoronas::RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha,
    const CVector* posn, float radius, float farClip, RwTexture* texture,
    eCoronaFlareType flareType, bool enableReflection, bool checkObstacles,
    int _param_not_used, float angle, bool longDistance, float nearClip,
    int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay)
{
    using Fn = void(*)(int, uintptr_t, int, int, int, int, const CVector*, float, float, RwTexture*,
                       eCoronaFlareType, bool, bool, int, float, bool, float, int, float, bool, bool);
    reinterpret_cast<Fn>(g_libGTASA + 0x52EA74 + 1)(
        id, attachTo, red, green, blue, alpha, posn, radius, farClip, texture,
        flareType, enableReflection, checkObstacles, _param_not_used, angle,
        longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

void CCoronas::RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha,
    const CVector* posn, float radius, float farClip, eCoronaType coronaType,
    eCoronaFlareType flareType, bool enableReflection, bool checkObstacles,
    int _param_not_used, float angle, bool longDistance, float nearClip,
    int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay)
{
    using Fn = void(*)(int, uintptr_t, int, int, int, int, const CVector*, float, float,
                       eCoronaType, eCoronaFlareType, bool, bool, int, float, bool,
                       float, int, float, bool, bool);
    reinterpret_cast<Fn>(g_libGTASA + 0x52EDD0 + 1)(
        id, attachTo, red, green, blue, alpha, posn, radius, farClip, coronaType,
        flareType, enableReflection, checkObstacles, _param_not_used, angle,
        longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

void CCoronas::InjectHooks()
{
    // The original JNJ implementation used CHook::Write().
    // DAG's hook layer has no CHook::Write API, so these globals are not patched here.
}

void CCoronas::Render()
{
    using Fn = void(*)();
    reinterpret_cast<Fn>(g_libGTASA + 0x52D528 + 1)();
}
