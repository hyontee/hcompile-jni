#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

void CActorPool::Free()
{
	for (uint16_t actorId = 0; actorId < MAX_ACTORS; actorId++)
		Delete(actorId);
}

bool CActorPool::Spawn(uint16_t actorId, int iSkin, CVector vecPos, float fRotation, float fHealth, float bInvulnerable)
{
	if (!IsValidActorId(actorId))
		return false;

	if (GetAt(actorId))
		Delete(actorId);

	list[actorId] = new CActorPed((uint16_t)iSkin, vecPos, fRotation, fHealth, bInvulnerable);

	return true;
}

bool CActorPool::Delete(uint16_t actorId) 
{
	if (!IsValidActorId(actorId))
		return false;

	if(!GetAt(actorId))
		return false;

	delete list[actorId];

	list.erase(actorId);

	return true;
}