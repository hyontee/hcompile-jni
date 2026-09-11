#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../chatwindow.h"
#include "../game/Custom/MasterCheat.h"
#include "util/CJavaWrapper.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern MasterCheat *g_pMasterCheat;

CRemotePlayer::CRemotePlayer()
{
	m_PlayerID = INVALID_PLAYER_ID;
	m_VehicleID = INVALID_VEHICLE_ID;
	m_pPlayerPed = nullptr;
	m_pCurrentVehicle = nullptr;
	m_bIsNPC = false;
	m_bIsAFK = true;
	m_dwMarkerID = 0;
	m_dwGlobalMarkerID = 0;
	m_byteState = PLAYER_STATE_NONE;
	m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
	m_bShowNameTag = true;

	m_dwLastRecvTick = 0;
	m_dwUnkTime = 0;

	m_byteSpecialAction = 0;
	m_byteSeatID = 0;
	m_byteWeaponShotID = 0xFF;
	m_fReportedHealth = 0.0f;
	m_fReportedArmour = 0.0f;
	m_bKeyboardOpened = false;

	m_dwCurrentAnimation = 0;
	m_usPlayingAnimIdx = 0;
	m_bWasSettedFlag = false;
}

CRemotePlayer::~CRemotePlayer()
{
	Remove();
}
extern uint32_t g_uiHeadMoveEnabled;
void CRemotePlayer::ProcessSpecialActions(BYTE byteSpecialAction)
{
	if (!m_pPlayerPed || !m_pPlayerPed->IsAdded()) return;

	if (GetState() != PLAYER_STATE_ONFOOT)
	{
		byteSpecialAction = SPECIAL_ACTION_NONE;
		m_ofSync.byteSpecialAction = SPECIAL_ACTION_NONE;
	}

	// headsync:always
	if (GetState() == PLAYER_STATE_ONFOOT && m_pPlayerPed->IsAdded())
	{
		if (byteSpecialAction == 0)
		{
			if (m_pPlayerPed->IsCrouching())
			{
				m_pPlayerPed->ResetCrouch();
			}
		}
		if (byteSpecialAction == 1)
		{
			if (!m_pPlayerPed->IsCrouching())
			{
				m_pPlayerPed->ApplyCrouch();
			}
		}
		if ((GetTickCount() - m_dwLastHeadUpdate) > 500 && g_uiHeadMoveEnabled)
		{
			CVector LookAt;
			CAMERA_AIM* Aim = GameGetRemotePlayerAim(m_pPlayerPed->m_bytePlayerNumber);
			LookAt.x = Aim->pos1x + (Aim->f1x * 20.0f);
			LookAt.y = Aim->pos1y + (Aim->f1y * 20.0f);
			LookAt.z = Aim->pos1z + (Aim->f1z * 20.0f);
			m_pPlayerPed->ApplyCommandTask(OBFUSCATE("FollowPedSA"), 0, 2000, -1, &LookAt, 0, 0.1f, 500, 3, 0);
			m_dwLastHeadUpdate = GetTickCount();
		}
	}
}

