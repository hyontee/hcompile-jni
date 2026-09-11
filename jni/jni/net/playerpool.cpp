#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "..//voice/CVoiceChatClient.h"
#include "CSettings.h"

extern CVoiceChatClient* pVoice;
extern CSettings* pSettings;

int g_iStatusDriftChanged = 0;

void CPlayerPool::Init()
{
	m_pLocalPlayer = new CLocalPlayer();
}

void CPlayerPool::Free()
{
//	delete m_pLocalPlayer;
//	m_pLocalPlayer = nullptr;

	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
		Delete(playerId, 0);
}

#include "..//chatwindow.h"
#include "..//game/game.h"
#include "..//net/netgame.h"
#include "playerpool.h"
extern CGame* pGame;
extern CNetGame* pNetGame;


void CPlayerPool::ApplyCollisionChecking()
{
	for(auto & pair : spawnedPlayers) {
		auto pPed = pair.second->GetPlayerPed();
		if(pPed) {
			if (!pPed->IsInVehicle()) {
				m_bCollisionChecking[pair.first] = pPed->GetCollisionChecking();
				pPed->SetCollisionChecking(true);
			}
		}
	}
}

void CPlayerPool::ResetCollisionChecking()
{
	for(auto & pair : spawnedPlayers) {
		auto pPed = pair.second->GetPlayerPed();
		if(pPed) {
			if (!pPed->IsInVehicle()) {
				m_bCollisionChecking[pair.first] = pPed->GetCollisionChecking();
				pPed->SetCollisionChecking(false);
			}
		}
	}
}

void CPlayerPool::UpdateScore(PLAYERID playerId, int iScore)
{
	if (playerId == m_LocalPlayerID)
	{
		m_iLocalPlayerScore = iScore;
	}
	else
	{
		if (playerId > MAX_PLAYERS - 1) { return; }
		m_iPlayerScores[playerId] = iScore;
	}
}

void CPlayerPool::UpdatePing(PLAYERID playerId, uint32_t dwPing)
{
	if (playerId == m_LocalPlayerID)
	{
		m_dwLocalPlayerPing = dwPing;
	}
	else
	{
		if (playerId > MAX_PLAYERS - 1) { return; }
		m_dwPlayerPings[playerId] = dwPing;
	}
}

