// -- -- -- -- -- --
// Created by Loony-dev � 2021
// VK: https://vk.com/loonydev
// -- -- -- -

// -- INCLUDE`S
#include "CSkyBox.h"

#include "../game/game.h"
#include "../game/CVector.h"

#include "../net/netgame.h"
#include "../gui/gui.h"

#include "../CSettings.h"

// -- EXTERN`S
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CSettings *pSettings;

// -- VARIABLE`S
CObject* CSkyBox::m_pSkyObject = nullptr;
RwTexture* pSkyTexture = nullptr;

bool CSkyBox::m_bNeedRender = true;
float CSkyBox::m_fRotSpeed = 0.01f;
uint8_t LoadedTextureSky = 0;

uint8_t pChangeTime = 0;
// -- METHOD`S
MATRIX4X4* RwMatrixMultiplyByVector(VECTOR* out, MATRIX4X4* a2, VECTOR* in);

extern uint8_t m_SelectedServer;
void CSkyBox::Initialise()
{
	int d_ModelArray;
	if (m_SelectedServer == ServerTape) d_ModelArray = 17476;
	else d_ModelArray = 18500;
	
	auto* dwModelArray = (uintptr_t*)(SA_ADDR(0x87BF48));
	if (!dwModelArray[d_ModelArray])
	{
		LoadedTextureSky = 2;
		return;
	}
	
	LoadedTextureSky = 1;
	m_pSkyObject = CreateObjectScaled(d_ModelArray, 0.8f);
}

void CSkyBox::Process()
{
	if (!pSettings->GetReadOnly().iSkyBox)
	{
		return;
	}

	if (LoadedTextureSky == 2)
	{
		return;
	}
	
	if (!m_pSkyObject)
	{
		Initialise();
	}
	
	if (m_pSkyObject)
	{
		CAMERA_AIM *aim = GameGetInternalAim();
		MATRIX4X4 matrix;

		m_pSkyObject->GetMatrix(&matrix);

		matrix.pos.X = aim->pos1x;
		matrix.pos.Y = aim->pos1y;
		matrix.pos.Z = aim->pos1z;

		RwMatrixRotate(&matrix, eAxis::Z, m_fRotSpeed);

		m_bNeedRender = true;

		ReTexture();

		m_pSkyObject->UpdateMatrix(matrix);
		if (m_pSkyObject->m_pEntity) // Render
		{
			uintptr_t pRwObject = m_pSkyObject->m_pEntity->m_RwObject;
			if (!pRwObject) return;

			// CEntity::PreRender
			((void (*)(ENTITY_TYPE*))(*(void**)(m_pSkyObject->m_pEntity->vtable + 0x48)))(m_pSkyObject->m_pEntity);

			// CRenderer::RenderOneNonRoad
			((void (*)(ENTITY_TYPE*))(SA_ADDR(0x3B1690 + 1)))(m_pSkyObject->m_pEntity);
		}
		//m_pSkyObject->Render();

		m_bNeedRender = false;
	}
}

