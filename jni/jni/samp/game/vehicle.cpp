#include "../main.h"
#include "game.h"
#include <algorithm>
extern CGame* pGame;
#include "..//CDebugInfo.h"
#include "..//net/netgame.h"
#include "game/Core/Vector.h"
#include "game/Models/ModelInfo.h"
#include "StreamingInfo.h"
#include "chatwindow.h"

#include "vehicle.h"

#include "java_systems/CSpeedometr.h"
#include "CCustomPlateManager.h"
#include "Timer.h"
#include "CShadows.h"
#include "CCoronas.h"
#include "game/vehicle_utils.h"

static inline bool SpecialsShouldRenderFor(const CVehicle* v)
{
    if (!v || !v->m_pVehicle) return false;
    if (v->m_pVehicle->nModelIndex != 598) return false;        // только 598
    return IsSpecialsEnabledForVehicle(GetSampVehicleIdFromPtr(v->m_pVehicle));          // флаг из vehicle_utils.cpp
}


// --- helpers: положи это в vehicle.cpp (вне класса), ровно один раз ---

// Найти LTM-матрицу фрейма по имени в данном clump-е.
static bool GetFrameLTM(RpClump* clump, const char* name, RwMatrix& outLTM)
{
    if (!clump || !name) return false;
#if !VER_x32
    RwFrame* f = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(clump, name);
#else
    RwFrame* f = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(clump, name);
#endif
    if (!f) return false;
    outLTM = f->ltm; // В твоём SDK у фрейма есть ltm
    return true;
}

// Перевести мировую точку в локальные координаты конкретного автомобиля.
static CVector WorldToVehicleLocal(const CMatrix* M, const CVector& world)
{
    CVector rel{ world.x - M->pos.x, world.y - M->pos.y, world.z - M->pos.z };
    CVector local;
    local.x = rel.x * M->right.x + rel.y * M->right.y + rel.z * M->right.z;
    local.y = rel.x * M->up.x    + rel.y * M->up.y    + rel.z * M->up.z;
    local.z = rel.x * M->at.x    + rel.y * M->at.y    + rel.z * M->at.z;
    return local;
}


void CVehicle::Init(){
    char* file = new char[512];
    if(!CVehicle::m_pTurnLightTickSound1) {
        snprintf(file, 512, "%saudio/turnlight_tick_1.mp3", g_pszStorage);
        CVehicle::m_pTurnLightTickSound1 = BASS_StreamCreateFile(false, file, 0, 0, 0);
        BASS_ChannelSetAttribute(CVehicle::m_pTurnLightTickSound1, BASS_ATTRIB_VOL,
                                 0.5f);
    }
    if(!CVehicle::m_pTurnLightTickSound2) {
        snprintf(file, 512, "%saudio/turnlight_tick_2.mp3", g_pszStorage);
        CVehicle::m_pTurnLightTickSound2 = BASS_StreamCreateFile(false, file, 0, 0, 0);
        BASS_ChannelSetAttribute(CVehicle::m_pTurnLightTickSound2, BASS_ATTRIB_VOL,
                                 0.5f);
    }
}
CVehicle::CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bSiren)
{
	Log("CVehicle(%d, %4.f, %4.f, %4.f, %4.f)", iType, fPosX, fPosY, fPosZ, fRotation);

    CDebugInfo::uiStreamedVehicles++;
    RwMatrix mat;
    uint32_t dwRetID = 0;

//	m_pCustomHandling = nullptr;

    m_pLeftFrontTurnLighter = nullptr;
    m_pRightFrontTurnLighter = nullptr;
    m_pLeftRearTurnLighter = nullptr;
    m_pRightRearTurnLighter = nullptr;

    m_pLeftReverseLight = nullptr;
    m_pRightReverseLight = nullptr;

    m_pVehicle = nullptr;
    m_dwGTAId = 0;
    m_pTrailer = nullptr;

    // normal vehicle
    if (!pGame->IsModelLoaded(iType)) {
        CStreaming::RequestModel(iType, STREAMING_GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        while (!pGame->IsModelLoaded(iType)) usleep(10);
    }
    m_bHasSiren = false;

    ScriptCommand(&create_car, iType, fPosX, fPosY, fPosZ, &dwRetID);
//	Log("create_car");
    ScriptCommand(&set_car_z_angle, dwRetID, fRotation);
//	Log("set_car_z_angle");
    ScriptCommand(&car_gas_tank_explosion, dwRetID, 0);
//	Log("car_gas_tank_explosion");
    ScriptCommand(&set_car_hydraulics, dwRetID, 0);
//	Log("set_car_hydraulics");
    ScriptCommand(&toggle_car_tires_vulnerable, dwRetID, 1);
//	Log("toggle_car_tires_vulnerable");
//	ScriptCommand(&set_car_immunities, dwRetID, 0, 0, 0, 0, 0);
//	Log("set_car_immunities");
    m_pVehicle = (CVehicleGta*)GamePool_Vehicle_GetAt(dwRetID);
    m_pEntity = m_pVehicle;
    m_dwGTAId = dwRetID;

    if (m_pVehicle) {
        //m_pVehicle->m_nOverrideLights = eVehicleOverrideLightsState::NO_CAR_LIGHT_OVERRIDE;
        m_pVehicle->dwDoorsLocked = 0;
//		m_pVehicle->fHealth = 1000.0;
        m_bIsLocked = false;

        GetMatrix(&mat);
        mat.pos.x = fPosX;
        mat.pos.y = fPosY;
        mat.pos.z = fPosZ;

        if (GetVehicleSubtype() != VEHICLE_SUBTYPE_BIKE &&
            GetVehicleSubtype() != VEHICLE_SUBTYPE_PUSHBIKE)
            mat.pos.z += 0.25f;

        SetMatrix(mat);
    }
//	Log("m_pVehicle");

    m_byteObjectiveVehicle = 0;
    m_bSpecialMarkerEnabled = false;
    m_bIsLocked = false;
    m_dwMarkerID = 0;
    m_bIsInvulnerable = false;
    uint8_t defComp = 0;
    BIT_SET(defComp, 0);

//	Log("defComp");
    for (int i = 0; i < E_CUSTOM_COMPONENTS::ccMax; i++)
    {
        if (i == E_CUSTOM_COMPONENTS::ccExtra)
        {
            uint16_t defComp_extra = 0;
            BIT_SET(defComp_extra, EXTRA_COMPONENT_BOOT);
            BIT_SET(defComp_extra, EXTRA_COMPONENT_BONNET);
            BIT_SET(defComp_extra, EXTRA_COMPONENT_DEFAULT_DOOR);
            BIT_SET(defComp_extra, EXTRA_COMPONENT_WHEEL);
            BIT_SET(defComp_extra, EXTRA_COMPONENT_BUMP_REAR);
            BIT_SET(defComp_extra, EXTRA_COMPONENT_BUMP_FRONT);
            SetComponentVisible(i, defComp_extra);
        }
        else
        {
            SetComponentVisible(i, (uint16_t)defComp);
        }
    }

//	Log("foreach 1");
    for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
    {
        m_bReplaceTextureStatus[i] = false;
        memset(&(m_szReplacedTextures[i].szOld[0]), 0, MAX_REPLACED_TEXTURE_NAME);
        m_szReplacedTextures[i].pTexture = nullptr;
    }



    m_bReplacedTexture = false;

    bHasSuspensionLines = false;
//	Log("foreach 2");
    m_pSuspensionLines = nullptr;
//	Log("foreach 2");
    //if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
    //{
    //    CopyGlobalSuspensionLinesToPrivate();
    //}
//	Log("foreach 2");

//	Log("VEHICLE_SUBTYPE_CAR");

    m_bHeadlightsColor = false;
    m_bWheelSize = false;
    m_bWheelWidth = false;
    m_bWheelAlignmentX = false;
    m_bWheelAlignmentY = false;
    m_bWheelOffsetX = false;
    m_bWheelOffsetY = false;
    m_fWheelOffsetX = 0.0f;
    m_fWheelOffsetY = 0.0f;
    m_fNewOffsetX = 0.0f;
    m_fNewOffsetY = 0.0f;
    m_bWasWheelOffsetProcessedX = true;
    m_bWasWheelOffsetProcessedY = true;
    m_uiLastProcessedWheelOffset = 0;

    m_bShadow = false;
    m_Shadow.pTexture = nullptr;


    m_iTickTurnLight = GetTickCount();
    m_bIsTurnLight = false;
    m_iTurnState = CVehicle::eTurnState::TURN_OFF;
    neonStage = 0;
    m_bSirenEnabled = false;          // ВРЕМЕННО: пусть горит всегда (для проверки)
    m_tickCoplight  = GetTickCount();
    m_coplightState = false;


#if VER_x32

//	Log("VEHICLE_SUBTYPE_CAR");
    RwFrame* pWheelLF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_lf_dummy"); // GetFrameFromname
    RwFrame* pWheelRF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_rf_dummy"); // GetFrameFromname
    RwFrame* pWheelRB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_rb_dummy"); // GetFrameFromname
    RwFrame* pWheelLB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_lb_dummy"); // GetFrameFromname
#else
    RwFrame* pWheelLF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, "wheel_lf_dummy"); // GetFrameFromname
    RwFrame* pWheelRF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, "wheel_rf_dummy"); // GetFrameFromname
    RwFrame* pWheelRB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, "wheel_rb_dummy"); // GetFrameFromname
    RwFrame* pWheelLB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, "wheel_lb_dummy"); // GetFrameFromname
#endif
    if (pWheelLF && pWheelRF && pWheelRB && pWheelLB)
    {
        memcpy(&m_vInitialWheelMatrix[0], (const void*)&(pWheelLF->modelling), sizeof(RwMatrix));
        memcpy(&m_vInitialWheelMatrix[1], (const void*)&(pWheelRF->modelling), sizeof(RwMatrix));
        memcpy(&m_vInitialWheelMatrix[2], (const void*)&(pWheelRB->modelling), sizeof(RwMatrix));
        memcpy(&m_vInitialWheelMatrix[3], (const void*)&(pWheelLB->modelling), sizeof(RwMatrix));
    }
}

void CVehicle::CopyGlobalSuspensionLinesToPrivate()
{
    if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
    {
        return;
    }

    if (!bHasSuspensionLines)
    {
        int numWheels;
        void* pOrigSuspension = GetSuspensionLinesFromModel(m_pVehicle->nModelIndex, numWheels);

        if (pOrigSuspension && numWheels)
        {
            bHasSuspensionLines = true;
            m_pSuspensionLines = new uint8_t[0x20 * numWheels];
        }
    }

    int numWheels;
    void* pOrigSuspension = GetSuspensionLinesFromModel(m_pVehicle->nModelIndex, numWheels);

    if (pOrigSuspension && numWheels)
    {
        m_pSuspensionLines = malloc(0x20 * numWheels);
        memcpy(m_pSuspensionLines, pOrigSuspension, 0x20 * numWheels);
    }
}

CVehicle::~CVehicle()
{
    if(!m_dwGTAId)return;

    CDebugInfo::uiStreamedVehicles--;
    m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

    if(!m_pVehicle)return;

    m_bReplacedTexture = false;

    for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
    {
        if (m_bReplaceTextureStatus[i] && m_szReplacedTextures[i].pTexture)
        {
            m_bReplaceTextureStatus[i] = false;
            RwTextureDestroy(m_szReplacedTextures[i].pTexture);
        }

        m_bReplaceTextureStatus[i] = false;
        memset(&(m_szReplacedTextures[i].szOld[0]), 0, MAX_REPLACED_TEXTURE_NAME);
        m_szReplacedTextures[i].pTexture = nullptr;
    }

    if(IsTrailer()){
        CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
        CVehicle *tmpVeh = pVehiclePool->GetVehicleFromTrailer(this);
        if(tmpVeh)
        {
            ScriptCommand(&detach_trailer_from_cab, m_dwGTAId, tmpVeh->m_dwGTAId);
            tmpVeh->m_pTrailer = nullptr;
        }
    }
    if (m_bShadow) {
        if (m_Shadow.pTexture) {
            RwTextureDestroy(m_Shadow.pTexture);
            m_Shadow.pTexture = nullptr;
        }
        m_bShadow = false;
        neonStage = 0;
    }


    if (m_dwMarkerID) {
        ScriptCommand(&disable_marker, m_dwMarkerID);
        m_dwMarkerID = 0;
    }
    RemoveEveryoneFromVehicle();

    if(m_pTrailer) {
        Log("m_pTrailer");
        ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
        m_pTrailer = nullptr;
    }

    if (m_pVehicle->nModelIndex == TRAIN_PASSENGER_LOCO ||
        m_pVehicle->nModelIndex == TRAIN_FREIGHT_LOCO) {
        ScriptCommand(&destroy_train, m_dwGTAId);
        Log("destroy_train");
    }
    else {
        ScriptCommand(&destroy_car, m_dwGTAId);
        OnVehicleDestroyed(GetSampVehicleIdFromPtr(m_pVehicle));
        Log("destroy_car");
    }

//	delete m_pCustomHandling;
//	m_pCustomHandling = nullptr;


    if (bHasSuspensionLines && m_pSuspensionLines) {
        delete[] m_pSuspensionLines;
        m_pSuspensionLines = nullptr;
        bHasSuspensionLines = false;
    }

    //

}



