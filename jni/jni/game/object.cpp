#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include <cmath>
#include "materialtext.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CMaterialText *pMaterialText;

std::vector<CObject*> CObject::ms_aPaintedObjects;

float fixAngle(float angle)
{
	if (angle > 180.0f) angle -= 360.0f;
	if (angle < -180.0f) angle += 360.0f;

	return angle;
}

float subAngle(float a1, float a2)
{
	return fixAngle(fixAngle(a2) - a1);
}

CObject::CObject(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistance)
{
	uint32_t dwRetID 	= 0;
	m_pEntity 			= nullptr;
	m_dwGTAId 			= 0;
	m_ObjectModel		= iModel;

	ScriptCommand(&create_object, iModel, fPosX, fPosY, fPosZ, &dwRetID);
	ScriptCommand(&put_object_at, dwRetID, fPosX, fPosY, fPosZ);

	m_pEntity = GamePool_Object_GetAt(dwRetID);
	m_dwGTAId = dwRetID;
	m_byteMoving = 0;
	m_fMoveSpeed = 0.0;

	m_vecRot = vecRot;
	m_vecTargetRotTar = vecRot;
	
	m_bIsPlayerSurfing = false;
	m_bNeedRotate = false;
	
	m_bAttached = false;
	m_bAttachedType = 0;
	m_usAttachedVehicle = 0xFFFF;

	for(int i = 0; i < 2; i++){
		m_pLastTextureAttach[i] = nullptr;
		m_pLastTextureAttach[i] = nullptr;
		m_setTextureColor[i] = 0;
		m_setTextureColor[i] = 0;
		m_setTextureAlpha[i] = 0;
		m_setTextureAlpha[i] = 0;
		//m_setTextureAlpha[i] = 0;
		//m_setTextureAlpha[i] = 0;
		m_cacheTextureColor[i] = nullptr;
		m_cacheTextureColor[i] = nullptr;
	}


	m_bMaterials = false;
	for (auto & m_pMaterial : m_pMaterials)
	{
		m_pMaterial.m_bCreated = 0;
		m_pMaterial.pTex = nullptr;
	}
	
    m_bHasMaterialText = false;
	for(int i = 0; i <= MAX_MATERIALS_PER_MODEL; i++)
    {
        m_MaterialTextTexture[i] = 0;
    }

	InstantRotate(vecRot.x, vecRot.y, vecRot.z);
}
// todo
CObject::~CObject()
{
	m_bMaterials = false;
	for (auto & m_pMaterial : m_pMaterials)
	{
		if (m_pMaterial.m_bCreated && m_pMaterial.pTex)
		{
			m_pMaterial.m_bCreated = 0;
			RwTextureDestroy(m_pMaterial.pTex);
			m_pMaterial.pTex = nullptr;
		}
	}
	
	m_bHasMaterialText = false;
	for(int i = 0; i <= MAX_MATERIALS_PER_MODEL; i++)
    {
		if (m_MaterialTextTexture[i])
		{
			RwTextureDestroy(m_MaterialTextTexture[i]);
		}
        m_MaterialTextTexture[i] = 0;
    }
	
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
	if(m_pEntity)
		ScriptCommand(&destroy_object, m_dwGTAId);

	CObjectPool::RemoveColoredObject(this);

	if(m_pLastTextureAttach[0] != nullptr){
	    RwTextureDestroy(m_pLastTextureAttach[0]);
	}
	if(m_pLastTextureAttach[1] != nullptr){
        RwTextureDestroy(m_pLastTextureAttach[1]);
    }
}

