#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"

extern CGame *pGame;
extern CNetGame* pNetGame;

void CPickupPool::Free()
{
	for(int i=0; i<MAX_PICKUPS; i++)
	{
		auto pPickup = GetAt(i);
		if(pPickup)
			ScriptCommand(&destroy_pickup, pPickup->dwGtaId);
	}
}

void CPickupPool::New(int pickupId, NEW_PICKUP_DATA *pPickup)
{
	if(GetAt(pickupId))
		Destroy(pickupId);

	auto newPickup = new PICKUP();
	newPickup->iModel = pPickup->iModel;
	newPickup->iType = pPickup->iType;
	newPickup->pos = pPickup->pos;

	newPickup->m_droppedWeapon.bDroppedWeapon = false;

	newPickup->dwGtaId = pGame->CreatePickup(pPickup->iModel, pPickup->iType, newPickup->pos.x, newPickup->pos.y, newPickup->pos.z, (int*)&newPickup->m_iPickupID);

	list[pickupId] = newPickup;
}

void CPickupPool::Destroy(int pickupId)
{
	auto pPickup = GetAt(pickupId);
	if(pPickup)
	{
		ScriptCommand(&destroy_pickup, pPickup->dwGtaId);

		delete pPickup;
		list.erase(pickupId);
	}
}

int CPickupPool::GetNumberFromID(int iPickup)
{
	for(auto &pair : list) {
		auto pPickup = pair.second;
		if (pPickup->m_iPickupID == iPickup)
			return pair.first;
	}

	return -1;
}

void CPickupPool::PickedUp(int iPickup)
{
	int index = GetNumberFromID(iPickup);
	if(index == -1)
		return;

	auto pPickup = GetAt(index);
	if(pPickup->m_iTimer == 0) {
		//Log("PickedUp = %d", index);
		if (pPickup->m_droppedWeapon.bDroppedWeapon) return;

		RakNet::BitStream bsPickup;
		bsPickup.Write(index);

		pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
		pPickup->m_iTimer = 15;
	}
}

void CPickupPool::Process()
{
	for(auto &pair : list) {
		auto pPickup = pair.second;
		auto pickupId = pair.first;

//		if (pPickup->m_iTimer > 0)
//			pPickup->m_iTimer--;

		if (pPickup->m_droppedWeapon.bDroppedWeapon || pPickup->iType == 14) {
			if (ScriptCommand(&is_pickup_picked_up, pPickup->dwGtaId)) {
				Log("Picked up %u", pickupId);
				RakNet::BitStream bsPickup;
				if (pPickup->m_droppedWeapon.bDroppedWeapon) {
					bsPickup.Write(pPickup->m_droppedWeapon.fromPlayer);
					pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
				} else {
					bsPickup.Write(pickupId);
					pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
				}

			}
		} else if (pPickup->m_iTimer > 0)
			pPickup->m_iTimer--;

	}
}