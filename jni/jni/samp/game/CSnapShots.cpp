//
// Created by Error on 18.10.2024.
//

#include "CSnapShots.h"
#include "net/netgame.h"
#include "StreamingInfo.h"
#include "CRenderTarget.h"
#include "game/Models/ModelInfo.h"

extern CJavaWrapper* g_pJavaWrapper;
extern CGame *pGame;
extern CNetGame *pNetGame;
RwTexture* CSnapShots::CreatePedSnapShot(int iModel, uint32_t dwColor, CVector *vecRot, float fZoom) {
    Log("CreatePedSnapShot: %d, %f, %f, %f", iModel, vecRot->x, vecRot->y, vecRot->z);

    CPlayerPed *pPed = new CPlayerPed(208, 0, 0.0f, 0.0f, 0.0f, 0.0f);

    float posZ = iModel == 162 ? 50.15f : 50.05f;
    float posY = fZoom * -2.25f;
    pPed->TeleportTo(0.0f, posY, posZ);
    pPed->SetModelIndex(iModel);
    pPed->SetGravityProcessing(false);
    pPed->SetCollisionChecking(false);

    RwMatrix mat;
    pPed->GetMatrix(&mat);

    if(vecRot->x != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::X, vecRot->x);
        CVector a2a;
        a2a.y = 0.0;
        a2a.z = 0.0;
        a2a.x = 1.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->x, 1);
    }
    if(vecRot->y != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::Y, vecRot->y);
        CVector a2a;
        a2a.z = 0.0;
        a2a.x = 0.0;
        a2a.y = 1.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->y, 1);
    }
    if(vecRot->z != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::Z, vecRot->z);
        CVector a2a;
        a2a.z = 1.0;
        a2a.x = 0.0;
        a2a.y = 0.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->z, 1);
    }

    pPed->UpdateMatrix(mat);



    pPed->Add();

    CRenderTarget::Begin(512, 512, (RwRGBA*)&dwColor,false);
    pPed->ClumpUpdateAnimations(100.0f, 1);
    pPed->Render();

    pPed->Remove();

    delete pPed;

    return CRenderTarget::End();
}

RwTexture *
CSnapShots::CreateVehicleSnapShot(int iModel, uint32_t dwColor, CVector *vecRot, float fZoom,
                                  int dwColor1, int dwColor2) {
    Log("CreateVehicleSnapShot: %d, %f, %f, %f", iModel, vecRot->x, vecRot->y, vecRot->z);



    //CVehicle *pVehicle = new CVehicle(iModel, 0.0f, 0.0f, 50.0f, 0.0f);
    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    NEW_VEHICLE NewVehicle;
    NewVehicle.VehicleID = MAX_VEHICLES - 1;
    NewVehicle.cColor1 = dwColor1;
    NewVehicle.cColor2 = dwColor2;
    NewVehicle.iVehicleType = iModel;
    NewVehicle.fHealth = 1300.0f;

    if (!pVehiclePool->New(&NewVehicle))
        return nullptr;
    CVehicle *pVehicle = pVehiclePool->GetAt(NewVehicle.VehicleID);
    pVehicle->SetGravityProcessing(false);
    pVehicle->SetCollisionChecking(true);

    //float fRadius = GetModelColSphereRadius(iModel);
    float fRadius = CModelInfo::GetModelInfo(iModel)->m_pColModel->GetBoundRadius();
    float posY = (-1.0 - (fRadius + fRadius)) * fZoom;

    if(pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT)
    {
        posY = -5.5 - fRadius * 2.5;
    }

    pVehicle->TeleportTo(0.0f, posY, 50.0f);

    if(dwColor1 != 0xFFFF && dwColor2 != 0xFFFF)
        pVehicle->SetColor(dwColor1, dwColor2);

    RwMatrix mat;
    pVehicle->GetMatrix(&mat);

    if(vecRot->x != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::X, vecRot->x);
        CVector a2a;
        a2a.y = 0.0;
        a2a.z = 0.0;
        a2a.x = 1.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->x, 1);
    }
    if(vecRot->y != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::Y, vecRot->y);
        CVector a2a;
        a2a.z = 0.0;
        a2a.x = 0.0;
        a2a.y = 1.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->y, 1);
    }
    if(vecRot->z != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::Z, vecRot->z);
        CVector a2a;
        a2a.z = 1.0;
        a2a.x = 0.0;
        a2a.y = 0.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->z, 1);
    }

    pVehicle->UpdateMatrix(mat);



    pVehicle->Add();
    CRenderTarget::Begin(512, 512, (RwRGBA*)&dwColor,false);
    pVehicle->Render();

    pVehicle->Remove();

    pVehiclePool->Delete(NewVehicle.VehicleID);

    return CRenderTarget::End();
}



void CSnapShots::Initialise() {
    //m_pRenderTarget = new CRenderTarget(512, 512, false, 000000);
}

