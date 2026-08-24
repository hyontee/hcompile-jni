#pragma once

class CEntity
{
public:
	CEntity() {};
	virtual ~CEntity() {};

	virtual void Add();
	virtual void Remove();
	void Render();
	void UpdateMatrix(RwMatrix mat);
	void SetCollisionChecking();
	void SetGravityProcessing(bool bProcess);
	void UpdateRwMatrixAndFrame();

	void RemovePhysical();
	void AddPhysical();

	// 0.3.7
	void GetMatrix(RwMatrix* Matrix);
	// 0.3.7
	void SetMatrix(RwMatrix Matrix);
	// 0.3.7
	void GetMoveSpeedVector(CVector* Vector);
	// 0.3.7
	void SetMoveSpeedVector(CVector Vector);

	void GetTurnSpeedVector(CVector* Vector);
	void SetTurnSpeedVector(CVector Vector);
	
	// 0.3.7
	uint16_t GetModelIndex();
	// 0.3.7
	void TeleportTo(float fX, float fY, float fZ);

	bool SetModelIndex(unsigned int uiModel);
	// 0.3.7
	bool IsAdded();
	// 0.3.7
	float GetDistanceFromCamera();
	float GetDistanceFromLocalPlayerPed();
	float GetDistanceFromPoint(float x, float y, float z);
	
	bool IsStationary();

	bool GetCollisionChecking();
	void SetCollisionChecking(bool bCheck);
	
	uintptr_t GetRWObject();

	bool IsEntityIgnored();
	bool TestSphereCastVsEntity(int *CColSphere1, int *CColSphere2, bool bTrue);
public:
	ENTITY_TYPE		*m_pEntity;
	uint32_t		m_dwGTAId;
};