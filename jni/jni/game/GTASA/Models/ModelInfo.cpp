//
// Created by plaka on 21.02.2023.
//

#include "ModelInfo.h"
#include "util/patch.h"
#include "..//util/armhook.h"
CBaseModelInfo *CModelInfo::ms_modelInfoPtrs[30000] {};

/*CStore<CPedModelInfo, CModelInfo::NUM_PED_MODEL_INFOS> CModelInfo::ms_pedModelInfoStore;
CStore<CAtomicModelInfo, CModelInfo::NUM_ATOMIC_MODEL_INFOS> CModelInfo::ms_atomicModelInfoStore;*/

CStore<CVehicleModelInfo, 600> CModelInfo::ms_vehicleModelInfoStore;
typedef struct _VEHICLE_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[932];
} VEHICLE_MODEL; // SIZE = 936

VEHICLE_MODEL VehicleModels[370];
int VehicleModelsCount = 0;

VEHICLE_MODEL* (*CModelInfo_AddVehicleModel)(int id);
VEHICLE_MODEL* CModelInfo_AddVehicleModel_hook(int id)
{

	
	VEHICLE_MODEL* model = &VehicleModels[VehicleModelsCount];
	memset(model, 0, sizeof(VEHICLE_MODEL));

	((void(*)(void* thiz))(g_libGTASA + 0x00337AA0 + 1))((void*)model); // CVehicleModelInfo::CVehicleModelInfo();

	model->vtable = (uintptr_t)(g_libGTASA + 0x005C6EE0);			// assign CVehicleModelInfo vmt

	((uintptr_t(*)(VEHICLE_MODEL*))(*(void**)(model->vtable + 0x1C)))(model); // CVehicleModelInfo::Init()

	*(VEHICLE_MODEL * *)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	VehicleModelsCount++;
	auto& pInfo = CModelInfo::ms_vehicleModelInfoStore.AddItem();
	
	((void(*)(void* thiz))(g_libGTASA + 0x00337AA0 + 1))((void*)&pInfo);
	
	pInfo.vtable = g_libGTASA + 0x005C6EE0;
	
	((uintptr_t(*)(CVehicleModelInfo*))(*(void**)(pInfo.vtable + 0x1C)))(&pInfo);
	
	CModelInfo::SetModelInfo(id, &pInfo);
	return model;
}
void CModelInfo::injectHooks()
{
//	auto* dwModelArray = (uintptr_t*)(SA_ADDR(0x87BF48));
	/*CModelInfo::ms_modelInfoPtrs = dwModelArray;*/
	//CHook::Write(g_libGTASA + 0x005D1634, &CModelInfo::ms_modelInfoPtrs);
//	memcpy(CModelInfo::ms_modelInfoPtrs, dwModelArray, sizeof(CBaseModelInfo*) * 30000);
  //  CHook::Write(g_libGTASA + 0x005D1634, &CModelInfo::ms_modelInfoPtrs);
  SetUpHook(g_libGTASA + 0x00336618, (uintptr_t)CModelInfo_AddVehicleModel_hook, (uintptr_t*)& CModelInfo_AddVehicleModel); 
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

