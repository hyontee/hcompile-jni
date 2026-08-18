//
// Created by plaka on 23.02.2023.
//

#include "CTaskManager.h"
#include "patch.h"

CTask * CTaskManager::FindActiveTaskByType(const CTaskManager *pTaskManager, const uint32_t iTaskType) {
    return CHook::CallFunction<CTask *>("_ZNK12CTaskManager20FindActiveTaskByTypeEi", pTaskManager,
                                        iTaskType);
}

