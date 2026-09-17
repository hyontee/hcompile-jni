// -- -- -- -- -- --
// Created by Loony-dev © 2021
// VK: https://vk.com/loonydev
// -- -- -- -

// -- INCLUDE`S
#include "Skybox.h"

#include "../game/game.h"
#include "../game/CVector.h"

#include "../net/netgame.h"
#include "../gui/gui.h"

#include "../CSettings.h"
#include "../chatwindow.h"

// -- EXTERN`S
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CSettings *pSettings;
extern CChatWindow *pChatWindow;

// -- VARIABLE`S
CObject* Skybox::m_pSkyObject = nullptr;
RwTexture* pSkyTexture = nullptr;

const char *Skybox::m_TextureName = nullptr;

bool Skybox::m_bNeedRender = true;
float Skybox::m_fRotSpeed = 0.005f;

uint8_t pChangeTime;
// -- METHOD`S
MATRIX4X4* RwMatrixMultiplyByVector(VECTOR* out, MATRIX4X4* a2, VECTOR* in);

void Skybox::Initialise()
{
    auto* dwModelArray = (uintptr_t*)(SA_ADDR(0x87BF48));
    if (!dwModelArray[18659])
        return;

    m_pSkyObject = CreateObjectScaled(18659, 1.0f);

    SetTexture("daily_sky_1");
}

void Skybox::Process()
{
    if (!m_pSkyObject)
        Initialise();

    if (m_pSkyObject)
    {
        CAMERA_AIM *aim = GameGetInternalAim();
        MATRIX4X4 matrix;

        m_pSkyObject->GetMatrix(&matrix);

        matrix.pos.X = aim->pos1x;
        matrix.pos.Y = aim->pos1y;
        matrix.pos.Z = aim->pos1z;

        RwMatrixRotate(&matrix, eAxis::Z, m_fRotSpeed);

        m_bNeedRender = true;

        ReTexture();

        m_pSkyObject->SetMatrix(matrix);
        m_pSkyObject->UpdateMatrix(matrix);
        m_pSkyObject->Render();

        m_bNeedRender = false;
    }
}

CObject* Skybox::CreateObjectScaled(int iModel, float fScale)
{
    auto *vecRot = new CVector();
    auto *vecScale = new CVector(fScale);

    if (!pNetGame)
        return nullptr;

    if(!pNetGame->GetObjectPool())
        return nullptr;

    auto *object = pGame->NewObject(iModel, 0.0f, 0.0f, 0.0f, vecRot->Get(), 0.0f);

    *(uint32_t*)((uintptr_t)object->m_pEntity + 28) &= 0xFFFFFFFE;
    *(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;

    object->RemovePhysical();

    MATRIX4X4 matrix;
    object->GetMatrix(&matrix);

    RwMatrixScale(&matrix, vecScale);

    object->SetMatrix(matrix);
    object->UpdateRwMatrixAndFrame();

    *(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;
    object->AddPhysical();

    return object;
}

void Skybox::ReTexture()
{
    int iHours = pNetGame->m_byteWorldTime;

    if (pChangeTime != iHours)
    {
        if (iHours >= 6 && iHours <= 11) SetTexture("afternoon_sky_1");
        if (iHours >= 12 && iHours <= 17) SetTexture("daily_sky_1");
        if (iHours >= 18 && iHours <= 22) SetTexture("evening_sky_1");
        if (iHours >= 0 && iHours <= 5) SetTexture("night_sky_1");
        if (iHours >= 23 && iHours <= 24) SetTexture("night_sky_1");
    }

    uintptr_t pAtomic = m_pSkyObject->m_pEntity->m_RwObject;
    if (!pAtomic)
        return;

    if (!*(uintptr_t*)(pAtomic + 4))
        return;

    ((void(*)())(SA_ADDR(0x559EF8 + 1)))(); // -- DeActivateDirectional
    ((void*(*)())(SA_ADDR(0x559FC8 + 1)))(); // -- SetFullAmbient
    ((void(*)())(SA_ADDR(0x559FEC + 1)))(); // -- SetAmbientColours
    ((uintptr_t(*)(uintptr_t, uintptr_t, CObject*))(SA_ADDR(0x1AEE2C + 1)))(*(uintptr_t*)(pAtomic + 4), (uintptr_t)RwFrameForAllObjectsCallback, m_pSkyObject); // RwFrameForAllObjects
}

uintptr_t Skybox::RwFrameForAllObjectsCallback(uintptr_t object, CObject* pObject)
{
    if (*(uint8_t*)object != 1)
        return object;

    uintptr_t pAtomic = object;
    RpGeometry* pGeom = *(RpGeometry * *)(pAtomic + 24);
    if (!pGeom)
        return object;

    int numMats = pGeom->matList.numMaterials;
    if (numMats > 16)
        numMats = 16;

    for (int i = 0; i < numMats; i++)
    {
        RpMaterial* pMaterial = pGeom->matList.materials[i];
        if (!pMaterial)
            continue;

        if (pSkyTexture)
            pMaterial->texture = pSkyTexture;
    }

    return object;
}

void Skybox::SetTexture(const char *texName)
{
    if (texName == nullptr)
        return;

    m_TextureName = texName;
    pSkyTexture = (RwTexture*)LoadTextureFromDB("samp", texName);
}

void Skybox::SetRotSpeed(float speed)
{
    m_fRotSpeed = speed;
}

bool Skybox::IsNeedRender()
{
    return m_bNeedRender;
}

CObject *Skybox::GetSkyObject()
{
    return m_pSkyObject;
}

