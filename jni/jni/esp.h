#ifndef ESP_H
#define ESP_H

#include "offsets.h"
#include "memory.h"

class ESP {
public:
    void forceVisible() {
        uintptr_t list = Memory::read<uintptr_t>(Memory::getBase() + OFFSET_ENTITY_LIST);
        int count = Memory::read<int>(Memory::getBase() + OFFSET_ENTITY_COUNT);

        for (int i = 0; i < count; i++) {
            uintptr_t player = Memory::read<uintptr_t>(list + i * 8);
            if (player) {
                Memory::write<unsigned char>(player + OFF_VISIBLE, 1);
            }
        }
    }

    void noRecoil() {
        uintptr_t local = Memory::read<uintptr_t>(Memory::getBase() + OFFSET_LOCAL_PLAYER);
        if (local) {
            Memory::write<float>(local + OFF_RECOIL_X, 0.0f);
            Memory::write<float>(local + OFF_RECOIL_Y, 0.0f);
            Memory::write<float>(local + OFF_SPREAD, 0.001f);
        }
    }

    void speedFire() {
        uintptr_t local = Memory::read<uintptr_t>(Memory::getBase() + OFFSET_LOCAL_PLAYER);
        if (local) {
            float rate = Memory::read<float>(local + OFF_FIRE_RATE);
            if (rate > 0.05f) {
                Memory::write<float>(local + OFF_FIRE_RATE, rate * 0.5f);
            }
        }
    }
};

#endif