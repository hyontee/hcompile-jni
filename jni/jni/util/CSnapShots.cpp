//
// Created by Error on 18.10.2024.
//

#include "CSnapShots.h"
#include "net/netgame.h"

CRenderTarget* CSnapShots::m_pRenderTarget = nullptr;
extern CJavaWrapper* g_pJavaWrapper;
extern CGame *pGame;
extern CNetGame *pNetGame;
RwTexture* CSnapShots::CreatePedSnapShot(int iModel, uint32_t dwColor, VECTOR *vecRot, float fZoom) {
    Log("CreatePedSnapShot: %d, %f, %f, %f", iModel, vecRot->X, vecRot->Y, vecRot->Z);
    if(!IsPedModel(iModel)) {
        Log("Invalid skin model: %d", iModel);
        return nullptr;
    }

    CPlayerPed *pPed = new CPlayerPed(208, 0, 0.0f, 0.0f, 0.0f, 0.0f);

    float posZ = iModel == 162 ? 50.15f : 50.05f;
    float posY = fZoom * -2.25f;
    pPed->TeleportTo(0.0f, posY, posZ);
    pPed->SetModelIndex(iModel);
    pPed->SetGravityProcessing(false);
    pPed->SetCollisionChecking(false);

    MATRIX4X4 mat;
    pPed->GetMatrix(&mat);

    if(vecRot->X != 0.0f)
        RwMatrixRotate(&mat, eAxis::X, vecRot->X);
    if(vecRot->Y != 0.0f)
        RwMatrixRotate(&mat, eAxis::Y, vecRot->Y);
    if(vecRot->Z != 0.0f)
        RwMatrixRotate(&mat, eAxis::Z, vecRot->Z);

    pPed->UpdateMatrix(mat);



    pPed->Add();

    m_pRenderTarget->Begin();
    pPed->ClumpUpdateAnimations(100.0f, 1);
    pPed->Render();

    pPed->Remove();

    delete pPed;

    return m_pRenderTarget->End();
}

RwTexture *
CSnapShots::CreateVehicleSnapShot(int iModel, uint32_t dwColor, VECTOR *vecRot, float fZoom,
                                  int dwColor1, int dwColor2) {
    Log("CreateVehicleSnapShot: %d, %f, %f, %f", iModel, vecRot->X, vecRot->Y, vecRot->Z);

   

    CVehicle *pVehicle = new CVehicle(iModel, 0.0f, 0.0f, 50.0f, 0.0f);
	
    if(!pVehicle || !pVehicle->m_pVehicle) {
        delete pVehicle;
        return nullptr;
    }


    pVehicle->SetGravityProcessing(false);
    pVehicle->SetCollisionChecking(true);

    float fRadius = GetModelColSphereRadius(iModel);
    float posY = (-1.0 - (fRadius + fRadius)) * fZoom;

    if(pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT)
    {
        posY = -5.5 - fRadius * 2.0;
    }

    pVehicle->TeleportTo(0.0f, posY, 50.0f);

    if(dwColor1 != 0xFFFF && dwColor2 != 0xFFFF)
        pVehicle->SetColor(dwColor1, dwColor2);

    MATRIX4X4 mat;
    pVehicle->GetMatrix(&mat);

    if(vecRot->X != 0.0f)
        RwMatrixRotate(&mat, eAxis::X, vecRot->X);
    if(vecRot->Y != 0.0f)
        RwMatrixRotate(&mat, eAxis::Y, vecRot->Y);
    if(vecRot->Z != 0.0f)
        RwMatrixRotate(&mat, eAxis::Z, vecRot->Z);

    pVehicle->UpdateMatrix(mat);



    pVehicle->Add();
    m_pRenderTarget->Begin();
    pVehicle->Render();

    pVehicle->Remove();
    delete pVehicle;

    return m_pRenderTarget->End();
}



void CSnapShots::Initialise() {
	
   // m_pRenderTarget = new CRenderTarget(512, 512, false, 0xC69C7B);
	
	
}

