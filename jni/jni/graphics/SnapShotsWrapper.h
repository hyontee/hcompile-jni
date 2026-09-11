//
// Created by admin on 02.10.2023.
//

#pragma once

#include <unordered_map>
#include "../game/game.h"
#include <queue>
#include <jni.h>
#include <mutex>

class SnapShotsWrapper {
public:
    enum class Types {
        OBJECT,
        PED,
        VEHICLE
    };
    struct QueueItem {
        int id;
        int modelId;
        int type;
        float rotX;
        float rotY;
        float rotZ;
        float zoom;
        std::string vehNumber;
    };

    static inline std::queue<QueueItem> queue;
    static inline std::mutex queueMutex;

public:
    static void Process();

    static jbyteArray ConvertTexToBitMapBytes(RwTexture *tex);
};
