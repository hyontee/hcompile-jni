#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../chatwindow.h"
#include "../util/CJavaWrapper.h"
#include "../keyboard.h"
#include "../game/CTurnlights.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CKeyBoard *pKeyBoard;

void CVehiclePool::Free()
{
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++)
		Delete(VehicleID);
}

void CVehiclePool::Process()
{
	for(auto & pair : list) {
		auto pVehicle = pair.second;

		if(pVehicle->m_iTurnState == eTurnState::TURN_RIGHT)
		{
			if( !pVehicle->bIsOnRightPovorotnik ) {
				CTurnlights::SetEnabledLeft(pVehicle, false);
				CTurnlights::SetEnabledRight(pVehicle, true);
			}

		}
		else if(pVehicle->m_iTurnState == eTurnState::TURN_LEFT)
		{
			if( !pVehicle->bIsOnLeftPovorotnik ) {
				CTurnlights::SetEnabledLeft(pVehicle, true);
				CTurnlights::SetEnabledRight(pVehicle, false);
			}
		}
		else if(pVehicle->m_iTurnState == eTurnState::TURN_ALL)
		{
			if( !pVehicle->bIsOnAvariyka )
			{
				CTurnlights::SetEnabledLeft(pVehicle, true);
				CTurnlights::SetEnabledRight(pVehicle, true);
				pVehicle->m_iTurnState = eTurnState::TURN_ALL;
			}

		}
		else
		{
			if( pVehicle->bIsOnRightPovorotnik )
				CTurnlights::SetEnabledRight(pVehicle, false);

			if( pVehicle->bIsOnLeftPovorotnik )
				CTurnlights::SetEnabledLeft(pVehicle, false);

			if( pVehicle->bIsOnAvariyka ) {
				CTurnlights::SetEnabledLeft(pVehicle, false);
				CTurnlights::SetEnabledRight(pVehicle, false);

			}
			pVehicle->bIsOnAvariyka = false;
			pVehicle->m_iTurnState = eTurnState::TURN_OFF;
		}

		CVehicle* pTrailer = pVehicle->GetTrailer();
		if (pTrailer && !pTrailer->IsAdded())
		{
			RwMatrix matPos;
			pVehicle->GetMatrix(&matPos);
			pTrailer->TeleportTo(matPos.pos.x, matPos.pos.y, matPos.pos.z);
			pTrailer->Add();
		}

		pVehicle->ProcessDamage();

		if(pNetGame->m_bManualVehicleEngineAndLight)
		{
			pVehicle->EnableEngine(pVehicle->GetEngineState() == 1);
		}
		else
		{
			if(pVehicle->GetEngineState() == -1)
			{
				if(!pVehicle->IsDriverLocalPlayer())
					pVehicle->EnableEngine(false);

				else
					pVehicle->EnableEngine(true);
			}
			else if(pVehicle->GetEngineState() != -1)
			{
				if(pVehicle->GetEngineState() == 0)
					pVehicle->EnableEngine(false);

				if(pVehicle->GetEngineState() == 1)
					pVehicle->EnableEngine(true);
			}
		}

		if (pVehicle->GetDoorState())
			pVehicle->SetDoorState(1);

		else
			pVehicle->SetDoorState(0);

		if(pVehicle->IsDriverLocalPlayer())
		{
			pVehicle->SetInvulnerable(false);
		}
		else
		{
			pVehicle->SetInvulnerable(true);
		}

		pVehicle->ProcessMarkers(200.0f);
	}
}
#include "..//game/CCustomPlateManager.h"
bool CVehiclePool::New(NEW_VEHICLE *pNewVehicle)
{
#ifdef _CDEBUG
	pChatWindow->AddDebugMessage(OBFUSCATE("Added veh %d %d"), pNewVehicle->VehicleID, pNewVehicle->iVehicleType);
#endif
	if(GetAt(pNewVehicle->VehicleID))
	{
		pChatWindow->AddDebugMessage(OBFUSCATE("Warning: vehicle %u was not deleted"), pNewVehicle->VehicleID);
		Delete(pNewVehicle->VehicleID);
	}

	CVehicle* pVehicle;
	try {
		pVehicle = new CVehicle(pNewVehicle->iVehicleType,
							pNewVehicle->vecPos.x,
							pNewVehicle->vecPos.y,
							pNewVehicle->vecPos.z,
							pNewVehicle->fRotation,
							pNewVehicle->byteAddSiren);

		list[pNewVehicle->VehicleID] = pVehicle;
	} catch (const std::exception &e) {
		pChatWindow->AddDebugMessage("Warning: vehicle %u not created", pNewVehicle->VehicleID);
		return false;
	}
	// colors
	if(pNewVehicle->aColor1 != -1 || pNewVehicle->aColor2 != -1)
	{
		pVehicle->SetColor(
			pNewVehicle->aColor1, pNewVehicle->aColor2 );
	}

	// health
	pVehicle->SetHealth(pNewVehicle->fHealth);

	// interior
	if (pNewVehicle->byteInterior) {
		pVehicle->LinkToInterior(pNewVehicle->byteInterior);
	}

	// damage status
	if(pNewVehicle->dwPanelDamageStatus ||
		pNewVehicle->dwDoorDamageStatus ||
		pNewVehicle->byteLightDamageStatus)
	{
		pVehicle->UpdateDamageStatus(
			pNewVehicle->dwPanelDamageStatus,
			pNewVehicle->dwDoorDamageStatus,
			pNewVehicle->byteLightDamageStatus, pNewVehicle->byteTireDamageStatus);
	}

	pVehicle->SetWheelPopped(pNewVehicle->byteTireDamageStatus);
	return true;
}