RwTexture *
CSnapShots::CreateObjectSnapShot(int iModel, uint32_t dwColor, VECTOR *vecRot, float fZoom) {
    if (iModel == 1373 || iModel == 3118 || iModel == 3552 || iModel == 3553)
        iModel = 18631;
Log("CreateObjectSnapShot: %d, %f, %f, %f", iModel, vecRot->X, vecRot->Y, vecRot->Z);
    bool bNeedRemoveModel = false;
    if (!pGame->IsModelLoaded(iModel))
    {
        pGame->RequestModel(iModel);
        pGame->LoadRequestedModels();
        while (!pGame->IsModelLoaded(iModel)) sleep(1);
        bNeedRemoveModel = true;
    }

    uintptr_t pRwObject = ModelInfoCreateInstance(iModel);

    float fRadius = GetModelColSphereRadius(iModel);

    VECTOR vecCenter = { 0.0f, 0.0f, 0.0f };
    GetModelColSphereVecCenter(iModel, &vecCenter);

    uintptr_t parent = *(uintptr_t*)(pRwObject + 4);

    if (parent == 0) return 0;
    // RwFrameTranslate
    float v[3] = {
            -vecCenter.X,
            (-0.1f - fRadius * 2.25f) * fZoom,
            50.0f - vecCenter.Z };
 //   ((void(*)(uintptr_t, float*, int))(SA_FUNC(0x1D8614)))(parent, v, 1);

    if (iModel == 18631)
    {
        // RwFrameRotate X
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 1.0f;
      //  ((void(*)(uintptr_t, float*, float, int))(SA_FUNC(0x1D8728)))(parent, v, 180.0f, 1);
    }
    else
    {
        if (vecRot->X != 0.0f)
        {
            // RwFrameRotate X
            v[0] = 1.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
          //  ((void(*)(uintptr_t, float*, float, int))(SA_FUNC(0x1D8728)))(parent, v, vecRot->X, 1);
        }

        if (vecRot->Y != 0.0f)
        {
            // RwFrameRotate Y
            v[0] = 0.0f;
            v[1] = 1.0f;
            v[2] = 0.0f;
           // ((void(*)(uintptr_t, float*, float, int))(SA_FUNC(0x1D8728)))(parent, v, vecRot->Y, 1);
        }

        if (vecRot->Z != 0.0f)
        {
            // RwFrameRotate Z
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 1.0f;
           // ((void(*)(uintptr_t, float*, float, int))(SA_FUNC(0x1D8728)))(parent, v, vecRot->Z, 1);
        }
    }

    m_pRenderTarget->Begin();

    RenderClumpOrAtomic(pRwObject);

    DestroyAtomicOrClump(pRwObject);

    if (bNeedRemoveModel) {
        pGame->RemoveModel(iModel, false);
    }

    return m_pRenderTarget->End();
}

RwTexture *CSnapShots::CreateTextureSnapShot(const char *tex) {
    CRGBA color_white;
    color_white.A = 255;
    color_white.B = 255;
    color_white.G = 255;
    color_white.R = 255;
    RwTexture* texture = (RwTexture*) LoadTextureFromDB("samp", tex);
    CSprite2d* m_pSprite2d = new CSprite2d();
    m_pSprite2d->m_pRwTexture = texture;

    m_pRenderTarget->Begin();

    m_pSprite2d->Draw(0,0,texture->raster->height, texture->raster->width, color_white);


    return m_pRenderTarget->End();
}

RwTexture *CSnapShots::CreatePlayerSnapShot(VECTOR *vecRot, float fZoom) {
    Log("CreatePlayerSnapShot: , %f, %f, %f", vecRot->X, vecRot->Y, vecRot->Z);

    CPlayerPed *pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();

    float posZ = pPed->GetModelIndex() == 162 ? 50.15f : 50.05f;
    float posY = fZoom * -2.25f;
    pPed->TeleportTo(0.0f, posY, posZ);

    pPed->SetGravityProcessing(false);
    pPed->SetCollisionChecking(false);

    MATRIX4X4 mat;
    pPed->GetMatrix(&mat);

    if(vecRot->X != 0.0f)
        RwMatrixRotate(&mat, eAxis::X, vecRot->X);
    if(vecRot->Y != 0.0f)
        RwMatrixRotate(&mat, eAxis::Y, vecRot->Y);
    if(vecRot->Z != 0.0f)
        RwMatrixRotate(&mat, eAxis::Z, vecRot->Z);

    pPed->UpdateMatrix(mat);



    pPed->Add();

    m_pRenderTarget->Begin();
    pPed->ClumpUpdateAnimations(100.0f, 1);
    pPed->Render();

    pPed->Remove();

    delete pPed;

    return m_pRenderTarget->End();
}
