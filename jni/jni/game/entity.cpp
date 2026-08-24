#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../chatwindow.h"
#include "entity.h"
#include "../util/CJavaWrapper.h"
#include "CSettings.h"

#include <cmath>

extern CJavaWrapper* pJavaWrapper;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

void CEntity::Add()
{
	if (!m_pEntity || m_pEntity->vtable == 0x5C7358)
	{
		return;
	}

	if (!m_pEntity->dwUnkModelRel)
	{
		CVector vec = { 0.0f,0.0f,0.0f };
		SetMoveSpeedVector(vec);
		SetTurnSpeedVector(vec);

		WorldAddEntity((uintptr_t)m_pEntity);

		RwMatrix mat;
		GetMatrix(&mat);
		TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
	}
}

void CEntity::SetGravityProcessing(bool bProcess)
{
	if(!m_pEntity || IsGameEntityArePlaceable(m_pEntity))
		return;
}

void CEntity::UpdateRwMatrixAndFrame()
{
	if (m_pEntity && m_pEntity->vtable != SA_ADDR(0x5C7358))
	{
		if (m_pEntity->m_RwObject)
		{
			if (m_pEntity->mat)
			{
				uintptr_t pRwMatrix = *(uintptr_t*)(m_pEntity->m_RwObject + 4) + 0x10;
				// CMatrix::UpdateRwMatrix
				((void (*) (RwMatrix*, uintptr_t))(SA_ADDR(0x3E862C + 1)))(m_pEntity->mat, pRwMatrix);

				// CEntity::UpdateRwFrame
				((void (*) (ENTITY_TYPE*))(SA_ADDR(0x39194C + 1)))(m_pEntity);
			}
		}
	}
}

void CEntity::UpdateMatrix(RwMatrix mat)
{
	if (m_pEntity)
	{
		if (m_pEntity->mat)
		{
			// CPhysical::Remove
			((void (*)(ENTITY_TYPE*))(*(uintptr_t*)(m_pEntity->vtable + 0x10)))(m_pEntity);

			SetMatrix(mat);
			UpdateRwMatrixAndFrame();

			// CPhysical::Add
			((void (*)(ENTITY_TYPE*))(*(uintptr_t*)(m_pEntity->vtable + 0x8)))(m_pEntity);
		}
	}
}

void CEntity::SetCollisionChecking()
{
	*(uint32_t*)((uintptr_t)m_pEntity + 28) &= 0xFFFFFFFE;
}

void CEntity::Render()
{
	uintptr_t pRwObject = m_pEntity->m_RwObject;

	int iModel = GetModelIndex();
	if (iModel >= 400 && iModel <= 611 && pRwObject)
	{
		// CVisibilityPlugins::SetupVehicleVariables
	}

	// CEntity::PreRender
	((void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable + 0x48)))(m_pEntity);

	// CRenderer::RenderOneNonRoad
	((void (*)(ENTITY_TYPE*))(SA_ADDR(0x3B1690 + 1)))(m_pEntity);
}

void CEntity::Remove()
{
	if (!m_pEntity || m_pEntity->vtable == 0x5C7358)
	{
		return;
	}

	if (m_pEntity->dwUnkModelRel) {
		WorldRemoveEntity((uintptr_t)m_pEntity);
	}
}

// 0.3.7
void CEntity::GetMatrix(RwMatrix* Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	Matrix->right.x = m_pEntity->mat->right.x;
	Matrix->right.y = m_pEntity->mat->right.y;
	Matrix->right.z = m_pEntity->mat->right.z;

	Matrix->up.x = m_pEntity->mat->up.x;
	Matrix->up.y = m_pEntity->mat->up.y;
	Matrix->up.z = m_pEntity->mat->up.z;

	Matrix->at.x = m_pEntity->mat->at.x;
	Matrix->at.y = m_pEntity->mat->at.y;
	Matrix->at.z = m_pEntity->mat->at.z;

	Matrix->pos.x = m_pEntity->mat->pos.x;
	Matrix->pos.y = m_pEntity->mat->pos.y;
	Matrix->pos.z = m_pEntity->mat->pos.z;
}

