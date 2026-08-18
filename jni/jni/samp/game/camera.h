#pragma once

#include "game/Enums/CamMode.h"
#include "CameraGta.h"


class CCamera
{
public:
	CCamera() {
		m_matPos = (RwMatrix*)(g_libGTASA + 0x951FA8 + 0x8FC);

		TheCamera = (uintptr_t*)(g_libGTASA + 0x00951FA8);
	}

	~CCamera() {}

	// 0.3.7
	void SetBehindPlayer();
	// 0.3.7
	void Restore();
	// 0.3.7
	void SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);
	// 0.3.7
	void LookAtPoint(float fX, float fY, float fZ, int iType);

	void GetMatrix(RwMatrix* mat);
	// 0.3.7
	void InterpolateCameraPos(CVector *posFrom, CVector *posTo, int time, uint8_t mode);
	// 0.3.7
	void InterpolateCameraLookAt(CVector *posFrom, CVector *posTo, int time, uint8_t mode);

	static void TakeControl(uintptr_t *thiz, CEntityGta *NewTarget, eCamMode CamMode, eSwitchType CamSwitchStyle, int32_t WhoIsTakingControl);

    RwMatrix *m_matPos;
private:
	CEntityGta* m_pEntity;

public:
	uintptr_t* TheCamera; // this

    static inline float m_f3rdPersonCHairMultX{0.53f};
    static inline float m_f3rdPersonCHairMultY{0.4f};
};