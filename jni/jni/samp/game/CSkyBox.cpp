

// -- INCLUDE`S
#include "CSkyBox.h"

#include "../game/game.h"


#include "../net/netgame.h"
#include "../gui/gui.h"
#include "CSettings.h"
#include "game/Models/ModelInfo.h"


// -- EXTERN`S
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;


// -- VARIABLE`S
CObject* CSkyBox::m_pSkyObject = nullptr;
RwTexture* pSkyTexture = nullptr;

RwTexture* Day = nullptr;
RwTexture* Night = nullptr;
RwTexture* morning = nullptr;
RwTexture* sunset = nullptr;

const char *CSkyBox::m_TextureName = nullptr;



bool CSkyBox::m_bNeedRender = true;
float CSkyBox::m_fRotSpeed = 0.005f;

uint8_t pChangeTime;
// -- METHOD`S
CMatrix* RwMatrixMultiplyByVector(CVector* out, CMatrix* a2, CVector* in);
bool end = true;
void CSkyBox::Initialise()
{
    if(!end) return;

    //auto* dwModelArray = (uintptr_t*)(g_libGTASA + 0x87BF48);
    //if (!dwModelArray[17364])
    //   return;
    auto dwModelInfo = CModelInfo::ms_modelInfoPtrs[17364];
    if (!dwModelInfo) {
        end = false;
        Log("SkyBox not model!");
        return;
    }
    m_pSkyObject = CreateObjectScaled(17364, 0.7f);

}

void CSkyBox::Process() {


    if (!m_pSkyObject)
        Initialise();

    if (m_pSkyObject) {
        CAMERA_AIM *aim = GameGetInternalAim();
        RwMatrix matrix;

        m_pSkyObject->GetMatrix(&matrix);

        matrix.pos.x = aim->pos1x;
        matrix.pos.y = aim->pos1y;
        matrix.pos.z = aim->pos1z;

        //RwMatrixRotate(&matrix, eAxis::Z, m_fRotSpeed);
        CVector a2a;
        a2a.z = 1.0;
        a2a.x = 0.0;
        a2a.y = 0.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&matrix, &a2a, m_fRotSpeed, 1);
        m_bNeedRender = true;

        ReTexture();

        m_pSkyObject->SetMatrix(matrix);
        m_pSkyObject->UpdateMatrix(matrix);
        m_pSkyObject->Render();

        m_bNeedRender = false;
    }
}

CObject* CSkyBox::CreateObjectScaled(int iModel, float fScale)
{
    auto *vecRot = new CVector();
    auto *vecScale = new CVector(fScale);

    if (!pNetGame)
        return nullptr;

    if(!pNetGame->GetObjectPool())
        return nullptr;

    auto *object = pGame->NewObject(iModel, 0.0f, 0.0f, 0.0f, vecRot, 0.0f);

    if(!object) return nullptr;
    Log("%d model", object->m_pEntity->nModelIndex);

#if VER_x32
    *(uint32_t*)((uintptr_t)object->m_pEntity + 28) &= 0xFFFFFFFE;
    *(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;
#else
    *(uint32_t*)((uintptr_t)object->m_pEntity + 40) &= 0xFFFFFFFE;
#endif
    Log("sert");
    //*(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;

    object->RemovePhysical();

    RwMatrix matrix;
    object->GetMatrix(&matrix);

    RwMatrixScale(&matrix, reinterpret_cast<CVector *>(vecScale));

    object->SetMatrix(matrix);
    object->UpdateRwMatrixAndFrame();
    object->AddPhysical();
#if VER_x32
    *(uint32_t*)((uintptr_t)object->m_pEntity + 28) &= 0xFFFFFFFE;
    *(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;
#else
    *(uint32_t*)((uintptr_t)object->m_pEntity + 40) &= 0xFFFFFFFE;
#endif
    Log("SkyBox Create : %d", iModel);
    return object;
}

void CSkyBox::ReTexture()
{
    int iHours = pNetGame->m_byteWorldTime;

    // -- TODO - Accidentally from several
    if (pChangeTime != iHours) {
        pChangeTime = iHours;


        if ( iHours - 0x13 > 0xFFFFFFF2 )
        {
            if ( iHours > 7 )
            {
                if ( iHours > 10 )
                {
                    if ( iHours > 15 )
                    {
                        if ( iHours == 17 )
                        {
                            SetTexture("skybox_6");
                        }
                        else if ( iHours == 16 )
                        {
                            SetTexture("skybox_5");
                        }
                        else
                        {
                            SetTexture("skybox_7");
                        }
                    }
                    else
                    {
                        SetTexture("skybox_4");
                    }
                }
                else
                {
                    SetTexture("skybox_3");
                }
            }
            else
            {
                SetTexture("skybox_2");
            }
        }
        else
        {
            SetTexture("skybox_1");
        }
        // ---
    }
    uintptr_t pAtomic = reinterpret_cast<uintptr_t>(m_pSkyObject->m_pEntity->m_pRwObject);
    if (!pAtomic)
        return;

    if (!*(uintptr_t*)(pAtomic + (VER_x32 ? 4 : 8)))
        return;

    ((void(*)())(g_libGTASA + (VER_x32 ? 0x5D1F98 + 1 : 0x6F6664)))(); // -- DeActivateDirectional
    ((void*(*)())(g_libGTASA + (VER_x32 ? 0x5D204C + 1 : 0x6F6720)))(); // -- SetFullAmbient
    ((void(*)())(g_libGTASA + (VER_x32 ? 0x5D2068 + 1 : 0x6F6738)))(); // -- SetAmbientColours


#if VER_x32
    *(uintptr_t *)(g_libGTASA + 0x6B7094) |= 0x1000000;
    *(uintptr_t *)(g_libGTASA + 0x6B7094) |= 0x08;
    ((uintptr_t(*)(uintptr_t, uintptr_t, CObject*))(g_libGTASA + 0x1D8858 + 1))(*(uintptr_t*)(pAtomic + 4), (uintptr_t)RwFrameForAllObjectsCallback, m_pSkyObject); // RwFrameForAllObjects
    *(uintptr_t *)(g_libGTASA + 0x6B7094) &= ~0x1000000;
    *(uintptr_t *)(g_libGTASA + 0x6B7094) &= ~0x08;
#else
    ((uintptr_t(*)(uintptr_t, uintptr_t, CObject*))(g_libGTASA + 0x2703BC))(*(uintptr_t*)(pAtomic + 8), (uintptr_t)RwFrameForAllObjectsCallback, m_pSkyObject); // RwFrameForAllObjects
#endif
}

uintptr_t CSkyBox::RwFrameForAllObjectsCallback(uintptr_t object, CObject* pObject)
{
    if (*(uint8_t*)object != 1)
        return object;

    uintptr_t pAtomic = object;
    RpGeometry* pGeom = *(RpGeometry * *)(pAtomic + (VER_x32 ? 24 : 48));
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

        if (pSkyTexture) {
            pMaterial->texture = pSkyTexture;
        }
    }

    return object;
}

void CSkyBox::SetTexture(const char *texName)
{
    if (texName == nullptr)
        return;
    Log("SkyBox: %s", texName);
    m_TextureName = texName;
    pSkyTexture = CUtil::LoadTextureFromDB("gta3", texName);
}
void CSkyBox::SetRotSpeed(float speed)
{
    m_fRotSpeed = speed;
}

bool CSkyBox::IsNeedRender()
{
    return m_bNeedRender;
}

CObject *CSkyBox::GetSkyObject()
{
    return m_pSkyObject;
}

