#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../util/armhook.h"
#include "RW/common.h"

#include "..//CDebugInfo.h"

#include "CActorPed.h"

extern CGame* pGame;
extern CNetGame* pNetGame;

CActorPed::CActorPed(uint16_t usModel, VECTOR vecPosition, float fRotation, float fHealth, bool bInvulnerable)
{
	m_dwGTAId = 0;
	m_pPed = nullptr;
	m_pEntity = nullptr;

	if (!IsPedModel(usModel)) 
		usModel = 0;
	
	int iTryCount = 0;
	if (!pGame->IsModelLoaded(usModel)) 
	{
		pGame->RequestModel(usModel);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(usModel))
		{
			usleep(1);
			if(iTryCount > 20)
			{
				Log(OBFUSCATE("Warning: Model %u wouldn't load in time!"), usModel);
				return;
			}

			iTryCount++;
		}
	}
	
	if (Travel.X != 0.0 && Travel.Y != 0.0 && Travel.Z != 0.0) Log(OBFUSCATE("[CActorPed] usModel: %d, Travel.X: %f, Travel.Y: %f, Travel.Z: %f"), usModel, Travel.X, Travel.Y, Travel.Z);
	else Log(OBFUSCATE("[CActorPed] usModel: %d, vecPosition.X: %f, vecPosition.Y: %f, vecPosition.Z: %f"), usModel, vecPosition.X, vecPosition.Y, vecPosition.Z);

	uint32_t actorGTAId = 0;
	if (Travel.X != 0.0 && Travel.Y != 0.0 && Travel.Z != 0.0) ScriptCommand(&create_actor, 22, usModel, Travel.X, Travel.Y, Travel.Z, &actorGTAId);
	else ScriptCommand(&create_actor, 22, usModel, vecPosition.X, vecPosition.Y, vecPosition.Z, &actorGTAId);

	m_dwGTAId = actorGTAId;
	m_pPed = GamePool_Ped_GetAt(m_dwGTAId);
	m_pEntity = (ENTITY_TYPE*)m_pPed;

	ForceTargetRotation(fRotation);
	TeleportTo(vecPosition.X, vecPosition.Y, vecPosition.Z);

	if (fHealth < 1.0f)
	{
		SetDead();
	}
	else 
	{
		SetHealth(fHealth);
	}

	//ScriptCommand(&lock_actor, m_dwGTAId, 1);
	m_pPed->bNoCriticalHits = 1;
	m_pEntity->physicalFlags.bDontApplySpeed = 1;

	if (bInvulnerable)
	{
		m_pEntity->physicalFlags.bBulletProof = 1;
		m_pEntity->physicalFlags.bFireProof = 1;
		m_pEntity->physicalFlags.bCollisionProof = 1;
		m_pEntity->physicalFlags.bMeeleProof = 1;
		m_pEntity->physicalFlags.bInvulnerable = 1;
		//ScriptCommand(&set_actor_immunities, m_dwGTAId, 1, 1, 1, 1, 1);
	}
	else
	{
		m_pEntity->physicalFlags.bBulletProof = 0;
		m_pEntity->physicalFlags.bFireProof = 0;
		m_pEntity->physicalFlags.bCollisionProof = 0;
		m_pEntity->physicalFlags.bMeeleProof = 0;
		m_pEntity->physicalFlags.bInvulnerable = 0;
		//ScriptCommand(&set_actor_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
	}
	
	/*if (pNetGame && pNetGame->GetPlayerPool())
	{
		CPlayerPed* pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
		if (pPlayerPed)
		{
			Attack(pPlayerPed);
			
			MATRIX4X4 posq;
			pPlayerPed->GetMatrix(&posq);
			VECTOR vec = {posq.pos.X, posq.pos.Y, posq.pos.Z};
			MoveToPoint(vec);
		}
	}*/
	SetNeedDraw(false);
}

CActorPed::~CActorPed()
{
	Destroy();
}

void CActorPed::Destroy()
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if (IsValidGamePed(m_pPed)) 
	{
		// CPed::entity.vtable + 0x4 destructor
		((void (*)(PED_TYPE*))(*(void**)(m_pPed->entity.vtable + 0x4)))(m_pPed);
	}

	m_pPed = nullptr;
	m_pEntity = nullptr;
	m_dwGTAId = 0;
}

