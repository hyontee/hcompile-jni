#pragma once

#define MAX_ACTORS 1000
#define INVALID_ACTOR_ID 0xFFFF

class CActorPool
{
private:
	static inline std::unordered_map<uint16_t, CActorPed*> list;

public:
	static void Free();

	static bool Spawn(uint16_t actorId, int iSkin, CVector vecPos, float fRotation, float fHealth, float bInvulnerable);
	static bool Delete(uint16_t actorId);

	static bool IsValidActorId(uint16_t actorId)
	{
		if (actorId >= 0 && actorId < MAX_ACTORS) 
		{
			return true;
		}
		return false;
	};

	static CActorPed* GetAt(uint16_t actorId)
	{
		auto it = list.find(actorId);
		if(it != list.end())
			return list[actorId];

		return nullptr;
	};
};