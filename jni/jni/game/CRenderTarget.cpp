#include "CRenderTarget.h"

#include "../main.h"
#include "game.h"
#include "game/RW/rpworld.h"
#include "Scene.h"
struct RwTexture* CRenderTarget::m_pResultTexture;
bool CRenderTarget::m_bReady = false;
bool CRenderTarget::InitialiseScene()
{

    CRenderTarget::m_pLight = RpLightCreate(2);

    if (CRenderTarget::m_pLight == nullptr)
    {
        Log("CRenderTarget Light not created!");
        return false;
    }

    //RwRGBAReal rwColor;

    //RpLightSetColor(CRenderTarget::m_pLight, &rwColor);

    float rwColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    ((struct RpLight* (*)(struct RpLight*, float*))(g_libGTASA + (VER_x32 ? 0x216746 + 1 : 0x2BD930)))(m_pLight, rwColor);

    // RwCameraCreate
    CRenderTarget::m_pCamera = ((struct RwCamera* (*)())(g_libGTASA + (VER_x32 ? 0x001D5EE0 + 1 : 0x26D454)))();

    if (CRenderTarget::m_pCamera == nullptr)
    {
        Log("CRenderTarget Camera not created!");
        return false;
    }

    // RwFrameCreate
    CRenderTarget::m_pFrame = RwFrameCreate();

    if (CRenderTarget::m_pFrame == nullptr)
    {
        Log("CRenderTarget frame not created!");
        return false;
    }

    // RwFrameTranslate
    float v[3] = { 0.0f, 0.0f, 50.0f };
    ((RwFrame*(*)(struct RwFrame*, float*, int))(g_libGTASA + (VER_x32 ? 0x001D8614 + 1 : 0x270060)))(CRenderTarget::m_pFrame, v, 1);

    // RwFrameRotate
    v[0] = 1.0f; v[1] = 0.0f; v[2] = 0.0f;
    ((RwFrame*(*)(struct RwFrame*, float*, float, int))(g_libGTASA + (VER_x32 ? 0x001D8728 + 1 : 0x270204)))(CRenderTarget::m_pFrame, v, 90.0f, 1);


    // RwObjectHasFrameSetFrame
    ((void(*)(struct RwCamera*, struct RwFrame*))(g_libGTASA + (VER_x32 ? 0x001DCF64 + 1 : 0x275CF0)))(CRenderTarget::m_pCamera, CRenderTarget::m_pFrame);

    // RwCameraSetFarClipPlane
    ((struct RwCamera*(*)(struct RwCamera*, float))(g_libGTASA +(VER_x32 ?  0x001D5ACC + 1 : 0x26D034)))(CRenderTarget::m_pCamera, 300.0f);

    // RwCameraSetNearClipPlane
    ((struct RwCamera* (*)(struct RwCamera*, float))(g_libGTASA + (VER_x32 ? 0x001D5A38 + 1 : 0x26CF8C)))(CRenderTarget::m_pCamera, 0.01f);

    // RwCameraSetViewWindow
    float view[2] = { 0.5f, 0.5f };
    ((struct RwCamera* (*)(struct RwCamera*, float*))(g_libGTASA + (VER_x32 ? 0x001D5E04 + 1 : 0x26D330)))(CRenderTarget::m_pCamera, view);

    // RwCameraSetProjection
    ((struct RwCamera* (*)(struct RwCamera*, int))(g_libGTASA + (VER_x32 ? 0x001D5D28 + 1 : 0x26D24C)))(CRenderTarget::m_pCamera, 1);

    // RpWorldAddCamera
    if (Scene.m_pRpWorld)
    {
        //((uintptr_t(*)(RwWorld*, struct RwCamera*))(g_libGTASA + 0x0021DF84 + 1))(Scene.m_pRpWorld, m_pCamera);
        CHook::CallFunction<uintptr_t>(g_libGTASA + (VER_x32 ? 0x0021DF84 + 1 : 0x2C78F0), Scene.m_pRpWorld, (struct RwCamera*)CRenderTarget::m_pCamera);
    }
    else
    {
        return false;
    }

    return true;
}

void CRenderTarget::Initialise(){
    CRenderTarget::m_pResultTexture = nullptr;

    CRenderTarget::m_pCamera = nullptr;
    CRenderTarget::m_pLight = nullptr;
    CRenderTarget::m_pFrame = nullptr;
    CRenderTarget::m_zBuffer = nullptr;


    m_bReady = false;

    if(InitialiseScene()){
        m_bReady = true;
    }
}