CObject* CSkyBox::CreateObjectScaled(int iModel, float fScale)
{
	auto *vecRot = new CVector();
	auto *vecScale = new CVector(fScale);

	if (!pNetGame)
		return nullptr;

	if(!pNetGame->GetObjectPool())
		return nullptr;

	auto *object = pGame->NewObject(iModel, 1.0f, 1.0f, 1.0f, vecRot->Get(), 0.0f);

	object->SetCollisionChecking();
	*(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;
	((void (*)(ENTITY_TYPE*))(*(void**)(object->m_pEntity->vtable + 16)))(object->m_pEntity); // CPhysical::Remove

	//object->RemovePhysical();

	MATRIX4X4 matrix;
	object->GetMatrix(&matrix);

	RwMatrixScale(&matrix, vecScale);

	object->SetMatrix(matrix);
	if (object->m_pEntity && object->m_pEntity->vtable != SA_ADDR(0x5C7358))
	{
		if (object->m_pEntity->m_RwObject)
		{
			if (object->m_pEntity->mat)
			{
				uintptr_t pRwMatrix = *(uintptr_t*)(object->m_pEntity->m_RwObject + 4) + 0x10;
				// CMatrix::UpdateRwMatrix
				((void (*) (MATRIX4X4*, uintptr_t))(SA_ADDR(0x3E862C + 1)))(object->m_pEntity->mat, pRwMatrix);

				// CEntity::UpdateRwFrame
				((void (*) (ENTITY_TYPE*))(SA_ADDR(0x39194C + 1)))(object->m_pEntity);
			}
		}
	}

	*(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;
	((void (*)(ENTITY_TYPE*))(*(void**)(object->m_pEntity->vtable + 8)))(object->m_pEntity); // CPhysical::Add
	//object->AddPhysical();

	return object;
}

void CSkyBox::ReTexture()
{
	int iHours = pNetGame->m_byteWorldTime;

	// -- TODO - Accidentally from several
	if (pChangeTime != iHours)
	{
		pChangeTime = iHours;
		
		if (m_SelectedServer == ServerBoost)
		{
			if (iHours >= 6 && iHours <= 21)
			  SetTexture("skybox_xyli3");

			if (iHours >= 22 || iHours <= 5)
			  SetTexture("skybox_xyli4");
		}
		else
		{
			if (iHours >= 0 && iHours <= 5)
			SetTexture("skyboxnight");

			if (iHours >= 6 && iHours <= 10)
				SetTexture("afternoon_sky_1");

			if (iHours >= 11 && iHours <= 18)
				SetTexture("lkskybox");

			if (iHours >= 19 && iHours <= 24)
				SetTexture("evening_sky_1");
		}
		/*if (iHours >= 0 && iHours <= 5)
			SetTexture("night_sky_1");

		if (iHours >= 6 && iHours <= 10)
			SetTexture("afternoon_sky_1");

		if (iHours >= 11 && iHours <= 18)
			SetTexture("daily_sky_1");

		if (iHours >= 19 && iHours <= 24)
			SetTexture("evening_sky_1");*/
	}
	// ---

	if (!m_pSkyObject)
		return;
	
	if (!m_pSkyObject->m_pEntity)
		return;

	uintptr_t pAtomic = m_pSkyObject->m_pEntity->m_RwObject;
	if (!pAtomic)
		return;

	if (!*(uintptr_t*)(pAtomic + 4))
		return;

	((void(*)())(SA_ADDR(0x559EF8 + 1)))(); // -- DeActivateDirectional
	((void*(*)())(SA_ADDR(0x559FC8 + 1)))(); // -- SetFullAmbient
	((void(*)())(SA_ADDR(0x559FEC + 1)))(); // -- SetAmbientColours
	((uintptr_t(*)(uintptr_t, uintptr_t, CObject*))(SA_ADDR(0x1AEE2C + 1)))(*(uintptr_t*)(pAtomic + 4), (uintptr_t)RwFrameForAllObjectsCallback, m_pSkyObject); // RwFrameForAllObjects
}

uintptr_t CSkyBox::RwFrameForAllObjectsCallback(uintptr_t object, CObject* pObject)
{
	if (*(uint8_t*)object != 1)
		return object;

	uintptr_t pAtomic = object;
	RpGeometry* pGeom = *(RpGeometry * *)(pAtomic + 24);
	if (!pGeom)
		return object;

	int numMats = pGeom->matList.numMaterials;
	if (numMats > 16)
		numMats = 16;

	for (int i = 0; i < numMats; i++)
	{
		RpMaterial* pMaterial = pGeom->matList.materials[i];
		if (!pMaterial)
			continue;

		if (pSkyTexture)
			pMaterial->texture = pSkyTexture;
	}

	return object;
}

void CSkyBox::SetTexture(const char *texName)
{
	if (texName == nullptr)
		return;

	pSkyTexture = nullptr;
	pSkyTexture = (RwTexture*)LoadTextureFromDB(CMain::GetCustomStorage(), texName);
}

void CSkyBox::SetRotSpeed(float speed)
{
	m_fRotSpeed = speed;
}

bool CSkyBox::IsNeedRender()
{
	return m_bNeedRender;
}

CObject *CSkyBox::GetSkyObject()
{
	return m_pSkyObject;
}

