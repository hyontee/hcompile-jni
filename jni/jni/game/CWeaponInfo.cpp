#include "CWeaponInfo.h"
#include "../main.h"

CWeaponInfo* CWeaponInfo::GetWeaponInfo(unsigned int weaponType, signed char skill)
{
	return reinterpret_cast<CWeaponInfo*(*)(unsigned int weaponType, signed char skill)>(g_libGTASA + 0x56BD60 + 1)(weaponType, skill);
}