void CObject::Process(float fElapsedTime)
{
	if (m_bAttachedType == 1 && !m_bAttached)
	{
		CVehicle* pVehicle = CVehiclePool::GetAt(m_usAttachedVehicle);
		if (pVehicle)
		{
			if (pVehicle->IsAdded())
			{
				if (m_vecAttachedOffset.x > 10000.0f || m_vecAttachedOffset.y > 10000.0f || m_vecAttachedOffset.z > 10000.0f ||
					m_vecAttachedOffset.x < -10000.0f || m_vecAttachedOffset.y < -10000.0f || m_vecAttachedOffset.z < -10000.0f)
				{ 
					// пропускаем действие
				}
				else
				{	
					m_bAttached = true;
					ProcessAttachToVehicle(pVehicle);
				}
			}
		}
	}
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
	if (!m_pEntity) return;
	if (!(m_pEntity->mat)) return;
	if (m_byteMoving & 1)
	{
		CVector vecSpeed = { 0.0f, 0.0f, 0.0f };
		RwMatrix matEnt;
		GetMatrix(&matEnt);
		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matEnt);
		uint32_t dwThisTick = GetTickCount();

		float posX = matEnt.pos.x;
		float posY = matEnt.pos.y;
		float posZ = matEnt.pos.z;

		float f1 = ((float)(dwThisTick - m_dwMoveTick)) * 0.001f * m_fMoveSpeed;
		float f2 = m_fDistanceToTargetPoint - remaining;

		if (distance >= remaining)
		{
			StopMoving();
			TeleportTo(m_matTarget.pos.x, m_matTarget.pos.y, m_matTarget.pos.z);
	
			Log("[CObject] distance >= remaining");
			InstantRotate(m_vecTargetRot.x, m_vecTargetRot.y, m_vecTargetRot.z);
			return;
		}

		if (fElapsedTime <= 0.0f)
			return;

		float delta = 1.0f / (remaining / distance);
		matEnt.pos.x += ((m_matTarget.pos.x - matEnt.pos.x) * delta);
		matEnt.pos.y += ((m_matTarget.pos.y - matEnt.pos.y) * delta);
		matEnt.pos.z += ((m_matTarget.pos.z - matEnt.pos.z) * delta);

		distance = remaining / m_fDistanceToTargetPoint;
		float slerpDelta = 1.0f - distance;

		delta = 1.0f / fElapsedTime;
		vecSpeed.x = (matEnt.pos.x - posX) * delta * 0.02f;
		vecSpeed.y = (matEnt.pos.y - posY) * delta * 0.02f;
		vecSpeed.z = (matEnt.pos.z - posZ) * delta * 0.02f;

		if (FloatOffset(f1, f2) > 0.1f)
		{
			if (f1 > f2)
			{
				delta = (f1 - f2) * 0.1f + 1.0f;
				vecSpeed.x *= delta;
				vecSpeed.y *= delta;
				vecSpeed.z *= delta;
			}

			if (f2 > f1)
			{
				delta = 1.0f - (f2 - f1) * 0.1f;
				vecSpeed.x *= delta;
				vecSpeed.y *= delta;
				vecSpeed.z *= delta;
			}
		}

		SetMoveSpeedVector(vecSpeed);
		ApplyMoveSpeed();

		if (m_bNeedRotate)
		{
			float fx, fy, fz;
			GetRotation(&fx, &fy, &fz);
			distance = m_vecRotationTarget.z - distance * m_vecSubRotationTarget.z;
			vecSpeed.x = 0.0f;
			vecSpeed.y = 0.0f;
			vecSpeed.z = subAngle(remaining, distance) * 0.01f;
			if (vecSpeed.z <= 0.001f)
			{
				if (vecSpeed.z < -0.001f)
					vecSpeed.z = -0.001f;
			}
			else
			{
				vecSpeed.z = 0.001f;
			}

			SetTurnSpeedVector(vecSpeed);
			GetMatrix(&matEnt);
			CQuaternion quat;
			quat.Slerp(&m_quatStart, &m_quatTarget, slerpDelta);
			quat.Normalize();
			quat.GetMatrix(&matEnt);
		}
		else
		{
			GetMatrix(&matEnt);
		}

		UpdateMatrix(matEnt);
	}
}

float CObject::DistanceRemaining(RwMatrix *matPos)
{
	float	fSX,fSY,fSZ;
	fSX = (matPos->pos.x - m_matTarget.pos.x) * (matPos->pos.x - m_matTarget.pos.x);
	fSY = (matPos->pos.y - m_matTarget.pos.y) * (matPos->pos.y - m_matTarget.pos.y);
	fSZ = (matPos->pos.z - m_matTarget.pos.z) * (matPos->pos.z - m_matTarget.pos.z);
	return (float)sqrt(fSX + fSY + fSZ);
}

float CObject::RotaionRemaining(CVector matPos)
{
	float fSX, fSY, fSZ;
	fSX = (matPos.x - m_vecTargetRot.x) * (matPos.x - m_vecTargetRot.x);
	fSY = (matPos.y - m_vecTargetRot.y) * (matPos.y - m_vecTargetRot.y);
	fSZ = (matPos.z - m_vecTargetRot.z) * (matPos.z - m_vecTargetRot.z);
	return (float)sqrt(fSX + fSY + fSZ);
}

float CObject::DistanceRemaining(RwMatrix *matPos, RwMatrix *m_matPositionTarget)
{
	float	fSX,fSY,fSZ;
	fSX = (matPos->pos.x - m_matPositionTarget->pos.x) * (matPos->pos.x - m_matPositionTarget->pos.x);
	fSY = (matPos->pos.y - m_matPositionTarget->pos.y) * (matPos->pos.y - m_matPositionTarget->pos.y);
	fSZ = (matPos->pos.z - m_matPositionTarget->pos.z) * (matPos->pos.z - m_matPositionTarget->pos.z);
	return (float)sqrt(fSX + fSY + fSZ);
}

