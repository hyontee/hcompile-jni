#include "Coronas.h"
#include "../util/armhook.h"

void CCoronas::RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha, const CVector* posn, float radius, float farClip, RwTexture* texture, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay) {
    reinterpret_cast<void(*)(int, uintptr_t, int, int, int, int, const CVector*, float, float, RwTexture*, eCoronaFlareType, bool, bool, int, float, bool, float, int, float, bool, bool)>(g_libGTASA + 0x52EA74 + 1)( id, attachTo, red, green, blue, alpha, posn, radius, farClip, texture, flareType, enableReflection, checkObstacles, _param_not_used, angle, longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

void CCoronas::RegisterCorona(int id, uintptr_t attachTo, int red, int green, int blue, int alpha, const CVector* posn, float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, int fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay) {
    reinterpret_cast<void(*)(int, uintptr_t, int, int, int, int, const CVector*, float, float, eCoronaType, eCoronaFlareType, bool, bool, int, float, bool, float, int, float, bool, bool)>(g_libGTASA + 0x52EDD0 + 1)( id, attachTo, red, green, blue, alpha, posn, radius, farClip, coronaType, flareType, enableReflection, checkObstacles, _param_not_used, angle, longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

void CCoronas::InjectHooks() {
    /* DAG hook API does not expose the JNJ CHook::Write API. */

}

void CCoronas::Render() {
    reinterpret_cast<void(*)()>(g_libGTASA + 0x52D528 + 1)();
}