void CRenderTarget::PreProcessCamera(RwInt32 sizeX, RwInt32 sizeY)
{
    if(CRenderTarget::m_zBuffer) {
        RwRasterDestroy(CRenderTarget::m_zBuffer);
        *(RwRaster**)((uintptr_t)CRenderTarget::m_pCamera + 0x64) = nullptr;
    }
    CRenderTarget::m_zBuffer = RwRasterCreate(sizeX, sizeY, 0, rwRASTERTYPEZBUFFER);

    if (CRenderTarget::m_zBuffer == nullptr)
    {
        Log("CRenderTarget buffer raster not created!");
        return;
    }

    *(RwRaster**)((uintptr_t)CRenderTarget::m_pCamera + 0x64) = CRenderTarget::m_zBuffer;

    struct RwRaster* pRaster = RwRasterCreate(sizeX, sizeY, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
    if(!pRaster)
        Log("CRenderTarget raster not created!");
    // RwTextureCreate
    CRenderTarget::m_pResultTexture = ((struct RwTexture*(*)(struct RwRaster*))(g_libGTASA + 0x001DB7BC + 1))(pRaster);
    if(!CRenderTarget::m_pResultTexture)
        Log("CRenderTarget Texture not Created!");
    *(RwRaster**)((uintptr_t)CRenderTarget::m_pCamera + (VER_x32 ? 0x60 : 0x80)) = pRaster;

    // CVisibilityPlugins::SetRenderWareCamera
    ((void(*)(struct RwCamera*))(g_libGTASA + 0x005D6248 + 1))(CRenderTarget::m_pCamera);
}

void CRenderTarget::ProcessCamera(RwRGBA* bgcolor, bool b2D)
{
    // RwCameraClear
    //uint32_t dwColor = 0xFFFFFFFF;
    ((struct RwCamera* (*)(struct RwCamera*, RwRGBA*, int))(g_libGTASA + 0x001D5CF0 + 1))(CRenderTarget::m_pCamera, bgcolor, 3);

    if (!RwCameraBeginUpdate(CRenderTarget::m_pCamera))
    {
        CRenderTarget::m_bSucessfull = false;
        return;
    }
    CRenderTarget::m_bSucessfull = true;

    if (Scene.m_pRpWorld)
    {
        RpWorldAddLight(Scene.m_pRpWorld, CRenderTarget::m_pLight);
    }

    if (b2D)
    {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)0);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)0);
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
        RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, (void*)0);
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATER);
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)2);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEARMIPLINEAR);
        RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);

        DefinedState2d();
    }
    else
    {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
        RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

        // DefinedState
        ((void(*) (void))(g_libGTASA + 0x005D0C10 + 1))();
    }
}

void CRenderTarget::PostProcessCamera()
{
    if (CRenderTarget::m_bSucessfull)
    {
        RwCameraEndUpdate(CRenderTarget::m_pCamera);

        if (Scene.m_pRpWorld)
        {
            RpWorldRemoveLight(Scene.m_pRpWorld, CRenderTarget::m_pLight);
        }
    }
}

void CRenderTarget::shutDown()
{
    RpLightDestroy(CRenderTarget::m_pLight);

    // RwObjectHasFrameSetFrame
    ((void(*)(struct RwCamera*, struct RwFrame*))(g_libGTASA + (VER_x32 ? 0x001DCF64 + 1 : 0x275CF0)))(CRenderTarget::m_pCamera, nullptr);

    // RwBool RwFrameDestroy(RwFrame* frame);
    ((RwBool(*)(struct RwFrame*))(g_libGTASA + 0x001D83EC + 1))(CRenderTarget::m_pFrame);

    RwRasterDestroy(CRenderTarget::m_zBuffer);
    *(RwRaster**)(CRenderTarget::m_pCamera + 0x64) = nullptr;

    // RwBool RwCameraDestroy(RwCamera* camera);

    ((RwBool(*)(struct RwCamera*))(g_libGTASA + 0x001D5EA0 + 1))(CRenderTarget::m_pCamera);
}

void CRenderTarget::Begin(RwInt32 sizeX, RwInt32 sizeY, RwRGBA* bgColor, bool b2D)
{
    if (!CRenderTarget::m_bReady)
    {
        return;
    }

    struct RwRaster* pRaster = RwRasterCreate(sizeX, sizeY, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);

    *(RwRaster**)((uintptr_t)m_pCamera + (VER_x32 ? 0x60 : 0x80)) = pRaster;

    ((void(*)(struct RwCamera*))(g_libGTASA + (VER_x32 ? 0x5D6248 + 1 : 0x6FB5BC)))(m_pCamera);


    ((struct RwCamera* (*)(struct RwCamera*, RwRGBA*, int))(g_libGTASA + (VER_x32 ? 0x1D5CF0 + 1 : 0x26D1E8)))(m_pCamera, bgColor, 3);

    if (!RwCameraBeginUpdate(m_pCamera))
    {
        m_bSucessfull = false;
        return;
    }
    m_bSucessfull = true;

    if (Scene.m_pRpWorld)
    {
        RpWorldAddLight(Scene.m_pRpWorld, m_pLight);
    }

    if (b2D)
    {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)0);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)0);
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
        RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, (void*)0);
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATER);
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)2);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEARMIPLINEAR);
        RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);

        DefinedState2d();
    }
    else
    {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
        RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

        // DefinedState
        ((void(*) (void))(g_libGTASA + (VER_x32 ? 0x005D0C10 + 1 : 0x6F4EC4)))();
    }

    //PreProcessCamera(sizeX, sizeY);
    //ProcessCamera(bgColor, b2D);
}

RwTexture* CRenderTarget::End()
{
    if (!m_bReady)
    {
        Log("not ready!");
        return nullptr;
    }

    //PostProcessCamera();
    if ( m_bSucessfull )
    {
        RwCameraEndUpdate(CRenderTarget::m_pCamera);
        RpWorldRemoveLight(Scene.m_pRpWorld, m_pLight);
        Log("RpWorldRemoveLight");
    }

    struct RwTexture* pTexture = RwTextureCreate(*(RwRaster**)((uintptr_t)m_pCamera + (VER_x32 ? 0x60 : 0x80)));

    RwTextureSetName(pTexture, "rtarget");
    m_pResultTexture = nullptr;
    *(RwRaster **)(m_pCamera + (VER_x32 ? 0x60 : 0x80)) = nullptr;

    return pTexture;
}

