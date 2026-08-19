//
// Created by EDGAR 3.0
//
#pragma once
#include "../main.h"

class CVehicle;

class CTurnLeftAndRight //: public CEntity
{
public:
	CTurnLeftAndRight();
	~CTurnLeftAndRight();
	void CVehSet(CVehicle* pVeh);

public:

	// поворотники
	CVehicle* veh;
};