RwTexture *
CSnapShots::CreateObjectSnapShot(int iModel, uint32_t dwColor, CVector *vecRot, CVector* offset) {
    if (iModel == 1373 || iModel == 3118 || iModel == 3552 || iModel == 3553)
        iModel = 18631;
    Log("CreateObjectSnapShot: %d, %f, %f, %f", iModel, vecRot->x, vecRot->y, vecRot->z);
    bool bNeedRemoveModel = false;
    if (!pGame->IsModelLoaded(iModel))
    {
        CStreaming::RequestModel(iModel, STREAMING_PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(false);
        while (!pGame->IsModelLoaded(iModel)) sleep(1);
        bNeedRemoveModel = true;
    }

    uintptr_t pRwObject = ModelInfoCreateInstance(iModel);

    float fRadius = GetModelColSphereRadius(iModel);

    CVector vecCenter = { 0.0f, 0.0f, 0.0f };
    GetModelColSphereVecCenter(iModel, &vecCenter);

    uintptr_t parent = *(uintptr_t*)(pRwObject + 4);

    if (parent == 0) return 0;
    // RwFrameTranslate
    float v[3] = {
            offset->x - vecCenter.x,
            ((fRadius * -2.25f) + -0.1f) + offset->y,
           (50.0f - vecCenter.z) - offset->z};
    ((void(*)(uintptr_t, float*, int))(g_libGTASA + (VER_x32 ? 0x1D8614 + 1 : 0x270060)))(parent, v, 1);

    if (iModel == 18631)
    {
        // RwFrameRotate X
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 1.0f;
        ((void(*)(uintptr_t, float*, float, int))(g_libGTASA + (VER_x32 ? 0x1D8728 + 1 : 0x270204)))(parent, v, 180.0f, 1);
    }
    else
    {
        if (vecRot->x != 0.0f)
        {
            // RwFrameRotate X
            v[0] = 1.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            ((void(*)(uintptr_t, float*, float, int))(g_libGTASA + (VER_x32 ? 0x1D8728 + 1 : 0x270204)))(parent, v, vecRot->x, 1);
        }

        if (vecRot->y != 0.0f)
        {
            // RwFrameRotate Y
            v[0] = 0.0f;
            v[1] = 1.0f;
            v[2] = 0.0f;
            ((void(*)(uintptr_t, float*, float, int))(g_libGTASA + (VER_x32 ? 0x1D8728 + 1 : 0x270204)))(parent, v, vecRot->y, 1);
        }

        if (vecRot->z != 0.0f)
        {
            // RwFrameRotate Z
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 1.0f;
            ((void(*)(uintptr_t, float*, float, int))(g_libGTASA + (VER_x32 ? 0x1D8728 + 1 : 0x270204)))(parent, v, vecRot->z, 1);
        }
    }

    CRenderTarget::Begin(512, 512, (RwRGBA*)&dwColor,false);

    RenderClumpOrAtomic(pRwObject);

    DestroyAtomicOrClump(pRwObject);

    if (bNeedRemoveModel) {
        //pGame->RemoveModel(iModel, false);
        CStreaming::RemoveModel(iModel);
    }

    return CRenderTarget::End();
}

RwTexture *CSnapShots::CreateTextureSnapShot(const char *tex) {
    CRGBA color_white;
    color_white.A = 255;
    color_white.B = 255;
    color_white.G = 255;
    color_white.R = 255;
    RwTexture* texture = (RwTexture*) CUtil::LoadTextureFromDB("txd", tex);
    CSprite2d* m_pSprite2d = new CSprite2d();
    m_pSprite2d->m_pRwTexture = texture;
    uint32_t dwColor = 000000;

    CRenderTarget::Begin(512, 512, (RwRGBA*)&dwColor, false);

    m_pSprite2d->Draw(0.0f,0.0f,512, 512, color_white);


    return CRenderTarget::End();
}

RwTexture *CSnapShots::CreatePlayerSnapShot(CVector *vecRot, float fZoom) {
    Log("CreatePlayerSnapShot: , %f, %f, %f", vecRot->x, vecRot->y, vecRot->z);

    //CPlayerPed *pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    CPlayerPed *pPed = new CPlayerPed(208, 0, 0.0f, 0.0f, 0.0f, 0.0f);
    //pPed->m_aAttachedObjects = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_aAttachedObjects;
    memcpy((void*)pPed->m_aAttachedObjects, (void*)pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_aAttachedObjects, sizeof(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_aAttachedObjects));
    uint32_t dwColor = 000000;
    float posZ = pPed->GetModelIndex() == 162 ? 50.15f : 50.05f;
    float posY = fZoom * -2.25f;
    pPed->TeleportTo(0.0f, posY, posZ);
    pPed->SetModelIndex(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_pPed->nModelIndex);
    pPed->SetGravityProcessing(false);
    pPed->SetCollisionChecking(false);

    RwMatrix mat;
    pPed->GetMatrix(&mat);

    if(vecRot->x != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::X, vecRot->x);
        CVector a2a;
        a2a.y = 0.0;
        a2a.z = 0.0;
        a2a.x = 1.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->x, 1);
    }
    if(vecRot->y != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::Y, vecRot->y);
        CVector a2a;
        a2a.z = 0.0;
        a2a.x = 0.0;
        a2a.y = 1.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->y, 1);
    }
    if(vecRot->z != 0.0f) {
        //RwMatrixRotate(&mat, eAxis::Z, vecRot->z);
        CVector a2a;
        a2a.z = 1.0;
        a2a.x = 0.0;
        a2a.y = 0.0;
        ((void (*)(RwMatrix*, CVector*, float, int))(g_libGTASA + (VER_x32 ? 0x001E38F4 + 1 : 0x27E710)))(&mat, &a2a, vecRot->z, 1);
    }

    pPed->UpdateMatrix(mat);



    pPed->Add();

    CRenderTarget::Begin(512, 512, (RwRGBA*)&dwColor,false);
    pPed->ClumpUpdateAnimations(100.0f, 1);
    pPed->Render();

    pPed->Remove();

    delete pPed;

    return CRenderTarget::End();
}
