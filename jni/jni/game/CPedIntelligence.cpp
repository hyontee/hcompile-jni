//
// Created by plaka on 23.02.2023.
//

#include "CPedIntelligence.h"
#include "patch.h"

void CPedIntelligence__Process(CPedIntelligence *a1){
    CHook::CallFunction<void>("_ZN15CVehicleScanner22ScanForVehiclesInRangeERK4CPed",
                              &a1->m_vehicleScanner, a1->m_pPed);
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x4BAEC0 + 1 : 0x5B6608), &a1->m_pedScanner, a1->m_pPed);
    CHook::CallFunction<void>("_ZN13CEventScanner13ScanForEventsER4CPed", &a1->m_eventScanner,
                              a1->m_pPed);
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x37B818 + 1 : 0x45084C), &a1->m_eventHandler);
    CHook::CallFunction<void>("_ZN12CTaskManager11ManageTasksEv", &a1->m_TaskMgr);
}

void CPedIntelligence::InjectHooks(){
//#if VER_x32
    //CHook::HookRedirect("_ZN16CPedIntelligence7ProcessEv", CPedIntelligence__Process);
//#endif
}
