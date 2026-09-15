#pragma once

#define ACTION_CLIMB_OR_JUMP 2
#define ACTION_REMOVE_FROM_VEHICLE 3
#define ACTION_PUT_INTO_VEHICLE 4
#define ACTION_SET_DEAD 5
#define ACTION_CLEAR_TASKS 6
#define ACTION_LOCK_ACTOR 7
#define ACTION_ENTER_VEHICLE 8
#define ACTION_EXIT_VEHICLE 9
#define ACTION_UPDATE_HEALTH 10
#define ACTION_UPDATE_ARMOUR 11

class CActorPed : public CEntity
{
public:
	CActorPed(uint16_t usModel, VECTOR vecPosition, float fRotation, float fHealth, bool bInvulnerable);
	~CActorPed();

	void Destroy();
	void SetHealth(float fHealth);
	void SetDead();
	bool IsDead();
	void ForceTargetRotation(float fRotation);
	void ApplyAnimation(char* szAnimName, char* szAnimFile, float fDelta, int bLoop, int bLockX, int bLockY, int bFreeze, int uiTime);
	void PutDirectlyInVehicle(int iVehicleID, int iSeat);
	void RemoveFromVehicle();
	void RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);
	
	void GiveWeapon(int iWeaponID, int iAmmo);
	void ClearAllTasks();
	void ClearWeapons();
	void Attack(CPlayerPed* pPlayerPed);
	void MoveToPoint(VECTOR vecPos, bool fastTravel);
	void setInvulnerable(bool status);
	
	void SetWeaponSkill(int8_t weaponSkill);
	void GetBonePosition(int iBoneID, VECTOR* vecOut);
	void SetFloatingText(char* szText, uint32_t dwColor);
	char* GetFloatingTagText();
	uint32_t GetFloatingTagTextColor();
	
	void PlayCustomAction(int type, int action, int action2);
	void JumpAction(int action);
	void LockActor(int state);
	void EnterVehicle(int iVehicleID, bool bPassenger);
	int GetCurrentVehicleID();
	void ExitCurrentVehicle();
	
	float GetHealth();
	float GetArmour();
	
	void Process();
	
	void SetDeadState(bool state)
	{
		bIsDeadState = state;
	};

	bool HasDeadState()
	{
		return bIsDeadState;
	}
	
	void SetNeedDraw(bool state)
	{
		bIsDraw = state;
	};
	
	bool HasNeedDraw()
	{
		return bIsDraw;
	};
	
	
	PED_TYPE * GetGtaActor() { return m_pPed; };

	PED_TYPE* 	m_pPed;
	bool 		bIsDeadState;
	bool		bIsDraw;

	char 		m_szFloatingText[2048+1]{'\0'};
	uint32_t 	m_dwFloatingTextColor;
	
	VECTOR		Travel = {0.0, 0.0, 0.0};
};