void CVehicle::toggleRightTurnLight(bool toggle)
{
    m_bIsOnRightTurnLight = toggle;

    CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
            m_pVehicle->nModelIndex));

    CVector* m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

    CVector vecFront;
    // 0 - front light
    vecFront.x = m_avDummyPos[0].x + 0.1f;
    vecFront.y = m_avDummyPos[0].y;
    vecFront.z = m_avDummyPos[0].z;

    CVector vecRear;
    vecRear.x = m_avDummyPos[1].x + 0.1f;
    vecRear.y = m_avDummyPos[1].y;
    vecRear.z = m_avDummyPos[1].z;

    CVector vec;
    vec.x = vec.y = vec.z = 0;

    if(m_pRightFrontTurnLighter != nullptr)
    {
        delete m_pRightFrontTurnLighter;
        m_pRightFrontTurnLighter = nullptr;
    }
    if(m_pRightRearTurnLighter != nullptr)
    {
        delete m_pRightRearTurnLighter;
        m_pRightRearTurnLighter = nullptr;
    }

    if(!toggle) return;

    m_pRightFrontTurnLighter = pGame->NewObject(19294, 0.0, 0.0, 0.0, vec, 300.0);
    m_pRightFrontTurnLighter->AttachToVehicle(getSampId(), &vecFront, &vecFront);

    m_pRightRearTurnLighter = pGame->NewObject(19294, 0.0, 0.0, 0.0, vec, 300.0);
    m_pRightRearTurnLighter->AttachToVehicle(getSampId(), &vecRear, &vecRear);

    m_pRightFrontTurnLighter->ProcessAttachToVehicle(this);
    m_pRightRearTurnLighter->ProcessAttachToVehicle(this);
}

void CVehicle::toggleReverseLight(bool toggle)
{
    CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
            m_pVehicle->nModelIndex));

    CVector* m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

    CVector vecRight;
    vecRight.x = m_avDummyPos[1].x;
    vecRight.y = m_avDummyPos[1].y;
    vecRight.z = m_avDummyPos[1].z;

    CVector vecLeft;
    vecLeft.x = -m_avDummyPos[1].x;
    vecLeft.y = m_avDummyPos[1].y;
    vecLeft.z = m_avDummyPos[1].z;

    CVector vec;
    vec.x = vec.y = vec.z = 0;

    if(m_pLeftReverseLight != nullptr)
    {
        delete m_pLeftReverseLight;
        m_pLeftReverseLight = nullptr;
    }
    if(m_pRightReverseLight != nullptr)
    {
        delete m_pRightReverseLight;
        m_pRightReverseLight = nullptr;
    }

    if(!toggle) return;

    m_pLeftReverseLight = pGame->NewObject(19281, 0.0, 0.0, 0.0, vec, 300.0);
    m_pLeftReverseLight->AttachToVehicle(getSampId(), &vecLeft, &vecLeft);

    m_pRightReverseLight = pGame->NewObject(19281, 0.0, 0.0, 0.0, vec, 300.0);
    m_pRightReverseLight->AttachToVehicle(getSampId(), &vecRight, &vecRight);

    m_pRightReverseLight->ProcessAttachToVehicle(this);
    m_pLeftReverseLight->ProcessAttachToVehicle(this);
}

void CVehicle::toggleLeftTurnLight(bool toggle)
{
    m_bIsOnLeftTurnLight = toggle;

    CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
            m_pVehicle->nModelIndex));

    CVector* m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

    CVector vecFront;
    // 0 - front light
    vecFront.x = -(m_avDummyPos[0].x + 0.1f);
    vecFront.y = m_avDummyPos[0].y;
    vecFront.z = m_avDummyPos[0].z;

    CVector vecRear;
    vecRear.x = -(m_avDummyPos[1].x + 0.1f);
    vecRear.y = m_avDummyPos[1].y;
    vecRear.z = m_avDummyPos[1].z;

    CVector vec;
    vec.x = vec.y = vec.z = 0;

    if(m_pLeftFrontTurnLighter != nullptr)
    {
        delete m_pLeftFrontTurnLighter;
        m_pLeftFrontTurnLighter = nullptr;
    }
    if(m_pLeftRearTurnLighter != nullptr)
    {
        delete m_pLeftRearTurnLighter;
        m_pLeftRearTurnLighter = nullptr;
    }

    if(!toggle) return;

    m_pLeftFrontTurnLighter = pGame->NewObject(19294, 0.0, 0.0, 0.0, vec, 300.0);
    m_pLeftFrontTurnLighter->AttachToVehicle(getSampId(), &vecFront, &vecFront);

    m_pLeftRearTurnLighter = pGame->NewObject(19294, 0.0, 0.0, 0.0, vec, 300.0);
    m_pLeftRearTurnLighter->AttachToVehicle(getSampId(), &vecRear, &vecRear);

    m_pLeftFrontTurnLighter->ProcessAttachToVehicle(this);
    m_pLeftRearTurnLighter->ProcessAttachToVehicle(this);
}

VEHICLEID CVehicle::getSampId()
{
    return pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(m_pVehicle);
}

void CVehicle::LinkToInterior(int iInterior)
{
    if (GamePool_Vehicle_GetAt(m_dwGTAId))
    {
        ScriptCommand(&link_vehicle_to_interior, m_dwGTAId, iInterior);
    }
}

int32_t CVehicle::AddVehicleUpgrade(int32_t modelId)
{
    CStreaming::RequestModel(modelId, STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
//	pGame->RequestModel(modelId, STREAMING_GAME_REQUIRED);
//	pGame->LoadRequestedModels();

    ScriptCommand(&request_car_component, modelId);

    while (!ScriptCommand(&is_component_available, modelId))
    {
        usleep(5);
    }
#if !VER_x32
    return ( ( int32_t(*)(CVehicleGta*, int32_t) )(g_libGTASA + 0x006AFF4C) )(m_pVehicle, modelId);
#else
    return ( ( int32_t(*)(CVehicleGta*, int32_t) )(g_libGTASA + 0x0058C66C + 1) )(m_pVehicle, modelId);
#endif
}

void CVehicle::SetColor(int iColor1, int iColor2)
{
//	if (iColor1 >= 256 || iColor1 < 0)
//	{
//		iColor1 = 0;
//	}
//	if (iColor2 >= 256 || iColor2 < 0)
//	{
//		iColor2 = 0;
//	}
    if (m_pVehicle)
    {
        if (GamePool_Vehicle_GetAt(m_dwGTAId))
        {
            m_pVehicle->m_nPrimaryColor = (uint8_t)iColor1;
            m_pVehicle->m_nSecondaryColor = (uint8_t)iColor2;
        }
    }

    //m_byteColor1 = iColor1;
    color1.Set(iColor1);
    m_byteColor2 = (uint8_t)iColor2;
    m_bColorChanged = true;
}

void CVehicle::AttachTrailer()
{
    if (m_pTrailer && GamePool_Vehicle_GetAt(m_pTrailer->m_dwGTAId) )
    {
        ScriptCommand(&put_trailer_on_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
    }
}

//-----------------------------------------------------------

void CVehicle::DetachTrailer()
{
    if (m_pTrailer && GamePool_Vehicle_GetAt(m_pTrailer->m_dwGTAId))
    {
        ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
    }
    m_pTrailer = nullptr;
}

//-----------------------------------------------------------

void CVehicle::SetTrailer(CVehicle* pTrailer)
{
    m_pTrailer = pTrailer;
}

//-----------------------------------------------------------

CVehicle* CVehicle::GetTrailer()
{
    if (!m_pVehicle) return nullptr;

    return m_pTrailer;
}

void CVehicle::SetHealth(float fHealth)
{
    if (m_pVehicle)
    {
        m_pVehicle->fHealth = fHealth;
    }
}

float CVehicle::GetHealth()
{
    if (m_pVehicle) return m_pVehicle->fHealth;
    else return 0.0f;
}

// 0.3.7
void CVehicle::SetInvulnerable(bool bInv)
{
    if (!m_pVehicle) return;
    if (!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

    if (bInv)
    {
        ScriptCommand(&set_car_immunities, m_dwGTAId, 1, 1, 1, 1, 1);
        ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 0);
        m_bIsInvulnerable = true;
    }
    else
    {
        ScriptCommand(&set_car_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
        ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 1);
        m_bIsInvulnerable = false;
    }
}

// 0.3.7
bool CVehicle::IsDriverLocalPlayer()
{
    if (m_pVehicle)
    {
        if ((CPedGta*)m_pVehicle->pDriver == GamePool_FindPlayerPed())
            return true;
    }

    return false;
}

// 0.3.7
bool CVehicle::HasSunk()
{
    if (!m_pVehicle) return false;
    return ScriptCommand(&has_car_sunk, m_dwGTAId);
}

bool IsValidGamePed(CPedGta* pPed);

void CVehicle::RemovePassenger(CPedGta *pPed)
{
#if !VER_x32
    ((bool (*)(CVehicleGta*, CPedGta*))(g_libGTASA + 0x006A813C))(m_pVehicle, pPed);
#else
    ((bool (*)(CVehicleGta*, CPedGta*))(g_libGTASA + 0x00584548 + 1))(m_pVehicle, pPed);
#endif

}

void CVehicle::RemoveDriver(bool bDontTurnOffEngine)
{
#if !VER_x32

    ((bool (*)(CVehicleGta*, bool))(g_libGTASA + 0x006A842C))(m_pVehicle, bDontTurnOffEngine);
#else
    ((bool (*)(CVehicleGta*, bool))(g_libGTASA + 0x005847CC + 1))(m_pVehicle, bDontTurnOffEngine);
#endif
}

void CVehicle::RemoveEveryoneFromVehicle()
{
    Log("RemoveEveryoneFromVehicle");
    if (!m_pVehicle) return;
    if(!m_dwGTAId)return;
    if (!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

    float fPosX = m_pVehicle->mat->pos.x;
    float fPosY = m_pVehicle->mat->pos.y;
    float fPosZ = m_pVehicle->mat->pos.z;

    int iPlayerID = 0;
    if (m_pVehicle->pDriver)
    {
        //RemoveDriver(true);

        iPlayerID = GamePool_Ped_GetIndex(m_pVehicle->pDriver);
        ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2.0f);
        /*CPedGta* pPed = m_pVehicle->pDriver;
        CHook::CallFunction<void>(g_libGTASA + 0x4C0AB4 + 1, pPed->pPedIntelligence, 1);
    RemoveDriver(true);
    pPed->m_matrix->to = CVector{ fPosX, fPosY, fPosZ };*/
    }

    for(const auto & pPassenger : m_pVehicle->pPassengers)
    {
        if(!pPassenger) return;
        //RemovePassenger(pPassenger);

        iPlayerID = GamePool_Ped_GetIndex(pPassenger);
        ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2.0f);
        /*CHook::CallFunction<void>(g_libGTASA + 0x4C0AB4 + 1, pPassenger->pPedIntelligence, 1);
        RemoveDriver(true);
        pPassenger->m_matrix->to = CVector{ fPosX, fPosY, fPosZ };*/

    }
}

// 0.3.7
bool CVehicle::IsOccupied()
{
    if (m_pVehicle)
    {
        if (m_pVehicle->pDriver) return true;
        if (m_pVehicle->pPassengers[0]) return true;
        if (m_pVehicle->pPassengers[1]) return true;
        if (m_pVehicle->pPassengers[2]) return true;
        if (m_pVehicle->pPassengers[3]) return true;
        if (m_pVehicle->pPassengers[4]) return true;
        if (m_pVehicle->pPassengers[5]) return true;
        if (m_pVehicle->pPassengers[6]) return true;
    }

    return false;
}

void CVehicle::ProcessMarkers()
{
    if (!m_pVehicle) return;

    if (m_byteObjectiveVehicle)
    {
        if (!m_bSpecialMarkerEnabled)
        {
            if (m_dwMarkerID)
            {
                ScriptCommand(&disable_marker, m_dwMarkerID);
                m_dwMarkerID = 0;
            }

            ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 3, &m_dwMarkerID);
            ScriptCommand(&set_marker_color, m_dwMarkerID, 1006);
            ScriptCommand(&show_on_radar, m_dwMarkerID, 3);
            m_bSpecialMarkerEnabled = true;
        }

        return;
    }

    if (!m_byteObjectiveVehicle && m_bSpecialMarkerEnabled)
    {
        if (m_dwMarkerID)
        {
            ScriptCommand(&disable_marker, m_dwMarkerID);
            m_bSpecialMarkerEnabled = false;
            m_dwMarkerID = 0;
        }
    }

    if (GetDistanceFromLocalPlayerPed() < 200.0f && !IsOccupied())
    {
        if (!m_dwMarkerID)
        {
            // toggle
            ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
            ScriptCommand(&set_marker_color, m_dwMarkerID, 1004);
        }
    }

    else if (IsOccupied() || GetDistanceFromLocalPlayerPed() >= 200.0f)
    {
        // remove
        if (m_dwMarkerID)
        {
            ScriptCommand(&disable_marker, m_dwMarkerID);
            m_dwMarkerID = 0;
        }
    }
}