float CObject::RotaionRemaining(CVector matPos, CVector m_vecRot)
{
	float fSX,fSY,fSZ;
	fSX = (matPos.x - m_vecRot.x) * (matPos.x - m_vecRot.x);
	fSY = (matPos.y - m_vecRot.y) * (matPos.y - m_vecRot.y);
	fSZ = (matPos.z - m_vecRot.z) * (matPos.z - m_vecRot.z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CObject::SetPos(float x, float y, float z)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
		ScriptCommand(&put_object_at, m_dwGTAId, x, y, z);
}

void CObject::SetColorAlpha(uint8_t alpha1, uint8_t alpha2){
	m_setTextureAlpha[0] = alpha1;
	m_setTextureAlpha[1] = alpha2;

	CObjectPool::AddColoredObject(this);
}

void CObject::SetColor(uint32_t color1, uint32_t color2){
	m_setTextureColor[0] = color1;
	m_setTextureColor[1] = color2;

	CObjectPool::AddColoredObject(this);
}

void CObject::StopMoving()
{
	m_byteMoving = 0;
}

void CObject::MoveTo(float fX, float fY, float fZ, float fSpeed, float fRotX, float fRotY, float fRotZ)
{
	RwMatrix mat;
	GetMatrix(&mat);

	if (m_byteMoving & 1) {
		StopMoving();
	}

	m_dwMoveTick = GetTickCount();
	m_fMoveSpeed = fSpeed;
	m_matTarget.pos.x = fX;
	m_matTarget.pos.y = fY;
	m_matTarget.pos.z = fZ;

	m_vecTargetRot.x = fRotX;
	m_vecTargetRot.y = fRotY;
	m_vecTargetRot.z = fRotZ;
	m_byteMoving |= 1;

	if (fRotX <= -999.0f || fRotY <= -999.0f || fRotZ <= -999.0f) {
		m_bNeedRotate = false;
	}
	else
	{
		m_bNeedRotate = true;

		CVector vecRot;
		RwMatrix matrix;
		GetRotation(&vecRot.x, &vecRot.y, &vecRot.z);
		m_vecRotationTarget.x = fixAngle(fRotX);
		m_vecRotationTarget.y = fixAngle(fRotY);
		m_vecRotationTarget.z = fixAngle(fRotZ);

		m_vecSubRotationTarget.x = subAngle(vecRot.x, fRotX);
		m_vecSubRotationTarget.y = subAngle(vecRot.y, fRotY);
		m_vecSubRotationTarget.z = subAngle(vecRot.z, fRotZ);

		Log("[MoveTo] InstantRotate1");
		InstantRotate(fRotX, fRotY, fRotZ);
		GetMatrix(&matrix);

		m_matTarget.right = matrix.right;
		m_matTarget.at = matrix.at;
		m_matTarget.up = matrix.up;

		Log("[MoveTo] InstantRotate2");
		InstantRotate(vecRot.x, vecRot.y, vecRot.z);
		GetMatrix(&matrix);

		m_quatStart.SetFromMatrix(matrix);
		m_quatTarget.SetFromMatrix(m_matTarget);
		m_quatStart.Normalize();
		m_quatTarget.Normalize();
	}

	m_fDistanceToTargetPoint = GetDistanceFromPoint(m_matTarget.pos.x, m_matTarget.pos.y, m_matTarget.pos.z);

	// sub_1009F070
	m_pEntity->m_nEntityFlags &= 0xFFFFFFF7;
}

void CObject::ApplyMoveSpeed()
{
	if (m_pEntity)
	{
		float fTimeStep = *(float*)(g_libGTASA + 0x8C9BB4);

		RwMatrix mat;
		GetMatrix(&mat);
		mat.pos.x += fTimeStep * m_pEntity->vecMoveSpeed.x;
		mat.pos.y += fTimeStep * m_pEntity->vecMoveSpeed.y;
		mat.pos.z += fTimeStep * m_pEntity->vecMoveSpeed.z;
		UpdateMatrix(mat);
	}
}

void CObject::GetRotation(float* pfX, float* pfY, float* pfZ)
{
	if (m_pEntity)
	{
		RwMatrix* mat = m_pEntity->mat;

		if (mat)
		{
			// CMatrix::ConvertToEulerAngles
			((void(*)(RwMatrix*, float*, float*, float*, int))(g_libGTASA + 0x3E8098 + 1))(mat, pfX, pfY, pfZ, 21);
		}

		*pfX = *pfX * 57.295776 * -1.0;
		*pfY = *pfY * 57.295776 * -1.0;
		*pfZ = *pfZ * 57.295776 * -1.0;
	}
}

void CObject::AttachToVehicle(uint16_t usVehID, CVector* pVecOffset, CVector* pVecRot)
{
	m_bAttached = false;
	m_bAttachedType = 1;
	m_usAttachedVehicle = usVehID;
	m_vecAttachedOffset.x = pVecOffset->x;
	m_vecAttachedOffset.y = pVecOffset->y;
	m_vecAttachedOffset.z = pVecOffset->z;

	m_vecAttachedRotation.x = pVecRot->x;
	m_vecAttachedRotation.y = pVecRot->y;
	m_vecAttachedRotation.z = pVecRot->z;
}

void CObject::ProcessAttachToVehicle(CVehicle* pVehicle)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
		m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
		*(uint32_t*)((uintptr_t)m_pEntity + 28) &= 0xFFFFFFFE;

		if (!ScriptCommand(&is_object_attached, m_dwGTAId))
		{
			ScriptCommand(&attach_object_to_car, m_dwGTAId, pVehicle->m_dwGTAId, m_vecAttachedOffset.x,
				m_vecAttachedOffset.y, m_vecAttachedOffset.z, m_vecAttachedRotation.x, m_vecAttachedRotation.y, m_vecAttachedRotation.z);
		}
	}
}

