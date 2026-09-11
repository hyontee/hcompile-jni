#pragma once

#define MAX_PICKUPS 4096

#pragma pack(push, 1)
struct NEW_PICKUP_DATA
{
	int iModel;
	int iType;
	CVector pos;
};
#pragma pack(pop)

struct DROPPED_WEAPON
{
	bool bDroppedWeapon;
	PLAYERID fromPlayer;
};

struct PICKUP
{
	int iModel;
	int iType;
	CVector pos;

	uint32_t dwGtaId;
	uint32_t m_iPickupID{static_cast<uint32_t>(-1)};
	uint32_t m_iTimer{};
	DROPPED_WEAPON m_droppedWeapon{};
};

class CPickupPool
{
private:
	//static inline uint32_t 			m_dwHnd[MAX_PICKUPS]{};
	//static inline uint32_t 			m_iPickupID[MAX_PICKUPS]{static_cast<uint32_t>(-1)};
	//static inline uint32_t 			m_iTimer[MAX_PICKUPS]{};
	//static inline DROPPED_WEAPON 	m_droppedWeapon[MAX_PICKUPS];
	//static inline PICKUP* 			m_Pickups[MAX_PICKUPS]{};


public:
	static inline std::unordered_map<int, PICKUP*> list;

	static void Free();

	static void New(int pickupId, NEW_PICKUP_DATA *pPickup);
	static void Destroy(int iPickup);
	static void PickedUp(int iPickup);
	static void Process();

	static int GetNumberFromID(int id);

	static PICKUP* GetAt(int pickupId)
	{
		auto it = list.find(pickupId);
		if(it != list.end())
			return list[pickupId];

		return nullptr;
	}
};