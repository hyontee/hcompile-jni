#pragma once

#include "math/matrix.h"
#include "math/vector.h"

class CPlayerPed
{
public:
	float GetWeaponRadiusOnScreen();
	void TransformToNode(CVector* vec, int node);
	uint8_t GetCurrentWeaponID();
	void* GetCurrentWeapon();
};

CMatrix* GetBoneMatrix(CPlayerPed* ped, int bone);