void CVehicle::SetDoorState(int iState)
{
    if (!m_pVehicle) return;
    if (iState)
    {
        m_pVehicle->dwDoorsLocked = 2;
        m_bIsLocked = true;
        CVehicle::fDoorState = 1;
    }
    else
    {
        m_pVehicle->dwDoorsLocked = 0;
        m_bIsLocked = false;
        CVehicle::fDoorState = 0;
    }
}

int CVehicle::GetDoorState(){
    return CVehicle::fDoorState;
}

void CVehicle::SetLightsState(bool iState)
{
    if(!m_dwGTAId)return;
    if(!m_pVehicle)return;

    //if (GamePool_Vehicle_GetAt(m_dwGTAId))
    //{


    m_pVehicle->m_nOverrideLights = 0;
    m_pVehicle->m_nVehicleFlags.bLightsOn = iState;
//		ScriptCommand(&FORCE_CAR_LIGHTS, m_dwGTAId, iState ? 2 : 1);
    //}
    m_bLightsOn = iState;
}

bool CVehicle::GetLightsState(){
    return m_pVehicle->m_nVehicleFlags.bLightsOn;
}

void CVehicle::SetBootAndBonnetState(int iBoot, int iBonnet)
{
    if (GamePool_Vehicle_GetAt(m_dwGTAId) && m_pVehicle)
    {
        if (iBoot == 1)
        {
            SetComponentAngle(1, 17, 1.0f);
        }
        else
        {
            SetComponentAngle(1, 17, 0.0f);
        }

        if (iBonnet == 1)
        {
            SetComponentAngle(0, 16, 1.0f);
        }
        else
        {
            SetComponentAngle(0, 16, 0.0f);
        }
    }
}

void CVehicle::RemoveComponent(uint16_t uiComponent)
{

    int component = (uint16_t)uiComponent;

    if (!m_dwGTAId || !m_pVehicle)
    {
        return;
    }

    if (GamePool_Vehicle_GetAt(m_dwGTAId))
    {
        ScriptCommand(&remove_component, m_dwGTAId, component);
    }
}

void CVehicle::SetComponentVisible(uint8_t group, uint16_t components)
{

    if (group == E_CUSTOM_COMPONENTS::ccExtra)
    {
        for (int i = 0; i < 16; i++)
        {
            std::string szName = GetComponentNameByIDs(group, i);
            SetComponentVisibleInternal(szName.c_str(), false);

            if (BIT_CHECK(components, i))
            {
                SetComponentVisibleInternal(szName.c_str(), true);
            }
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            std::string szName = GetComponentNameByIDs(group, i);
            SetComponentVisibleInternal(szName.c_str(), false);
            if (BIT_CHECK(components, i))
            {
                SetComponentVisibleInternal(szName.c_str(), true);
            }
        }
    }
}

void* GetSuspensionLinesFromModel(int nModelIndex, int& numWheels)
{
    Log("numWheels: %d", numWheels);
    uint8_t* pCollisionData = GetCollisionDataFromModel(nModelIndex);

    if (!pCollisionData)
    {
        return nullptr;
    }

    void* pLines = *(void**)(pCollisionData + 16);

    numWheels = *(uint8_t*)(pCollisionData + 6);

    return pLines;
}