#define OUTCOMING_KEY 0x87
#define OUTCOMING_NUM 0x80
void CRemotePlayer::Process()
{
	CLocalPlayer *pLocalPlayer = CPlayerPool::GetLocalPlayer();
	RwMatrix matPlayer, matVehicle;
	CVector vecMoveSpeed;

	if(IsActive())
	{
		// ---- ONFOOT NETWORK PROCESSING ----
		if(GetState() == PLAYER_STATE_ONFOOT && 
			m_byteUpdateFromNetwork == UPDATE_TYPE_ONFOOT && !m_pPlayerPed->IsInVehicle())
		{
			// UPDATE CURRENT WEAPON
			if (m_iKey0 ^ OUTCOMING_KEY != OUTCOMING_NUM)
			{
				
			}
			
			WEAPON_SLOT_TYPE* pwstWeapon = GetPlayerPed()->GetCurrentWeaponSlot();
			if (pwstWeapon->dwType != m_ofSync.byteCurrentWeapon)
			//if (GetPlayerPed()->GetCurrentWeapon() != m_ofSync.byteCurrentWeapon)
			{
				GetPlayerPed()->GiveWeapon(m_ofSync.byteCurrentWeapon, 9999);
				GetPlayerPed()->SetArmedWeapon(m_ofSync.byteCurrentWeapon);
			}
			// First person weapon action hack
			/*if (!IS_TARGETING(m_ofSync.wKeys)) 
			{
				//0x0043893C
				((uint32_t(*)(uintptr_t, uintptr_t))(SA_ADDR(0x0043893C + 1)))((uintptr_t)m_pPlayerPed, 1);
			}*/
			

			UpdateOnFootPositionAndSpeed(&m_ofSync.vecPos, &m_ofSync.vecMoveSpeed);
			UpdateOnFootTargetPosition();
		}
		else if(GetState() == PLAYER_STATE_DRIVER &&
			m_byteUpdateFromNetwork == UPDATE_TYPE_INCAR && m_pPlayerPed->IsInVehicle())
		{
			if (!m_pCurrentVehicle)
			{
				return;
			}
			if (!GamePool_Vehicle_GetAt(m_pCurrentVehicle->m_dwGTAId))
			{
				return;
			}

			m_icSync.quat.Normalize();
			m_icSync.quat.GetMatrix(&matVehicle);
			matVehicle.pos.x = m_icSync.vecPos.x;
			matVehicle.pos.y = m_icSync.vecPos.y;
			matVehicle.pos.z = m_icSync.vecPos.z;

			if( m_pCurrentVehicle->GetModelIndex() == TRAIN_PASSENGER_LOCO ||
				m_pCurrentVehicle->GetModelIndex() == TRAIN_FREIGHT_LOCO ||
				m_pCurrentVehicle->GetModelIndex() == TRAIN_TRAM)
			{
				UpdateTrainDriverMatrixAndSpeed(&matVehicle, &m_icSync.vecMoveSpeed, m_icSync.fTrainSpeed);
			}
			else
			{
				UpdateInCarMatrixAndSpeed(&matVehicle, &m_icSync.vecPos, &m_icSync.vecMoveSpeed);
				UpdateInCarTargetPosition();
			}
			if (m_icSync.fCarHealth <= 300.0f)
			{
				m_icSync.fCarHealth = 300.0f;
			}
			m_pCurrentVehicle->SetHealth(m_icSync.fCarHealth);

			// TRAILER AUTOMATIC ATTACHMENT AS THEY MOVE INTO IT
			if (m_pCurrentVehicle->GetDistanceFromLocalPlayerPed() < 200.0f)
			{
				if (m_icSync.TrailerID && m_icSync.TrailerID < MAX_VEHICLES)
				{
					CVehicle* pRealTrailer = m_pCurrentVehicle->GetTrailer();
					CVehicle* pTrailerVehicle = CVehiclePool::GetAt(m_icSync.TrailerID);
					if (!pRealTrailer) 
					{
						if (pTrailerVehicle) 
						{
							m_pCurrentVehicle->SetTrailer(pTrailerVehicle);
							m_pCurrentVehicle->AttachTrailer();
						}
					}
				}
				else
				{
					if (m_pCurrentVehicle->GetTrailer()) 
					{
						m_pCurrentVehicle->DetachTrailer();
						m_pCurrentVehicle->SetTrailer(NULL);
					}
				}
			}
			else
			{
				m_pCurrentVehicle->SetTrailer(NULL);
			}
		}
		else if(GetState() == PLAYER_STATE_PASSENGER && 
			m_byteUpdateFromNetwork == UPDATE_TYPE_PASSENGER)
		{
			if(!m_pCurrentVehicle) return;
		}

		m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;

		// ------ PROCESSED FOR ALL FRAMES ----- 
		if(GetState() == PLAYER_STATE_ONFOOT && !m_pPlayerPed->IsInVehicle())
		{
			ProcessSpecialActions(m_ofSync.byteSpecialAction);
			SlerpRotation();
			
			HandleAnimations();

			if (m_byteWeaponShotID != 0xFF)
			{
				//weapon sync
				//RwMatrix localMat;
				//pPool->GetLocalPlayer()->GetPlayerPed()->GetMatrix(&localMat);

				m_ofSync.byteCurrentWeapon = m_byteWeaponShotID;
				m_pPlayerPed->SetArmedWeapon((int)m_byteWeaponShotID);
				//ScriptCommand(&task_shoot_at_coord, m_pPlayerPed->m_dwGTAId, localMat.pos.x, localMat.pos.y, localMat.pos.z, 10);
				m_pPlayerPed->SetCurrentAim(CPlayerPool::GetLocalPlayer()->GetPlayerPed()->GetCurrentAim());
				m_pPlayerPed->SetKeys((uint16_t)4, m_ofSync.lrAnalog, m_ofSync.udAnalog);

				//unknown weapon
				m_byteWeaponShotID = 0xFF;
			}
			else
				m_pPlayerPed->SetKeys(m_ofSync.wKeys, m_ofSync.lrAnalog, m_ofSync.udAnalog);

			if (RemotePlayerKeys[GetPlayerPed()->m_bytePlayerNumber].bKeys[ePadKeys::KEY_HANDBRAKE])
			{
				GetPlayerPed()->SetPlayerAimState();
			}
			else
			{
				GetPlayerPed()->ClearPlayerAimState();
			}
			
			if( m_ofSync.vecMoveSpeed.x == 0.0f &&
				m_ofSync.vecMoveSpeed.y == 0.0f &&
				m_ofSync.vecMoveSpeed.z == 0.0f)
			{
				m_pPlayerPed->SetMoveSpeedVector(m_ofSync.vecMoveSpeed);
			}
			
			if(SurfingOnVehicle() || SurfingOnObject()) 
			{
				ProcessSurfing();
				m_pPlayerPed->SetGravityProcessing(0);
			} 
			else m_pPlayerPed->SetGravityProcessing(1);

			if((GetTickCount() - m_dwLastRecvTick) > 1500)
				m_bIsAFK = true;

			if(m_bIsAFK && ((GetTickCount() - m_dwLastRecvTick) > 3000))
			{
				m_ofSync.lrAnalog = 0;
				m_ofSync.udAnalog = 0;
				
				vecMoveSpeed.x = 0.0f;
				vecMoveSpeed.y = 0.0f;
				vecMoveSpeed.z = 0.0f;
				m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
				
				m_pPlayerPed->GetMatrix(&matPlayer);
				matPlayer.pos.x = m_ofSync.vecPos.x;
				matPlayer.pos.y = m_ofSync.vecPos.y;
				matPlayer.pos.z = m_ofSync.vecPos.z;
				m_pPlayerPed->SetMatrix(matPlayer);
			}
		}
		else if(GetState() == PLAYER_STATE_DRIVER && m_pPlayerPed->IsInVehicle())
		{
			if (!m_pCurrentVehicle)
			{
				return;
			}
			if (!GamePool_Vehicle_GetAt(m_pCurrentVehicle->m_dwGTAId))
			{
				return;
			}
			if( m_pCurrentVehicle->GetModelIndex() != TRAIN_PASSENGER_LOCO &&
				m_pCurrentVehicle->GetModelIndex() != TRAIN_FREIGHT_LOCO &&
				m_pCurrentVehicle->GetModelIndex() != TRAIN_TRAM)
			{
				UpdateVehicleRotation();
			}

			if(	m_icSync.vecMoveSpeed.x == 0.0f &&
				m_icSync.vecMoveSpeed.y == 0.0f &&
				m_icSync.vecMoveSpeed.z == 0.0f)
			{
				m_pCurrentVehicle->SetMoveSpeedVector(m_icSync.vecMoveSpeed);
			}

			m_pPlayerPed->SetKeys(m_icSync.wKeys, m_icSync.lrAnalog, m_icSync.udAnalog);

			if((GetTickCount() - m_dwLastRecvTick) > 1500)
				m_bIsAFK = true;

		}
		else if(GetState() == PLAYER_STATE_PASSENGER)
		{
			if((GetTickCount() - m_dwLastRecvTick) >= 3000)
				m_bIsAFK = true;
		}
		else
		{
			m_pPlayerPed->SetKeys(0, 0, 0);
			vecMoveSpeed.x = 0.0f;
			vecMoveSpeed.y = 0.0f;
			vecMoveSpeed.z = 0.0f;
			m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
		}

		if(m_byteState != PLAYER_STATE_WASTED)
			m_pPlayerPed->SetHealth(1000.0f);

		if((GetTickCount() - m_dwLastRecvTick) < 1500)
			m_bIsAFK = false;
	}
	else
	{
		if(m_pPlayerPed)
		{
			//ResetAllSyncAttributes();
			pGame->RemovePlayer(m_pPlayerPed);
			m_pPlayerPed = nullptr;
		}
	}
}

