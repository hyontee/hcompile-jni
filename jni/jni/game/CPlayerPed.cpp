#include "CPlayerPed.h"
#include "xorstr.h"

#include "plugin.h"

CMatrix* GetBoneMatrix(CPlayerPed* ped, int bone)
{
	// Защита: эти оффсеты не зарегистрированы ни под ARM, ни под ARM64 —
	// без проверки был бы вызов по адресу 0 и краш на обеих архитектурах.
	uintptr_t fn = CGameAPI::GetBase(xorstr("GetBoneMatrix"));
	if (fn < 0x1000 || !ped) return nullptr;
	return ((CMatrix*(*)(uintptr_t, int))fn)(((uintptr_t)ped), bone);
}

float CPlayerPed::GetWeaponRadiusOnScreen()
{
	uintptr_t fn = CGameAPI::GetBase(xorstr("CPlayerPed::GetWeaponRadiusOnScreen"));
	if (fn < 0x1000) return 0.0f;
	return ((float(*)(CPlayerPed*))fn)(this);
}

void CPlayerPed::TransformToNode(CVector* vec, int node)
{
	uintptr_t fn = CGameAPI::GetBase(xorstr("CPed::TransformToNode"));
	if (fn < 0x1000) return;
	((void(*)(CPlayerPed*, CVector*, int))fn)(this, vec, node);
}

uint8_t CPlayerPed::GetCurrentWeaponID()
{
	return *(uint8_t *)(((uintptr_t)this) + 24 * *(uint8_t *)(((uintptr_t)this) + 1156) + 832);
}

void* CPlayerPed::GetCurrentWeapon()
{
	return (void *)(((uintptr_t)this) + 24 * *(uint8_t *)(((uintptr_t)this) + 1156) + 832);
}