uint8_t* GetCollisionDataFromModel(int nModelIndex)
{
    auto* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(nModelIndex));


    if (!pModelInfoStart)
    {
        return nullptr;
    }


    if (!pModelInfoStart->m_pColModel)
    {
        return nullptr;
    }

    uint8_t* pCollisionData = *(uint8_t * *)(pModelInfoStart->m_pColModel + 44);

    return pCollisionData;
}
void CVehicle::SetHandlingData(std::vector<SHandlingData>& vHandlingData)
{
    if (!m_pVehicle || !m_dwGTAId)
    {
        return;
    }
    if (!GamePool_Vehicle_GetAt(m_dwGTAId))
    {
        return;
    }

    if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
    {
        return;
    }


//	if (!m_pCustomHandling)
//	{
//		m_pCustomHandling = new tHandlingData;
//	}

    auto pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(m_pVehicle->nModelIndex));

    if (!pModelInfoStart)
    {
        return;
    }

    CChatWindow::AddDebugMessage("handling id %d", *(uint16_t*)(pModelInfoStart + 98));

    CHandlingDefault::GetDefaultHandling(pModelInfoStart->m_nHandlingId, m_pCustomHandling);

    CChatWindow::AddDebugMessage("mass %f", m_pCustomHandling->m_fMass);
    CChatWindow::AddDebugMessage("turn %f", m_pCustomHandling->m_fTurnMass);
    CChatWindow::AddDebugMessage("m_fEngineAcceleration %f", m_pCustomHandling->m_transmissionData.m_fEngineAcceleration);
    CChatWindow::AddDebugMessage("m_fMaxGearVelocity %f", m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity);
    CChatWindow::AddDebugMessage("flags 0x%x", m_pCustomHandling->m_nHandlingFlags);

    bool bNeedRecalculate = false;

    for (auto& i : vHandlingData)
    {
        switch (i.flag)
        {
            case E_HANDLING_PARAMS::hpMaxSpeed:
                m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity = i.fValue * 0.84;
                break;
            case E_HANDLING_PARAMS::hpAcceleration: {
                float sampSpeed = m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity * 1.2f;
                m_pCustomHandling->m_transmissionData.m_fEngineAcceleration = sampSpeed / 12.0f;
//				m_pCustomHandling->m_transmissionData.m_fEngineAcceleration =  i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpEngineInertion:
                m_pCustomHandling->m_transmissionData.m_fEngineInertia = i.fValue;
                break;
            case E_HANDLING_PARAMS::hpGear:

                if (i.fValue == 1)
                {
                    m_pCustomHandling->m_transmissionData.m_nDriveType = 'R';
                }

                if (i.fValue == 2)
                {
                    m_pCustomHandling->m_transmissionData.m_nDriveType = 'F';
                }

                if (i.fValue == 3)
                {
                    m_pCustomHandling->m_transmissionData.m_nDriveType = '4';
                }

                break;
            case E_HANDLING_PARAMS::hpMass:
                m_pCustomHandling->m_fMass = i.fValue;
                break;
            case E_HANDLING_PARAMS::hpMassTurn:
                m_pCustomHandling->m_fTurnMass = i.fValue;
                break;
            case E_HANDLING_PARAMS::hpBrakeDeceleration:
            {
                m_pCustomHandling->m_fBrakeDeceleration = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpTractionMultiplier:
            {
                m_pCustomHandling->m_fTractionMultiplier = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpTractionLoss:
            {
                m_pCustomHandling->m_fTractionLoss = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpTractionBias:
            {
                m_pCustomHandling->m_fTractionBias = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpSuspensionLowerLimit:
            {
                m_pCustomHandling->m_fSuspensionLowerLimit = i.fValue;
                bNeedRecalculate = true;
                break;
            }
            case E_HANDLING_PARAMS::hpSuspensionBias:
            {
                m_pCustomHandling->m_fSuspensionBiasBetweenFrontAndRear = i.fValue;
                bNeedRecalculate = true;
                break;
            }
            case E_HANDLING_PARAMS::hpWheelSize:
            {
                m_bWheelSize = true;
                m_fWheelSize = i.fValue;
                bNeedRecalculate = true;
                break;
            }
        }
    }

    float fOldFrontWheelSize = 0.0f;
    float fOldRearWheelSize = 0.0f;

    if (m_bWheelSize)
    {
        fOldFrontWheelSize = pModelInfoStart->m_fWheelSizeFront;
        pModelInfoStart->m_fWheelSizeFront = m_fWheelSize;

        fOldRearWheelSize = pModelInfoStart->m_fWheelSizeRear;
        pModelInfoStart->m_fWheelSizeRear = m_fWheelSize;
    }

    /*CChatWindow::AddDebugMessage("AFTER");
    CChatWindow::AddDebugMessage("mass %f", m_pCustomHandling->m_fMass);
    CChatWindow::AddDebugMessage("turn %f", m_pCustomHandling->m_fTurnMass);
    CChatWindow::AddDebugMessage("m_fEngineAcceleration %f", m_pCustomHandling->m_transmissionData.m_fEngineAcceleration);
    CChatWindow::AddDebugMessage("m_fMaxGearVelocity %f", m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity);
    CChatWindow::AddDebugMessage("flags 0x%x", m_pCustomHandling->m_nHandlingFlags);*/

//	((void (*)(int, tHandlingData*))(g_libGTASA + 0x00570DC8 + 1))(0, m_pCustomHandling);
//	m_pVehicle->pHandling = m_pCustomHandling;
//
//	if (bNeedRecalculate)
//	{
//		((void (*)(CVehicleGta*))(g_libGTASA + 0x0054EC38 + 1))(m_pVehicle); // CAutomobile::SetupSuspensionLines
//
//		CopyGlobalSuspensionLinesToPrivate();
//	}

    if (m_bWheelSize)
    {
        pModelInfoStart->m_fWheelSizeFront = fOldFrontWheelSize;
        pModelInfoStart->m_fWheelSizeRear = fOldRearWheelSize;
    }

    if (bNeedRecalculate)
    {
        ((void (*)(CVehicleGta*))(g_libGTASA + (VER_x32 ? 0x0055F430 + 1 : 0x68036C)))(m_pVehicle); // process suspension
    }
    //ScriptCommand(&set_car_heavy, m_dwGTAId, 1);
}

void CVehicle::ResetVehicleHandling()
{
    Log("ResetVehicleHandling");
    if (!m_pVehicle || !m_dwGTAId)
    {
        return;
    }
    if (!GamePool_Vehicle_GetAt(m_dwGTAId))
    {
        return;
    }

    if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
    {
        return;
    }

    if (!m_pCustomHandling)
    {
        m_pCustomHandling = new tHandlingData;
    }

    auto pModelInfoStart = CModelInfo::GetModelInfo(m_pVehicle->nModelIndex);

    if (!pModelInfoStart)
    {
        return;
    }

    CHandlingDefault::GetDefaultHandling(*(uint16_t*)(pModelInfoStart + 98), m_pCustomHandling);

    ((void (*)(int, tHandlingData*))(g_libGTASA + 0x00570DC8 + 1))(0, m_pCustomHandling);

    m_pVehicle->pHandling = m_pCustomHandling;

    ((void (*)(CVehicleGta*))(g_libGTASA + 0x0054EC38 + 1))(m_pVehicle); // CAutomobile::SetupSuspensionLines
    CopyGlobalSuspensionLinesToPrivate();

    Log("Reseted to defaults");
}

void CVehicle::ApplyVinyls(uint8_t bSlot1, uint8_t bSlot2)
{
    if (bSlot1 == 0)
    {
        RemoveTexture("remap_cbody_0");
        return;
    }
    if (bSlot2 == 0)
    {
        RemoveTexture("remap_cbody_0");
        return;
    }

    char szTex[MAX_REPLACED_TEXTURE_NAME];

    if (bSlot1 != 255)
    {
        sprintf(&szTex[0], "v_cust_body_%d", bSlot1);
        ApplyTexture("remap_cbody_0", &szTex[0]);
    }

    if (bSlot2 != 255)
    {
        sprintf(&szTex[0], "v_cust_body_%d", bSlot2);
        ApplyTexture("remap_cbody_0", &szTex[0]);
    }

}

void CVehicle::ApplyToner(uint8_t bSlot, uint8_t bID)
{
    char szOld[MAX_REPLACED_TEXTURE_NAME];
    char szNew[MAX_REPLACED_TEXTURE_NAME];

    if (bID == 0)
    {
        sprintf(&szOld[0], "remap_toner_%d", bSlot);
        RemoveTexture(&szOld[0]);
        return;
    }
    if (bID == 255)
    {
        return;
    }

    sprintf(&szOld[0], "remap_toner_%d", bSlot);
    sprintf(&szNew[0], "v_cust_t_%d", bID);
    ApplyTexture(&szOld[0], &szNew[0]);
}

RwObject* GetAllAtomicObjectCB(RwObject* object, void* data)
{

    std::vector<RwObject*>& result = *((std::vector<RwObject*>*) data);
    result.push_back(object);
    return object;
}

// Get all atomics for this frame (even if they are invisible)
void GetAllAtomicObjects(RwFrame* frame, std::vector<RwObject*>& result)
{
    RwFrameForAllObjects(frame, (RwObjectCallBack)GetAllAtomicObjectCB, (void*)& result);

    //((uintptr_t(*)(RwFrame*, void*, uintptr_t))(g_libGTASA + 0x001D8858 + 1))(frame, (void*)GetAllAtomicObjectCB, (uintptr_t)& result);
}

void CVehicle::ApplyTexture(const char* szTexture, const char* szNew)
{
    Log("ApplyTexture");
    if (IsRetextured(szTexture))
    {
        RemoveTexture(szTexture);
    }

    uint8_t bID = 255;
    for (uint8_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
    {
        if (m_bReplaceTextureStatus[i] == false)
        {
            bID = i;
            break;
        }
    }

    if (bID == 255)
    {
        return;
    }

    m_bReplaceTextureStatus[bID] = true;
    strcpy(&(m_szReplacedTextures[bID].szOld[0]), szTexture);
    m_szReplacedTextures[bID].pTexture = CUtil::LoadTextureFromDB("samp", szNew);

    m_bReplacedTexture = true;
}

void CVehicle::ApplyTexture(const char* szTexture, RwTexture* pTexture)
{
    if (IsRetextured(szTexture))
    {
        RemoveTexture(szTexture);
    }
    //CChatWindow::AddDebugMessage("apply tex %s", szTexture);
    uint8_t bID = 255;
    for (uint8_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
    {
        if (m_bReplaceTextureStatus[i] == false)
        {
            bID = i;
            break;
        }
    }

    if (bID == 255)
    {
        return;
    }

    m_bReplaceTextureStatus[bID] = true;
    strcpy(&(m_szReplacedTextures[bID].szOld[0]), szTexture);
    m_szReplacedTextures[bID].pTexture = pTexture;

    m_bReplacedTexture = true;
}

void CVehicle::RemoveTexture(const char* szOldTexture)
{
    for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
    {
        if (m_bReplaceTextureStatus[i])
        {
            if (!strcmp(m_szReplacedTextures[i].szOld, szOldTexture))
            {
                m_bReplaceTextureStatus[i] = false;

                if (m_szReplacedTextures[i].pTexture)
                {
                    RwTextureDestroy(m_szReplacedTextures[i].pTexture);
                    m_szReplacedTextures[i].pTexture = nullptr;
                }
                break;
            }
        }
    }
}

bool CVehicle::IsRetextured(const char* szOldTexture)
{
    for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
    {
        if (m_bReplaceTextureStatus[i])
        {
            if (!strcmp(m_szReplacedTextures[i].szOld, szOldTexture))
            {
                return true;
            }
        }
    }
    return false;
}

void CVehicle::SetHeadlightsColor(uint8_t r, uint8_t g, uint8_t b)
{
    if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
    {
        return;
    }

    m_bHeadlightsColor = true;
    m_bHeadlightsR = r;
    m_bHeadlightsG = g;
    m_bHeadlightsB = b;
}

void CVehicle::ProcessHeadlightsColor(uint8_t& r, uint8_t& g, uint8_t& b)
{
    if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
    {
        return;
    }

    if (m_bHeadlightsColor)
    {
        r = m_bHeadlightsR;
        g = m_bHeadlightsG;
        b = m_bHeadlightsB;
    }
}

void CVehicle::SetWheelAlignment(int iWheel, float angle)
{
    if (!m_pVehicle || !m_dwGTAId)
    {
        return;
    }

    if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
    {
        return;
    }

    if (iWheel == 0)
    {
        m_bWheelAlignmentX = true;
        m_fWheelAlignmentX = (M_PI / 180.0f) * angle;
    }
    else
    {
        m_bWheelAlignmentY = true;
        m_fWheelAlignmentY = (M_PI / 180.0f) * angle;
    }
}

void CVehicle::SetWheelOffset(int iWheel, float offset)
{
    if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
    {
        return;
    }

    //CChatWindow::AddDebugMessage("set for %d wheel %f offset", iWheel, offset);
    if (iWheel == 0)
    {
        m_bWheelOffsetX = true;
        m_fNewOffsetX = offset;
        m_bWasWheelOffsetProcessedX = false;
    }
    else
    {
        m_bWheelOffsetY = true;
        m_fNewOffsetY = offset;
        m_bWasWheelOffsetProcessedY = false;
    }

    m_uiLastProcessedWheelOffset = GetTickCount();
}

void CVehicle::SetWheelWidth(float fValue)
{
    if (fValue == 20.0f)
    {
        m_bWheelWidth = false;
        return;
    }
    m_bWheelWidth = true;
    m_fWheelWidth = fValue;
}

RwMatrix* RwMatrixMultiplyByVector(CVector* out, RwMatrix* a2, CVector* in);

void CVehicle::ProcessWheelsOffset()
{
    if (GetTickCount() - m_uiLastProcessedWheelOffset <= 30)
    {
        return;
    }

    if (!m_bWasWheelOffsetProcessedX)
    {
        if (m_bWheelOffsetX)
        {
            //CChatWindow::AddDebugMessage("setting wheel offset X");
#if !VER_x32
            RwFrame* pWheelLF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, "wheel_lf_dummy"); // GetFrameFromname
			RwFrame* pWheelRF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, "wheel_rf_dummy"); // GetFrameFromname
#else
            RwFrame* pWheelLF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_lf_dummy"); // GetFrameFromname
            RwFrame* pWheelRF = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_rf_dummy"); // GetFrameFromname
#endif
            /*if (m_fNewOffsetX)
            {
                ProcessWheelOffset(pWheelLF, true, 0.0f - m_fWheelOffsetX);
                ProcessWheelOffset(pWheelRF, false, 0.0f - m_fWheelOffsetX);

                m_fWheelOffsetX = m_fNewOffsetX;
                m_fNewOffsetX = 0.0f;
                //CChatWindow::AddDebugMessage("moved old X");
            }*/
            m_fWheelOffsetX = m_fNewOffsetX;

            ProcessWheelOffset(pWheelLF, true, m_fWheelOffsetX, 0);
            ProcessWheelOffset(pWheelRF, false, m_fWheelOffsetX, 1);

        }
        m_bWasWheelOffsetProcessedX = true;
    }
    if (!m_bWasWheelOffsetProcessedY)
    {
        if (m_bWheelOffsetY)
        {
            //CChatWindow::AddDebugMessage("setting wheel offset Y");
#if !VER_x32
            RwFrame* pWheelRB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, "wheel_rb_dummy"); // GetFrameFromname
            RwFrame* pWheelLB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, "wheel_lb_dummy"); // GetFrameFromname
#else
            RwFrame* pWheelRB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_rb_dummy"); // GetFrameFromname
            RwFrame* pWheelLB = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, "wheel_lb_dummy"); // GetFrameFromname
#endif

            /*if (m_fNewOffsetY)
            {
                ProcessWheelOffset(pWheelRB, false, 0.0f - m_fWheelOffsetY);
                ProcessWheelOffset(pWheelLB, true, 0.0f - m_fWheelOffsetY);
                m_fWheelOffsetY = m_fNewOffsetY;
                m_fNewOffsetY = 0.0f;

                //CChatWindow::AddDebugMessage("moved old Y");
            }*/
            m_fWheelOffsetY = m_fNewOffsetY;
            ProcessWheelOffset(pWheelRB, false, m_fWheelOffsetY, 2);
            ProcessWheelOffset(pWheelLB, true, m_fWheelOffsetY, 3);
        }
        m_bWasWheelOffsetProcessedY = true;
    }
}

void CVehicle::SetCustomShadow(uint8_t r, uint8_t g, uint8_t b, float fSizeX, float fSizeY, const char* szTex)
{
    if (fSizeX == 0.0f || fSizeY == 0.0f)
    {
        m_bShadow = false;

        if (m_Shadow.pTexture)
        {
            RwTextureDestroy(m_Shadow.pTexture);
            m_Shadow.pTexture = nullptr;
        }

        return;
    }

    m_bShadow = true;

    m_Shadow.r = r;
    m_Shadow.g = g;
    m_Shadow.b = b;
    m_Shadow.fSizeX = fSizeX;
    m_Shadow.fSizeY = fSizeY;
    m_Shadow.pTexture = CUtil::LoadTextureFromDB("samp", szTex);
}

void CVehicle::ProcessWheelOffset(RwFrame* pFrame, bool bLeft, float fValue, int iID)
{
    CVector vecOffset;
    vecOffset.x = 0.0f - fValue;
    vecOffset.y = 0.0f;
    vecOffset.z = 0.0f;
    if (bLeft)
    {
        vecOffset.x *= -1.0f;
    }

    CVector vecOut;
    RwMatrixMultiplyByVector(&vecOut, &(m_vInitialWheelMatrix[iID]), &vecOffset);

    pFrame->modelling.pos.x = vecOut.x;
    pFrame->modelling.pos.y = vecOut.y;
    pFrame->modelling.pos.z = vecOut.z;
}

void CVehicle::SetComponentAngle(bool bUnk, int iID, float angle)
{
    if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
    {
#if !VER_x32
        ((void(*)(CVehicleGta*, int a2, int a3, int a4, float a5, uint8_t a6))(g_libGTASA + 0x00670B10))(m_pVehicle, 0, iID, bUnk, angle, 1); // CAutomobile::OpenDoor
#else
        ((void(*)(CVehicleGta*, int a2, int a3, int a4, float a5, uint8_t a6))(g_libGTASA + 0x005507F4 + 1))(m_pVehicle, 0, iID, bUnk, angle, 1); // CAutomobile::OpenDoor
#endif
    }
}


void CVehicle::SetComponentVisibleInternal(const char* szComponent, bool bVisible)
{
    if (!m_pVehicle || !m_dwGTAId)
    {
        return;
    }

    if (!GamePool_Vehicle_GetAt(m_dwGTAId))
    {
        return;
    }

    if (!m_pVehicle->m_pRwObject)
    {
        return;
    }
#if !VER_x32
    RwFrame* pFrame = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(m_pVehicle->m_pRpClump, szComponent); // GetFrameFromname
#else
    RwFrame* pFrame = ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(m_pVehicle->m_pRpClump, szComponent); // GetFrameFromname
#endif

    if (pFrame != nullptr)
    {
        // Get all atomics for this component - Usually one, or two if there is a damaged version
        std::vector<RwObject*> atomicList;
        GetAllAtomicObjects(pFrame, atomicList);

        // Count number currently visible
        uint uiNumAtomicsCurrentlyVisible = 0;
        for (uint i = 0; i < atomicList.size(); i++)
        {
            if (!atomicList[i])
            {
                continue;
            }
            if (atomicList[i]->flags & 0x04)
            {
                uiNumAtomicsCurrentlyVisible++;
            }
        }

        if (bVisible && uiNumAtomicsCurrentlyVisible == 0)
        {
            // Make atomic (undamaged version) visible. TODO - Check if damaged version should be made visible instead
            for (uint i = 0; i < atomicList.size(); i++)
            {
                RwObject* pAtomic = atomicList[i];
                if (!pAtomic)
                {
                    continue;
                }
#if !VER_x32
                int       AtomicId = ((int(*)(RwObject*))(g_libGTASA + 0x006F9D68))(pAtomic); // CVisibilityPlugins::GetAtomicId
#else
                int       AtomicId = ((int(*)(RwObject*))(g_libGTASA + 0x005D4B54 + 1))(pAtomic); // CVisibilityPlugins::GetAtomicId
#endif

                if (!(AtomicId & ATOMIC_ID_FLAG_TWO_VERSIONS_DAMAGED))
                {
                    // Either only one version, or two versions and this is the undamaged one
                    pAtomic->flags |= 0x04;
                }
            }
        }
        else if (!bVisible && uiNumAtomicsCurrentlyVisible > 0)
        {
            // Make all atomics invisible
            for (uint i = 0; i < atomicList.size(); i++)
            {
                if (!atomicList[i])
                {
                    continue;
                }
                atomicList[i]->flags &= ~0x05;            // Mimic what GTA seems to do - Not sure what the bottom bit is for
            }
        }
    }
}

std::string CVehicle::GetComponentNameByIDs(uint8_t group, int subgroup)
{

    if (group == E_CUSTOM_COMPONENTS::ccExtra && subgroup >= EXTRA_COMPONENT_BOOT)
    {
        switch (subgroup)
        {
            case EXTRA_COMPONENT_BOOT:
                return std::string("boot_dummy");
            case EXTRA_COMPONENT_BONNET:
                return std::string("bonnet_dummy");
            case EXTRA_COMPONENT_BUMP_REAR:
                return std::string("bump_rear_dummy");
            case EXTRA_COMPONENT_DEFAULT_DOOR:
                return std::string("door_lf_dummy");
            case EXTRA_COMPONENT_WHEEL:
                return std::string("wheel_lf_dummy");
            case EXTRA_COMPONENT_BUMP_FRONT:
                return std::string("bump_front_dummy");
        }
    }

    std::string retn;

    switch (group)
    {
        case E_CUSTOM_COMPONENTS::ccBumperF:
            retn += "bumberF_";
            break;
        case E_CUSTOM_COMPONENTS::ccBumperR:
            retn += "bumberR_";
            break;
        case E_CUSTOM_COMPONENTS::ccFenderF:
            retn += "fenderF_";
            break;
        case E_CUSTOM_COMPONENTS::ccFenderR:
            retn += "fenderR_";
            break;
        case E_CUSTOM_COMPONENTS::ccSpoiler:
            retn += "spoiler_";
            break;
        case E_CUSTOM_COMPONENTS::ccExhaust:
            retn += "exhaust_";
            break;
        case E_CUSTOM_COMPONENTS::ccRoof:
            retn += "roof_";
            break;
        case E_CUSTOM_COMPONENTS::ccTaillights:
            retn += "taillights_";
            break;
        case E_CUSTOM_COMPONENTS::ccHeadlights:
            retn += "headlights_";
            break;
        case E_CUSTOM_COMPONENTS::ccDiffuser:
            retn += "diffuser_";
            break;
        case E_CUSTOM_COMPONENTS::ccSplitter:
            retn += "splitter_";
            break;
        case E_CUSTOM_COMPONENTS::ccExtra:
            retn += "ext_";
            break;
        default:
            retn = std::string("err");
            break;
    }

    retn += ('0' + (char)subgroup);

    return retn;
}



void CVehicle::SetEngineState(bool bEnable)
{
    if(!m_dwGTAId)return;
    if(!m_pVehicle)return;
    if (!GamePool_Vehicle_GetAt(m_dwGTAId)) {
        return;
    }
    //m_pVehicle->m_nVehicleFlags.bEngineBroken = 1;
    //m_bEngineOn = bEnable;
    m_pVehicle->m_nVehicleFlags.bEngineOn = m_bEngineOn = bEnable;
}

bool CVehicle::HasDamageModel()
{
    if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
        return true;
    return false;
}

uint8_t CVehicle::GetPanelStatus(uint8_t bPanel)
{
    if (m_pVehicle && bPanel < MAX_PANELS)
    {

        return ((uint8_t(*)(uintptr_t, uint8_t))(g_libGTASA + (VER_x32 ? 0x0056E78A + 1 : 0x6909A4)))(((uintptr_t)m_pVehicle + (VER_x32 ? 1460 : 1880)), bPanel);

    }
    return 0;
}

void CVehicle::SetPanelStatus(uint8_t bPanel, uint8_t bPanelStatus)
{
    if (m_pVehicle && bPanel < MAX_PANELS && bPanelStatus <= 3)
    {
        if (GetPanelStatus(bPanel) != bPanelStatus)
        {

            ((uint8_t(*)(uintptr_t, uint8_t, uint8_t))(g_libGTASA + (VER_x32 ? 0x0056E770 + 1 : 0x690980)))(((uintptr_t)m_pVehicle + (VER_x32 ? 1460 : 1880)), bPanel, bPanelStatus);


            if (bPanelStatus == DT_PANEL_INTACT)
            {
                // Grab the car node index for the given panel
                static int s_iCarNodeIndexes[7] = { 0x0F, 0x0E, 0x00 /*?*/, 0x00 /*?*/, 0x12, 0x0C, 0x0D };
                int iCarNodeIndex = s_iCarNodeIndexes[bPanel];

                // CAutomobile::FixPanel
                ((uint8_t(*)(uintptr_t, uint32_t, uint32_t))(g_libGTASA + (VER_x32 ? 0x0055D7A6 + 1 : 0x67E1EC)))((uintptr_t)m_pVehicle, iCarNodeIndex, static_cast<uint32_t>(bPanel));
            }
            else
            {
                ((uint8_t(*)(uintptr_t, uint32_t, bool))(g_libGTASA + (VER_x32 ? 0x00552CDC + 1 : 0x6731C0)))((uintptr_t)m_pVehicle, static_cast<uint32_t>(bPanel), false);
            }
        }
    }
}

uint8_t CVehicle::GetDoorStatus(eDoors bDoor)
{
    if (m_pVehicle && bDoor < MAX_DOORS)
    {
        DAMAGE_MANAGER_INTERFACE* pDamageManager = (DAMAGE_MANAGER_INTERFACE*)((uintptr_t)m_pVehicle + (VER_x32 ? 1460 : 1880));
        if (pDamageManager) return pDamageManager->Door[bDoor];
    }
    return 0;
}

void CVehicle::SetDoorStatus(eDoors bDoor, uint8_t bDoorStatus, bool spawnFlyingComponen)
{
    if (m_pVehicle && bDoor < MAX_DOORS)
    {
        if (GetDoorStatus(bDoor) != bDoorStatus)
        {
            uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + (VER_x32 ? 1460 : 1880));

            ((uint8_t(*)(uintptr_t, uint8_t, uint8_t, bool))(g_libGTASA + (VER_x32 ? 0x0056E7B0 + 1 : 0x6909E4)))(((uintptr_t)m_pVehicle + (VER_x32 ? 1460 : 1880)), bDoor, bDoorStatus, spawnFlyingComponen);

            if (bDoorStatus == DT_DOOR_INTACT || bDoorStatus == DT_DOOR_SWINGING_FREE)
            {
                // Grab the car node index for the given door id
                static int s_iCarNodeIndexes[6] = { 0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09 };
                int iCarNodeIndex = s_iCarNodeIndexes[bDoor];

                // CAutomobile::FixDoor
                ((uint8_t(*)(uintptr_t, uint32_t, uint32_t))(g_libGTASA + (VER_x32 ? 0x0055D6AA + 1 : 0x67E058)))((uintptr_t)m_pVehicle, iCarNodeIndex, static_cast<uint32_t>(bDoor));
            }
            else
            {
                bool bQuiet = !spawnFlyingComponen;
                ((uint8_t(*)(uintptr_t, uint32_t, bool))(g_libGTASA + (VER_x32 ? 0x00552E3C + 1 : 0x6733BC)))((uintptr_t)m_pVehicle, static_cast<uint32_t>(bDoor), bQuiet);
            }
        }
    }
}