uint32_t CRemotePlayer::GetPlayerColorAsARGB()
{
	return (TranslateColorCodeToRGBA(m_PlayerID) >> 8) | 0xFF000000;
}


void CRemotePlayer::SlerpRotation()
{
	RwMatrix mat;
	CQuaternion quatPlayer, quatResult;

	if(m_pPlayerPed)
	{
		m_pPlayerPed->GetMatrix(&mat);

		quatPlayer.SetFromMatrix(mat);

		quatResult.Slerp(&m_ofSync.quat, &quatPlayer, 0.75f);
		quatResult.GetMatrix(&mat);
		m_pPlayerPed->SetMatrix(mat);

		float fZ = atan2(-mat.up.x, mat.up.y) * 57.295776; /* rad to deg */
		if(fZ > 360.0f) fZ -= 360.0f;
		if(fZ < 0.0f) fZ += 360.0f;
		m_pPlayerPed->SetRotation(fZ);
	}
}

void CRemotePlayer::UpdateTrainDriverMatrixAndSpeed(RwMatrix* matWorld, CVector* vecMoveSpeed, float fTrainSpeed)
{
	RwMatrix matVehicle;
	CVector vecInternalMoveSpeed;
	bool bTeleport = false;
	float fDif;

	if (!m_pPlayerPed || !m_pCurrentVehicle) return;

	m_pCurrentVehicle->GetMatrix(&matVehicle);

	if (matWorld->pos.x >= matVehicle.pos.x) {
		fDif = matWorld->pos.x - matVehicle.pos.x;
	}
	else {
		fDif = matVehicle.pos.x - matWorld->pos.x;
	}
	if (fDif > 10.0f) bTeleport = true;

	if (matWorld->pos.y >= matVehicle.pos.y) {
		fDif = matWorld->pos.y - matVehicle.pos.y;
	}
	else {
		fDif = matVehicle.pos.y - matWorld->pos.y;
	}
	if (fDif > 10.0f) bTeleport = true;

	if (bTeleport) m_pCurrentVehicle->TeleportTo(matWorld->pos.x, matWorld->pos.y, matWorld->pos.z);

	m_pCurrentVehicle->GetMoveSpeedVector(&vecInternalMoveSpeed);
	vecInternalMoveSpeed.x = vecMoveSpeed->x;
	vecInternalMoveSpeed.y = vecMoveSpeed->y;
	vecInternalMoveSpeed.z = vecMoveSpeed->z;
	m_pCurrentVehicle->SetMoveSpeedVector(vecInternalMoveSpeed);
	m_pCurrentVehicle->SetTrainSpeed(fTrainSpeed);
}
void CRemotePlayer::UpdateInCarMatrixAndSpeed(RwMatrix* mat, CVector* pos, CVector* speed)
{
	m_InCarQuaternion.SetFromMatrix(*mat);

	m_vecInCarTargetPos.x = pos->x;
	m_vecInCarTargetPos.y = pos->y;
	m_vecInCarTargetPos.z = pos->z;

	m_vecInCarTargetSpeed.x = speed->x;
	m_vecInCarTargetSpeed.y = speed->y;
	m_vecInCarTargetSpeed.z = speed->z;

	m_pCurrentVehicle->SetMoveSpeedVector(*speed);
}

