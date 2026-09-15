#pragma once

#define MAX_ACTORS 1000
#define INVALID_ACTOR_ID 0xFFFF

class CActorPool
{
private:
	bool m_bActorSlotState[MAX_ACTORS];
	CActorPed* m_pActors[MAX_ACTORS];
	std::list<uint16_t> m_CountActors;

public:
	CActorPool();
	~CActorPool();

	bool Spawn(uint16_t actorId, int iSkin, VECTOR vecPos, float fRotation, float fHealth, float bInvulnerable);
	bool Delete(uint16_t actorId);
	PLAYERID FindRemoteActorIDFromGtaPtr(PED_TYPE * pActor);
	void Process();
	void Render();
	void Draw(VECTOR* vec, float fDist, char* szInfo, uint32_t dwColor, float fHealth, float fArmour, bool draw);

	bool IsValidActorId(uint16_t actorId) 
	{
		if (actorId >= 0 && actorId < MAX_ACTORS) 
		{
			return true;
		}
		return false;
	};

	bool GetSlotState(uint16_t actorId) 
	{
		if (IsValidActorId(actorId)) 
		{
			return m_bActorSlotState[actorId];
		}
		return false;
	};

	CActorPed* GetAt(uint16_t actorId) 
	{
		if (!GetSlotState(actorId)) 
		{
			return nullptr;
		}
		return m_pActors[actorId];
	};
};