void CVehicle::SetDoorStatus(uint32_t dwDoorStatus, bool spawnFlyingComponen)
{
    if (m_pVehicle)
    {
        for (uint8_t uiIndex = 0; uiIndex < MAX_DOORS; uiIndex++)
        {
            SetDoorStatus(static_cast<eDoors>(uiIndex), static_cast<uint8_t>(dwDoorStatus), spawnFlyingComponen);
            dwDoorStatus >>= 8;
        }
    }
}

void CVehicle::SetPanelStatus(uint32_t ulPanelStatus)
{
    if (m_pVehicle)
    {
        for (uint8_t uiIndex = 0; uiIndex < MAX_PANELS; uiIndex++)
        {
            SetPanelStatus(uiIndex, static_cast<uint8_t>(ulPanelStatus));
            ulPanelStatus >>= 4;
        }
    }
}

void CVehicle::SetLightStatus(uint8_t bLight, uint8_t bLightStatus)
{
    if (m_pVehicle && bLight < MAX_LIGHTS)
    {
        uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1460);
#if !VER_x32
        ((uint8_t(*)(uintptr_t, uint8_t, uint8_t))(g_libGTASA + 0x690948))(((uintptr_t)m_pVehicle + 1880), bLight, bLightStatus);
#else
        ((uint8_t(*)(uintptr_t, uint8_t, uint8_t))(g_libGTASA + 0x0056E748 + 1))(((uintptr_t)m_pVehicle + 1460), bLight, bLightStatus);
#endif
    }
}

void CVehicle::SetLightStatus(uint8_t ucStatus)
{
    if (m_pVehicle)
    {
        DAMAGE_MANAGER_INTERFACE* pDamageManager = (DAMAGE_MANAGER_INTERFACE*)((uintptr_t)m_pVehicle + (VER_x32 ? 1460 : 1880));
        if (pDamageManager) pDamageManager->Lights = static_cast<uint32_t>(ucStatus);
    }
}

uint8_t CVehicle::GetLightStatus(uint8_t bLight)
{
    if (m_pVehicle && bLight < MAX_LIGHTS)
    {
        uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1460);
#if !VER_x32
        return ((uint8_t(*)(uintptr_t, uint8_t))(g_libGTASA + 0x69096C))(((uintptr_t)m_pVehicle + 1880), bLight);
#else
        return ((uint8_t(*)(uintptr_t, uint8_t))(g_libGTASA + 0x0056E762 + 1))(((uintptr_t)m_pVehicle + 1460), bLight);
#endif
    }
    return 0;
}

uint8_t CVehicle::GetWheelStatus(eWheelPosition bWheel)
{
    if (m_pVehicle && bWheel < MAX_WHEELS)
    {
#if !VER_x32
        return ((uint8_t(*)(uintptr_t, uint8_t))(g_libGTASA + 0x006909C4))(((uintptr_t)m_pVehicle + 1880), bWheel);
#else
        return ((uint8_t(*)(uintptr_t, uint8_t))(g_libGTASA + 0x0056E79E + 1))(((uintptr_t)m_pVehicle + 1460), bWheel);
#endif
    }
    return 0;
}

void CVehicle::SetWheelStatus(eWheelPosition bWheel, uint8_t bTireStatus)
{
    if (m_pVehicle && bWheel < MAX_WHEELS)
    {
        uintptr_t* pDamageManager = (uintptr_t*)((uintptr_t)m_pVehicle + 1460);
#if !VER_x32
        ((uint8_t(*)(uintptr_t, uint8_t, uint8_t))(g_libGTASA + 0x006909B8))(((uintptr_t)m_pVehicle + 1880), bWheel, bTireStatus);
#else
        ((uint8_t(*)(uintptr_t, uint8_t, uint8_t))(g_libGTASA + 0x0056E798 + 1))(((uintptr_t)m_pVehicle + 1460), bWheel, bTireStatus);
#endif
    }
}

void CVehicle::SetBikeWheelStatus(uint8_t bWheel, uint8_t bTireStatus)
{
    if (m_pVehicle && bWheel < 2)
    {
        if (bWheel == 0)
        {
            *(uint8_t*)((uintptr_t)m_pVehicle + (VER_x32 ? 1648 : 2124)) = bTireStatus;
        }
        else
        {
            *(uint8_t*)((uintptr_t)m_pVehicle + (VER_x32 ? 1649 : 2125)) = bTireStatus;
        }
    }
}

uint8_t CVehicle::GetBikeWheelStatus(uint8_t bWheel)
{
    if (m_pVehicle && bWheel < 2)
    {
        if (bWheel == 0)
        {
            return *(uint8_t*)((uintptr_t)m_pVehicle + (VER_x32 ? 1648 : 2124));
        }
        else
        {
            return *(uint8_t*)((uintptr_t)m_pVehicle + (VER_x32 ? 1649 : 2125));
        }
    }
    return 0;
}

void CVehicle::UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage, uint8_t byteTireDamage)
{
    if (HasDamageModel())
    {
        SetPanelStatus(dwPanelDamage);
        SetDoorStatus(dwDoorDamage, false);

        SetLightStatus(eLights::LEFT_HEADLIGHT, byteLightDamage & 1);
        SetLightStatus(eLights::RIGHT_HEADLIGHT, (byteLightDamage >> 2) & 1);
        if ((byteLightDamage >> 6) & 1)
        {
            SetLightStatus(eLights::LEFT_TAIL_LIGHT, 1);
            SetLightStatus(eLights::RIGHT_TAIL_LIGHT, 1);
        }

        SetWheelStatus(eWheelPosition::REAR_RIGHT_WHEEL, byteTireDamage & 1);
        SetWheelStatus(eWheelPosition::FRONT_RIGHT_WHEEL, (byteTireDamage >> 1) & 1);
        SetWheelStatus(eWheelPosition::REAR_LEFT_WHEEL, (byteTireDamage >> 2) & 1);
        SetWheelStatus(eWheelPosition::FRONT_LEFT_WHEEL, (byteTireDamage >> 3) & 1);
    }
    else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE)
    {
        SetBikeWheelStatus(1, byteTireDamage & 1);
        SetBikeWheelStatus(0, (byteTireDamage >> 1) & 1);
    }
}
#if !VER_x32
uint64_t CVehicle::GetVehicleSubtype()
{
    auto* pVehicles = (CVehicleGta*)m_pVehicle;
    uintptr_t this_vtable = *(uintptr_t*)pVehicles;
	if (m_pVehicle)
	{
		if (this_vtable == g_libGTASA + 0x83BB50) // 0x871120
		{
			return VEHICLE_SUBTYPE_CAR;
		}
		else if (this_vtable == g_libGTASA + 0x83C2A0) // 0x8721A0
		{
			return VEHICLE_SUBTYPE_BOAT;
		}
		else if (this_vtable == g_libGTASA + 0x83BE40) // 0x871360
		{
			return VEHICLE_SUBTYPE_BIKE;
		}
		else if (this_vtable == g_libGTASA + 0x83C968) // 0x871948
		{
			return VEHICLE_SUBTYPE_PLANE;
		}
		else if (this_vtable == g_libGTASA + 0x83C4C8) // 0x871680
		{
			return VEHICLE_SUBTYPE_HELI;
		}
		else if (this_vtable == g_libGTASA + 0x83C070) // 0x871528
		{ // bmx?
			return VEHICLE_SUBTYPE_PUSHBIKE;
		}
		else if (this_vtable == g_libGTASA + 0x83D058) // 0x872370
		{
			return VEHICLE_SUBTYPE_TRAIN;
		}
//		else if (this_vtable == g_libGTASA + 0x0066DFD4) // 0x872370
//		{
//			return VEHICLE_SUBTYPE_TRAILER;
//		}
		//
	}

	return 0;
}
#else
unsigned int CVehicle::GetVehicleSubtype()
{
    auto* pVehicles = (CVehicleGta*)m_pVehicle;
    uintptr_t this_vtable = *(uintptr_t*)pVehicles;
    if (m_pVehicle)
    {
        if (this_vtable == g_libGTASA + 0x0066D678) // 0x871120
        {
            return VEHICLE_SUBTYPE_CAR;
        }
        else if (this_vtable == g_libGTASA + 0x0066DA20) // 0x8721A0
        {
            return VEHICLE_SUBTYPE_BOAT;
        }
        else if (this_vtable == g_libGTASA + 0x0066D7F0) // 0x871360
        {
            return VEHICLE_SUBTYPE_BIKE;
        }
        else if (this_vtable == g_libGTASA + 0x0066DD84) // 0x871948
        {
            return VEHICLE_SUBTYPE_PLANE;
        }
        else if (this_vtable == g_libGTASA + 0x0066DB34) // 0x871680
        {
            return VEHICLE_SUBTYPE_HELI;
        }
        else if (this_vtable == g_libGTASA + 0x0066D908) // 0x871528
        { // bmx?
            return VEHICLE_SUBTYPE_PUSHBIKE;
        }
        else if (this_vtable == g_libGTASA + 0x0066E0FC) // 0x872370
        {
            return VEHICLE_SUBTYPE_TRAIN;
        }
//		else if (this_vtable == g_libGTASA + 0x0066DFD4) // 0x872370
//		{
//			return VEHICLE_SUBTYPE_TRAILER;
//		}
        //
    }

    return 0;
}
#endif
bool CVehicle::IsTrailer()
{
    if(!m_pVehicle)return false;
    if(!m_pVehicle->nModelIndex)return false;
#if !VER_x32
    return ((bool (*)(int)) (g_libGTASA + 0x0045D064))(m_pVehicle->nModelIndex);
#else
    return ((bool (*)(int)) (g_libGTASA + 0x003863B0 + 1))(m_pVehicle->nModelIndex);
#endif
}