void CRemotePlayer::UpdateInCarTargetPosition()
{
	RwMatrix matEnt;
	CVector vec = { 0.0f, 0.0f, 0.0f };

	float delta = 0.0f;

	if(!m_pCurrentVehicle) return;

	m_pCurrentVehicle->GetMatrix(&matEnt);

	if(m_pCurrentVehicle->IsAdded())
	{
		m_vecPosOffset.x = FloatOffset(m_vecInCarTargetPos.x, matEnt.pos.x);
		m_vecPosOffset.y = FloatOffset(m_vecInCarTargetPos.y, matEnt.pos.y);
		m_vecPosOffset.z = FloatOffset(m_vecInCarTargetPos.z, matEnt.pos.z);

		if(m_vecPosOffset.x > 0.05f || m_vecPosOffset.y > 0.05f || m_vecPosOffset.z > 0.05f)
		{
			delta = 0.5f;
			if( m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_HELI)
			{
				delta = 2.0f;
			}

			if(m_vecPosOffset.x > 8.0f || m_vecPosOffset.y > 8.0f || m_vecPosOffset.z > delta)
			{
				matEnt.pos.x = m_vecInCarTargetPos.x;
				matEnt.pos.y = m_vecInCarTargetPos.y;
				matEnt.pos.z = m_vecInCarTargetPos.z;
				m_pCurrentVehicle->SetMatrix(matEnt);
				m_pCurrentVehicle->SetMoveSpeedVector(m_vecInCarTargetSpeed);
			}
			else
			{
				m_pCurrentVehicle->GetMoveSpeedVector(&vec);
				if(m_vecPosOffset.x > 0.05f)
					vec.x += (m_vecInCarTargetPos.x - matEnt.pos.x) * 0.06f;
				if(m_vecPosOffset.y > 0.05f)
					vec.y += (m_vecInCarTargetPos.y - matEnt.pos.y) * 0.06f;
				if(m_vecPosOffset.z > 0.05f)
					vec.z += (m_vecInCarTargetPos.z - matEnt.pos.z) * 0.06f;

				if( FloatOffset(vec.x, 0.0f) > 0.01f ||
					FloatOffset(vec.y, 0.0f) > 0.01f ||
					FloatOffset(vec.z, 0.0f) > 0.01f)
				{
					m_pCurrentVehicle->SetMoveSpeedVector(vec);
				}
			}
		}
	}
	else
	{
		matEnt.pos.x = m_vecInCarTargetPos.x;
		matEnt.pos.y = m_vecInCarTargetPos.y;
		matEnt.pos.z = m_vecInCarTargetPos.z;
		m_pCurrentVehicle->SetMatrix(matEnt);
	}
}

void CRemotePlayer::UpdateVehicleRotation()
{
	CQuaternion quat, qresult;
	RwMatrix matEnt;
	CVector vec = { 0.0f, 0.0f, 0.0f };


	if(!m_pCurrentVehicle) return;

	m_pCurrentVehicle->GetTurnSpeedVector(&vec);
	if(vec.x <= 0.02f)
	{
		if(vec.x < -0.02f) vec.x = -0.02f;
	}
	else vec.x = 0.02f;

	if(vec.y <= 0.02f)
	{
		if(vec.y < -0.02f) vec.y = -0.02f;
	}
	else vec.y = 0.02f;

	if(vec.z <= 0.02f)
	{
		if(vec.z < -0.02f) vec.z = -0.02f;
	}
	else vec.z = 0.02f;

	m_pCurrentVehicle->SetTurnSpeedVector(vec);

	m_pCurrentVehicle->GetMatrix(&matEnt);
	quat.SetFromMatrix(matEnt);
	qresult.Slerp(&m_InCarQuaternion, &quat, 0.75f);
	qresult.Normalize();
	qresult.GetMatrix(&matEnt);
	m_pCurrentVehicle->SetMatrix(matEnt);
}