// 0.3.7
void CEntity::SetMatrix(RwMatrix Matrix)
{
	if (!m_pEntity) return;
	if (!m_pEntity->mat) return;

	m_pEntity->mat->right.x = Matrix.right.x;
	m_pEntity->mat->right.y = Matrix.right.y;
	m_pEntity->mat->right.z = Matrix.right.z;

	m_pEntity->mat->up.x = Matrix.up.x;
	m_pEntity->mat->up.y = Matrix.up.y;
	m_pEntity->mat->up.z = Matrix.up.z;

	m_pEntity->mat->at.x = Matrix.at.x;
	m_pEntity->mat->at.y = Matrix.at.y;
	m_pEntity->mat->at.z = Matrix.at.z;

	m_pEntity->mat->pos.x = Matrix.pos.x;
	m_pEntity->mat->pos.y = Matrix.pos.y;
	m_pEntity->mat->pos.z = Matrix.pos.z;
}

// 0.3.7
void CEntity::GetMoveSpeedVector(CVector* Vector)
{
	if (!m_pEntity) return;
	Vector->x = m_pEntity->vecMoveSpeed.x;
	Vector->y = m_pEntity->vecMoveSpeed.y;
	Vector->z = m_pEntity->vecMoveSpeed.z;
}

// 0.3.7
void CEntity::SetMoveSpeedVector(CVector Vector)
{
	if (!m_pEntity) return;
	m_pEntity->vecMoveSpeed.x = Vector.x;
	m_pEntity->vecMoveSpeed.y = Vector.y;
	m_pEntity->vecMoveSpeed.z = Vector.z;
}

void CEntity::GetTurnSpeedVector(CVector* Vector)
{
	if (!m_pEntity) return;
	Vector->x = m_pEntity->vecTurnSpeed.x;
	Vector->y = m_pEntity->vecTurnSpeed.y;
	Vector->z = m_pEntity->vecTurnSpeed.z;
}

void CEntity::SetTurnSpeedVector(CVector Vector)
{
	if (!m_pEntity) return;
	m_pEntity->vecTurnSpeed.x = Vector.x;
	m_pEntity->vecTurnSpeed.y = Vector.y;
	m_pEntity->vecTurnSpeed.z = Vector.z;
}

// 0.3.7
uint16_t CEntity::GetModelIndex()
{
	if (!m_pEntity)
	{
		return 0;
	}
	return m_pEntity->nModelIndex;
}

// 0.3.7
bool CEntity::IsAdded()
{
	if(m_pEntity)
	{
		if(m_pEntity->vtable == SA_ADDR(0x5C7358)) // CPlaceable
			return false;

		if(m_pEntity->dwUnkModelRel)
			return true;
	}

	return false;
}

// 0.3.7
bool CEntity::SetModelIndex(unsigned int uiModel)
{
	if(!m_pEntity) return false;

	int iTryCount = 0;
	if(!pGame->IsModelLoaded(uiModel) && !IsValidModel(uiModel))
	{
		pGame->RequestModel(uiModel);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(uiModel))
		{
			usleep(1000);
			if(iTryCount > 200)
			{
				if(pChatWindow) pChatWindow->AddDebugMessage(OBFUSCATE("Warning: Model %u wouldn't load in time!"), uiModel);
				return false;
			}

			iTryCount++;
		}
	}

	// CEntity::DeleteRWObject()
	(( void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable+0x24)))(m_pEntity);
	m_pEntity->nModelIndex = uiModel;
	// CEntity::SetModelIndex()
	(( void (*)(ENTITY_TYPE*, unsigned int))(*(void**)(m_pEntity->vtable+0x18)))(m_pEntity, uiModel);

	return true;
}

extern int showHud;
extern CSettings* pSettings;
// 0.3.7
void CEntity::TeleportTo(float fX, float fY, float fZ)
{
	if(m_pEntity && m_pEntity->vtable != (SA_ADDR(0x5C7358))) /* CPlaceable */
	{
		uint16_t modelIndex = m_pEntity->nModelIndex;
		if(	modelIndex != TRAIN_PASSENGER_LOCO &&
			modelIndex != TRAIN_FREIGHT_LOCO &&
			modelIndex != TRAIN_TRAM)
			(( void (*)(ENTITY_TYPE*, float, float, float, bool))(*(void**)(m_pEntity->vtable+0x3C)))(m_pEntity, fX, fY, fZ, 0);
		else
			ScriptCommand(&put_train_at, m_dwGTAId, fX, fY, fZ);
	}

	if(pNetGame)
	{
		if(pNetGame->GetGameState() == GAMESTATE_CONNECTED)
		{
			if(showHud)
			{
                if(pSettings->GetReadOnly().iNewHud)
                    g_pJavaWrapper->ShowHUD(true);
			}
		}
	}
}