void CVehicle::GetDamageStatusEncoded(uint8_t* byteTyreFlags, uint8_t* byteLightFlags, uint32_t* dwDoorFlags, uint32_t* dwPanelFlags)
{
    if (byteTyreFlags) *byteTyreFlags = GetWheelStatus(eWheelPosition::REAR_RIGHT_WHEEL) | (GetWheelStatus(eWheelPosition::FRONT_RIGHT_WHEEL) << 1)
                                        | (GetWheelStatus(eWheelPosition::REAR_LEFT_WHEEL) << 2) | (GetWheelStatus(eWheelPosition::FRONT_LEFT_WHEEL) << 3);

    if (byteLightFlags) *byteLightFlags = GetLightStatus(eLights::LEFT_HEADLIGHT) | (GetLightStatus(eLights::RIGHT_HEADLIGHT) << 2);
    if (GetLightStatus(eLights::LEFT_TAIL_LIGHT) && GetLightStatus(eLights::RIGHT_TAIL_LIGHT))
        *byteLightFlags |= (1 << 6);

    if (dwDoorFlags) *dwDoorFlags = GetDoorStatus(eDoors::BONNET) | (GetDoorStatus(eDoors::BOOT) << 8) |
                                    (GetDoorStatus(eDoors::FRONT_LEFT_DOOR) << 16) | (GetDoorStatus(eDoors::FRONT_RIGHT_DOOR) << 24);

    if (dwPanelFlags) *dwPanelFlags = GetPanelStatus(ePanels::FRONT_LEFT_PANEL) | (GetPanelStatus(ePanels::FRONT_RIGHT_PANEL) << 4)
                                      | (GetPanelStatus(ePanels::REAR_LEFT_PANEL) << 8) | (GetPanelStatus(ePanels::REAR_RIGHT_PANEL) << 12)
                                      | (GetPanelStatus(ePanels::WINDSCREEN_PANEL) << 16) | (GetPanelStatus(ePanels::FRONT_BUMPER) << 20)
                                      | (GetPanelStatus(ePanels::REAR_BUMPER) << 24);
}

void CVehicle::ProcessDamage()
{
    if (pNetGame)
    {
        VEHICLEID vehId = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(m_pVehicle);
        if (vehId != INVALID_VEHICLE_ID)
        {
            if (HasDamageModel())
            {
                uint8_t byteTyreFlags, byteLightFlags;
                uint32_t dwDoorFlags, dwPanelFlags;

                GetDamageStatusEncoded(&byteTyreFlags, &byteLightFlags, &dwDoorFlags, &dwPanelFlags);
                if (byteTyreFlags != m_byteTyreStatus || byteLightFlags != m_byteLightStatus ||
                    dwDoorFlags != m_dwDoorStatus || dwPanelFlags != m_dwPanelStatus)
                {
                    m_byteLightStatus = byteLightFlags;
                    m_byteTyreStatus = byteTyreFlags;
                    m_dwDoorStatus = dwDoorFlags;
                    m_dwPanelStatus = dwPanelFlags;

                    RakNet::BitStream bsDamage;

                    bsDamage.Write(vehId);
                    bsDamage.Write(dwPanelFlags);
                    bsDamage.Write(dwDoorFlags);
                    bsDamage.Write(byteLightFlags);
                    bsDamage.Write(byteTyreFlags);

                    pNetGame->GetRakClient()->RPC(&RPC_VehicleDamage, &bsDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
                }
            }
            else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE)
            {
                uint8_t byteTyreFlags = GetBikeWheelStatus(1) | (GetBikeWheelStatus(0) << 1);
                if (m_byteTyreStatus != byteTyreFlags)
                {
                    m_byteTyreStatus = byteTyreFlags;

                    RakNet::BitStream bsDamage;
                    bsDamage.Write(vehId);
                    bsDamage.Write((uint32_t)0);
                    bsDamage.Write((uint32_t)0);
                    bsDamage.Write((uint8_t)0);
                    bsDamage.Write(byteTyreFlags);

                    pNetGame->GetRakClient()->RPC(&RPC_VehicleDamage, &bsDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
                }
            }
        }
    }
}

extern void (*CShadows__StoreCarLightShadow)(CVehicleGta* vehicle, int id, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle);

void CVehicle::DoRearLightReflectionTwin(uint8 intensity){
    CVehicleGta* v2; // r4
    int v4; // r0
    float v5; // s16
    float v6; // s18
    int v7; // r0
    float *v8; // r1
    float v10; // s0
    float v11; // s4
    float v12; // s4
    float v13; // s2
    float v14; // s4
    CVector v15; // [sp+24h] [bp-44h] BYREF
    float v16; // [sp+30h] [bp-38h] BYREF
    CVector2D v17; // [sp+34h] [bp-34h]
    CVector v18; // [sp+38h] [bp-30h] BYREF
    v2 = this->m_pVehicle;
    CVehicleModelInfo *pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
            v2->nModelIndex));

    CVector * m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;


    v5 = m_avDummyPos[0].y;
    v6 = m_avDummyPos[1].y;
#if VER_x32
    v7 = *((uintptr_t*)v2 + 5);
    if ( v7 )
        v8 = (float *)(v7 + 48);
    else
        v8 = (float *)((char*)v2 + 4);

    v18.z = v8[2];
    v18.x = v8[0];
    v17 = CVector2D((const CVector *)(v7 + 0x10));
#else
    CVector pos;
    CMatrix* mat;
    mat = v2->m_matrix;
    if ( mat )
        pos = mat->pos;
    else
        pos = v2->m_transform.m_vPosn;

    v18.z = pos.z;
    v18.x = pos.x;

    v17 = CVector2D(mat->up);
#endif
    v10 = v17.y;
    v11 = (float)(v10 * v10) + (float)(v17.x * v17.x);
    if ( v11 <= 0.0 )
    {
        v13 = 1.0;
    }
    else
    {
        v12 = 1.0 / sqrtf(v11);
        v10 = v17.y * v12;
        v13 = v17.x * v12;
        v17.y = v17.y * v12;
    }
    v15.z = 2.0;
    v17.x = v13;
    v14 = (float)(v5 + 6.0) + (float)((float)(v6 + -0.6) - (float)(v5 + 6.0));
    v15.y = v14 * *(float *)&v10;
    v15.x = v14 * v13;

    v18.operator+=(v15);
    v18.y += v2->mat->pos.y;


    CShadows__StoreCarLightShadow(
            v2,
            (uintptr_t)v2 + 24,
            gpShadowExplosionTex,
            &v18,
            3.0,
            0.0,
            0.0,
            -3.0,
            intensity >> 2,
            0,
            0,
            7.0f);
}

void CVehicle::ProcessTurnLight(CVehicleGta *m_pVehicle, int isRight, int lightId) {

    if (!pNetGame) return;
        if (SpecialsShouldRenderFor(this)) {
            ProcessCoplightBarBlink();
            ProcessHeadlightStrobes();
    }
// -------------------------------------

    if(m_iTurnState == CVehicle::eTurnState::TURN_OFF) return;

    auto *pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
            this->m_pVehicle->nModelIndex));
    CVector * m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

    CVector vecFront;
    vecFront.x = m_avDummyPos[0].x;
    vecFront.y = m_avDummyPos[0].y;
    vecFront.z = m_avDummyPos[0].z;

    CVector vecRear;
    vecRear.x = m_avDummyPos[1].x;
    vecRear.y = m_avDummyPos[1].y;
    vecRear.z = m_avDummyPos[1].z;

    if (!isRight) {
        vecFront.x = -vecFront.x;
        vecRear.x = -vecRear.x;
    }

    uint32_t lastTick = GetTickCount();

    if (lastTick - m_iTickTurnLight >= 400) {
        m_iTickTurnLight = lastTick;
        m_bIsTurnLight = !m_bIsTurnLight;
        CPedGta* pDriver;
        if(pNetGame)
            if(pNetGame->GetPlayerPool())
                if(pNetGame->GetPlayerPool()->GetLocalPlayer())
                    if(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed())
                        pDriver = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_pPed;
        if (pDriver->bInVehicle) {
            if (pDriver->pVehicle) {
                if (pDriver->pVehicle == this->m_pVehicle) {
                    CSpeedometr::updateTurn(m_bIsTurnLight);

                    HSTREAM sound = m_pTurnLightTickSound1;
                    if (!m_bIsTurnLight)
                        sound = m_pTurnLightTickSound2;
                    BASS_ChannelPlay(sound, false);
                }
            }
        }
    }
    if (m_bIsTurnLight) {

        if(m_iTurnState == CVehicle::eTurnState::TURN_LEFT && !isRight|| m_iTurnState == CVehicle::eTurnState::TURN_RIGHT && isRight || m_iTurnState == CVehicle::eTurnState::TURN_ALL ) {
            uintptr_t v19 = reinterpret_cast<uintptr_t>(this->m_pVehicle) + 4;
            uintptr_t v20 = *(uintptr_t *)(this->m_pVehicle + 20);
            uintptr_t v21 = reinterpret_cast<uintptr_t>(this->m_pVehicle) + 4;
            if ( v20 )
                v21 = v20 + 48;
            int v22 = 1;
            if ( isRight )
                v22 = 2;
            CCoronas::RegisterCorona(
                    v22 + 2 * lightId + v21 + 4,
                    this->m_pVehicle,
                    255,
                    180,
                    0,
                    170,
                    &vecFront,
                    0.5f,
                    70.0f,
                    eCoronaType::CORONATYPE_HEADLIGHT,
                    eCoronaFlareType::FLARETYPE_NONE,
                    0,
                    0,
                    0,
                    0.0f,
                    0,
                    0.0f,
                    0,
                    80.0f,
                    0,
                    0
            );

            uintptr_t v23 = *(uintptr_t *)(reinterpret_cast<uintptr_t>(this->m_pVehicle) + 0x14);
            uintptr_t v24 = 55;
            if ( v23 )
                v19 = v23 + 48;
            if ( isRight )
                v24 = 56;
            CCoronas::RegisterCorona(
                    v24 + 2 * lightId + v19 + 4,
                    this->m_pVehicle,
                    255,
                    180,
                    0,
                    170,
                    &vecRear,
                    0.5f,
                    70.0f,
                    eCoronaType::CORONATYPE_HEADLIGHT,
                    eCoronaFlareType::FLARETYPE_NONE,
                    0,
                    0,
                    0,
                    0.0f,
                    0,
                    0.0f,
                    0,
                    80.0f,
                    0,
                    0
            );
            CVehicle::ProcessShadowLight(this->m_pVehicle, vecFront, 0, v24);
        }

    }

}


// ---- RW helpers: получить кадр по имени (как ты уже делаешь в коде) ----
static RwFrame* GetFrameFromName(RpClump* clump, const char* name)
{
#if !VER_x32
    return ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x0045BE78))(clump, name);
#else
    return ((RwFrame * (*)(RpClump*, const char*))(g_libGTASA + 0x003856F4 + 1))(clump, name);
#endif
}

// Перебрать список возможных имён и вернуть первый найденный кадр
static RwFrame* TryGetFrameByAnyName(RpClump* clump, const char* const* names)
{
    if (!clump || !names) return nullptr;
    for (const char* n = *names; n; n = *++names) {
        if (RwFrame* f = GetFrameFromName(clump, n))
            return f;
    }
    return nullptr;
}


// Рекурсивный проход по иерархии кадров и поиск по подстроке имени
static RwFrame* FindFrameContains(RwFrame* root, const char* sub)
{
    if (!root || !sub) return nullptr;

    // пройти детей
    RwFrame* child = root->child;
    while (child) {
        if (RwFrame* hit = FindFrameContains(child, sub))
            return hit;
        child = child->next;
    }
    return nullptr;
}