bool CRemotePlayer::Spawn(uint8_t byteTeam, unsigned int iSkin, CVector *vecPos, float fRotation,
	uint32_t dwColor, uint8_t byteFightingStyle, bool bVisible)
{
	if(m_pPlayerPed)
	{
		pGame->RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}

	CPlayerPed *pPlayer = pGame->NewPlayer(iSkin, vecPos->x, vecPos->y, vecPos->z, fRotation);

	if(pPlayer)
	{
		if(dwColor != 0) SetPlayerColor(dwColor);

		if(m_dwMarkerID)
		{
			pGame->DisableMarker(m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		if(pNetGame->m_iShowPlayerMarkers) 
			pPlayer->ShowMarker(m_PlayerID);

		m_pPlayerPed = pPlayer;
		m_fReportedHealth = 100.0f;
		if(byteFightingStyle != 4)
			m_pPlayerPed->SetFightingStyle(byteFightingStyle);

		SetState(PLAYER_STATE_SPAWNED);

		CPlayerPool::spawnedPlayers[GetID()] = this;
		return true;
	}

	SetState(PLAYER_STATE_NONE);
	return false;
}

void CRemotePlayer::Remove()
{
	if(m_dwMarkerID)
	{
		pGame->DisableMarker(m_dwMarkerID);
		m_dwMarkerID = 0;
	}

	if(m_dwGlobalMarkerID)
	{
		pGame->DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}

	if(m_pPlayerPed)
	{
		pGame->RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}

	if(CPlayerPool::GetSpawnedPlayer(GetID()))
		CPlayerPool::spawnedPlayers.erase(GetID());
}

void CRemotePlayer::HandleVehicleEntryExit()
{
	if(!m_pPlayerPed) return;

	if(!m_pPlayerPed->IsInVehicle())
	{
		if(CVehiclePool::GetAt(m_VehicleID))
		{
			int iCarID = CVehiclePool::FindGtaIDFromID(m_VehicleID);
			m_pPlayerPed->PutDirectlyInVehicle(iCarID, m_byteSeatID);
		}
	}
}

void CRemotePlayer::EnterVehicle(VEHICLEID VehicleID, bool bPassenger)
{
	if( m_pPlayerPed &&
		CVehiclePool::GetAt(VehicleID) &&
		!m_pPlayerPed->IsInVehicle())
	{
		int iGtaVehicleID = CVehiclePool::FindGtaIDFromID(VehicleID);
		if(iGtaVehicleID && iGtaVehicleID != INVALID_VEHICLE_ID)
		{
			m_pPlayerPed->SetKeys(0, 0, 0);
			m_pPlayerPed->EnterVehicle(iGtaVehicleID, bPassenger);
		}
	}
}

void CRemotePlayer::ExitVehicle()
{
	if(m_pPlayerPed && m_pPlayerPed->IsInVehicle())
	{
		m_pPlayerPed->SetKeys(0, 0, 0);
		m_pPlayerPed->ExitCurrentVehicle();
	}
}

void CRemotePlayer::UpdateOnFootPositionAndSpeed(CVector *vecPos, CVector *vecMove)
{
	m_vecOnFootTargetPos.x = vecPos->x;
	m_vecOnFootTargetPos.y = vecPos->y;
	m_vecOnFootTargetPos.z = vecPos->z;
	m_vecOnFootTargetSpeed.x = vecMove->x;
	m_vecOnFootTargetSpeed.y = vecMove->y;
	m_vecOnFootTargetSpeed.z = vecMove->z;

	m_pPlayerPed->SetMoveSpeedVector(m_vecOnFootTargetSpeed);
}

void CRemotePlayer::StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync)
{
    VEHICLEID TrailerID = trSync->trailerID;
    if (!TrailerID) return;

    CVehicle* pVehicle = CVehiclePool::GetAt(TrailerID);

    if (pVehicle)
    {
        RwMatrix matWorld;

        trSync->quat.GetMatrix(&matWorld);
        matWorld.pos.x = trSync->vecPos.x;
        matWorld.pos.y = trSync->vecPos.y;
        matWorld.pos.z = trSync->vecPos.z;

        pVehicle->SetMatrix(matWorld);
        pVehicle->SetMoveSpeedVector(trSync->vecMoveSpeed);
        pVehicle->SetTurnSpeedVector(trSync->vecTurnSpeed);
    }
}

void CRemotePlayer::UpdateOnFootTargetPosition()
{
	RwMatrix mat;
	CVector vec;

	if(!m_pPlayerPed) return;
	m_pPlayerPed->GetMatrix(&mat);

	if(!m_pPlayerPed->IsAdded())
	{
		mat.pos.x = m_vecOnFootTargetPos.x;
		mat.pos.y = m_vecOnFootTargetPos.y;
		mat.pos.z = m_vecOnFootTargetPos.z;

		m_pPlayerPed->SetMatrix(mat);
		return;
	}

	m_vecPosOffset.x = FloatOffset(m_vecOnFootTargetPos.x, mat.pos.x);
	m_vecPosOffset.y = FloatOffset(m_vecOnFootTargetPos.y, mat.pos.y);
	m_vecPosOffset.z = FloatOffset(m_vecOnFootTargetPos.z, mat.pos.z);

	if(m_vecPosOffset.x > 0.00001f || m_vecPosOffset.y > 0.00001f || m_vecPosOffset.z > 0.00001f)
	{
		if(m_vecPosOffset.x > 2.0f || m_vecPosOffset.y > 2.0f || m_vecPosOffset.z > 1.0f)
		{
			mat.pos.x = m_vecOnFootTargetPos.x;
			mat.pos.y = m_vecOnFootTargetPos.y;
			mat.pos.z = m_vecOnFootTargetPos.z;
			m_pPlayerPed->SetMatrix(mat);
			return;
		}

		m_pPlayerPed->GetMoveSpeedVector(&vec);
		if(m_vecPosOffset.x > 0.00001f)
			vec.x += (m_vecOnFootTargetPos.x - mat.pos.x) * 0.1f;
		if(m_vecPosOffset.y > 0.00001f)
			vec.y += (m_vecOnFootTargetPos.y - mat.pos.y) * 0.1f;
		if(m_vecPosOffset.z > 0.00001f)
			vec.z += (m_vecOnFootTargetPos.z - mat.pos.z) * 0.1f;

		m_pPlayerPed->SetMoveSpeedVector(vec);
	}
}
float m_fWeaponDamages[43 + 1]
= {
5.0f, /* Fist */
5.0f, /* Brass Knuckles */
5.0f, /* Golf Club */
5.0f, /* Nightstick */
5.0f, /* Knife */
5.0f, /* Baseball Bat */
5.0f, /* Shovel */
5.0f, /* Pool Cue */
5.0f, /* Katana */
5.0f, /* Chainsaw */
5.0f, /* Purple Dildo */
5.0f, /* Dildo */
5.0f, /* Vibrator */
5.0f, /* Silver Vibrator */
5.0f, /* Flowers */
5.0f, /* Cane */
75.0f, /* Grenade */
0.0f, /* Tear Gas */
75.0f, /* Molotov Cocktail */
0.0f, /* +EMPTY+ */
0.0f, /* +EMPTY+ */
0.0f, /* +EMPTY+ */
25.0f, /* 9mm (pistol) */
10.0f, /* Silenced 9mm */
45.0f, /* Desert Eagle */
40.0f, /* Shotgun */
10.0f, /* Sawnoff Shotgun */
10.0f, /* Combat Shotgun */
10.0f, /* Micro SMG/Uzi */
10.0f, /* MP5 */
10.0f, /* AK-47 */
10.0f, /* M4 */
20.0f, /* Tec-9 */
45.0f, /* Country Rifle */
45.0f, /* Sniper Rifle */
75.0f, /* RPG */
75.0f, /* HS Rocket */
5.0f, /* Flamethrower */
70.0f, /* Minigun */
75.0f, /* Satchel Charge */
0.0f, /* Detonator */
1.0f, /* Spraycan */
5.0f, /* Fire Extinguisher */
0.0f /* Camera */
};

extern uint32_t (*CWeapon_FireInstantHit)(WEAPON_SLOT_TYPE* _this, PED_TYPE* pFiringEntity, CVector* vecOrigin, CVector* muzzlePos, ENTITY_TYPE* targetEntity, CVector *target, CVector* originForDriveBy, int arg6, int muzzle);
extern uint32_t (*CWeapon__FireSniper)(WEAPON_SLOT_TYPE *pWeaponSlot, PED_TYPE *pFiringEntity, ENTITY_TYPE *a3, CVector *vecOrigin);

void CRemotePlayer::StoreBulletSyncData(BULLET_SYNC_DATA* pBulletSync)
{
	if (!m_pPlayerPed || !m_pPlayerPed->IsAdded()) return;

	BULLET_DATA btData;
	memset(&btData, 0, sizeof(BULLET_DATA));

	btData.vecOrigin.x = pBulletSync->vecOrigin.x;
	btData.vecOrigin.y = pBulletSync->vecOrigin.y;
	btData.vecOrigin.z = pBulletSync->vecOrigin.z;

	btData.vecPos.x = pBulletSync->vecPos.x;
	btData.vecPos.y = pBulletSync->vecPos.y;
	btData.vecPos.z = pBulletSync->vecPos.z;

	btData.vecOffset.x = pBulletSync->vecOffset.x;
	btData.vecOffset.y = pBulletSync->vecOffset.y;
	btData.vecOffset.z = pBulletSync->vecOffset.z;

	btData.uWeaponID = pBulletSync->byteWeaponID;

	Log("Receiving button data..");
	Log("vecOrigin: %f, %f, %f", btData.vecOrigin.x, btData.vecOrigin.y, btData.vecOrigin.z);
	Log("vecPos: %f, %f, %f", btData.vecPos.x, btData.vecPos.y, btData.vecPos.z);
	Log("vecOffset: %f, %f, %f", btData.vecOffset.x, btData.vecOffset.y, btData.vecOffset.z);
	Log("hit: %d, ID: %d", pBulletSync->byteHitType, pBulletSync->PlayerID);
	if (pBulletSync->byteHitType != 0)
	{
		if (btData.vecOffset.x > 300.0f ||
			btData.vecOffset.x < -300.0f ||
			btData.vecOffset.y > 300.0f ||
			btData.vecOffset.y < -300.0f ||
			btData.vecOffset.z > 300.0f ||
			btData.vecOffset.z < -300.0f
			)
		{
			return;
		}

		if (pBulletSync->byteHitType == 1)
		{
			if (pBulletSync->PlayerID == CPlayerPool::GetLocalPlayerID())
			{
				btData.pEntity = &pGame->FindPlayerPed()->m_pPed->entity;
			}
			else if (pBulletSync->PlayerID == m_PlayerID)
			{
				return;
			}
			else
			{
				CRemotePlayer* pRemotePlayer = CPlayerPool::GetSpawnedPlayer(pBulletSync->PlayerID);
				if (pRemotePlayer)
				{
					CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();

					if (pPlayerPed)
						btData.pEntity = &pPlayerPed->m_pPed->entity;
				}
			}
		}
		else if (pBulletSync->byteHitType == 2)
		{
			CVehicle* pVehicle = CVehiclePool::GetAt(pBulletSync->PlayerID);
			if (pVehicle)
			{
				btData.pEntity = &pVehicle->m_pVehicle->entity;
			}
		}

	}

	if (m_pPlayerPed->IsAdded())
	{
		uint8_t byteWeapon = pBulletSync->byteWeaponID;
		if (m_pPlayerPed->GetCurrentWeapon() != byteWeapon)
		{
			m_pPlayerPed->SetArmedWeapon(byteWeapon);
			if (m_pPlayerPed->GetCurrentWeapon() != byteWeapon)
			{
				m_pPlayerPed->GiveWeapon(byteWeapon, 9999);
				m_pPlayerPed->SetArmedWeapon(byteWeapon);
			}
		}
	}

	if(m_pPlayerPed->m_pPed && IsValidGamePed(m_pPlayerPed->m_pPed)) {
		WEAPON_SLOT_TYPE* pSlot =m_pPlayerPed->GetCurrentWeaponSlot();
		if(pSlot->dwState != 2){
			m_pPlayerPed->m_bWaitAnimationGunFlash = true;
		}

	}

	m_pPlayerPed->ProcessBulletData(&btData);
	m_pPlayerPed->FireInstant();
}

void CRemotePlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(m_PlayerID, dwColor);
}