bool CPlayerPool::Process()
{
	for(auto & pair : spawnedPlayers) {
		auto pPlayer = pair.second;
		pPlayer->Process();

		CPlayerPed* pPed = m_pLocalPlayer->GetPlayerPed();
		CPlayerPed* pPedRemote = pPlayer->GetPlayerPed();
		if (pPed && pPedRemote && pPed->IsInVehicle())
		{
			if (pPed->GetGtaVehicle() == pPedRemote->GetGtaVehicle())
			{
				RwMatrix mat;
				pPed->GetMatrix(&mat);
				PLAYER_VOICE_POS pos;
				pos.pos.x = mat.pos.x;
				pos.pos.y = mat.pos.y;
				pos.pos.z = mat.pos.z;
				if (pVoice) pVoice->PostNewPlayerInfo(pair.first, &pos);
				//pChatWindow->AddDebugMessage("Playerid %d is in the same vehicle", playerId);
			}
			else
			{
				if (pPedRemote)
				{
					RwMatrix mat;
					pPedRemote->GetMatrix(&mat);
					PLAYER_VOICE_POS pos;
					pos.pos.x = mat.pos.x;
					pos.pos.y = mat.pos.y;
					pos.pos.z = mat.pos.z;
					if (pVoice) pVoice->PostNewPlayerInfo(pair.first, &pos);
				}
			}
		}

		else
		{
			if (pPedRemote)
			{
				RwMatrix mat;
				pPedRemote->GetMatrix(&mat);
				PLAYER_VOICE_POS pos;
				pos.pos.x = mat.pos.x;
				pos.pos.y = mat.pos.y;
				pos.pos.z = mat.pos.z;
				if (pVoice) pVoice->PostNewPlayerInfo(pair.first, &pos);
			}
		}
	}

	m_pLocalPlayer->Process();

	if (g_iStatusDriftChanged)
	{
		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_CUSTOM_PACKET_SYSTEM);
		bs.Write((uint8_t)(pSettings->GetReadOnly().iSkyBox));

		pNetGame->GetRakClient()->Send(&bs, HIGH_PRIORITY, RELIABLE, 0);
		g_iStatusDriftChanged = 0;
	}

	CPlayerPed* pPed = m_pLocalPlayer->GetPlayerPed();
	if (pPed)
	{
		RwMatrix mat;
		RwMatrix cam;
		pPed->GetMatrix(&mat);

		pNetGame->GetStreamPool()->PostListenerMatrix(&mat);

		if (pGame->GetCamera())
		{
			pGame->GetCamera()->GetMatrix(&cam);
		}
		PLAYER_VOICE_POS pos;
		CVector up;
		CVector front;
		pos.pos.x = mat.pos.x;
		pos.pos.y = mat.pos.y;
		pos.pos.z = mat.pos.z;

		if (m_pLocalPlayer->IsSpectating())
		{
			if (m_pLocalPlayer->m_byteSpectateType == SPECTATE_TYPE_PLAYER)
			{
				CRemotePlayer* pSpectated = GetAt(m_pLocalPlayer->m_SpectateID);
				if (pSpectated)
				{
					if (pSpectated->GetPlayerPed())
					{
						pSpectated->GetPlayerPed()->GetMatrix(&mat);
						pos.pos.x = mat.pos.x;
						pos.pos.y = mat.pos.y;
						pos.pos.z = mat.pos.z;
					}
				}
			}
			if (m_pLocalPlayer->m_byteSpectateType == SPECTATE_TYPE_VEHICLE)
			{
				CVehicle* pSpectated = CVehiclePool::GetAt(m_pLocalPlayer->m_SpectateID);
				if (pSpectated)
				{
					pSpectated->GetMatrix(&mat);
					pos.pos.x = mat.pos.x;
					pos.pos.y = mat.pos.y;
					pos.pos.z = mat.pos.z;
				}

			}
		}

		up.x = cam.up.x;
		up.y = cam.up.y;
		up.z = cam.up.z;

		front.x = cam.at.x*(-1.0f);
		front.y = cam.at.y * (-1.0f);
		front.z = cam.at.z * (-1.0f);
		if (pVoice) pVoice->PostNewLocalPlayerInfo(&pos, &up, &front);
	}
	return true;
}

bool CPlayerPool::New(PLAYERID playerId, char *szPlayerName, bool IsNPC)
{
	if(GetAt(playerId))
		Delete( playerId, 0 );

	auto newPlayer = list[playerId] = new CRemotePlayer();

	strcpy(m_szPlayerNames[playerId], szPlayerName);
	newPlayer->SetID(playerId);
	newPlayer->SetNPC(IsNPC);
	return true;

}

bool CPlayerPool::Delete(PLAYERID playerId, uint8_t byteReason)
{
	if(!GetAt(playerId))
		return false;

	if(GetLocalPlayer()->IsSpectating() && GetLocalPlayer()->m_SpectateID == playerId)
		GetLocalPlayer()->ToggleSpectating(false);

	delete list[playerId];

	list.erase(playerId);

	if(pVoice) pVoice->UnMutePlayer(playerId);

	return true;
}

PLAYERID CPlayerPool::FindRemotePlayerIDFromGtaPtr(PED_TYPE * pActor)
{
	for(auto & pair : spawnedPlayers) {
		if(pair.second->GetPlayerPed()) {
			PED_TYPE *pTestActor = pair.second->GetPlayerPed()->GetGtaActor();
			if((pTestActor != NULL) && (pActor == pTestActor)) // found it
				return pair.first;
		}
	}
	return INVALID_PLAYER_ID;
}