static RwFrame* GetClumpRoot(RpClump* clump)
{
    if (!clump) return nullptr;
    // Обычно root = RpClumpGetFrame(clump)
#if !VER_x32
    return ((RwFrame* (*)(RpClump*))(g_libGTASA + 0x0045BDE8))(clump);
#else
    return ((RwFrame* (*)(RpClump*))(g_libGTASA + 0x00385674 + 1))(clump);
#endif
}

// Конвертировать мировую позицию (из frame->ltm.pos) в локальные координаты авто
static CVector FrameWorldToLocal(const CMatrix* M, const RwMatrix& ltm)
{
    CVector w { ltm.pos.x, ltm.pos.y, ltm.pos.z };
    CVector rel { w.x - M->pos.x, w.y - M->pos.y, w.z - M->pos.z };
    return {
            rel.x * M->right.x + rel.y * M->right.y + rel.z * M->right.z,
            rel.x * M->up.x    + rel.y * M->up.y    + rel.z * M->up.z,
            rel.x * M->at.x    + rel.y * M->at.y    + rel.z * M->at.z
    };
}

void CVehicle::SetSirenEnabled(bool on)
{
    m_bSirenEnabled = on;
    m_tickCoplight  = GetTickCount();
}



void CVehicle::ProcessCoplightBarBlink()
{
    if (!m_pVehicle || !m_bSirenEnabled) return;
    if (!m_pVehicle) return;
    if (!SpecialsShouldRenderFor(this)) return;
    UpdateSiren3D();
    // ===== ПАТТЕРН =====
    struct PatCfg { uint32_t onMS, offMS, gapMS, flashes; };
    const uint32_t seed   = (uint32_t)((uintptr_t)m_pVehicle) & 0xFFFF;
    const uint32_t jitter = (seed * 2654435761u) & 7; // 0..7 ms
    const PatCfg   pc     = { 45u + jitter, 45u, 110u, 3u };

    const uint32_t now = GetTickCount();
    static bool     st_on       = true;
    static bool     st_right    = false;  // false=LEFT(красная), true=RIGHT(синяя)
    static int      st_flashIdx = 0;
    static uint32_t st_tick     = now;

    const uint32_t phaseLen = st_on ? pc.onMS : (st_flashIdx + 1 < (int)pc.flashes ? pc.offMS : pc.gapMS);
    if (now - st_tick >= phaseLen) {
        st_tick = now;
        if (st_on) {
            st_on = false;
        } else {
            if (++st_flashIdx >= (int)pc.flashes) { st_flashIdx = 0; st_right = !st_right; }
            st_on = true;
        }
    }

    auto easeOut = [](float t)->float { return 1.0f - (1.0f - t)*(1.0f - t); };
    const float t0    = float(now - (st_tick - phaseLen));
    const float phase = phaseLen ? std::min(1.0f, t0 / float(phaseLen)) : 1.0f;
    const float pulse = st_on ? easeOut(phase) : 0.0f;
    if (pulse <= 0.0f) return;

    // ===== ВИЗУАЛ / ГЕОМЕТРИЯ =====
    const float pairHalfSpan = 0.44f;   // разнос L/R капсул
    const float podHalfLen   = 0.18f;   // полудлина линзы-капсулы
    const float offsetUp     = 0.10f;
    const float extraLift    = 0.01f;   // +1 см вверх
    const float backShift    = 0.0f;    // всегда внутри плафона
    const float lowerDZ      = 0.10f;   // утопить внутрь
    const float faceOffset   = 0.022f;  // двусторонность (вперёд/назад по at)

    const float ledDX[3] = { -0.12f, 0.0f, +0.12f };
    const float ledDZ[3] = { -0.004f, 0.0f, -0.004f };

    const float sizeCore = 0.40f;
    const float sizeHalo = 0.50f;
    const float drawDist = 320.0f;

    const int   lensSegs    = 7;
    const float lensHalfLen = podHalfLen * 0.95f;
    const float lensHeight  = 0.30f;
    const int   lensAlpha   = 130;
    const int   lensCapA    = 160;

    // цвета (слева КРАСНЫЙ, справа СИНИЙ)
    const int redCoreR  = 255, redCoreG  =  70, redCoreB  =  70, redCoreA  = 255;
    const int redHaloR  = 255, redHaloG  =  55, redHaloB  =  55, redHaloA  = 150;
    const int blueCoreR =  50, blueCoreG = 155, blueCoreB = 255, blueCoreA = 255;
    const int blueHaloR =  30, blueHaloG = 110, blueHaloB = 255, blueHaloA = 145;

    // масштаб подземной «подсветки»
    const float UNDERGLOW_SCALE = 1.0f; // уменьшено в 3 раза (было 3.0f)
    const float UNDERGLOW_PUSH  = 1.25f;

    // ===== ОСИ/БАЗА БАЛКИ (МИР) =====
#if !VER_x32
    auto GetFrameFromNameFn = reinterpret_cast<RwFrame* (*)(RpClump*, const char*)>(g_libGTASA + 0x0045BE78);
#else
    auto GetFrameFromNameFn = reinterpret_cast<RwFrame* (*)(RpClump*, const char*)>(g_libGTASA + 0x003856F4 + 1);
#endif

    CVector R, U, A, P;
    if (RwFrame* frBar = GetFrameFromNameFn(m_pVehicle->m_pRpClump, "migalka")) {
        const RwMatrix& m = frBar->ltm;
        R.x = m.right.x; R.y = m.right.y; R.z = m.right.z;
        U.x = m.up.x;    U.y = m.up.y;    U.z = m.up.z;
        A.x = m.at.x;    A.y = m.at.y;    A.z = m.at.z;
        P.x = m.pos.x;   P.y = m.pos.y;   P.z = m.pos.z;
    } else if (RwFrame* frMid = GetFrameFromNameFn(m_pVehicle->m_pRpClump, "taillights")) {
        const RwMatrix& m = frMid->ltm;
        R.x = m.right.x; R.y = m.right.y; R.z = m.right.z;
        U.x = m.up.x;    U.y = m.up.y;    U.z = m.up.z;
        A.x = m.at.x;    A.y = m.at.y;    A.z = m.at.z;
        P.x = m.pos.x;   P.y = m.pos.y;   P.z = m.pos.z;
    } else {
        const CMatrixLink& vm = *m_pVehicle->mat;
        R.x = vm.right.x; R.y = vm.right.y; R.z = vm.right.z;
        U.x = vm.up.x;    U.y = vm.up.y;    U.z = vm.up.z;
        A.x = vm.at.x;    A.y = vm.at.y;    A.z = vm.at.z;
        P.x = vm.pos.x;   P.y = vm.pos.y;   P.z = vm.pos.z + 0.85f; // уровень крыши
    }

    // базовая точка балки (МИР)
    const CVector baseW(
            P.x + U.x * (offsetUp + extraLift - lowerDZ) + A.x * backShift,
            P.y + U.y * (offsetUp + extraLift - lowerDZ) + A.y * backShift,
            P.z + U.z * (offsetUp + extraLift - lowerDZ) + A.z * backShift
    );
    const CVector leftCenterW  = baseW - R * pairHalfSpan; // левая (красная)
    const CVector rightCenterW = baseW + R * pairHalfSpan; // правая (синяя)

    // локальные координаты для корон
    auto W2L = [&](const CVector& w)->CVector {
        return WorldToVehicleLocal(m_pVehicle->mat, w);
    };
    const CVector leftCenterL  = W2L(leftCenterW);
    const CVector rightCenterL = W2L(rightCenterW);

    // локальные оси (нормированные)
    const CVector R_tipL = W2L(baseW + R);
    const CVector A_tipL = W2L(baseW + A);
    const CVector baseL  = W2L(baseW);
    CVector R_loc; R_loc.x = R_tipL.x - baseL.x; R_loc.y = R_tipL.y - baseL.y; R_loc.z = R_tipL.z - baseL.z;
    CVector A_loc; A_loc.x = A_tipL.x - baseL.x; A_loc.y = A_tipL.y - baseL.y; A_loc.z = A_tipL.z - baseL.z;
    auto norm3 = [](CVector& v){
        float l = std::max(0.0001f, sqrtf(v.x*v.x + v.y*v.y + v.z*v.z));
        v.x/=l; v.y/=l; v.z/=l;
    };
    norm3(R_loc); norm3(A_loc);

    // ===== УТИЛИТЫ КОРОН =====
    const int baseId = (int)((uintptr_t)m_pVehicle & 0x00FFFFFF);
    auto mixA = [&](int a, float k=1.0f)->int {
        float v = std::clamp(a * k, 0.0f, 255.0f);
        return (int)v;
    };

    auto regCoronaDoubleSided = [&](int id, const CVector& lpos, int r,int g,int b,int a, float size, float kPulse){
        CVector fwd(lpos.x + A_loc.x * faceOffset,
                    lpos.y + A_loc.y * faceOffset,
                    lpos.z + A_loc.z * faceOffset);
        CVector back(lpos.x - A_loc.x * faceOffset,
                     lpos.y - A_loc.y * faceOffset,
                     lpos.z - A_loc.z * faceOffset);

        CCoronas::RegisterCorona(baseId + id,     m_pVehicle, r,g,b, mixA(a, kPulse), &fwd,  size, drawDist,
                                 eCoronaType::CORONATYPE_HEADLIGHT, eCoronaFlareType::FLARETYPE_NONE,
                                 0,0,0,0.0f,0,0.0f,0, 80.0f,0,0);
        CCoronas::RegisterCorona(baseId + id + 1, m_pVehicle, r,g,b, mixA(a, kPulse), &back, size, drawDist,
                                 eCoronaType::CORONATYPE_HEADLIGHT, eCoronaFlareType::FLARETYPE_NONE,
                                 0,0,0,0.0f,0,0.0f,0, 80.0f,0,0);
    };

    auto regLED = [&](int id, const CVector& lpos,
                      int rC,int gC,int bC,int aC,
                      int rH,int gH,int bH,int aH,
                      float kPulse)
    {
        regCoronaDoubleSided(id,     lpos, rC,gC,bC, aC,                sizeCore, kPulse);
        regCoronaDoubleSided(id + 2, lpos, rH,gH,bH, (int)(aH * 0.92f), sizeHalo, kPulse);
    };

    auto drawLens = [&](const CVector& centerL, int r,int g,int b, int idStart, float kPulse){
        for (int i=0;i<lensSegs;++i){
            float t = (float(i)/(lensSegs-1) - 0.5f)*2.0f; // -1..+1
            CVector p(centerL.x + R_loc.x*(t*lensHalfLen),
                      centerL.y + R_loc.y*(t*lensHalfLen),
                      centerL.z + R_loc.z*(t*lensHalfLen));
            float sz = lensHeight * (0.85f + 0.15f * (1.0f - std::fabs(t)));
            int   aa = (int)(lensAlpha * (0.75f + 0.25f * (1.0f - std::fabs(t))));
            regCoronaDoubleSided(idStart + i*4, p, r,g,b, aa, sz, kPulse);
        }
        CVector capL(centerL.x - R_loc.x*(lensHalfLen+0.03f),
                     centerL.y - R_loc.y*(lensHalfLen+0.03f),
                     centerL.z - R_loc.z*(lensHalfLen+0.03f));
        CVector capR(centerL.x + R_loc.x*(lensHalfLen+0.03f),
                     centerL.y + R_loc.y*(lensHalfLen+0.03f),
                     centerL.z + R_loc.z*(lensHalfLen+0.03f));
        regCoronaDoubleSided(idStart + 200, capL, r,g,b, lensCapA, lensHeight*0.85f, kPulse);
        regCoronaDoubleSided(idStart + 204, capR, r,g,b, lensCapA, lensHeight*0.85f, kPulse);
    };

    // ===== ЗЕМЛЯ: ЕДИНЫЙ RGB ГЛОУ ПОД АВТО =====
    auto estimateGroundZ = [&](float, float)->float {
        float zMin = 1e9f; int n=0;
        const char* wheels[4] = {"wheel_lf_dummy","wheel_rf_dummy","wheel_rb_dummy","wheel_lb_dummy"};
        for (int i=0;i<4;i++){
#if !VER_x32
            if (RwFrame* f = reinterpret_cast<RwFrame* (*)(RpClump*, const char*)>(g_libGTASA + 0x0045BE78)(m_pVehicle->m_pRpClump, wheels[i]))
#else
            if (RwFrame* f = reinterpret_cast<RwFrame* (*)(RpClump*, const char*)>(g_libGTASA + 0x003856F4 + 1)(m_pVehicle->m_pRpClump, wheels[i]))
#endif
            { zMin = std::min(zMin, f->ltm.pos.z); n++; }
        }
        if (n>0) return zMin - 0.06f;
        return m_pVehicle->mat->pos.z - 0.5f;
    };

    CVector midBarW;
    midBarW.x = (leftCenterW.x + rightCenterW.x) * 0.5f;
    midBarW.y = (leftCenterW.y + rightCenterW.y) * 0.5f;
    midBarW.z = (leftCenterW.z + rightCenterW.z) * 0.5f;

    const float gZ = estimateGroundZ(midBarW.x, midBarW.y) + 0.010f;
    CVector centerUnderW;
    centerUnderW.x = midBarW.x + A.x * (0.35f * UNDERGLOW_PUSH);
    centerUnderW.y = midBarW.y + A.y * (0.35f * UNDERGLOW_PUSH);
    centerUnderW.z = gZ;

    auto StoreRGBGlow = [&](const CVector& cW, float maj, float min, float kPulse, int idSalt){
        if (!gpShadowExplosionTex) return;

        const float fx = min * 0.6f;
        const float fy = 0.0f;
        const float sx = 0.0f;
        const float sy = -maj * 0.6f;

        const float dom = 0.85f * kPulse;
        const float sub = 0.55f * kPulse;
        const float wh  = 0.35f * kPulse;

        uint8 rMain = st_right ? (uint8)(sub*255.0f) : (uint8)(dom*255.0f);
        uint8 gMain = st_right ? (uint8)(sub*110.0f) : (uint8)(dom* 70.0f);
        uint8 bMain = st_right ? (uint8)(dom*255.0f) : (uint8)(sub* 70.0f);

        CShadows__StoreCarLightShadow(
                m_pVehicle,
                ((int)((uintptr_t)m_pVehicle & 0x00FFFFFF) ^ (0x710000 + idSalt)),
                gpShadowExplosionTex,
                const_cast<CVector*>(&cW),
                fx * UNDERGLOW_SCALE, fy,
                sx, sy * UNDERGLOW_SCALE,
                rMain, gMain, bMain,
                10.0f
        );

        CShadows__StoreCarLightShadow(
                m_pVehicle,
                ((int)((uintptr_t)m_pVehicle & 0x00FFFFFF) ^ (0x720000 + idSalt)),
                gpShadowExplosionTex,
                const_cast<CVector*>(&cW),
                fx * UNDERGLOW_SCALE * 0.75f, fy,
                sx, sy * UNDERGLOW_SCALE * 0.75f,
                (uint8)std::min(255, (int)(rMain*0.9f)),
                (uint8)std::min(255, (int)(gMain*0.9f)),
                (uint8)std::min(255, (int)(bMain*0.9f)),
                9.0f
        );

        CShadows__StoreCarLightShadow(
                m_pVehicle,
                ((int)((uintptr_t)m_pVehicle & 0x00FFFFFF) ^ (0x730000 + idSalt)),
                gpShadowExplosionTex,
                const_cast<CVector*>(&cW),
                fx * UNDERGLOW_SCALE * 0.35f, 0.0f,
                0.0f, -sy * UNDERGLOW_SCALE * 0.35f,
                (uint8)(wh*255.0f), (uint8)(wh*255.0f), (uint8)(wh*255.0f),
                8.0f
        );
    };

    auto drawPodVisual = [&](bool isRight, int idStart, const CVector& centerL){
        if (isRight) drawLens(centerL,  60,125,255, idStart + 3000, pulse);
        else         drawLens(centerL, 255, 60, 60, idStart + 2000, pulse);

        const float lead[3] = { 1.00f, 0.95f, 0.98f };
        for (int i=0;i<3;++i) {
            CVector p;
            p.x = centerL.x + R_loc.x * ledDX[i];
            p.y = centerL.y + R_loc.y * ledDX[i];
            p.z = centerL.z + R_loc.z * ledDX[i] - ledDZ[i];
            const float k = pulse * lead[i];
            if (isRight) {
                regLED(idStart + 100 + i*8, p,  blueCoreR,blueCoreG,blueCoreB,blueCoreA,
                       blueHaloR,blueHaloG,blueHaloB,blueHaloA, k);
            } else {
                regLED(idStart +   i*8,     p,  redCoreR, redCoreG, redCoreB, redCoreA,
                       redHaloR, redHaloG, redHaloB, redHaloA,  k);
            }
        }
    };

    if (st_right) drawPodVisual(true,  1000, rightCenterL);
    else          drawPodVisual(false,  900, leftCenterL);

    // ===== ЕДИНЫЙ RGB-АНДЕРГЛОУ ПОД АВТО =====
    const float ugMajor = (1.80f + UNDERGLOW_PUSH) * UNDERGLOW_SCALE; // поперёк кузова
    const float ugMinor = (2.60f + UNDERGLOW_PUSH) * UNDERGLOW_SCALE; // вдоль кузова
    StoreRGBGlow(centerUnderW, ugMinor, ugMajor, pulse, 555);
}

