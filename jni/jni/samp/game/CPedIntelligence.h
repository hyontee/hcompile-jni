//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CPEDINTELLIGENCE_H
#define LIVERUSSIA_CPEDINTELLIGENCE_H


#include "CTaskManager.h"

class CPedIntelligence
{
public:
    class CPedGta* m_pPed;
    CTaskManager   m_TaskMgr;
    uintptr_t m_eventHandler;
    uint8_t pad1[VER_x32 ? 156 : 272];
    uintptr_t m_vehicleScanner;
    uint8_t pad2[VER_x32 ? 76 : 144];
    uintptr_t m_pedScanner;
    uint8_t pad4[VER_x32 ? 104 : 172];
    uint8_t m_eventScanner;
    static void InjectHooks();
};


#endif //LIVERUSSIA_CPEDINTELLIGENCE_H
