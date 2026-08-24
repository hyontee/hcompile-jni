#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

#include "../util/armhook.h"
#include "../game/scripting.h"

#include "game/RW/common.h"
#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "../game/Custom/MasterCheat.h"
#include "../util/CJavaWrapper.h"
#include "CSettings.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;

extern CGame *pGame;
extern CNetGame *pNetGame;
extern MasterCheat *g_pMasterCheat;

bool bFirstSpawn = true;

extern int iNetModeNormalOnfootSendRate;
extern int iNetModeNormalInCarSendRate;
extern int iNetModeFiringSendRate;
extern bool bUsedPlayerSlots[];

#define IS_TARGETING(x) (x & 128)
#define IS_FIRING(x) (x & 4)

// SEND RATE TICKS
#define NETMODE_IDLE_ONFOOT_SENDRATE	80
#define NETMODE_NORMAL_ONFOOT_SENDRATE	30
#define NETMODE_IDLE_INCAR_SENDRATE		80
#define NETMODE_NORMAL_INCAR_SENDRATE	30

#define NETMODE_HEADSYNC_SENDRATE		1000
#define NETMODE_AIM_SENDRATE			100
#define NETMODE_FIRING_SENDRATE			30

#define LANMODE_IDLE_ONFOOT_SENDRATE	20
#define LANMODE_NORMAL_ONFOOT_SENDRATE	15
#define LANMODE_IDLE_INCAR_SENDRATE		30
#define LANMODE_NORMAL_INCAR_SENDRATE	15

#define STATS_UPDATE_TICKS				1000

CLocalPlayer::CLocalPlayer()
{
	m_pPlayerPed = pGame->FindPlayerPed();
	m_bIsActive = false;
	m_bIsWasted = false;

	m_iSelectedClass = 0;
	m_bHasSpawnInfo = false;
	m_bWaitingForSpawnRequestReply = false;
	m_bWantsAnotherClass = false;
	m_bInRCMode = false;

	memset(&m_OnFootData, 0, sizeof(ONFOOT_SYNC_DATA));

	for (size_t i = 0; i < MAX_VEHICLES; i++)
	{
		memset(&m_UnoccupiedData[i], 0, sizeof(UNOCCUPIED_SYNC_DATA));
	}

	m_dwLastSendTick = GetTickCount();
	m_dwLastSendAimTick = GetTickCount();
	m_dwLastSendSpecTick = GetTickCount();
	m_dwLastAimSendTick = m_dwLastSendTick;
	m_dwLastUpdateOnFootData = GetTickCount();
	m_dwLastStatsUpdateTick = GetTickCount();
	m_dwLastUpdateInCarData = GetTickCount();
	m_dwLastUpdatePassengerData = GetTickCount();
	m_dwPassengerEnterExit = GetTickCount();

	m_CurrentVehicle = 0;
	ResetAllSyncAttributes();

	m_bIsSpectating = false;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_SpectateID = 0xFFFFFFFF;

	uint8_t i;
	for (i = 0; i < 13; i++)
	{
		m_byteLastWeapon[i] = 0;
		m_dwLastAmmo[i] = 0;
	}

	m_surfData.bIsActive = false;
    m_surfData.pSurfInst = 0;
    m_surfData.bIsVehicle = false;
	m_surfData.vecOffsetPos = new CVector();
}

CLocalPlayer::~CLocalPlayer()
{
	
}

enum Flags {
    IDENTITY = 0x20000
};

enum Type {
    TYPENORMAL	= 1,
    TYPEORTHOGONAL	= 2,
    TYPEORTHONORMAL	= 3,
    TYPEMASK = 3
};

void mat_invertOrthonormal(RwMatrix *dst, const RwMatrix *src)
{
    dst->right.x = src->right.x;
    dst->right.y = src->up.x;
    dst->right.z = src->at.x;
    dst->up.x = src->right.y;
    dst->up.y = src->up.y;
    dst->up.z = src->at.y;
    dst->at.x = src->right.z;
    dst->at.y = src->up.z;
    dst->at.z = src->at.z;
    dst->pos.x = -(src->pos.x*src->right.x +
                   src->pos.y*src->right.y +
                   src->pos.z*src->right.z);
    dst->pos.y = -(src->pos.x*src->up.x +
                   src->pos.y*src->up.y +
                   src->pos.z*src->up.z);
    dst->pos.z = -(src->pos.x*src->at.x +
                   src->pos.y*src->at.y +
                   src->pos.z*src->at.z);
    dst->flags = TYPEORTHONORMAL;
}

RwMatrix* mat_invertGeneral(RwMatrix *dst, const RwMatrix *src)
{
    float det, invdet;
    // calculate a few cofactors
    dst->right.x = src->up.y*src->at.z - src->up.z*src->at.y;
    dst->right.y = src->at.y*src->right.z - src->at.z*src->right.y;
    dst->right.z = src->right.y*src->up.z - src->right.z*src->up.y;
    // get the determinant from that
    det = src->up.x * dst->right.y + src->at.x * dst->right.z + dst->right.x * src->right.x;
    invdet = 1.0;
    if(det != 0.0f)
        invdet = 1.0f/det;
    dst->right.x *= invdet;
    dst->right.y *= invdet;
    dst->right.z *= invdet;
    dst->up.x = invdet * (src->up.z*src->at.x - src->up.x*src->at.z);
    dst->up.y = invdet * (src->at.z*src->right.x - src->at.x*src->right.z);
    dst->up.z = invdet * (src->right.z*src->up.x - src->right.x*src->up.z);
    dst->at.x = invdet * (src->up.x*src->at.y - src->up.y*src->at.x);
    dst->at.y = invdet * (src->at.x*src->right.y - src->at.y*src->right.x);
    dst->at.z = invdet * (src->right.x*src->up.y - src->right.y*src->up.x);
    dst->pos.x = -(src->pos.x*dst->right.x + src->pos.y*dst->up.x + src->pos.z*dst->at.x);
    dst->pos.y = -(src->pos.x*dst->right.y + src->pos.y*dst->up.y + src->pos.z*dst->at.y);
    dst->pos.z = -(src->pos.x*dst->right.z + src->pos.y*dst->up.z + src->pos.z*dst->at.z);
    dst->flags &= ~IDENTITY;
    return dst;
}

RwMatrix* mat_invert(RwMatrix *dst, const RwMatrix *src)
{
    if(src->flags & IDENTITY)
        *dst = *src;
    else if((src->flags & TYPEMASK) == TYPEORTHONORMAL)
        mat_invertOrthonormal(dst, src);
    else
        return mat_invertGeneral(dst, src);
    return dst;
}

void CLocalPlayer::ResetAttachedObjects(){
    m_pPlayerPed->FlushAttach();
}

void CLocalPlayer::ResetAllSyncAttributes()
{
	m_byteCurInterior = 0;
	m_LastVehicle = INVALID_VEHICLE_ID;
	m_bInRCMode = false;
	memset(&m_aimSync, 0, sizeof(AIM_SYNC_DATA));
}

void CLocalPlayer::SendStatsUpdate()
{
	RakNet::BitStream bsStats;
	int iMoney = pGame->GetLocalMoney();
	uint32_t wAmmo = m_pPlayerPed->GetCurrentWeaponSlot()->dwAmmo;

	bsStats.Write((BYTE)ID_STATS_UPDATE);
	bsStats.Write(iMoney);
	bsStats.Write(wAmmo);
	pNetGame->GetRakClient()->Send(&bsStats, HIGH_PRIORITY, UNRELIABLE, 0);
}

