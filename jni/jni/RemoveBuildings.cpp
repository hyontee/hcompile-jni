//
// Created by plaka on 08.05.2023.
//

#include "RemoveBuildings.h"
#include "game/util.h"

void CRemoveBuildings::addObjectToList(uint32_t &modelId, CVector &pos, float &range) {
    m_aObjectsToRemove.emplace_back(modelId, pos, range);
}

bool CRemoveBuildings::isNeedRender(CEntityGta *pEntity) {
    for (auto & obj : m_aObjectsToRemove)
    {
        if (obj.dwModel == pEntity->nModelIndex)
        {
            if (GetDistanceBetween3DPoints(&pEntity->GetPosition(), &obj.vecPosition) <= obj.fRange)
            {
                return false;
            }
        }
    }
    return true;
}

void CRemoveBuildings::clearList() {
    m_aObjectsToRemove.clear();
}
