#include "RegisteredCorona.h"

inline const float invLerp(float fMin, float fMax, float fVal) {
    return (fVal - fMin) / (fMax - fMin);
}
auto CRegisteredCorona::CalculateIntensity(float scrZ, float farClip) const -> float {
    float value = invLerp(farClip, farClip / 2.f, scrZ);
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    return value;
}