void CActorPed::setInvulnerable(bool status)
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	
	if (status)
	{
		m_pEntity->physicalFlags.bDontApplySpeed = 1;
		
		m_pEntity->physicalFlags.bBulletProof = 1;
		m_pEntity->physicalFlags.bFireProof = 1;
		m_pEntity->physicalFlags.bCollisionProof = 1;
		m_pEntity->physicalFlags.bMeeleProof = 1;
		m_pEntity->physicalFlags.bInvulnerable = 1;
	}
	else
	{
		m_pEntity->physicalFlags.bDontApplySpeed = 0;
		
		m_pEntity->physicalFlags.bBulletProof = 0;
		m_pEntity->physicalFlags.bFireProof = 0;
		m_pEntity->physicalFlags.bCollisionProof = 0;
		m_pEntity->physicalFlags.bMeeleProof = 0;
		m_pEntity->physicalFlags.bInvulnerable = 0;
	}
}

void CActorPed::SetHealth(float fHealth)
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if (!IsValidGamePed(m_pPed)) return;

	m_pPed->fHealth = fHealth;
}

void CActorPed::SetDead()
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if (!IsValidGamePed(m_pPed)) return;

	MATRIX4X4 matEntity;
	GetMatrix(&matEntity);
	TeleportTo(matEntity.pos.X, matEntity.pos.Y, matEntity.pos.Z);

	SetHealth(0.0f);
	ScriptCommand(&kill_actor, m_dwGTAId);
}

bool CActorPed::IsDead()
{
	if(!m_pPed) return true;
	if(m_pPed->fHealth > 0.0f) return false;
	return true;
}

void CActorPed::ForceTargetRotation(float fRotation)
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if (!IsValidGamePed(m_pPed)) return;

	m_pPed->fRotation1 = DegToRad(fRotation);
	m_pPed->fRotation2 = DegToRad(fRotation);

	//ScriptCommand(&set_actor_z_angle, m_dwGTAId, fRotation);
	MATRIX4X4 mat;
	GetMatrix(&mat);
	RwMatrixRotate(&mat, eAxis::Z, fRotation);
	UpdateMatrix(mat);
}

void CActorPed::ApplyAnimation(char* szAnimName, char* szAnimFile, float fDelta, int bLoop, int bLockX, int bLockY, int bFreeze, int uiTime)
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if (!IsValidGamePed(m_pPed)) return;
	if (!strcasecmp(szAnimFile, "SEX")) return;
	
	int iWaitAnimLoad = 0, iRet = pGame->IsAnimationLoaded(szAnimFile);
	if (iRet == 0)
	{
		pGame->RequestAnimation(szAnimFile);
		while (!pGame->IsAnimationLoaded(szAnimFile))
		{
			usleep(1);
			iWaitAnimLoad++;
			if (iWaitAnimLoad > 10) return;
		}
	}
	else if(iRet == -1) return; // preventing client get stuck in loop when bad animation lib applies.

	ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimFile, fDelta, bLoop, bLockX, bLockY, bFreeze, uiTime);
}

void CActorPed::PutDirectlyInVehicle(int iVehicleID, int iSeat)
{
	if (!m_pPed) return;
	if (!GamePool_Vehicle_GetAt(iVehicleID)) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

	VEHICLE_TYPE* pVehicle = GamePool_Vehicle_GetAt(iVehicleID);

	if (pVehicle->fHealth == 0.0f) return;
	if (pVehicle->entity.vtable == SA_ADDR(0x5C7358)) return;
	// check seatid (допилить)

	if (iSeat == 0)
	{
		if (pVehicle->pDriver && IN_VEHICLE(pVehicle->pDriver)) return;
		ScriptCommand(&put_actor_in_car, m_dwGTAId, iVehicleID);
	}
	else
	{
		iSeat--;
		ScriptCommand(&put_actor_in_car2, m_dwGTAId, iVehicleID, iSeat);
	}
}

void CActorPed::RemoveFromVehicle()
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

	MATRIX4X4 mat;

	if (IN_VEHICLE(m_pPed))
	{
		GetMatrix(&mat);
		RemoveFromVehicleAndPutAt(mat.pos.X, mat.pos.Y, mat.pos.Z);
	}
}

void CActorPed::RemoveFromVehicleAndPutAt(float fX, float fY, float fZ)
{
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if (m_pPed && IN_VEHICLE(m_pPed))
	{
		ScriptCommand(&remove_actor_from_car_and_put_at, m_dwGTAId, fX, fY, fZ);
	}
}

