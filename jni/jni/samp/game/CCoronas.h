//
// Created by Error on 08.12.2024.
//

#ifndef WEIKTON_CCORONAS_H
#define WEIKTON_CCORONAS_H

#include "jni.h"
#include "common.h"
#include "RegisteredCorona.h"


class CCoronas {
public:
    static void RegisterCorona(uint32 id, CEntityGta* attachTo, uint8 red, uint8 green, uint8 blue, uint8 alpha, const CVector& posn,
                               float radius, float farClip, RwTexture* texture, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int32 _param_not_used,
                               float angle, bool longDistance, float nearClip, uint8 fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);
    static void RegisterCorona(uint32 id, CEntityGta* attachTo, uint8 red, uint8 green, uint8 blue, uint8 alpha, const CVector* posn,
                               float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int32 _param_not_used,
                               float angle, bool longDistance, float nearClip, uint8 fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);
};

#endif //WEIKTON_CCORONAS_H
