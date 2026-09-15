#include "../main.h"
#include "../game/game.h"
#include "../chatwindow.h"
#include "../net/netgame.h"
#include "../CSettings.h"
#include "..//util/armhook.h"

extern CGame *pGame;
extern CChatWindow *pChatWindow;
extern CNetGame *pNetGame;
extern CSettings *pSettings;

CPickupPool::CPickupPool()
{
	memset(&m_Pickups[0], 0, sizeof(PICKUP)*MAX_PICKUPS);
	for (int i = 0; i < MAX_PICKUPS; i ++)
	{
		m_dwHnd[i] = 0;
		m_iTimer[i] = 0;
		m_iPickupID[i] = -1;
	}
	
	m_PickupCounts.clear();
}

CPickupPool::~CPickupPool()
{
	for (int i = 0; i < MAX_PICKUPS; i ++)
	{
		if(m_dwHnd[i] != 0) ScriptCommand(&destroy_pickup, m_dwHnd[i]);
		m_dwHnd[i] = 0;
		m_iTimer[i] = 0;
		m_iPickupID[i] = -1;
	}
	
	m_PickupCounts.clear();
}

void CPickupPool::New(PICKUP *pPickup, int iPickup)
{
	if(iPickup < 0 || iPickup >= MAX_PICKUPS) return;
	if(m_dwHnd[iPickup] != 0) Destroy(iPickup);
	m_Pickups[iPickup].iModel = pPickup->iModel;
	m_Pickups[iPickup].iType = pPickup->iType;
	m_Pickups[iPickup].fX = pPickup->fX;
	m_Pickups[iPickup].fY = pPickup->fY;
	m_Pickups[iPickup].fZ = pPickup->fZ;
	m_droppedWeapon[iPickup].bDroppedWeapon = false;

	int pickupid = 0;
	m_dwHnd[iPickup] = pGame->CreatePickup(pPickup->iModel, pPickup->iType, pPickup->fX, pPickup->fY, pPickup->fZ, &pickupid);
	m_iPickupID[iPickup] = pickupid;
	
	if (m_dwHnd[iPickup]) m_PickupCounts.push_back(iPickup);
}

void CPickupPool::Destroy(int iPickup)
{
	if(iPickup < 0 || iPickup >= MAX_PICKUPS) return;
	if(m_dwHnd[iPickup] != 0) 
	{
		//ScriptCommand(&destroy_pickup, m_dwHnd[iPickup]);
		((void (*)(int))(SA_ADDR(0x2D7FF0 + 1)))(m_dwHnd[iPickup]); // CPickups::RemovePickUp
		m_PickupCounts.remove(iPickup);
	}
	m_dwHnd[iPickup] = 0;
	m_iTimer[iPickup] = 0;
	m_iPickupID[iPickup] = -1;
}

int CPickupPool::GetNumberFromID(int iPickup)
{
	for (int i: m_PickupCounts)
	{
		if(m_iPickupID[i] == iPickup)
			return i;
	}

	return -1;
}

int m_ActivePickup = -1;
uint32_t m_ActiveTickCount = 0;
uint32_t m_PickupInterval = GetTickCount();
void CPickupPool::PickedUp(int iPickup)
{
	if (m_ActivePickup != -1 && m_ActivePickup == iPickup)
	{
		if (GetTickCount() - m_ActiveTickCount > 3000)
			m_ActivePickup = -1;

		else
			return;
	}
	
	int index = GetNumberFromID(iPickup);

	Log("CPickupPool::PickedUp index = %d", index);

	if(index < 0 || index >= MAX_PICKUPS) return;
	if(m_dwHnd[index] != 0 && GetTickCount() - m_iTimer[index] > 500)
	{
		if(m_droppedWeapon[index].bDroppedWeapon) return;
	
		m_ActivePickup = iPickup;
		m_ActiveTickCount = GetTickCount();

		RakNet::BitStream bsPickup;
		bsPickup.Write(index);

		pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
		
		m_iTimer[index] = GetTickCount();
		m_PickupInterval = GetTickCount();
	}
}

void CPickupPool::Process()
{
	/*for (int i = 0; i < MAX_PICKUPS; i ++)
	{
		if(m_dwHnd[i] != 0)
		{
			if(m_droppedWeapon[i].bDroppedWeapon || m_Pickups[i].iType == 14)
			{
				if (ScriptCommand(&is_pickup_picked_up, m_dwHnd[i]))
				{
					Log(OBFUSCATE("Picked up %u"), i);
					RakNet::BitStream bsPickup;
					if(m_droppedWeapon[i].bDroppedWeapon)
					{
						bsPickup.Write(m_droppedWeapon[i].fromPlayer);
						pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
					}
					else
					{
						bsPickup.Write(i);
						pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
					}

				}
			}
			else if(m_iTimer[i] > 0)
				m_iTimer[i]--;
		}
	}*/

	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if(pPlayerPed && GetTickCount() - m_PickupInterval > 500)
	{
		m_PickupInterval = GetTickCount();
		
		for (int i: m_PickupCounts)
		{
			if(m_dwHnd[i] != 0)
			{
				if(m_droppedWeapon[i].bDroppedWeapon) continue;
		
				MATRIX4X4 playerMatrix;
				pPlayerPed->GetMatrix(&playerMatrix);
					
				VECTOR vecPos = { m_Pickups[i].fX, m_Pickups[i].fY, m_Pickups[i].fZ };
				if (m_ActivePickup != i && GetDistanceBetween3DPoints(&vecPos, &playerMatrix.pos) < 1)
				{
					RakNet::BitStream bsPickup;
					bsPickup.Write(i);

					pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
					
					m_ActivePickup = i;
					m_iTimer[i] = GetTickCount();
				}
				else m_ActivePickup = -1;
			}
		}
	}
}