void CObject::InstantRotate(float x, float y, float z)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
		m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
		
		if (!m_pEntity) return;
		if (!(m_pEntity->mat)) return;
		
		RwMatrix m_matObject;
		GetMatrix(&m_matObject);
		
		float f1 = x * 0.0174533;
		float f2 = y * 0.0174533;
		float f3 = z * 0.0174533;

		float f1cos = cos(f1);
		float f1sin = sin(f1);
		float f2cos = cos(f2);
		float f2sin = sin(f2);
		float f3cos = cos(f3);
		float f3sin = sin(f3);

		m_matObject.right.x = f3cos * f2cos - (f3sin * f1sin) * f2sin;
		m_matObject.right.y = (f3cos * f1sin) * f2sin + f3sin * f2cos;
		m_matObject.right.z = -(f2sin * f1cos);

		m_matObject.up.x = -(f3sin * f1cos);
		m_matObject.up.y = f3cos * f1cos;
		m_matObject.up.z = f1sin;
				
		m_matObject.at.x = (f3sin * f1sin) * f2cos + f3cos * f2sin;
		m_matObject.at.y = f3sin * f2sin - (f3cos * f1sin) * f2cos;
		m_matObject.at.z = f2cos * f1cos;
		UpdateMatrix(m_matObject);
	}
}

void CObject::SetMaterialText(int iMaterialIndex, uint8_t byteMaterialSize, const char *szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char *szText)
{
    if (iMaterialIndex < 16)
	{
		if(m_MaterialTextTexture[iMaterialIndex])
    	{
        	RwTextureDestroy(m_MaterialTextTexture[iMaterialIndex]);
        	m_MaterialTextTexture[iMaterialIndex] = 0;
    	}
    	m_MaterialTextTexture[iMaterialIndex] = pMaterialText->Generate(byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign, szText);
    	//m_dwMaterialTextColor[iMaterialIndex] = 0;
    	m_bHasMaterialText = true;
	}
}

void CObject::PaintMaterial(uint8_t ucColor1, uint8_t ucColor2)
{
	RwObject* pObject = (RwObject*)m_pEntity->m_RwObject;
	if(!pObject) {
		return;
	}
	
	if (pObject->type != rpATOMIC) {
		return;
	}

	RpAtomic* pAtomic = (RpAtomic*)pObject;
	RpGeometry* pGeometry = pAtomic->geometry;
	if (!pGeometry) {
		return;
	}
	
	if(pGeometry->matList.numMaterials <= 0) {
		return;
	}
	
	bool bPainted = false;
	
	if(ucColor1 != 0) {
		RpMaterial* pMaterial = pGeometry->matList.materials[0];
		if (pMaterial) {
			m_MaterialTextTexture[0] = pMaterialText->PaintTexture(pMaterial->texture, ucColor1);
			bPainted = true;
		}
	}
	
	if(ucColor2 != 0) {
		RpMaterial* pMaterial = pGeometry->matList.materials[1];
		if (pMaterial) {
			m_MaterialTextTexture[1] = pMaterialText->PaintTexture(pMaterial->texture, ucColor2);
			bPainted = true;
		}
	}
	
	if(bPainted) {
		ms_aPaintedObjects.push_back(this);
		m_bHasMaterialText = true;
	}
}

void CObject::SetCameraCollision(bool bNoCameraCol)
{
	m_bDontCollideWithCamera = bNoCameraCol;
}