void CRemotePlayer::Say(unsigned char* szText)
{
	char * szPlayerName = CPlayerPool::GetPlayerName(m_PlayerID);
	pChatWindow->AddChatMessage(szPlayerName,GetPlayerColor(), (char*)szText);
}

void calculateAimVector(CVector* vec1, CVector* vec2)
{
	float f1;
	float f2;
	float f3;

	f1 = atan2(vec1->x, vec1->y) - 1.570796370506287;
	f2 = sin(f1);
	f3 = cos(f1);
	vec2->x = vec1->y * 0.0 - f3 * vec1->z;
	vec2->y = f2 * vec1->z - vec1->x * 0.0;
	vec2->z = f3 * vec1->x - f2 * vec1->y;
}

void CRemotePlayer::UpdateAimFromSyncData(AIM_SYNC_DATA * pAimSync)
{
	if (!m_pPlayerPed) return;
	m_pPlayerPed->SetCameraMode(pAimSync->byteCamMode);

	CAMERA_AIM Aim;
	
    Aim.f1x = pAimSync->vecAimf.x;
    Aim.f1y = pAimSync->vecAimf.y;
    Aim.f1z = pAimSync->vecAimf.z;
    Aim.f2x = pAimSync->vecAimf.x;
    Aim.f2y = pAimSync->vecAimf.y;
    Aim.f2z = pAimSync->vecAimf.z;
    Aim.pos1x = pAimSync->vecAimPos.x;
    Aim.pos1y = pAimSync->vecAimPos.y;
    Aim.pos1z = pAimSync->vecAimPos.z;
    Aim.pos2x = pAimSync->vecAimPos.x;
    Aim.pos2y = pAimSync->vecAimPos.y;
    Aim.pos2z = pAimSync->vecAimPos.z;

	m_pPlayerPed->SetCurrentAim(&Aim);
	m_pPlayerPed->SetAimZ(pAimSync->fAimZ);
	
	float fExtZoom = (float)(pAimSync->byteCamExtZoom)/63.0f;
	m_pPlayerPed->SetCameraExtendedZoom(fExtZoom);

	WEAPON_SLOT_TYPE* pwstWeapon = m_pPlayerPed->GetCurrentWeaponSlot();
	if (pAimSync->byteWeaponState == WS_RELOADING)
		pwstWeapon->dwState = 2;		// Reloading
	else
		if (pAimSync->byteWeaponState != WS_MORE_BULLETS)
			pwstWeapon->dwAmmoInClip = (uint32_t)pAimSync->byteWeaponState;
		else
			if (pwstWeapon->dwAmmoInClip < 2)
				pwstWeapon->dwAmmoInClip = 2;
			
	m_bKeyboardOpened = pAimSync->bUnk;
}

