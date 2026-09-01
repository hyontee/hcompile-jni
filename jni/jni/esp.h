#ifndef ESP_H
#define ESP_H

#include "offsets.h"
#include "memory.h"
#include <vector>

struct ScreenPoint {
    float x, y;
    bool visible;
};

class ESP {
private:
    uintptr_t matrixAddr;
    float* viewProjMatrix;

public:
    ESP() {
        matrixAddr = Memory::getBase() + OFFSET_MATRIX_VP;
        viewProjMatrix = (float*)matrixAddr;
    }

    // Проекция 3D -> 2D
    ScreenPoint worldToScreen(Vector3 world, int screenWidth, int screenHeight) {
        ScreenPoint result = {0, 0, false};
        Vector3 camPos = Memory::read<Vector3>(Memory::getBase() + OFFSET_CAMERA + OFF_CAMERA_POS);
        
        // Матричное умножение (упрощённое для скорости)
        float x = world.x * viewProjMatrix[0] + world.y * viewProjMatrix[4] + world.z * viewProjMatrix[8] + viewProjMatrix[12];
        float y = world.x * viewProjMatrix[1] + world.y * viewProjMatrix[5] + world.z * viewProjMatrix[9] + viewProjMatrix[13];
        float z = world.x * viewProjMatrix[2] + world.y * viewProjMatrix[6] + world.z * viewProjMatrix[10] + viewProjMatrix[14];
        float w = world.x * viewProjMatrix[3] + world.y * viewProjMatrix[7] + world.z * viewProjMatrix[11] + viewProjMatrix[15];

        if (w < 0.01f) return result;
        
        result.x = (screenWidth / 2.0f) + (x / w) * (screenWidth / 2.0f);
        result.y = (screenHeight / 2.0f) - (y / w) * (screenHeight / 2.0f);
        result.visible = (z / w) > 0;
        return result;
    }

    // Валлхак: форсим видимость всех игроков
    void forceVisibility() {
        uintptr_t list = Memory::read<uintptr_t>(Memory::getBase() + OFFSET_ENTITY_LIST);
        int count = Memory::read<int>(Memory::getBase() + OFFSET_ENTITY_COUNT);
        
        for (int i = 0; i < count; i++) {
            uintptr_t player = Memory::read<uintptr_t>(list + i * 8);
            if (player) {
                // Принудительно делаем всех видимыми
                Memory::write<byte>(player + OFF_VISIBLE, 1);
                // Также обнуляем флаг "за стеной" в структуре рендера
                Memory::write<byte>(player + 0x288, 0);  // дополнительный флаг
            }
        }
    }

    // Скин-свапер (меняем скины оружия клиентски)
    void forceSkin(uintptr_t weaponPtr, int skinID) {
        if (!weaponPtr) return;
        // В Standoff 2 скины хранятся как int в оффсете 0x3A0
        Memory::write<int>(weaponPtr + 0x3A0, skinID);
        // Принудительный ререндер
        Memory::write<byte>(weaponPtr + 0x3A4, 1);
    }
};

#endif