bool CVehiclePool::Delete(VEHICLEID VehicleID)
{
	if(!GetAt(VehicleID))
		return false;

	delete list[VehicleID];

	list.erase(VehicleID);

	return true;
}

VEHICLEID CVehiclePool::FindIDFromGtaPtr(VEHICLE_TYPE *pGtaVehicle)
{
	for(auto & pair : list) {
		auto &VehicleID = pair.first;
		if(pair.second) {
			if (pair.second->m_pVehicle) {
				if (pGtaVehicle == pair.second->m_pVehicle) {
					return VehicleID;
				}
			}
		}
	}
	return INVALID_VEHICLE_ID;
}

VEHICLEID CVehiclePool::FindIDFromRwObject(RwObject* pRWObject)
{
	for(auto & pair : list) {
		auto &VehicleID = pair.first;
		if(pair.second->m_pVehicle)
		{
			if(pRWObject == (RwObject*)pair.second->m_pVehicle->entity.m_RwObject)
			{
				return VehicleID;
			}
		}
	}
	return INVALID_VEHICLE_ID;
}

int CVehiclePool::FindGtaIDFromID(int iID)
{
	auto pVehicle = GetAt(iID);
	if(pVehicle && pVehicle->m_pVehicle)
		return pVehicle->m_dwGTAId;

	return INVALID_VEHICLE_ID;
}

int CVehiclePool::FindNearestToLocalPlayerPed()
{
	float fLeastDistance = 10000.0f;
	float fThisDistance = 0.0f;
	VEHICLEID ClosetSoFar = INVALID_VEHICLE_ID;

	for(auto &pair : list) {
		auto pVehicle = pair.second;

		fThisDistance = pVehicle->GetDistanceFromLocalPlayerPed();
		if(fThisDistance < fLeastDistance)
		{
			fLeastDistance = fThisDistance;
			ClosetSoFar = pair.first;
		}
	}

	return ClosetSoFar;
}

void CVehiclePool::LinkToInterior(VEHICLEID VehicleID, int iInterior)
{
	if(GetAt(VehicleID))
		GetAt(VehicleID)->LinkToInterior(iInterior);
}

void CVehiclePool::NotifyVehicleDeath(VEHICLEID VehicleID)
{
	if(CPlayerPool::GetLocalPlayer()->m_LastVehicle != VehicleID) return;
	Log(OBFUSCATE("CVehiclePool::NotifyVehicleDeath"));

	RakNet::BitStream bsDeath;
	bsDeath.Write(VehicleID);
	pNetGame->GetRakClient()->RPC(&RPC_VehicleDestroyed, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	CPlayerPool::GetLocalPlayer()->m_LastVehicle = INVALID_VEHICLE_ID;
}

void CVehiclePool::AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked)
{
	CVehicle *pVehicle = list[VehicleID];

	if(pVehicle)
	{
		pVehicle->m_byteObjectiveVehicle = byteObjective;
		pVehicle->m_bSpecialMarkerEnabled = false;

		pVehicle->SetDoorState(byteDoorsLocked);
	}
}

#include "../game/CTurnlights.h"
#include "graphics/CBuyAuto.h"
#include "graphics/CInventory.h"
#include "graphics/CInventoryTrade.h"
#include "graphics/CInventoryUniversal.h"
#include "graphics/CTuning.h"

extern int showSpeed;
extern float fuel;
extern float maxFuel;
void CVehiclePool::UpdateSpeed()
{
	if (!pGame) {
		return;
	}

	if (!pGame->FindPlayerPed()) {
		return;
	}

    CPlayerPed *pPed = pGame->FindPlayerPed();
    if(!pPed)
        return;

    CVehicle* pVehicle = pPed->GetCurrentVehicle();

	if(!pVehicle)
        return;

	if(pVehicle)
	{

		CVector vecMoveSpeed = {1, 1, 1};
		pVehicle->GetMoveSpeedVector(&vecMoveSpeed);

		int speed =
				sqrt((vecMoveSpeed.x * vecMoveSpeed.x) + (vecMoveSpeed.y * vecMoveSpeed.y) +
					 (vecMoveSpeed.z * vecMoveSpeed.z)) * 183;

		g_pJavaWrapper->SetSpeedometerSpeed(speed);
		g_pJavaWrapper->SetSpeedometerCarHP((int) pVehicle->GetHealth());
		g_pJavaWrapper->SetSpeedometerFuel((fuel/maxFuel)*100, fuel);
		g_pJavaWrapper->SetEngineState(pVehicle->GetEngineState());
		g_pJavaWrapper->SetLightState(pVehicle->m_bLightState);
		g_pJavaWrapper->SetLockState(pVehicle->m_bDoorsLocked);
		g_pJavaWrapper->UpdateSpeedometerTurnlights(pVehicle->m_iTurnState);
	}
}

bool CVehiclePool::VehicleCollisionProcess(int CColSphere1, int CColSphere2)
{
	for(auto & pair : list) {
		auto pVehicle = pair.second;
		if(pVehicle)
		{
			if(pVehicle->IsAdded() &&
				pVehicle->GetDistanceFromCamera() <= 300.0f &&
			   	!pVehicle->IsEntityIgnored() &&
				   pVehicle->TestSphereCastVsEntity(&CColSphere1, &CColSphere2, true))
			{
				return true;
			}
		}
	}

	return false;
}