float CEntity::GetDistanceFromCamera()
{
	RwMatrix matEnt;

	if(!m_pEntity || m_pEntity->vtable == SA_ADDR(0x5C7358) /* CPlaceable */)
		return 100000.0f;

	this->GetMatrix(&matEnt);
	
	float tmpX = (matEnt.pos.x - *(float*)(SA_ADDR(0x8B1134)));
	float tmpY = (matEnt.pos.y - *(float*)(SA_ADDR(0x8B1138)));
	float tmpZ = (matEnt.pos.z - *(float*)(SA_ADDR(0x8B113C)));

	return sqrt( tmpX*tmpX + tmpY*tmpY + tmpZ*tmpZ );
}

float CEntity::GetDistanceFromLocalPlayerPed()
{
	RwMatrix	matFromPlayer;
	RwMatrix	matThis;
	float 		fSX, fSY, fSZ;

	CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer *pLocalPlayer  = nullptr;

	if(!pLocalPlayerPed) return 10000.0f;

	GetMatrix(&matThis);

	if(pNetGame)
	{
		pLocalPlayer = CPlayerPool::GetLocalPlayer();
		if(pLocalPlayer && (pLocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode()))
		{
			pGame->GetCamera()->GetMatrix(&matFromPlayer);
		}
		else
		{
			pLocalPlayerPed->GetMatrix(&matFromPlayer);
		}
	}
	else
	{
		pLocalPlayerPed->GetMatrix(&matFromPlayer);
	}

	fSX = (matThis.pos.x - matFromPlayer.pos.x) * (matThis.pos.x - matFromPlayer.pos.x);
	fSY = (matThis.pos.y - matFromPlayer.pos.y) * (matThis.pos.y - matFromPlayer.pos.y);
	fSZ = (matThis.pos.z - matFromPlayer.pos.z) * (matThis.pos.z - matFromPlayer.pos.z);

	return (float)sqrt(fSX + fSY + fSZ);
}

float CEntity::GetDistanceFromPoint(float X, float Y, float Z)
{
	RwMatrix	matThis;
	float		fSX,fSY,fSZ;

	GetMatrix(&matThis);
	fSX = (matThis.pos.x - X) * (matThis.pos.x - X);
	fSY = (matThis.pos.y - Y) * (matThis.pos.y - Y);
	fSZ = (matThis.pos.z - Z) * (matThis.pos.z - Z);
	
	return (float)sqrt(fSX + fSY + fSZ);
}

void CEntity::RemovePhysical()
{
	((void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable + 16)))(m_pEntity); // CPhysical::Remove
}

void CEntity::AddPhysical()
{
	((void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable + 8)))(m_pEntity); // CPhysical::Add
}

bool CEntity::IsStationary()
{
	if(!IsAdded()) return false; // movespeed vectors are invalid if its not added

    if( m_pEntity->vecMoveSpeed.x == 0.0f &&
		m_pEntity->vecMoveSpeed.y == 0.0f &&
		m_pEntity->vecMoveSpeed.z == 0.0f )
	{
		return true;
	}
    return false;
}

bool CEntity::GetCollisionChecking()
{
	if(!m_pEntity) {
		return true;
	}
	
	if(IsGameEntityArePlaceable(m_pEntity)) {
		return true;
	}

    return m_pEntity->nEntityFlags.m_bCollisionProcessed;
}

void CEntity::SetCollisionChecking(bool bCheck)
{
	if(!m_pEntity) {
		return;
	}
	
	if(IsGameEntityArePlaceable(m_pEntity)) {
		return;
	}

    m_pEntity->nEntityFlags.m_bCollisionProcessed = bCheck;
}

uintptr_t CEntity::GetRWObject()
{
	if (m_pEntity)
		return (uintptr_t)m_pEntity->m_RwObject;

	return 0;
}

bool CEntity::IsEntityIgnored()
{
	if(!m_pEntity) return false;

	// pIgnoreEntity
	return ((uintptr_t)m_pEntity == g_libGTASA+0x8E8640);
}

bool CEntity::TestSphereCastVsEntity(int *CColSphere1, int *CColSphere2, bool bTrue)
{
	if(m_pEntity && !IsGameEntityArePlaceable(m_pEntity) && GetModelColSphereRadius(m_pEntity->nModelIndex) > 0.0)
	{
		if(bTrue)
			return SphereCastVsEntity(*CColSphere1, *CColSphere2, m_pEntity, false);
		else
			return SphereCastVsEntity(*CColSphere1, *CColSphere2, m_pEntity, true);
	}

	return 0;
}