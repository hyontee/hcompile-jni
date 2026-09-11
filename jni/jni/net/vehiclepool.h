#pragma once

#include <unordered_map>

#pragma pack(push, 1)
typedef struct _NEW_VEHICLE
{
	VEHICLEID 	VehicleID;
	int 		iVehicleType;
	CVector		vecPos;
	float 		fRotation;
	uint8_t		aColor1;
	uint8_t		aColor2;
	float		fHealth;
	uint8_t		byteInterior;
	uint32_t	dwDoorDamageStatus;
	uint32_t 	dwPanelDamageStatus;
	uint8_t		byteLightDamageStatus;
	uint8_t		byteTireDamageStatus;
	uint8_t		byteAddSiren;
	uint8_t		byteModSlots[14];
	uint8_t	  	bytePaintjob;
	int	cColor1;
	int	cColor2;
} NEW_VEHICLE;
#pragma pack(pop)

class CVehiclePool
{
public:
	static void Free();

	static void Process();

	static void UpdateSpeed();

	static bool New(NEW_VEHICLE* pNewVehicle);
	static bool Delete(VEHICLEID VehicleID);

	static CVehicle* GetAt(VEHICLEID VehicleID)
	{
		auto it = list.find(VehicleID);
		if(it != list.end())
			return list[VehicleID];

		return nullptr;
	}

	static VEHICLEID FindIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle);
	static VEHICLEID FindIDFromRwObject(RwObject* pRWObject);
	static int FindGtaIDFromID(int ID);

	static void AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked);

	static int FindNearestToLocalPlayerPed();

	static void LinkToInterior(VEHICLEID VehicleID, int iInterior);

	static void NotifyVehicleDeath(VEHICLEID VehicleID);

	static bool VehicleCollisionProcess(int CColSphere1, int CColSphere2);

private:
	static inline std::unordered_map<VEHICLEID, CVehicle*> list;
};