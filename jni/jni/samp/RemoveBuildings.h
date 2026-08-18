//
// Created by plaka on 08.05.2023.
//

#pragma once

#include <vector>
#include "game/Core/Vector.h"
#include "game/Entity/CEntityGta.h"

struct REMOVE_BUILDING {
    uint32_t    dwModel;
    float       fRange;
    CVector     vecPosition;

    constexpr REMOVE_BUILDING(uint32_t model, CVector vec, float range) {
        dwModel     = model;
        vecPosition = vec;
        fRange      = range/* * 2.0f*/; // tut радиус можно умножить
    };
};

class CRemoveBuildings {
public:
    static inline std::vector<REMOVE_BUILDING>	m_aObjectsToRemove;

public:
    static void addObjectToList(uint32_t &modelId, CVector& pos, float &range);
    static bool isNeedRender(CEntityGta *instance);
    static void clearList();
};