void CLocalPlayer::CheckWeapons()
{
	if (m_pPlayerPed->IsInVehicle()) return;
	unsigned char i;
	bool bMSend = false;

	RakNet::BitStream bsWeapons;
	bsWeapons.Write((unsigned char)ID_WEAPONS_UPDATE);
	bsWeapons.Write((uint16_t)INVALID_PLAYER_ID);
	bsWeapons.Write((uint16_t)INVALID_PLAYER_ID);

	for (i = 0; i < 13; i++)
	{
		bool bSend = false;
		if (m_byteLastWeapon[i] != m_pPlayerPed->m_pPed->WeaponSlots[i].dwType)
		{
			m_byteLastWeapon[i] = (unsigned char)m_pPlayerPed->m_pPed->WeaponSlots[i].dwType;
			bSend = true;
		}

		if (m_dwLastAmmo[i] != m_pPlayerPed->m_pPed->WeaponSlots[i].dwAmmo)
		{
			m_dwLastAmmo[i] = m_pPlayerPed->m_pPed->WeaponSlots[i].dwAmmo;
			bSend = true;
		}

		if (bSend)
		{
			//pChatWindow->AddDebugMessage("Id: %u, Weapon: %u, Ammo: %d\n", i, m_byteLastWeapon[i], m_dwLastAmmo[i]);
			bsWeapons.Write((unsigned char)i);
			bsWeapons.Write((unsigned char)m_byteLastWeapon[i]);
			bsWeapons.Write((unsigned short)m_dwLastAmmo[i]);

			bMSend = true;
		}
	}

	if (bMSend)
		pNetGame->GetRakClient()->Send(&bsWeapons, HIGH_PRIORITY, UNRELIABLE, 0);
}

uint32_t CLocalPlayer::GetCurrentAnimationIndexFlag()
{
	uint32_t dwAnim = 0;

	float fBlendData = 4.0f;

	int iAnimIdx = m_pPlayerPed->GetCurrentAnimationIndex(fBlendData);

	uint32_t hardcodedBlend = 0b00000100;	// 4
	hardcodedBlend <<= 16;

	uint32_t hardcodedFlags = 0;

	if (iAnimIdx)
		hardcodedFlags = 0b00010001;
	else
	{
		hardcodedFlags = 0b10000000;
		iAnimIdx = 1189;
	}

	hardcodedFlags <<= 24;

	auto usAnimIdx = (uint16_t)iAnimIdx;

	dwAnim = (uint32_t)usAnimIdx;
	dwAnim |= hardcodedBlend;
	dwAnim |= hardcodedFlags;

	return dwAnim;
}

extern uint32_t g_uiHeadMoveEnabled;

unsigned long m_ulLastUpdateTime;

bool CLocalPlayer::Process()
{
	uint32_t dwThisTick = GetTickCount();

	if(m_bIsActive && m_pPlayerPed)
	{

		// handle dead
		if(!m_bIsWasted && m_pPlayerPed->GetActionTrigger() == ACTION_DEATH || m_pPlayerPed->IsDead())
		{
			ToggleSpectating(false);
			m_pPlayerPed->FlushAttach();
            pGame->FindPlayerPed()->TogglePlayerControllable(true);
            pGame->FindPlayerPed()->SetLockTogglePlayerControllable(false);
			// reset tasks/anims
            //m_pPlayerPed->SetTogglePlayerControllable(true, true);
            //m_pPlayerPed->SetTogglePlayerControllable(true, false);

			if(m_pPlayerPed->IsInVehicle() && !m_pPlayerPed->IsAPassenger())
			{
				SendInCarFullSyncData();
				m_LastVehicle = CVehiclePool::FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
			}

			m_pPlayerPed->SetHealth(0.0f);
			m_pPlayerPed->SetDead();
			SendWastedNotification();

			m_bIsActive = false;
			m_bIsWasted = true;

			return true;
		}

		/*if ((dwThisTick - m_dwLastStatsUpdateTick) > STATS_UPDATE_TICKS) 
		{
			SendStatsUpdate();
			m_dwLastStatsUpdateTick = dwThisTick;
		}*/

		CheckWeapons();
		CWeaponsOutFit::ProcessLocalPlayer(m_pPlayerPed);
        UpdateSurfing();
		if (m_pPlayerPed)
			m_pPlayerPed->ProcessSpecialAction();

		// handle interior changing
		/*uint8_t byteInterior = m_pPlayerPed->GetInterior();
		if (m_pPlayerPed->GetUpdatePlayer() && GetTickCount() - m_LastUpdateInterior > 4000)
		{
			m_LastUpdateInterior = GetTickCount();
			byteInterior = pGame->GetActiveInterior();
		}*/
		uint8_t byteInterior = m_pPlayerPed->GetInterior();
		if(byteInterior != m_byteCurInterior)
		{
			UpdateRemoteInterior(byteInterior);
		}
		// The new regime for adjusting sendrates is based on the number
		// of players that will be effected by this update. The more players
		// there are within a small radius, the more we must scale back
		// the number of sends.
		int iNumberOfPlayersInLocalRange=0;
		iNumberOfPlayersInLocalRange = DetermineNumberOfPlayersInLocalRange();
		if(!iNumberOfPlayersInLocalRange) iNumberOfPlayersInLocalRange = 10;

		// SPECTATING
		if(m_bIsSpectating)
			ProcessSpectating();

		// DRIVER
		else if(m_pPlayerPed->IsInVehicle() && !m_pPlayerPed->IsAPassenger())
		{
			CVehicle *pVehicle;

			m_CurrentVehicle = CVehiclePool::FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());

			pVehicle = CVehiclePool::GetAt(m_CurrentVehicle);

			if((dwThisTick - m_dwLastSendTick) > (unsigned int)GetOptimumInCarSendRate())
			{
				m_dwLastSendTick = GetTickCount();
				SendInCarFullSyncData();
			}
		}

		// ONFOOT
		else if(m_pPlayerPed->GetActionTrigger() == ACTION_NORMAL || m_pPlayerPed->GetActionTrigger() == ACTION_SCOPE)
		{

            ProcessSurfing();

			if ((dwThisTick - m_dwLastHeadUpdate) > 1000 && g_uiHeadMoveEnabled)
			{
				CVector LookAt;
				CAMERA_AIM* Aim = GameGetInternalAim();

				LookAt.x = Aim->pos1x + (Aim->f1x * 20.0f);
				LookAt.y = Aim->pos1y + (Aim->f1y * 20.0f);
				LookAt.z = Aim->pos1z + (Aim->f1z * 20.0f);

				pGame->FindPlayerPed()->ApplyCommandTask(OBFUSCATE("FollowPedSA"), 0, 2000, -1, &LookAt, 0, 0.1f, 500, 3, 0);

				m_dwLastHeadUpdate = dwThisTick;
			}

			if(m_CurrentVehicle != INVALID_VEHICLE_ID)
			{
				m_LastVehicle = m_CurrentVehicle;
				m_CurrentVehicle = INVALID_VEHICLE_ID;
			}

			if((dwThisTick - m_dwLastSendTick) > (unsigned int)GetOptimumOnFootSendRate() || LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] || LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] || LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK])
			{
				m_dwLastSendTick = GetTickCount();
				SendOnFootFullSyncData();
			}
			/*

			// TIMING FOR ONFOOT AIM SENDS
			uint16_t lrAnalog, udAnalog;
			uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

			// Not targeting or firing. We need a very slow rate to sync the head.
			if (!IS_TARGETING(wKeys) && !IS_FIRING(wKeys)) 
			{
				if ((dwThisTick - m_dwLastAimSendTick) > NETMODE_HEADSYNC_SENDRATE) 
				{
					m_dwLastAimSendTick = dwThisTick;
					SendAimSyncData();
				}
			}

			// Targeting only. Just synced for show really, so use a slower rate
			else if (IS_TARGETING(wKeys) && !IS_FIRING(wKeys))
			{
				if ((dwThisTick - m_dwLastAimSendTick) > (uint32_t)NETMODE_AIM_SENDRATE + (iNumberOfPlayersInLocalRange))
				{
					m_dwLastAimSendTick = dwThisTick;
					SendAimSyncData();
				}
			}

			// Targeting and Firing. Needs a very accurate send rate.
			else if (IS_TARGETING(wKeys) && IS_FIRING(wKeys)) 
			{
				if ((dwThisTick - m_dwLastAimSendTick) > (uint32_t)NETMODE_FIRING_SENDRATE + (iNumberOfPlayersInLocalRange))
				{
					m_dwLastAimSendTick = dwThisTick;
					SendAimSyncData();
				}
			}

			// Firing without targeting. Needs a normal onfoot sendrate.
			else if (!IS_TARGETING(wKeys) && IS_FIRING(wKeys)) 
			{
				if ((dwThisTick - m_dwLastAimSendTick) > (uint32_t)GetOptimumOnFootSendRate())
				{
					m_dwLastAimSendTick = dwThisTick;
					SendAimSyncData();
				}
			}*/
			bool bNeedAimSync = false;
            if (LocalPlayerKeys.bKeys[KEY_HANDBRAKE])
            {
            	bNeedAimSync = (dwThisTick - m_dwLastSendAimTick) > iNetModeFiringSendRate;
            }
            else
            {
            	bNeedAimSync = (dwThisTick - m_dwLastSendAimTick) > 1000;
            }

            if (bNeedAimSync) {
            	m_dwLastSendAimTick = dwThisTick;
            	SendAimSyncData();
            }
		}
		// PASSENGER
		else if(m_pPlayerPed->IsInVehicle() && m_pPlayerPed->IsAPassenger())
		{
			if((dwThisTick - m_dwLastSendTick) > (unsigned int)GetOptimumInCarSendRate())
			{
				m_dwLastSendTick = GetTickCount();
				SendPassengerFullSyncData();
				SendUnoccupiedSyncData();
			}
		}
	}

	// handle !IsActive spectating
	if(m_bIsSpectating && !m_bIsActive)
	{
		ProcessSpectating();
		return true;
	}

	// handle needs to respawn
	if(m_bIsWasted && (m_pPlayerPed->GetActionTrigger() != ACTION_WASTED) && (m_pPlayerPed->GetActionTrigger() != ACTION_DEATH) )
	{
		if(m_bClearedToSpawn && !m_bWantsAnotherClass && pNetGame->GetGameState() == GAMESTATE_CONNECTED)
		{
			if(m_pPlayerPed->GetHealth() > 0.0f)
				Spawn();
		}
		else
		{
			m_bIsWasted = false;
			HandleClassSelection();
			m_bWantsAnotherClass = false;
		}

		return true;
	}

	return true;
}