void CRemotePlayer::StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync, uint32_t dwTime, uint8_t key)
{
	if( !dwTime || (dwTime - m_dwUnkTime) >= 0 )
	{
		m_dwUnkTime = dwTime;
		m_iKey0 = key;
		m_dwLastRecvTick = GetTickCount();
		memcpy(&m_ofSync, pofSync, sizeof(ONFOOT_SYNC_DATA));
		m_fReportedHealth = (float)pofSync->byteHealth;
		m_fReportedArmour = (float)pofSync->byteArmour;
		m_byteSpecialAction = pofSync->byteSpecialAction;
		m_byteUpdateFromNetwork = UPDATE_TYPE_ONFOOT;
		
		
		if(m_pPlayerPed)
		{
			if(m_pPlayerPed->IsInVehicle())
			{
				if( m_byteSpecialAction != SPECIAL_ACTION_ENTER_VEHICLE && 
				m_byteSpecialAction != SPECIAL_ACTION_EXIT_VEHICLE /*&& !sub_100A6F00()*/)
					RemoveFromVehicle();
			}
		}

		SetState(PLAYER_STATE_ONFOOT);
	}
}

void CRemotePlayer::StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync, uint32_t dwTime)
{
	if(!dwTime || (dwTime - m_dwUnkTime >= 0))
	{
		m_dwUnkTime = dwTime;

		memcpy(&m_icSync, picSync, sizeof(INCAR_SYNC_DATA));
		m_VehicleID = picSync->VehicleID;
		if (CVehiclePool::GetAt(m_VehicleID))
		{
			m_pCurrentVehicle = CVehiclePool::GetAt(m_VehicleID);
			if (m_pCurrentVehicle && m_pCurrentVehicle->IsAdded())
			{
				m_byteSeatID = 0;
				m_fReportedHealth = (float)picSync->bytePlayerHealth;
				m_fReportedArmour = (float)picSync->bytePlayerArmour;
				m_byteUpdateFromNetwork = UPDATE_TYPE_INCAR;
				m_dwLastRecvTick = GetTickCount();

				m_byteSpecialAction = 0;
				m_pCurrentVehicle->SetSirenState(picSync->byteSirenOn);

				m_pCurrentVehicle->m_iTurnState = static_cast<eTurnState>(picSync->HydraThrustAngle);

				if (m_pPlayerPed && !m_pPlayerPed->IsInVehicle())
					HandleVehicleEntryExit();

				SetState(PLAYER_STATE_DRIVER);
			}
		}
	}
}

void CRemotePlayer::StorePassengerFullSyncData(PASSENGER_SYNC_DATA *ppsSync)
{
	memcpy(&m_psSync, ppsSync, sizeof(PASSENGER_SYNC_DATA));
	m_VehicleID = ppsSync->VehicleID;
	m_byteSeatID = ppsSync->byteSeatFlags & 127;
	m_pCurrentVehicle = CVehiclePool::GetAt(m_VehicleID);
	m_fReportedHealth = (float)ppsSync->bytePlayerHealth;
	m_fReportedArmour = (float)ppsSync->bytePlayerArmour;
	m_byteUpdateFromNetwork = UPDATE_TYPE_PASSENGER;
	m_dwLastRecvTick = GetTickCount();

	if(m_pPlayerPed && !m_pPlayerPed->IsInVehicle())
		HandleVehicleEntryExit();

	SetState(PLAYER_STATE_PASSENGER);
}

void CRemotePlayer::RemoveFromVehicle()
{
	RwMatrix mat;

	if(m_pPlayerPed)
	{
		if(m_pPlayerPed->IsInVehicle())
		{
			m_pPlayerPed->GetMatrix(&mat);
			m_pPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.x, mat.pos.y, mat.pos.z);
		}
	}
}

void CRemotePlayer::HandleDeath()
{
	if (GetPlayerPed())
	{
		GetPlayerPed()->SetKeys(0, 0, 0);
		GetPlayerPed()->SetDead();
	}
	m_fReportedHealth = 0.0f;
	m_fReportedArmour = 0.0f;
	SetState(PLAYER_STATE_WASTED);

	memset(&m_ofSync, 0, sizeof(m_ofSync));
	memset(&m_icSync, 0, sizeof(m_icSync));
	memset(&m_psSync, 0, sizeof(m_psSync));

	HideGlobalMarker();
}

