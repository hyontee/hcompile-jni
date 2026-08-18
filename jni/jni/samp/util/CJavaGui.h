//c
// Created by Error on 16.01.2025.
//

#ifndef LR64_CJAVAGUI_H
#define LR64_CJAVAGUI_H

#include <jni.h>
#include "raknet/NetworkTypes.h"


class CJavaGui {
    enum JavaGuiID {
        GASSTATION,
        CARSSHOWROOM,
        INVENTORY,
        TUNING,


        PAUSE,
        MaxId
    };
public:
    static jclass clazz;
    static bool drawHud;
    static bool drawChat;

    static void Create(int id);

    static void ReceivePacket(Packet* p);
    static void Destroy(int id);
    static void TempToggle(int id, bool toggle);

    static void DestroyAll();
};


#endif //LR64_CJAVAGUI_H
