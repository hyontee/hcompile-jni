#pragma once

class CObject : public CEntity
{
public:
	RwMatrix	m_matTarget;
	RwMatrix	m_matCurrent;
	uint8_t		m_byteMoving;
	float		m_fMoveSpeed;
	bool		m_bIsPlayerSurfing;
	bool		m_bNeedRotate;

	CQuaternion m_quatTarget;
	CQuaternion m_quatStart;

	CVector m_vecAttachedOffset;
	CVector m_vecAttachedRotation;
	uint16_t m_usAttachedVehicle;
	uint8_t m_bAttachedType;

	CVector 	m_vecRot;
	CVector		m_vecTargetRot;
	CVector		m_vecTargetRotTar;
	CVector		m_vecRotationTarget;
	CVector		m_vecSubRotationTarget;
	float		m_fDistanceToTargetPoint;
	uint32_t	m_dwMoveTick;

	RwTexture*	m_MaterialTexture[MAX_MATERIALS_PER_MODEL];
	uint32_t 	m_dwMaterialColor[MAX_MATERIALS_PER_MODEL];
	bool		m_bHasMaterial;

	RwTexture*	m_MaterialTextTexture[MAX_MATERIALS_PER_MODEL];
	bool		m_bHasMaterialText;

public:
	CObject(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistance);
	~CObject();

	void Process(float fElapsedTime);
	float DistanceRemaining(RwMatrix *matPos);
	//float RotaionRemaining(CVector matPos);

	void SetPos(float x, float y, float z);
	void MoveTo(float x, float y, float z, float speed, float rX, float rY, float rZ);

	void AttachToVehicle(uint16_t usVehID, CVector* pVecOffset, CVector* pVecRot);
	void ProcessAttachToVehicle(CVehicle* pVehicle);

	void InstantRotate(float x, float y, float z);
	void StopMoving();

	void ApplyMoveSpeed();
	void GetRotation(float* pfX, float* pfY, float* pfZ);

	void SetMaterial(int iModel, int iMaterialIndex, char* txdname, char* texturename, uint32_t dwColor);
	void SetMaterialText(int iMaterialIndex, uint8_t byteMaterialSize, const char *szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char *szText);

};