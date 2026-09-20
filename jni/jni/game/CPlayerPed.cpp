#include "CPlayerPed.h"
#include "xorstr.h"

#include "plugin.h"

CMatrix* GetBoneMatrix(CPlayerPed* ped, int bone)
{
	if (!ped) return nullptr;
	const uintptr_t fn = CGameAPI::GetBase(xorstr("GetBoneMatrix"));
	if (!fn) return nullptr;
	return reinterpret_cast<CMatrix*(*)(uintptr_t, int)>(fn)(reinterpret_cast<uintptr_t>(ped), bone);
}

float CPlayerPed::GetWeaponRadiusOnScreen()
{
	const uintptr_t fn = CGameAPI::GetBase(xorstr("CPlayerPed::GetWeaponRadiusOnScreen"));
	if (!fn || !this) return 0.0f;
	return reinterpret_cast<float(*)(CPlayerPed*)>(fn)(this);
}

void CPlayerPed::TransformToNode(CVector* vec, int node)
{
	const uintptr_t fn = CGameAPI::GetBase(xorstr("CPed::TransformToNode"));
	if (!fn || !this || !vec) return;
	reinterpret_cast<void(*)(CPlayerPed*, CVector*, int)>(fn)(this, vec, node);
}

uint8_t CPlayerPed::GetCurrentWeaponID()
{
	return *(uint8_t *)(((uintptr_t)this) + 24 * *(uint8_t *)(((uintptr_t)this) + 1156) + 832);
}

void* CPlayerPed::GetCurrentWeapon()
{
	return (void *)(((uintptr_t)this) + 24 * *(uint8_t *)(((uintptr_t)this) + 1156) + 832);
}