void CVehicle::ProcessHeadlightStrobes()
{
    if (!m_pVehicle || !m_bSirenEnabled) return;
    if (!m_pVehicle) return;
    if (!SpecialsShouldRenderFor(this)) return;
    // тайминг — 3 коротких вспышки на стороне, затем свап стороны
    struct Pat { uint32_t onMS, offMS, gapMS; int flashes; };
    static const Pat k = { 42u, 42u, 120u, 3 };

    const uint32_t now = GetTickCount();
    static uint32_t t0 = now;
    static bool onPhase = true;
    static int  flashIdx = 0;
    static bool rightSide = false; // false=левая, true=правая

    const uint32_t phaseLen = onPhase ? k.onMS : (flashIdx + 1 < k.flashes ? k.offMS : k.gapMS);
    if (now - t0 >= phaseLen) {
        t0 = now;
        if (onPhase) {
            onPhase = false;
        } else {
            if (++flashIdx >= k.flashes) { flashIdx = 0; rightSide = !rightSide; }
            onPhase = true;
        }
    }

    auto easeOut = [](float t)->float { return 1.0f - (1.0f - t)*(1.0f - t); };
    const float tprog = phaseLen ? std::min(1.0f, float(now - t0 + (onPhase?0:phaseLen)) / float(phaseLen)) : 1.0f;
    const float pulse = onPhase ? easeOut(tprog) : 0.0f;
    if (pulse <= 0.0f) return;

    // дамми фронт. фонарей (как в поворотниках)
    auto *pModelInfo = static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(m_pVehicle->nModelIndex));
    if (!pModelInfo || !pModelInfo->m_pVehicleStruct) return;
    CVector* d = pModelInfo->m_pVehicleStruct->m_avDummyPos;

    // d[0] — передняя фара (ваш же индекc в коде поворотников)
    CVector L = d[0];
    CVector R = d[0];
    L.x = -fabsf(L.x); // левая
    R.x = +fabsf(R.x); // правая

    // слегка вперёд и выше, чтобы «светило» из фары
    const float liftZ  = 0.03f;
    const float pushY  = 0.06f;
    L.y += pushY; L.z += liftZ;
    R.y += pushY; R.z += liftZ;

    // параметры корон (чёткая «вспышка» + мягкий ореол)
    const float sizeCore = 0.55f;
    const float sizeHalo = 0.95f;
    const float drawDist = 360.0f;

    auto mixA = [&](int a)->int {
        float v = std::clamp(a * pulse, 0.0f, 255.0f);
        return (int)v;
    };

    // белая вспышка + лёгкий «ксеноновый» оттенок
    const int  wR=255, wG=255, wB=245, wA=255;  // ядро
    const int  hR=200, hG=220, hB=255, hA=190;  // ореол
    const int  xR=160, xG=200, xB=255, xA=110;  // синеватая добавка

    const int base = (int)((uintptr_t)m_pVehicle & 0x00FFFFFF);

    auto emitOne = [&](int id, const CVector& lp, bool blueTint){
        // ядро
        CCoronas::RegisterCorona(base + id, m_pVehicle,
                                 wR, wG, wB, mixA(wA),
                                 &lp, sizeCore, drawDist,
                                 eCoronaType::CORONATYPE_HEADLIGHT,
                                 eCoronaFlareType::FLARETYPE_NONE,
                                 0,0,0,0.0f,0,0.0f,0, 80.0f,0,0);
        // ореол
        CCoronas::RegisterCorona(base + id + 1, m_pVehicle,
                                 hR, hG, hB, mixA(hA),
                                 &lp, sizeHalo, drawDist,
                                 eCoronaType::CORONATYPE_HEADLIGHT,
                                 eCoronaFlareType::FLARETYPE_NONE,
                                 0,0,0,0.0f,0,0.0f,0, 80.0f,0,0);
        // лёгкая синяя домишка
        if (blueTint) {
            CCoronas::RegisterCorona(base + id + 2, m_pVehicle,
                                     xR, xG, xB, mixA(xA),
                                     &lp, sizeHalo * 1.08f, drawDist,
                                     eCoronaType::CORONATYPE_HEADLIGHT,
                                     eCoronaFlareType::FLARETYPE_NONE,
                                     0,0,0,0.0f,0,0.0f,0, 80.0f,0,0);
        }
    };

    // двусторонний дубль по продольной оси (видно под углами)
    const float faceOffset = 0.020f; // 2см
    auto emitDoubleSided = [&](int baseId, const CVector& lp, bool blueTint){
        CVector fwd = lp;  fwd.y  += faceOffset;
        CVector back= lp;  back.y -= faceOffset;
        emitOne(baseId    , fwd,  blueTint);
        emitOne(baseId+10 , back, blueTint);
    };

    if (rightSide) {
        emitDoubleSided(0x3000, R, true);
    } else {
        emitDoubleSided(0x3020, L, true);
    }
}

void CVehicle::StartSirenSound() {
    if (m_hSiren) return;
    if (!m_pVehicle) return;

    char path[512];
    snprintf(path, sizeof(path), "%saudio/siren_loop.mp3", g_pszStorage);

    m_hSiren = BASS_StreamCreateFile(
            FALSE, path, 0, 0,
            BASS_SAMPLE_LOOP | BASS_SAMPLE_3D // важно: 3D + loop
    );
    if (!m_hSiren) return;

    // громкость и 3D аттенюация
    BASS_ChannelSetAttribute(m_hSiren, BASS_ATTRIB_VOL, 0.85f);

    // глобальные 3D параметры движка (однократно где-то при инициализации аудио):
    // BASS_Set3DFactors(/*distf=*/1.0f, /*rolloff=*/1.0f, /*doppler=*/1.0f);

    // старт
    UpdateSiren3D();            // начальная позиция
    BASS_ChannelPlay(m_hSiren, FALSE);
}

void CVehicle::StopSirenSound() {
    if (!m_hSiren) return;
    BASS_ChannelStop(m_hSiren);
    BASS_StreamFree(m_hSiren);
    m_hSiren = 0;
}

void CVehicle::UpdateSiren3D() {
    if (!m_hSiren || !m_pVehicle) return;

    const CVector& p = m_pVehicle->mat->pos;

    BASS_3DVECTOR pos(p.x, p.y, p.z);
    // скорость/направление необязательно — BASS сам посчитает доплер, если зададать vel/orient
    BASS_ChannelSet3DPosition(m_hSiren, &pos, nullptr, nullptr);

    // позиция слушателя — там, где камера.
    // В твоём месте, где обновляешь камеру, один раз за кадр:
    // BASS_Set3DPosition(&listenerPos, &listenerVel, &front, &top);

    BASS_Apply3D();
}

void CVehicle::ProcessShadowLight(CVehicleGta *m_pVehicle, CVector coords, int IsRight, int id){
    if(!gpShadowHeadLightsTex2) return;
    Log("HUI V SRAKU");
    float v4; // r6
    float v5; // r4
    double v7; // d8
    CMatrix *v8; // r5
    float v9; // r0
    float v10; // s2
    float v11; // s6
    float v12; // s4
    float v13; // s0
    float v14; // s5
    float v15; // s20
    float v16; // s18
    float v17; // s8
    float v18; // s12
    float v19; // s2
    float v20; // s2
    CVector v21; // [sp+38h] [bp-48h] BYREF
    CVector pCoors; // [sp+44h] [bp-3Ch] BYREF

    v4 = coords.z;
    v5 = coords.x;
    v7 = coords.y * 1.3;
    v8 = m_pVehicle->mat;
    v9 = v8->up.Magnitude2D();
    v10 = v7;
    v11 = v8->up.z;
    v12 = v8->up.y;
    v13 = v8->up.x;
    v14 = 0.0;
    if ( v9 != 0.0 )
        v14 = 1.0 / v9;
    v15 = v12 * v14;
    v16 = v13 * v14;
    v17 = (float)((float)((float)(v8->right.x * v5) + (float)(v13 * v10)) + (float)(v8->at.x * v4)) + v8->pos.x;
    v18 = (float)((float)((float)(v11 * v10) + (float)(v8->right.z * v5)) + (float)(v8->at.z * v4)) + v8->pos.z;
    pCoors.y = (float)((float)((float)(v8->right.y * v5) + (float)(v12 * v10)) + (float)(v8->at.y * v4)) + v8->pos.y;
    pCoors.x = v17;
    pCoors.z = v18;
    if ( !IsRight )
    {
        v19 = v10 + v10;
        v21.z = v19 * v11;
        v21.y = v19 * v12;
        v21.x = v19 * v13;
        pCoors.operator-=(&v21);
        v15 = -v15;
        v16 = -v16;
    }
    v20 = fabsf(v5);
    CShadows::StoreStaticShadow(
            (uintptr_t )m_pVehicle + id + IsRight + 0x18,
            eShadowType::SHADOW_ADDITIVE,
            gpShadowHeadLightsTex2,
            &pCoors,
            v20 * (float)(v16 * 0.7),
            v20 * (float)(v15 * 0.7),
            v20 * v15,
            -(float)(v16 * v20),
            50,
            127,
            90,
            0,
            6.0,
            1.0,
            70.0,
            0,
            0.05);//Marginvehicle
}