void CRemotePlayer::HandleAnimations() 
{
	if (!m_pPlayerPed)
	{
		return;
	}

	int flag = m_ofSync.dwAnimation >> 31;
	//pChatWindow->AddDebugMessage("FLAG REMOTE %d", flag);

	if (!flag)
	{
		uint16_t newAnim = (uint16_t)m_ofSync.dwAnimation;
		if (newAnim != m_usPlayingAnimIdx)
		{
			//pChatWindow->AddDebugMessage("applying %hu anium", newAnim);
			m_usPlayingAnimIdx = newAnim;
			m_pPlayerPed->PlayAnimByIdx(newAnim, 4.0f);
		}
		if (m_usPlayingAnimIdx)
		{
			if (!m_pPlayerPed->IsPlayingAnim(m_usPlayingAnimIdx))
			{
				m_pPlayerPed->PlayAnimByIdx(newAnim, 4.0f);
			}
		}
		m_bWasSettedFlag = true;
	}
	if (flag && m_bWasSettedFlag)
	{
		m_usPlayingAnimIdx = 0;
		// we have to reset anim
		m_pPlayerPed->PlayAnimByIdx(0, 4.0f);
		m_bWasSettedFlag = false;
	}
}

uint32_t CRemotePlayer::GetPlayerColor()
{
	return TranslateColorCodeToRGBA(m_PlayerID);
}

void CRemotePlayer::ShowGlobalMarker(short sX, short sY, short sZ)
{
	if(m_dwGlobalMarkerID)
	{
		pGame->DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}

	if(!m_pPlayerPed)
	{
		m_dwGlobalMarkerID = pGame->CreateRadarMarkerIcon(0, (float)sX, (float)sY, (float)sZ, m_PlayerID, 0);
	}
}

void CRemotePlayer::HideGlobalMarker()
{
	if(m_dwGlobalMarkerID)
	{
		pGame->DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}
}

void CRemotePlayer::StateChange(uint8_t byteNewState, uint8_t byteOldState)
{
}

bool CRemotePlayer::SurfingOnVehicle()
{
	if(GetState() == PLAYER_STATE_ONFOOT) 
	{
		if(m_ofSync.wSurfInfo != 0 && m_ofSync.wSurfInfo < MAX_VEHICLES &&
			m_ofSync.wSurfInfo != INVALID_VEHICLE_ID) // its an vehicle
		{
			return true;
		}
	}
	return false;
}

bool CRemotePlayer::SurfingOnObject()
{
	if(GetState() == PLAYER_STATE_ONFOOT) 
	{
		if(m_ofSync.wSurfInfo != 0 && m_ofSync.wSurfInfo >= MAX_VEHICLES &&
			m_ofSync.wSurfInfo < MAX_VEHICLES + MAX_OBJECTS && 
			m_ofSync.wSurfInfo != INVALID_OBJECT_ID) // its an object
		{
			return true;
		}
	}
	return false;
}

void CRemotePlayer::ProcessSurfing()
{
    if(SurfingOnVehicle()){
       	CVehicle *pVehicle = CVehiclePool::GetAt(m_ofSync.wSurfInfo);
		if(pVehicle){
			CVector vecOut;
			RwMatrix matVeh;
			RwMatrix matPed;

			pVehicle->GetMatrix(&matVeh);
			m_pPlayerPed->GetMatrix(&matPed);

			ProjectMatrix(&vecOut, &matVeh, &m_ofSync.vecSurfOffsets);
			matPed.pos.x = vecOut.x;
			matPed.pos.y = vecOut.y;
			matPed.pos.z = vecOut.z;
			m_pPlayerPed->SetMatrix(matPed);
		}
    }
	/*if(SurfingOnVehicle())
	{
		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
		if(pVehiclePool)
		{
			CVehicle *pVehicle = pVehiclePool->GetAt(m_ofSync.wSurfInfo);
			if(pVehicle) 
			{
				RwMatrix matVehicle;
				CVector vecMoveSpeed, vecTurnSpeed;

				pVehicle->GetMatrix(&matVehicle);
				pVehicle->GetMoveSpeedVector(&vecMoveSpeed);
				pVehicle->GetTurnSpeedVector(&vecTurnSpeed);
			
				matVehicle.pos.x += m_ofSync.vecSurfOffsets.x;
				matVehicle.pos.y += m_ofSync.vecSurfOffsets.y;
				matVehicle.pos.z += m_ofSync.vecSurfOffsets.z;

				m_pPlayerPed->SetMatrix(matVehicle);
				m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
				m_pPlayerPed->SetTurnSpeedVector(vecTurnSpeed);
			}
		}
	}
	else *//*if(SurfingOnObject())
	{
		m_ofSync.wSurfInfo -= MAX_VEHICLES; // derive proper object id
		CObjectPool *pObjectPool = pNetGame->GetObjectPool();
		if(pObjectPool)
		{
			CObject *pObject = pObjectPool->GetAt((uint8_t)m_ofSync.wSurfInfo);
			if(pObject) 
			{
				RwMatrix matObject;
				CVector vecMoveSpeed, vecTurnSpeed;

				pObject->GetMatrix(&matObject);
				pObject->GetMoveSpeedVector(&vecMoveSpeed);
				pObject->GetTurnSpeedVector(&vecTurnSpeed);
				
				matObject.pos.x += m_ofSync.vecSurfOffsets.x;
				matObject.pos.y += m_ofSync.vecSurfOffsets.y;
				matObject.pos.z += m_ofSync.vecSurfOffsets.z;

				m_pPlayerPed->SetMatrix(matObject);
				//m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
				//m_pPlayerPed->SetTurnSpeedVector(vecTurnSpeed);
			}
		}
	}*/
}