int GameGetWeaponModelIDFromWeaponID(int iWeaponID);
void CActorPed::GiveWeapon(int iWeaponID, int iAmmo)
{
	if (!m_pPed || !m_dwGTAId) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

	int iModelID = 0;
	iModelID = GameGetWeaponModelIDFromWeaponID(iWeaponID);

	if (iModelID == -1) return;

	if (!pGame->IsModelLoaded(iModelID))
	{
		pGame->RequestModel(iModelID);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(iModelID)) sleep(1);
	}

	((int(*)(uintptr_t, unsigned int, int))(SA_ADDR(0x43429C + 1)))((uintptr_t)m_pPed, iWeaponID, iAmmo); // CPed::GiveWeapon(thisptr, weapoid, ammo)
	((int(*)(uintptr_t, unsigned int))(SA_ADDR(0x434528 + 1)))((uintptr_t)m_pPed, iWeaponID);	// CPed::SetCurrentWeapon(thisptr, weapid)
}

void CActorPed::ClearAllTasks()
{
	if (!GamePool_Ped_GetAt(m_dwGTAId) || !m_pPed) return;

	ScriptCommand(&clear_char_tasks, m_dwGTAId);
}

void CActorPed::ClearWeapons()
{
	if (!GamePool_Ped_GetAt(m_dwGTAId) || !m_pPed) return;

	uintptr_t dwPedPtr = (uintptr_t)m_pPed;
	((uint32_t(*)(uintptr_t, int, int, int))(SA_ADDR(0x4345AC + 1)))(dwPedPtr, 1, 1, 1); // CPed::ClearWeapons(void)
}

void CActorPed::Attack(CPlayerPed* pPlayerPed)
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if (IN_VEHICLE(m_pPed)) return;

	ScriptCommand(&set_actor_attack_actor, m_dwGTAId, pPlayerPed->m_dwGTAId, 2500);
	ScriptCommand(&task_kill_char_on_foot, m_dwGTAId, pPlayerPed->m_dwGTAId);
}

void CActorPed::MoveToPoint(VECTOR vecPos, bool fastTravel)
{
	vecPos.Z = (( float (*)(float, float))(SA_ADDR(0x42A7C4 + 1)))(vecPos.X, vecPos.Y);
	vecPos.Z += 1.5f;
	
	Travel.X = vecPos.X;
	Travel.Y = vecPos.Y;
	Travel.Z = vecPos.Z;
	
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if (IN_VEHICLE(m_pPed)) return;
	
	if (fastTravel) TeleportTo(vecPos.X, vecPos.Y, vecPos.Z);
	else ScriptCommand(&task_char_slide_to_coord, m_dwGTAId, vecPos.X, vecPos.Y, vecPos.Z, 125, 4);
}

void CActorPed::Process()
{
	if (!m_pPed) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;
	
	MATRIX4X4 matPlayer;
	GetMatrix(&matPlayer);

	RwV3d rwPosition;
	rwPosition.x = matPlayer.pos.X;
	rwPosition.y = matPlayer.pos.Y;
	rwPosition.z = matPlayer.pos.Z;

	RwV3d rwOutResult;
				
	((void (*)(RwV3d const&, RwV3d *, float *, float *, bool, bool))(g_libGTASA + 0x54EEC0 + 1))(rwPosition, &rwOutResult, 0, 0, 0, 0);
	if(rwOutResult.z < 1.0f)
	{
		if (GetDistanceFromLocalPlayerPed() > 3.0f)
		{
			if (IsAdded())
			{
				Remove();
			}
		}
		else
		{
			if (!IsAdded())
			{
				MATRIX4X4 matPos;
				GetMatrix(&matPos);
				TeleportTo(matPos.pos.X, matPos.pos.Y, matPos.pos.Z);
				Add();
			}
		}
	}
	else
	{
		if (!IsAdded())
		{
			MATRIX4X4 matPos;
			GetMatrix(&matPos);
			TeleportTo(matPos.pos.X, matPos.pos.Y, matPos.pos.Z);
			Add();
		}
	}
}

void CActorPed::SetWeaponSkill(int8_t weaponSkill)
{
	if(!m_pPed) return;
	*(int8_t *)((uintptr_t)m_pPed + 0x72C) = weaponSkill;
}

void CActorPed::GetBonePosition(int iBoneID, VECTOR* vecOut)
{
	if(!m_pPed) return;
	if(m_pEntity->vtable == SA_ADDR(0x5C7358)) return;

	(( void (*)(PED_TYPE*, VECTOR*, int, int))(SA_ADDR(0x436590 + 1)))(m_pPed, vecOut, iBoneID, 0);
}

void CActorPed::SetFloatingText(char* szText, uint32_t dwColor)
{
	if(!szText) return;

	strncpy(m_szFloatingText, szText, 2048);
	m_dwFloatingTextColor = dwColor;
}

char* CActorPed::GetFloatingTagText()
{
	return m_szFloatingText;
}

