#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CActorPacket.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

void CNetGame::packetActor(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		PLAYERID actorId;
		bs.Read(actorId);
		
		int32_t weapon;
		bs.Read(weapon);
		
		int32_t ammo;
		bs.Read(ammo);
		
		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{
			if (pActorPool->GetAt(actorId))
				pActorPool->GetAt(actorId)->GiveWeapon(weapon, ammo);
		}
	}
	else if (typeInfo == 1)
	{
		PLAYERID actorId;
		bs.Read(actorId);
		
		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{
			if (pActorPool->GetAt(actorId))
				pActorPool->GetAt(actorId)->ClearAllTasks();
		}
	}
	else if (typeInfo == 2)
	{
		PLAYERID actorId;
		bs.Read(actorId);
		
		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{
			if (pActorPool->GetAt(actorId))
				pActorPool->GetAt(actorId)->ClearWeapons();
		}
	}
	else if (typeInfo == 3)
	{
		PLAYERID actorId;
		bs.Read(actorId);
		
		PLAYERID playerId;
		bs.Read(playerId);

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pPlayerPool && pActorPool)
		{
			CPlayerPed* pPlayerPed = nullptr;
			if (playerId == pPlayerPool->GetLocalPlayerID()) 
			{
				pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
			}
			if (pPlayerPool->GetSlotState(playerId))
			{
				pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
			}
			
			if (pPlayerPed && pActorPool->GetAt(actorId))
				pActorPool->GetAt(actorId)->Attack(pPlayerPed);
		}
	}
	else if (typeInfo == 4)
	{
		PLAYERID actorId;
		bs.Read(actorId);
		
		float posX;
		bs.Read(posX);
		
		float posY;
		bs.Read(posY);
		
		float posZ;
		bs.Read(posZ);
		
		bool fastTravel;
		bs.Read(fastTravel);

		VECTOR vec = {posX, posY, posZ};
		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{
			if (pActorPool->GetAt(actorId))
				pActorPool->GetAt(actorId)->MoveToPoint(vec, fastTravel);
		}
	}
	else if (typeInfo == 5)
	{
		PLAYERID actorId;
		bs.Read(actorId);
		
		int8_t weapSkill;
		bs.Read(weapSkill);

		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{
			if (pActorPool->GetAt(actorId))
				pActorPool->GetAt(actorId)->SetWeaponSkill(weapSkill);
		}
	}
	else if (typeInfo == 6)
	{
		PLAYERID actorId;
		bs.Read(actorId);
		
		char text[2048];
		
		uint16_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		uint32_t dwColor;
		bs.Read(dwColor);

		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{
			if (pActorPool->GetAt(actorId))
				pActorPool->GetAt(actorId)->SetFloatingText(text, dwColor);
		}
	}
	else if (typeInfo == 7)
	{
		PLAYERID actorId;
		bs.Read(actorId);
		
		bool status;
		bs.Read(status);

		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{
			if (pActorPool->GetAt(actorId))
				pActorPool->GetAt(actorId)->setInvulnerable(status);
		}
	}
}
