#ifndef AIMBOT_H
#define AIMBOT_H

#include "offsets.h"
#include "memory.h"
#include <cmath>
#include <vector>

struct Vector3 {
    float x, y, z;
    Vector3 operator-(const Vector3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
};

struct Player {
    uintptr_t ptr;
    Vector3 head;
    Vector3 pos;
    float health;
    int team;
    bool visible;
};

class Aimbot {
private:
    float smoothFactor = 0.15f;  // плавность (0-1)
    float fovLimit = 30.0f;      // угол обзора
    bool aimAtHead = true;
    uintptr_t localPtr;

public:
    void setLocal(uintptr_t ptr) { localPtr = ptr; }

    Player getClosestEnemy() {
        uintptr_t list = Memory::read<uintptr_t>(Memory::getBase() + OFFSET_ENTITY_LIST);
        int count = Memory::read<int>(Memory::getBase() + OFFSET_ENTITY_COUNT);
        Vector3 localPos = Memory::read<Vector3>(localPtr + OFF_POSITION);
        Vector3 localView = Memory::read<Vector3>(localPtr + OFF_VIEW_ANGLES);
        int localTeam = Memory::read<int>(localPtr + OFF_TEAM);

        Player best;
        float bestDist = fovLimit;

        for (int i = 0; i < count; i++) {
            uintptr_t playerPtr = Memory::read<uintptr_t>(list + i * 8);
            if (!playerPtr) continue;
            
            int team = Memory::read<int>(playerPtr + OFF_TEAM);
            if (team == localTeam) continue;
            
            float health = Memory::read<float>(playerPtr + OFF_HEALTH);
            if (health <= 0) continue;

            Vector3 head = Memory::read<Vector3>(playerPtr + OFF_HEAD);
            Vector3 delta = head - localPos;
            float distance = sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
            
            float pitch = -asin(delta.y / distance) * (180.0 / 3.14159);
            float yaw = atan2(delta.x, delta.z) * (180.0 / 3.14159);
            
            // Проверяем FOV
            float angleDiff = sqrt(pow(pitch - localView.x, 2) + pow(yaw - localView.y, 2));
            if (angleDiff < bestDist) {
                bestDist = angleDiff;
                best.ptr = playerPtr;
                best.head = head;
                best.health = health;
                best.team = team;
                best.visible = Memory::read<byte>(playerPtr + OFF_VISIBLE) == 1;
            }
        }
        return best;
    }

    void aimAt(Player target) {
        if (!target.ptr) return;
        
        Vector3 localPos = Memory::read<Vector3>(localPtr + OFF_POSITION);
        Vector3 currentView = Memory::read<Vector3>(localPtr + OFF_VIEW_ANGLES);
        Vector3 delta = target.head - localPos;
        
        float distance = sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
        float targetPitch = -asin(delta.y / distance) * (180.0 / 3.14159);
        float targetYaw = atan2(delta.x, delta.z) * (180.0 / 3.14159);

        // Сглаживание и предсказание (компенсация пинга)
        float pingComp = 0.05f; // 50 мс
        Vector3 predictedHead = target.head;
        predictedHead.x += 0.5f * pingComp;  // примерная коррекция
        
        float smoothPitch = currentView.x + (targetPitch - currentView.x) * smoothFactor;
        float smoothYaw = currentView.y + (targetYaw - currentView.y) * smoothFactor;
        
        Memory::write<float>(localPtr + OFF_VIEW_ANGLES, smoothPitch);
        Memory::write<float>(localPtr + OFF_VIEW_ANGLES + 4, smoothYaw);
    }

    void run() {
        if (!localPtr) return;
        Player target = getClosestEnemy();
        if (target.ptr && target.visible) {
            aimAt(target);
        }
    }
};

#endif