void CLocalPlayer::SendBulletSyncData(PLAYERID byteHitID, uint8_t byteHitType, CVector vecHitPos)
{
	if (!m_pPlayerPed) return;
	switch (byteHitType)
	{
	case BULLET_HIT_TYPE_NONE:
		break;
	case BULLET_HIT_TYPE_PLAYER:
		if (!CPlayerPool::GetSpawnedPlayer((PLAYERID)byteHitID)) return;
		break;

	}
	WEAPON_SLOT_TYPE* pwstWeapon = GetPlayerPed()->GetCurrentWeaponSlot();
	uint8_t byteCurrWeapon = pwstWeapon->dwType, byteShotWeapon;
	//uint8_t byteCurrWeapon = m_pPlayerPed->GetCurrentWeapon(), byteShotWeapon;

	RwMatrix matPlayer;
	BULLET_SYNC blSync;

	m_pPlayerPed->GetMatrix(&matPlayer);
	
	blSync.hitId = byteHitID;
	blSync.hitType = byteHitType;

	if (byteHitType == BULLET_HIT_TYPE_PLAYER)
	{
		float fDistance = CPlayerPool::GetSpawnedPlayer((PLAYERID)byteHitID)->GetPlayerPed()->GetDistanceFromLocalPlayerPed();
		if (byteCurrWeapon != 0 && fDistance < 1.0f)
			byteShotWeapon = 0;
		else
			byteShotWeapon = byteCurrWeapon;
	}
	else
	{
		byteShotWeapon = m_pPlayerPed->GetCurrentWeapon();
	}
	blSync.weapId = byteShotWeapon;

	blSync.hitPos[0] = vecHitPos.x;
	blSync.hitPos[1] = vecHitPos.y;
	blSync.hitPos[2] = vecHitPos.z;
	
	blSync.offsets[0] = 0.0f;
	blSync.offsets[1] = 0.0f;
	blSync.offsets[2] = 0.0f;
	
	RakNet::BitStream bsBulletSync;
	bsBulletSync.Write((uint8_t)ID_BULLET_SYNC);
	bsBulletSync.Write((const char*)& blSync, sizeof(BULLET_SYNC));
	pNetGame->GetRakClient()->Send(&bsBulletSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CLocalPlayer::SendWastedNotification()
{
	RakNet::BitStream bsPlayerDeath;
	PLAYERID WhoWasResponsible = INVALID_PLAYER_ID;
	
	uint8_t byteDeathReason = m_pPlayerPed->FindDeathReasonAndResponsiblePlayer(&WhoWasResponsible);

	bsPlayerDeath.Write(byteDeathReason);
	bsPlayerDeath.Write(WhoWasResponsible);

	pNetGame->GetRakClient()->RPC(&RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

bool CLocalPlayer::HandlePassengerEntryEx()
{
	if (GetTickCount() - m_dwPassengerEnterExit < 1000)
		return true;

	// CTouchInterface::IsHoldDown
	//int isHoldDown = (( int (*)(int, int, int))(SA_ADDR(0x270818 + 1)))(0, 1, 1);

	VEHICLEID ClosetVehicleID = CVehiclePool::FindNearestToLocalPlayerPed();
	if (ClosetVehicleID < MAX_VEHICLES && CVehiclePool::GetAt(ClosetVehicleID))
	{
		CVehicle* pVehicle = CVehiclePool::GetAt(ClosetVehicleID);
		if (pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f)
		{
			m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
			SendEnterVehicleNotification(ClosetVehicleID, true);
			m_dwPassengerEnterExit = GetTickCount();

			return true;
		}
	}

	return false;
}

void CLocalPlayer::HandleClassSelection()
{
	m_bClearedToSpawn = false;

	if(m_pPlayerPed)
	{
		m_pPlayerPed->SetInitialState();
		m_pPlayerPed->SetHealth(100.0f);
		m_pPlayerPed->TogglePlayerControllable(0);
	}
	
	RequestClass(m_iSelectedClass);
}

// 0.3.7
void CLocalPlayer::HandleClassSelectionOutcome()
{
	if(m_pPlayerPed)
	{
		m_pPlayerPed->ClearAllWeapons();
		m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
	}

	m_bClearedToSpawn = true;
}

void CLocalPlayer::SendNextClass()
{
	RwMatrix matPlayer;
	m_pPlayerPed->GetMatrix(&matPlayer);

	if(m_iSelectedClass == (pNetGame->m_iSpawnsAvailable - 1)) m_iSelectedClass = 0;
	else m_iSelectedClass++;

	pGame->PlaySound(1052, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z);
	RequestClass(m_iSelectedClass);
}

void CLocalPlayer::SendPrevClass()
{
	RwMatrix matPlayer;
	m_pPlayerPed->GetMatrix(&matPlayer);
	
	if(m_iSelectedClass == 0)
		m_iSelectedClass = (pNetGame->m_iSpawnsAvailable - 1);
	else m_iSelectedClass--;		

	pGame->PlaySound(1053,matPlayer.pos.x,matPlayer.pos.y,matPlayer.pos.z);
	RequestClass(m_iSelectedClass);
}

void CLocalPlayer::SendSpawn()
{
	RequestSpawn();
	m_bWaitingForSpawnRequestReply = true;
}

void CLocalPlayer::RequestClass(int iClass)
{
	RakNet::BitStream bsSpawnRequest;
	bsSpawnRequest.Write(iClass);
	pNetGame->GetRakClient()->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
}

void CLocalPlayer::RequestSpawn()
{
	RakNet::BitStream bsSpawnRequest;
	pNetGame->GetRakClient()->RPC(&RPC_RequestSpawn, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
}

uint32_t CLocalPlayer::GetPlayerColorAsARGB()
{
	return (TranslateColorCodeToRGBA(CPlayerPool::GetLocalPlayerID()) >> 8) | 0xFF000000;
}

bool CLocalPlayer::HandlePassengerEntry()
{
	if(GetTickCount() - m_dwPassengerEnterExit < 1000 )
		return true;

	// CTouchInterface::IsHoldDown
    int isHoldDown = (( int (*)(int, int, int))(SA_ADDR(0x270818 + 1)))(0, 1, 1);

	if (isHoldDown)
	{
		VEHICLEID ClosetVehicleID = CVehiclePool::FindNearestToLocalPlayerPed();
		if(ClosetVehicleID < MAX_VEHICLES && CVehiclePool::GetAt(ClosetVehicleID))
		{
			CVehicle* pVehicle = CVehiclePool::GetAt(ClosetVehicleID);
			if(pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f)
			{
				m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
				SendEnterVehicleNotification(ClosetVehicleID, true);
				m_dwPassengerEnterExit = GetTickCount();

				return true;
			}
		}
	}

	return false;
}

uint32_t CLocalPlayer::GetSpecialAction()
{
	CPlayerPed* pPed = GetPlayerPed();
	if (!pPed)
		return 0;

	if (pPed->IsCrouching())
		return 1;

	return 0;
}

void CLocalPlayer::SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger)
{
	RakNet::BitStream bsSend;
	uint8_t bytePassenger = 0;

	if(bPassenger)
		bytePassenger = 1;

	bsSend.Write(VehicleID);
	bsSend.Write(bytePassenger);

	pNetGame->GetRakClient()->RPC(&RPC_EnterVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0,false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::SendExitVehicleNotification(VEHICLEID VehicleID)
{
	RakNet::BitStream bsSend;

	CVehicle* pVehicle = CVehiclePool::GetAt(VehicleID);

	if(pVehicle)
	{ 
		if (!m_pPlayerPed->IsAPassenger()) 
			m_LastVehicle = VehicleID;

		bsSend.Write(VehicleID);
		pNetGame->GetRakClient()->RPC(&RPC_ExitVehicle,&bsSend,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,false, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void CLocalPlayer::UpdateRemoteInterior(uint8_t byteInterior)
{
	Log(OBFUSCATE("CLocalPlayer::UpdateRemoteInterior %d"), byteInterior);

	m_byteCurInterior = byteInterior;
	RakNet::BitStream bsUpdateInterior;
	bsUpdateInterior.Write(byteInterior);

	pNetGame->GetRakClient()->RPC(&RPC_SetInteriorId, &bsUpdateInterior, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CLocalPlayer::SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawn)
{
	memcpy(&m_SpawnInfo, pSpawn, sizeof(PLAYER_SPAWN_INFO));
	m_bHasSpawnInfo = true;
}

extern int showHud;
extern CSettings* pSettings;
bool CLocalPlayer::Spawn()
{
	if(!m_bHasSpawnInfo)
		return false;

	if(showHud) {
        if(pSettings->GetReadOnly().iNewHud)
            g_pJavaWrapper->ShowHUD(true);
		g_pJavaWrapper->CallLauncherActivity(1234);
		g_pJavaWrapper->CallLauncherActivity(1237);
		g_pJavaWrapper->CallLauncherActivity(1236);
		g_pJavaWrapper->ShowLogo(true);
	}

	CCamera *pGameCamera;
	pGameCamera = pGame->GetCamera();
	pGameCamera->Restore();
	pGameCamera->SetBehindPlayer();

	pGame->DisplayWidgets(true);
	pGame->DisplayHUD(true);
    pGame->FindPlayerPed()->TogglePlayerControllable(true);
    pGame->FindPlayerPed()->SetLockTogglePlayerControllable(false);
    //m_pPlayerPed->SetTogglePlayerControllable(true, true);
    //m_pPlayerPed->SetTogglePlayerControllable(true, false);
	
	if(!bFirstSpawn)
		m_pPlayerPed->SetInitialState();
	else bFirstSpawn = false;

	bFirstSpawn = false;

	pGame->RefreshStreamingAt(m_SpawnInfo.vecPos.x,m_SpawnInfo.vecPos.y);

	m_pPlayerPed->RestartIfWastedAt(&m_SpawnInfo.vecPos, m_SpawnInfo.fRotation);
	m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
	m_pPlayerPed->ClearAllWeapons();
	m_pPlayerPed->ResetDamageEntity();

	pGame->DisableTrainTraffic();

	WriteMemory(SA_ADDR(0x36EA2C), (uintptr_t)"\x70\x47", 2); // bx lr

	m_pPlayerPed->TeleportTo(m_SpawnInfo.vecPos.x, m_SpawnInfo.vecPos.y, (m_SpawnInfo.vecPos.z + 0.5f));

	m_pPlayerPed->ForceTargetRotation(m_SpawnInfo.fRotation);

	m_bIsWasted = false;
	m_bIsActive = true;
	m_bWaitingForSpawnRequestReply = false;
    m_surfData.bIsActive = false;

	RakNet::BitStream bsSendSpawn;
	pNetGame->GetRakClient()->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);

	return true;
}

uint32_t CLocalPlayer::GetPlayerColor()
{
	return TranslateColorCodeToRGBA(CPlayerPool::GetLocalPlayerID());
}

void CLocalPlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(CPlayerPool::GetLocalPlayerID(), dwColor);
}

void CLocalPlayer::ApplySpecialAction(uint8_t byteSpecialAction)
{
	if (m_pPlayerPed)
		m_pPlayerPed->SetPlayerSpecialAction(byteSpecialAction);

	switch(byteSpecialAction)
	{
		case SPECIAL_ACTION_NONE:
		break;

		case SPECIAL_ACTION_USEJETPACK:
		break;
	}
}

int CLocalPlayer::GetOptimumOnFootSendRate()
{
	if(!m_pPlayerPed)
		return 1000;

	return (iNetModeNormalOnfootSendRate + DetermineNumberOfPlayersInLocalRange());
}

int CLocalPlayer::GetOptimumInCarSendRate()
{
	if(!m_pPlayerPed)
		return 1000;

	return (iNetModeNormalInCarSendRate + DetermineNumberOfPlayersInLocalRange());
}

uint8_t CLocalPlayer::DetermineNumberOfPlayersInLocalRange()
{
	int iNumPlayersInRange = 0;

	for(int i = 2; i < PLAYER_PED_SLOTS; i++)
		if(bUsedPlayerSlots[i]) iNumPlayersInRange++;

	return iNumPlayersInRange;
}

void CLocalPlayer::SendOnFootFullSyncData()
{
	RakNet::BitStream bsPlayerSync;

	RwMatrix matPlayer;
	CVector vecMoveSpeed;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

	ONFOOT_SYNC_DATA ofSync;

	m_pPlayerPed->GetMatrix(&matPlayer);
	m_pPlayerPed->GetMoveSpeedVector(&vecMoveSpeed);

	ofSync.lrAnalog = lrAnalog;
	ofSync.udAnalog = udAnalog;
	ofSync.wKeys = wKeys;
	ofSync.vecPos.x = matPlayer.pos.x;
	ofSync.vecPos.y = matPlayer.pos.y;
	ofSync.vecPos.z = matPlayer.pos.z;

	ofSync.quat.SetFromMatrix(matPlayer);
	ofSync.quat.Normalize();

	if( FloatOffset(ofSync.quat.w, m_OnFootData.quat.w) < 0.00001 &&
		FloatOffset(ofSync.quat.x, m_OnFootData.quat.x) < 0.00001 &&
		FloatOffset(ofSync.quat.y, m_OnFootData.quat.y) < 0.00001 &&
		FloatOffset(ofSync.quat.z, m_OnFootData.quat.z) < 0.00001)
	{
		ofSync.quat.Set(m_OnFootData.quat);
	}

	ofSync.byteHealth = (uint8_t)m_pPlayerPed->GetHealth();
	ofSync.byteArmour = (uint8_t)m_pPlayerPed->GetArmour();

	uint8_t exKeys = GetPlayerPed()->GetExtendedKeys();
	ofSync.byteCurrentWeapon = (exKeys << 6) | ofSync.byteCurrentWeapon & 0x3F;
	WEAPON_SLOT_TYPE* pwstWeapon = GetPlayerPed()->GetCurrentWeaponSlot();
	ofSync.byteCurrentWeapon ^= (ofSync.byteCurrentWeapon ^ pwstWeapon->dwType) & 0x3F;
	//ofSync.byteCurrentWeapon ^= (ofSync.byteCurrentWeapon ^ GetPlayerPed()->GetCurrentWeapon()) & 0x3F;

	ofSync.byteSpecialAction = (uint8_t)GetSpecialAction();
	ofSync.vecMoveSpeed.x = vecMoveSpeed.x;
	ofSync.vecMoveSpeed.y = vecMoveSpeed.y;
	ofSync.vecMoveSpeed.z = vecMoveSpeed.z;

	ofSync.vecSurfOffsets.x = 0.0f;
	ofSync.vecSurfOffsets.y = 0.0f;
	ofSync.vecSurfOffsets.z = 0.0f;
	ofSync.wSurfInfo = 0;
    if(m_surfData.bIsActive){
        if(m_surfData.bIsVehicle && m_surfData.dwSurfVehID != INVALID_VEHICLE_ID){
            CVehicle* pVeh = (CVehicle*)m_surfData.pSurfInst;
            ofSync.vecSurfOffsets.x = m_surfData.vecOffsetPos.x;
            ofSync.vecSurfOffsets.y = m_surfData.vecOffsetPos.y;
            ofSync.vecSurfOffsets.z = m_surfData.vecOffsetPos.z;
            ofSync.wSurfInfo = m_surfData.dwSurfVehID;
        }
    }

/*
    if(m_surfData.bIsActive){
        if(m_surfData.bIsVehicle){
            static RwMatrix surfInstMatrix;
            static RwMatrix surfPedMatrix;
            CVehicle* pVeh = (CVehicle*) m_surfData.pSurfInst;
            pVeh->GetMatrix(&surfInstMatrix);
            m_pPlayerPed->GetMatrix(&surfPedMatrix);

            static RwMatrix matOut;
            mat_invert(&matOut, &surfInstMatrix);

            ofSync.wSurfInfo = pVeh->m_dwGTAId;
            ProjectMatrix(&m_surfData.vecOffsetPos, &matOut, &surfPedMatrix.pos);

            ofSync.vecSurfOffsets.x = m_surfData.vecOffsetPos.x;
            ofSync.vecSurfOffsets.y = m_surfData.vecOffsetPos.y;
            ofSync.vecSurfOffsets.z = m_surfData.vecOffsetPos.z;
        }
    }*/

	

	ofSync.dwAnimation = GetCurrentAnimationIndexFlag();

	if( (GetTickCount() - m_dwLastUpdateOnFootData) > 500 || memcmp(&m_OnFootData, &ofSync, sizeof(ONFOOT_SYNC_DATA)))
	{
		m_dwLastUpdateOnFootData = GetTickCount();

		bsPlayerSync.Write((uint8_t)ID_PLAYER_SYNC);
		bsPlayerSync.Write((char*)&ofSync, sizeof(ONFOOT_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		memcpy(&m_OnFootData, &ofSync, sizeof(ONFOOT_SYNC_DATA));
	}
}

void CLocalPlayer::SendInCarFullSyncData()
{
	RakNet::BitStream bsVehicleSync;

	RwMatrix matPlayer;
	CVector vecMoveSpeed;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);
	CVehicle *pVehicle;

	INCAR_SYNC_DATA icSync;
	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

	if(m_pPlayerPed)
	{
		icSync.VehicleID = CVehiclePool::FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());

		if(icSync.VehicleID == INVALID_VEHICLE_ID) return;

		icSync.lrAnalog = lrAnalog;
		icSync.udAnalog = udAnalog;
		icSync.wKeys = wKeys;

		pVehicle = CVehiclePool::GetAt(icSync.VehicleID);
		if(!pVehicle) return;

		pVehicle->GetMatrix(&matPlayer);
		pVehicle->GetMoveSpeedVector(&vecMoveSpeed);

		icSync.quat.SetFromMatrix(matPlayer);
		icSync.quat.Normalize();

		if(	FloatOffset(icSync.quat.w, m_InCarData.quat.w) < 0.00001 &&
			FloatOffset(icSync.quat.x, m_InCarData.quat.x) < 0.00001 &&
			FloatOffset(icSync.quat.y, m_InCarData.quat.y) < 0.00001 &&
			FloatOffset(icSync.quat.z, m_InCarData.quat.z) < 0.00001)
		{
			icSync.quat.Set(m_InCarData.quat);
		}

		// pos
		icSync.vecPos.x = matPlayer.pos.x;
		icSync.vecPos.y = matPlayer.pos.y;
		icSync.vecPos.z = matPlayer.pos.z;
		// move speed
		icSync.vecMoveSpeed.x = vecMoveSpeed.x;
		icSync.vecMoveSpeed.y = vecMoveSpeed.y;
		icSync.vecMoveSpeed.z = vecMoveSpeed.z;

		if (pVehicle->GetHealth() <= 300.0f)
			pVehicle->SetHealth(300.0f);

		icSync.fCarHealth = pVehicle->GetHealth();
		icSync.bytePlayerHealth = (uint8_t)m_pPlayerPed->GetHealth();
		icSync.bytePlayerArmour = (uint8_t)m_pPlayerPed->GetArmour();

		icSync.byteSirenOn = pVehicle->GetSirenState();

		icSync.HydraThrustAngle = pVehicle->m_iTurnState;

		//icSync.byteSirenOn = pVehicle->IsSirenOn() != 0;
		//icSync.byteLandingGearState = pVehicle->GetLandingGearState() != 0;
		uint8_t exKeys = GetPlayerPed()->GetExtendedKeys();
		icSync.byteCurrentWeapon = (exKeys << 6) | icSync.byteCurrentWeapon & 0x3F;
		WEAPON_SLOT_TYPE* pwstWeapon = GetPlayerPed()->GetCurrentWeaponSlot();
		icSync.byteCurrentWeapon ^= (icSync.byteCurrentWeapon ^ pwstWeapon->dwType) & 0x3F;
		//icSync.byteCurrentWeapon ^= (icSync.byteCurrentWeapon ^ GetPlayerPed()->GetCurrentWeapon()) & 0x3F;

		icSync.TrailerID = 0;
		auto* vehTrailer = (VEHICLE_TYPE*)pVehicle->m_pVehicle->dwTrailer;
		if (vehTrailer != nullptr)
		{
			uint16_t id = CVehiclePool::FindIDFromGtaPtr(vehTrailer);
			if (id == INVALID_OBJECT_ID) return;
			if (pVehicle->getProcessTrailer() == 0)
			//if (ScriptCommand(&is_trailer_on_cab, id, pVehicle->m_dwGTAId))
			{
				icSync.TrailerID = CVehiclePool::FindIDFromGtaPtr(vehTrailer);
			}
			else icSync.TrailerID = 0;
		}

		if ((pVehicle->m_pEntity->nModelIndex == TRAIN_PASSENGER_LOCO) ||
			(pVehicle->m_pEntity->nModelIndex == TRAIN_FREIGHT_LOCO) ||
			(pVehicle->m_pEntity->nModelIndex == TRAIN_TRAM))
		{
			icSync.fTrainSpeed = pVehicle->GetTrainSpeed();
		}

		if (icSync.TrailerID && icSync.TrailerID < MAX_VEHICLES)
		{
			RwMatrix matTrailer;
			TRAILER_SYNC_DATA trSync;

			CVehicle* pTrailer = CVehiclePool::GetAt(icSync.TrailerID);
			if (pTrailer)
			{
				pTrailer->GetMatrix(&matTrailer);
				trSync.trailerID = icSync.TrailerID;

				trSync.vecPos.x = matTrailer.pos.x;
				trSync.vecPos.y = matTrailer.pos.y;
				trSync.vecPos.z = matTrailer.pos.z;

				CQuaternion syncQuat;
				syncQuat.SetFromMatrix(matTrailer);
				syncQuat.Normalize();
				trSync.quat = syncQuat;

				pTrailer->GetMoveSpeedVector(&trSync.vecMoveSpeed);
				pTrailer->GetTurnSpeedVector(&trSync.vecTurnSpeed);

				RakNet::BitStream bsTrailerSync;
				bsTrailerSync.Write((BYTE)ID_TRAILER_SYNC);
				bsTrailerSync.Write((char*)& trSync, sizeof(TRAILER_SYNC_DATA));

				pNetGame->GetRakClient()->Send(&bsTrailerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
			}
		}

		// send
		if( (GetTickCount() - m_dwLastUpdateInCarData) > 500 || memcmp(&m_InCarData, &icSync, sizeof(INCAR_SYNC_DATA)))
		{
			m_dwLastUpdateInCarData = GetTickCount();

			bsVehicleSync.Write((uint8_t)ID_VEHICLE_SYNC);
			bsVehicleSync.Write((char*)&icSync, sizeof(INCAR_SYNC_DATA));
			pNetGame->GetRakClient()->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			memcpy(&m_InCarData, &icSync, sizeof(INCAR_SYNC_DATA));
		}
	}
}

void CLocalPlayer::SendPassengerFullSyncData()
{
	RakNet::BitStream bsPassengerSync;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);
	PASSENGER_SYNC_DATA psSync;
	RwMatrix mat;

	psSync.VehicleID = CVehiclePool::FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());

	if(psSync.VehicleID == INVALID_VEHICLE_ID)
		return;

	psSync.lrAnalog = lrAnalog;
	psSync.udAnalog = udAnalog;
	psSync.wKeys = wKeys;
	psSync.bytePlayerHealth = (uint8_t)m_pPlayerPed->GetHealth();
	psSync.bytePlayerArmour = (uint8_t)m_pPlayerPed->GetArmour();

	psSync.byteSeatFlags = m_pPlayerPed->GetVehicleSeatID();
	psSync.byteDriveBy = 0;//m_bPassengerDriveByMode;

	psSync.byteCurrentWeapon = 0;//m_pPlayerPed->GetCurrentWeapon();

	m_pPlayerPed->GetMatrix(&mat);

	psSync.vecPos.x = mat.pos.x;
	psSync.vecPos.y = mat.pos.y;
	psSync.vecPos.z = mat.pos.z;

	// send
	if((GetTickCount() - m_dwLastUpdatePassengerData) > 500 || memcmp(&m_PassengerData, &psSync, sizeof(PASSENGER_SYNC_DATA)))
	{
		m_dwLastUpdatePassengerData = GetTickCount();

		bsPassengerSync.Write((uint8_t)ID_PASSENGER_SYNC);
		bsPassengerSync.Write((char*)&psSync, sizeof(PASSENGER_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsPassengerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		memcpy(&m_PassengerData, &psSync, sizeof(PASSENGER_SYNC_DATA));
	}
}

void CLocalPlayer::SendAimSyncData()
{
	RakNet::BitStream bsAimSync;
	AIM_SYNC_DATA aimSync;

	CAMERA_AIM* caAim = m_pPlayerPed->GetCurrentAim();

	aimSync.byteCamMode = (uint8_t)m_pPlayerPed->GetCameraMode();
	aimSync.vecAimf.x = caAim->f1x;
	aimSync.vecAimf.y = caAim->f1y;
	aimSync.vecAimf.z = caAim->f1z;
	aimSync.vecAimPos.x = caAim->pos1x;
	aimSync.vecAimPos.y = caAim->pos1y;
	aimSync.vecAimPos.z = caAim->pos1z;
	aimSync.fAimZ = m_pPlayerPed->GetAimZ();
	
	aimSync.byteCamExtZoom = (uint8_t)(m_pPlayerPed->GetCameraExtendedZoom() * 63.0f);

	WEAPON_SLOT_TYPE* pwstWeapon = m_pPlayerPed->GetCurrentWeaponSlot();
	if (pwstWeapon->dwState == 2)
		aimSync.byteWeaponState = WS_RELOADING;
	else aimSync.byteWeaponState = (pwstWeapon->dwAmmoInClip > 1) ? WS_MORE_BULLETS : pwstWeapon->dwAmmoInClip;

	aimSync.bUnk = pKeyBoard->IsOpen();
	
	if ((GetTickCount() - m_dwLastSendTick) > 500 || memcmp(&m_aimSync, &aimSync, sizeof(AIM_SYNC_DATA)))
	{
		bsAimSync.Write((uint8_t)ID_AIM_SYNC);
		bsAimSync.Write((char*)&aimSync, sizeof(AIM_SYNC_DATA));

		pNetGame->GetRakClient()->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}

void CLocalPlayer::ProcessSpectating()
{
	RakNet::BitStream bsSpectatorSync;
	SPECTATOR_SYNC_DATA spSync;
	RwMatrix matPos;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);
	pGame->GetCamera()->GetMatrix(&matPos);

	spSync.vecPos.x = matPos.pos.x;
	spSync.vecPos.y = matPos.pos.y;
	spSync.vecPos.z = matPos.pos.z;
	spSync.lrAnalog = lrAnalog;
	spSync.udAnalog = udAnalog;
	spSync.wKeys = wKeys;

	if((GetTickCount() - m_dwLastSendSpecTick) > GetOptimumOnFootSendRate())
	{
		m_dwLastSendSpecTick = GetTickCount();

		bsSpectatorSync.Write((uint8_t)ID_SPECTATOR_SYNC);
		bsSpectatorSync.Write((char*)&spSync, sizeof(SPECTATOR_SYNC_DATA));

		pNetGame->GetRakClient()->Send(&bsSpectatorSync, HIGH_PRIORITY, UNRELIABLE, 0);

		if((GetTickCount() - m_dwLastSendAimTick) > (GetOptimumOnFootSendRate() * 2))
			m_dwLastSendAimTick = GetTickCount();
	}

	pGame->DisplayHUD(false);

	m_pPlayerPed->SetHealth(100.0f);
	GetPlayerPed()->TeleportTo(spSync.vecPos.x, spSync.vecPos.y, spSync.vecPos.z + 20.0f);

	// handle spectate player left the server
	if(m_byteSpectateType == SPECTATE_TYPE_PLAYER &&
		!CPlayerPool::GetAt(m_SpectateID))
	{
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	// handle spectate player is no longer active (ie Died)
	if(m_byteSpectateType == SPECTATE_TYPE_PLAYER &&
		CPlayerPool::GetAt(m_SpectateID) &&
		(!CPlayerPool::GetAt(m_SpectateID)->IsActive() ||
		CPlayerPool::GetAt(m_SpectateID)->GetState() == PLAYER_STATE_WASTED))
	{
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	if(m_bSpectateProcessed) return;

	if(m_byteSpectateType == SPECTATE_TYPE_NONE)
	{
		GetPlayerPed()->RemoveFromVehicleAndPutAt(0.0f, 0.0f, 10.0f);
		pGame->GetCamera()->SetPosition(50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f);
		pGame->GetCamera()->LookAtPoint(60.0f, 60.0f, 50.0f, 2);
		m_bSpectateProcessed = true;
	}
	else if(m_byteSpectateType == SPECTATE_TYPE_PLAYER)
	{
		uint32_t dwGTAId = 0;
		CPlayerPed *pPlayerPed = 0;

		if(CPlayerPool::GetSpawnedPlayer(m_SpectateID))
		{
			pPlayerPed = CPlayerPool::GetSpawnedPlayer(m_SpectateID)->GetPlayerPed();
			if(pPlayerPed)
			{
				dwGTAId = pPlayerPed->m_dwGTAId;
				ScriptCommand(&camera_on_actor, dwGTAId, m_byteSpectateMode, 2);
				m_bSpectateProcessed = true;
			}
		}
	}
	else if(m_byteSpectateType == SPECTATE_TYPE_VEHICLE)
	{
		CVehicle *pVehicle = nullptr;
		uint32_t dwGTAId = 0;

		if (CVehiclePool::GetAt((VEHICLEID)m_SpectateID))
		{
			pVehicle = CVehiclePool::GetAt((VEHICLEID)m_SpectateID);
			if(pVehicle) 
			{
				dwGTAId = pVehicle->m_dwGTAId;
				ScriptCommand(&camera_on_vehicle, dwGTAId, m_byteSpectateMode, 2);
				m_bSpectateProcessed = true;
			}
		}
	}	
}

void CLocalPlayer::ToggleSpectating(bool bToggle)
{
	if(m_bIsSpectating && !bToggle)
		Spawn();

	m_bIsSpectating = bToggle;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_SpectateID = 0xFFFFFFFF;
	m_bSpectateProcessed = false;
}

void CLocalPlayer::SpectatePlayer(PLAYERID playerId)
{
	if(CPlayerPool::GetAt(playerId))
	{
		if(CPlayerPool::GetAt(playerId)->GetState() != PLAYER_STATE_NONE &&
			CPlayerPool::GetAt(playerId)->GetState() != PLAYER_STATE_WASTED)
		{
			m_byteSpectateType = SPECTATE_TYPE_PLAYER;
			m_SpectateID = playerId;
			m_bSpectateProcessed = false;
		}
	}
}

void CLocalPlayer::SpectateVehicle(VEHICLEID VehicleID)
{
	if (CVehiclePool::GetAt(VehicleID))
	{
		m_byteSpectateType = SPECTATE_TYPE_VEHICLE;
		m_SpectateID = VehicleID;
		m_bSpectateProcessed = false;
	}
}

void CLocalPlayer::GiveTakeDamage(bool bGiveOrTake, uint16_t wPlayerID, float damage_amount, uint32_t weapon_id, uint32_t bodypart)
{
	RakNet::BitStream bitStream;

	bitStream.Write((bool)bGiveOrTake);
	bitStream.Write((uint16_t)wPlayerID);
	bitStream.Write((float)damage_amount);
	bitStream.Write((uint32_t)weapon_id);
	bitStream.Write((uint32_t)bodypart);

	Log("GiveTakeDamage %d %d %f %d %d", bGiveOrTake, wPlayerID, damage_amount, weapon_id, bodypart);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
RwMatrix* RwMatrixMultiplyByVector(CVector* out, RwMatrix* a2, CVector* in);

void CLocalPlayer::ProcessSurfing() {
    if(m_pPlayerPed && !m_pPlayerPed->IsDead() && !LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]) {
        VEHICLE_TYPE* contactVeh = m_pPlayerPed->GetGtaContactVehicle();
        if(contactVeh){
            VEHICLEID vehicleId = CVehiclePool::FindIDFromGtaPtr(contactVeh);
            if(vehicleId && vehicleId != INVALID_VEHICLE_ID){
                CVehicle* pVeh = CVehiclePool::GetAt(vehicleId);
                if(pVeh && pVeh->IsOccupied()){
                    /*bool onFootObject = ScriptCommand(&is_char_touching_vehicle, m_pPlayerPed->m_dwGTAId, pVeh->m_dwGTAId);
                    if(onFootObject){*/
                        if(m_surfData.bIsActive){
                            return;
                        }
                        memset(&m_surfData, 0, sizeof(m_surfData));
                        m_surfData.vecOffsetPos = new CVector();
                        m_surfData.dwSurfVehID = vehicleId;
                        m_surfData.pSurfInst = (uintptr_t)pVeh;
                        m_surfData.bIsVehicle = true;

                        static RwMatrix matVeh;
                        pVeh->GetMatrix(&matVeh);
                        static RwMatrix matPed;
                        m_pPlayerPed->GetMatrix(&matPed);
                        static RwMatrix matOut;
                        mat_invert(&matOut, &matVeh);
                        ProjectMatrix(&m_surfData.vecOffsetPos, &matOut, &matPed.pos);

                        m_surfData.bIsActive = true;
                        return;
                    //}
                }
            }else{
                ENTITY_TYPE* contactEntity = m_pPlayerPed->GetGtaContactEntity();
                if(contactEntity){
                    uint32_t objectId = CObjectPool::FindIDFromGtaPtr((ENTITY_TYPE*)contactEntity);
                    if(objectId && objectId != INVALID_OBJECT_ID){
                        CObject* pObject = CObjectPool::GetAt(objectId);
                        if(pObject){
                            //bool onFootObject = ScriptCommand(&is_char_touching_object, m_pPlayerPed->m_dwGTAId, pObject->m_dwGTAId);
                            //if(onFootObject) {
                                if(m_surfData.bIsActive){
                                    return;
                                }
                                memset(&m_surfData, 0, sizeof(m_surfData));
                                m_surfData.bIsVehicle = false;
                                m_surfData.pSurfInst = (uintptr_t) pObject;
                                m_surfData.bIsActive = true;
                                return;
                            //}
                        }
                    }
                }
            }
        }
    }

    m_surfData.bIsActive = false;
    m_surfData.dwSurfVehID = INVALID_VEHICLE_ID;
    m_surfData.pSurfInst = 0;
    m_surfData.vecOffsetPos = new CVector();

}
void CLocalPlayer::UpdateSurfing() {
    static RwMatrix surfInstMatrix;
    static RwMatrix surfPedMatrix;
    static CVector surfInstMoveSpeed;
    static CVector surfInstTurnSpeed;
    if(m_pPlayerPed) {
		if(LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]){
            return;
        }
        if(m_surfData.bIsActive){
            if(m_surfData.bIsVehicle){
                CVehicle* pVeh = (CVehicle*) m_surfData.pSurfInst;
                pVeh->GetMatrix(&surfInstMatrix);
                m_pPlayerPed->GetMatrix(&surfPedMatrix);
                pVeh->GetMoveSpeedVector(&surfInstMoveSpeed);
                pVeh->GetTurnSpeedVector(&surfInstTurnSpeed);

                uint16_t lrAnalog;
                uint16_t udAnalog;
                uint8_t ext;
                m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

                if(lrAnalog || udAnalog) {
                    static RwMatrix matOut;
                    mat_invert(&matOut, &surfInstMatrix);
                    ProjectMatrix(&m_surfData.vecOffsetPos, &matOut, &surfPedMatrix.pos);
                }else {
                    ProjectMatrix(&surfPedMatrix.pos, &surfInstMatrix, &m_surfData.vecOffsetPos);

                    m_pPlayerPed->SetMatrix(surfPedMatrix);
                    CVector vecMoveSpeed;
                    m_pPlayerPed->GetMoveSpeedVector(&vecMoveSpeed);
                    m_pPlayerPed->SetMoveSpeedVector(CVector { surfInstMoveSpeed.x, surfInstMoveSpeed.y, vecMoveSpeed.z});

                    CVector vecTurnSpeed;
                    m_pPlayerPed->GetTurnSpeedVector(&vecTurnSpeed);
                    m_pPlayerPed->SetTurnSpeedVector(CVector{vecTurnSpeed.x, vecTurnSpeed.y, surfInstTurnSpeed.z});
                }
            }else{
                CObject* pObject = (CObject*) m_surfData.pSurfInst;
                if(pObject->m_byteMoving & 1){
                    pObject->GetMatrix(&surfInstMatrix);
                    m_pPlayerPed->GetMatrix(&surfPedMatrix);
                    pObject->GetMoveSpeedVector(&surfInstMoveSpeed);
                    pObject->GetTurnSpeedVector(&surfInstTurnSpeed);

                    uint16_t lrAnalog;
                    uint16_t udAnalog;
                    uint8_t ext;
                    m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

                    if(lrAnalog || udAnalog) {
                        static RwMatrix matOut;
                        mat_invert(&matOut, &surfInstMatrix);
                        ProjectMatrix(&m_surfData.vecOffsetPos, &matOut, &surfPedMatrix.pos);
                    }else {
                        ProjectMatrix(&surfPedMatrix.pos, &surfInstMatrix, &m_surfData.vecOffsetPos);

                        m_pPlayerPed->SetMatrix(surfPedMatrix);
                        CVector vecMoveSpeed;
                        m_pPlayerPed->GetMoveSpeedVector(&vecMoveSpeed);
                        m_pPlayerPed->SetMoveSpeedVector(CVector { surfInstMoveSpeed.x, surfInstMoveSpeed.y, vecMoveSpeed.z});

                        CVector vecTurnSpeed;
                        m_pPlayerPed->GetTurnSpeedVector(&vecTurnSpeed);
                        m_pPlayerPed->SetTurnSpeedVector(CVector{vecTurnSpeed.x, vecTurnSpeed.y, surfInstTurnSpeed.z});
                    }
                }

            }
        }

    }



}

float CLocalPlayer::DistanceRemaining(RwMatrix *matPos, RwMatrix *m_matPositionTarget)
{
	float	fSX,fSY,fSZ;
	fSX = (matPos->pos.x - m_matPositionTarget->pos.x) * (matPos->pos.x - m_matPositionTarget->pos.x);
	fSY = (matPos->pos.y - m_matPositionTarget->pos.y) * (matPos->pos.y - m_matPositionTarget->pos.y);
	fSZ = (matPos->pos.z - m_matPositionTarget->pos.z) * (matPos->pos.z - m_matPositionTarget->pos.z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CLocalPlayer::SendTakeDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType)
{
	RakNet::BitStream bsSend;
	bsSend.Write(true);
	bsSend.Write(PlayerID);
	bsSend.Write(fDamageFactor);
	bsSend.Write(weaponType);
	bsSend.Write(pedPieceType);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
// 0.3.7
void CLocalPlayer::SendGiveDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType)
{
	RakNet::BitStream bsSend;
	bsSend.Write(false);
	bsSend.Write(PlayerID);
	bsSend.Write(fDamageFactor);
	bsSend.Write(weaponType);
	bsSend.Write(pedPieceType);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

bool NeedToSendUnoccupiedSync(UNOCCUPIED_SYNC_DATA* data, UNOCCUPIED_SYNC_DATA* sync)
{
	if (FloatOffset(data->vecPos.x, sync->vecPos.x) >= 1.0f || FloatOffset(data->vecPos.y, sync->vecPos.y) >= 1.0f || FloatOffset(data->vecPos.z, sync->vecPos.z) >= 1.0f)
	{
		return true;
	}
	return false;
}

void CLocalPlayer::SendUnoccupiedSyncData()
{
	for (int i = 0; i < MAX_VEHICLES; i++)
	{
		CVehicle* pVehicle = CVehiclePool::GetAt(i);
		if (pVehicle && pVehicle->IsAdded())
		{
			if (pVehicle->GetDistanceFromLocalPlayerPed() <= 200.0f)
			{
				UNOCCUPIED_SYNC_DATA unocSync;
				memset(&unocSync, 0, sizeof(UNOCCUPIED_SYNC_DATA));

				RwMatrix mat;
				CVector vecMoveSpeed;
				CVector vecTurnSpeed;

				unocSync.VehicleID = i;
				unocSync.byteSeatId = (m_pPlayerPed->GetVehicleSeatID() == -1) ? 0 : m_pPlayerPed->GetVehicleSeatID();

				pVehicle->GetMatrix(&mat);
				pVehicle->GetMoveSpeedVector(&vecMoveSpeed);
				pVehicle->GetTurnSpeedVector(&vecTurnSpeed);

				unocSync.vecPos.x = mat.pos.x;
				unocSync.vecPos.y = mat.pos.y;
				unocSync.vecPos.z = mat.pos.z;

				unocSync.vecDirection.x = mat.up.x;
				unocSync.vecDirection.y = mat.up.y;
				unocSync.vecDirection.z = mat.up.z;

				unocSync.vecRoll.x = mat.right.x;
				unocSync.vecRoll.y = mat.right.y;
				unocSync.vecRoll.z = mat.right.z;

				unocSync.vecMoveSpeed.x = vecMoveSpeed.x;
				unocSync.vecMoveSpeed.y = vecMoveSpeed.y;
				unocSync.vecMoveSpeed.z = vecMoveSpeed.z;

				unocSync.vecTurnSpeed.x = vecTurnSpeed.x;
				unocSync.vecTurnSpeed.y = vecTurnSpeed.y;
				unocSync.vecTurnSpeed.z = vecTurnSpeed.z;

				unocSync.fHealth = pVehicle->GetHealth();

				/* mecmp doesn't work here sadly */
				if (NeedToSendUnoccupiedSync(&m_UnoccupiedData[i], &unocSync))
				{
					RakNet::BitStream bsUnoccupiedSync;

					bsUnoccupiedSync.Write((uint8_t)ID_UNOCCUPIED_SYNC);
					bsUnoccupiedSync.Write((char*)&unocSync, sizeof(UNOCCUPIED_SYNC_DATA));
					pNetGame->GetRakClient()->Send(&bsUnoccupiedSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

					m_UnoccupiedData[i].vecPos.x = unocSync.vecPos.x;
					m_UnoccupiedData[i].vecPos.y = unocSync.vecPos.y;
					m_UnoccupiedData[i].vecPos.z = unocSync.vecPos.z;
				}
			}
		}
	}
}