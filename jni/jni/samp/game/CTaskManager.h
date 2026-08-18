//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CTASKMANAGER_H
#define LIVERUSSIA_CTASKMANAGER_H

#include <stdint.h>

enum ePrimaryTasks //array indexes
{
    TASK_PRIMARY_PHYSICAL_RESPONSE = 0,
    TASK_PRIMARY_EVENT_RESPONSE_TEMP,
    TASK_PRIMARY_EVENT_RESPONSE_NONTEMP,
    TASK_PRIMARY_PRIMARY,
    TASK_PRIMARY_DEFAULT,
    TASK_PRIMARY_MAX
};

enum eSecondaryTasks //array indexes
{
    TASK_SECONDARY_ATTACK = 0,                // want duck to be after attack
    TASK_SECONDARY_DUCK,                    // because attack controls ducking movement
    TASK_SECONDARY_SAY,
    TASK_SECONDARY_FACIAL_COMPLEX,
    TASK_SECONDARY_PARTIAL_ANIM,
    TASK_SECONDARY_IK,
    TASK_SECONDARY_MAX
};
enum eAbortPriority : int32_t {
    ABORT_PRIORITY_LEISURE = 0,
    ABORT_PRIORITY_URGENT,
    ABORT_PRIORITY_IMMEDIATE
};
struct CTask{
    uintptr_t * _vptr$CTask;
    uintptr_t * m_pParent;
    uintptr_t * field_8;
    uintptr_t * field_C;



};
class CTaskManager
{
public:
    void* m_aPrimaryTasks[5];
    void* m_aSecondaryTasks[6];
    class CPedGta* m_pPed;

    static CTask *FindActiveTaskByType(const CTaskManager* pTaskManager, const uint32_t iTaskType);
    void FlushImmediately();

    void ApplyToRootTasks(const auto& fn) {
        for(auto & i : m_aPrimaryTasks) {
            fn(i);
        }
        for(auto & i : m_aSecondaryTasks) {
            fn(i);
        }
//        rng::for_each(m_aPrimaryTasks, fn);
//        rng::for_each(m_aSecondaryTasks, fn);
    }
};


#endif //LIVERUSSIA_CTASKMANAGER_H
