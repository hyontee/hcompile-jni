//
// Created by plaka on 21.02.2023.
//

#include "ModelInfo.h"
#include "util/patch.h"
#include "..//util/armhook.h"

CBaseModelInfo *CModelInfo::ms_modelInfoPtrs[NUM_MODEL_INFOS];

/*CStore<CPedModelInfo, CModelInfo::NUM_PED_MODEL_INFOS> CModelInfo::ms_pedModelInfoStore;
CStore<CAtomicModelInfo, CModelInfo::NUM_ATOMIC_MODEL_INFOS> CModelInfo::ms_atomicModelInfoStore;*/
CStore<CVehicleModelInfo, CModelInfo::NUM_VEHICLE_MODEL_INFOS> CModelInfo::ms_vehicleModelInfoStore;

void CModelInfo::InjectHooks()
{
    /*WriteMemory(g_libGTASA + 0x00336AF8, CModelInfo::ms_vehicleModelInfoStore, 0);
    WriteMemory(g_libGTASA + 0x0087BF48, CModelInfo::ms_modelInfoPtrs, 0);*/

   // (g_libGTASA, 0x336618, &CModelInfo::AddVehicleModel);
    //CustomSetUpHook(g_libGTASA + 0x00336618, (uintptr_t*)&CModelInfo::AddVehicleModel);

}
CVehicleModelInfo* CModelInfo::AddVehicleModel(int index)
{
    auto& pInfo = CModelInfo::ms_vehicleModelInfoStore.AddItem();

    ((void(*)(CVehicleModelInfo*))(g_libGTASA + 0x00337AA0 + 1))(&pInfo); // CVehicleModelInfo::CVehicleModelInfo(); 0x00337AA0 + 1

    //pInfo.vtable = g_libGTASA + 0x006676A8; 2.10

    pInfo.vtable = (uintptr_t)(g_libGTASA + 0x005C6EE0); // assign CVehicleModelInfo vmt

    ((void(*)(CVehicleModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo); // CVehicleModelInfo::Init()

    //*(VEHICLE_MODEL * *)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

    Log("ну какая ты шлюха шлюха = " + index);
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

/*CPedModelInfo* CModelInfo::AddPedModel(int index)
{
    auto& pInfo = CModelInfo::ms_pedModelInfoStore.AddItem();

    ((void(*)(CPedModelInfo*))(g_libGTASA + 0x00384FD8 + 1))(&pInfo); // CBaseModelInfo::CBaseModelInfo();

    pInfo.vtable = g_libGTASA + 0x00667658;

    ((void(*)(CPedModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CAtomicModelInfo* CModelInfo::AddAtomicModel(int index)
{
    auto& pInfo = ms_atomicModelInfoStore.AddItem();

    ((void(*)(CAtomicModelInfo*))(g_libGTASA + 0x00384FD8 + 1))(&pInfo);

    pInfo.vtable = g_libGTASA + 0x00667444;

    ((void(*)(CAtomicModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}*/