uint32_t CActorPed::GetFloatingTagTextColor()
{
	return m_dwFloatingTextColor;
}

float CActorPed::GetHealth()
{
	if(!m_pPed)
		return 0.0f;

	return m_pPed->fHealth;
}

float CActorPed::GetArmour()
{
	if(!m_pPed)
		return 0.0f;

	return m_pPed->fArmour;
}

void CActorPed::PlayCustomAction(int type, int action, int action2)
{
	switch(type)
	{
		case ACTION_UPDATE_HEALTH:
		{
			if(m_pPed)
				m_pPed->fHealth = (float)action;
			break;
		}

		case ACTION_UPDATE_ARMOUR:
		{
			if(m_pPed)
				m_pPed->fArmour = (float)action;
			break;
		}

		case ACTION_CLIMB_OR_JUMP:
		{
			this->JumpAction(action);
			break;
		}

		case ACTION_REMOVE_FROM_VEHICLE:
		{
			this->RemoveFromVehicle();
			break;
		}

		case ACTION_PUT_INTO_VEHICLE:
		{
			if(pNetGame)
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

				if(pVehiclePool)
				{
					CVehicle* pVehicle = pVehiclePool->GetAt(action);

					if(pVehicle)
					{
						int iCarID = pVehiclePool->FindGtaIDFromID((int)action);
						this->PutDirectlyInVehicle(iCarID, action2);
					}
				}
			}

			break;
		}

		case ACTION_SET_DEAD:
		{
			this->SetDead();
			break;
		}

		case ACTION_CLEAR_TASKS:
		{
			this->ClearAllTasks();
			break;
		}

		case ACTION_LOCK_ACTOR:
		{
			this->LockActor(action);
			break;
		}

		case ACTION_ENTER_VEHICLE:
		{
			if(pNetGame)
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

				if(pVehiclePool)
				{
					CVehicle* pVehicle = pVehiclePool->GetAt(action);

					if(pVehicle)
					{
						int iCarID = pVehiclePool->FindGtaIDFromID((int)action);
						// this->PutDirectlyInVehicle(iCarID, action2);
						this->EnterVehicle(iCarID, action2 == 1 ? true : false);
					}
				}
			}

			break;
		}

		case ACTION_EXIT_VEHICLE:
		{
			this->ExitCurrentVehicle();
			break;
		}

		default:
		{
			break;
		}
	}
}

void CActorPed::JumpAction(int action)
{
	ScriptCommand(&task_climb, m_dwGTAId, action);
}

void CActorPed::LockActor(int state)
{
	ScriptCommand(&lock_actor, m_dwGTAId, state);
}

void CActorPed::EnterVehicle(int iVehicleID, bool bPassenger)
{
	if (!m_pPed) return;
	
	VEHICLE_TYPE* ThisVehicleType;
	if ((ThisVehicleType = GamePool_Vehicle_GetAt(iVehicleID)) == 0) return;
	if (!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if (bPassenger)
	{
		if (ThisVehicleType->entity.nModelIndex == TRAIN_PASSENGER &&
			(m_pPed == GamePool_FindPlayerPed()))
		{
			ScriptCommand(&put_actor_in_car2, m_dwGTAId, iVehicleID, -1);
		}
		else
		{
			ScriptCommand(&send_actor_to_car_passenger,m_dwGTAId,iVehicleID, 3000, -1);
		}
	}
	else
		ScriptCommand(&send_actor_to_car_driverseat, m_dwGTAId, iVehicleID, 3000);
}

int CActorPed::GetCurrentVehicleID()
{
	if(!m_pPed) return 0;

	VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE *)m_pPed->pVehicle;
	return GamePool_Vehicle_GetIndex(pVehicle);
}

void CActorPed::ExitCurrentVehicle()
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	VEHICLE_TYPE* ThisVehicleType = 0;

	if(IN_VEHICLE(m_pPed))
	{
		if(GamePool_Vehicle_GetIndex((VEHICLE_TYPE*)m_pPed->pVehicle))
		{
			int index = GamePool_Vehicle_GetIndex((VEHICLE_TYPE*)m_pPed->pVehicle);
			ThisVehicleType = GamePool_Vehicle_GetAt(index);
			if(ThisVehicleType)
			{
				if(	ThisVehicleType->entity.nModelIndex != TRAIN_PASSENGER &&
					ThisVehicleType->entity.nModelIndex != TRAIN_PASSENGER_LOCO)
				{
					ScriptCommand(&make_actor_leave_car, m_dwGTAId, GetCurrentVehicleID());
				}
			}
		}
	}
}
