//
// Created by Error on 03.03.2025.
//

#ifndef LR64_CRADAR_H
#define LR64_CRADAR_H

#include "jni.h"
#include "main.h"
#include "Vector.h"

class CRadar {
public:
    static int SetCoordBlip(int r0, CVector pos, int r4);
    static void ChangeBlipScale(int32 nIndex, int32 NewScale);
    static void ChangeBlipColour(int32 nIndex, int32 nNewColour);
    static void SetBlipSprite(int32 nIndex, int32 BlipSprite);

};


#endif //LR64_CRADAR_H
