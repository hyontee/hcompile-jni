#ifndef AIMBOT_H
#define AIMBOT_H

#include "offsets.h"
#include "memory.h"
#include <cmath>

struct Vector3 {
    float x, y, z;
    Vector3 operator-(const Vector3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
};

class Aimbot {
private:
    float smooth = 0.15f;
    float fov = 30.0f;
    uintptr_t local = 0;

public:
    void setLocal(uintptr_t ptr) { local = ptr; }

    void run() {
        if (!local) return;

        uintptr_t list = Memory::read<uintptr_t>(Memory::getBase() + OFFSET_ENTITY_LIST);
        int count = Memory::read<int>(Memory::getBase() + OFFSET_ENTITY_COUNT);
        Vector3 localPos = Memory::read<Vector3>(local + OFF_POSITION);
        Vector3 localView = Memory::read<Vector3>(local + OFF_VIEW_ANGLES);
        int localTeam = Memory::read<int>(local + OFF_TEAM);

        float bestDist = fov;
        uintptr_t target = 0;
        Vector3 targetHead = {0,0,0};

        for (int i = 0; i < count; i++) {
            uintptr_t player = Memory::read<uintptr_t>(list + i * 8);
            if (!player || player == local) continue;

            int team = Memory::read<int>(player + OFF_TEAM);
            if (team == localTeam) continue;

            float health = Memory::read<float>(player + OFF_HEALTH);
            if (health <= 0) continue;

            Vector3 head = Memory::read<Vector3>(player + OFF_HEAD);
            Vector3 delta = head - localPos;
            float dist = sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);

            float pitch = -asin(delta.y / dist) * (180.0 / 3.14159);
            float yaw = atan2(delta.x, delta.z) * (180.0 / 3.14159);

            float angleDiff = sqrt(pow(pitch - localView.x, 2) + pow(yaw - localView.y, 2));
            if (angleDiff < bestDist) {
                bestDist = angleDiff;
                target = player;
                targetHead = head;
            }
        }

        if (target) {
            Vector3 delta = targetHead - localPos;
            float dist = sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
            float targetPitch = -asin(delta.y / dist) * (180.0 / 3.14159);
            float targetYaw = atan2(delta.x, delta.z) * (180.0 / 3.14159);

            Memory::write<float>(local + OFF_VIEW_ANGLES,
                localView.x + (targetPitch - localView.x) * smooth);
            Memory::write<float>(local + OFF_VIEW_ANGLES + 4,
                localView.y + (targetYaw - localView.y) * smooth);
        }
    }
};

#endif