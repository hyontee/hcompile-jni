#include "../main.h"
#include "RW/RenderWare.h"
#include <sstream>
#include "game.h"
#include "game/Enums/PedState.h"

#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../util/CJavaWrapper.h"
#include "../java_systems/CHUD.h"
#include "..///..//santrope-tea-gtasa/encryption/CTinyEncrypt.h"
#include "..///..//santrope-tea-gtasa/encryption/encrypt.h"
#include "CGtaWidgets.h"
extern "C"
{
#include "..//santrope-tea-gtasa/encryption/aes.h"
}

#include "..///..//santrope-tea-gtasa/CGameResourcesDecryptor.cpp"
#include "..///..//santrope-tea-gtasa/CGameResourcesDecryptor.h"
#include "chatwindow.h"
#include "../util/patch.h"
#include "java_systems/CSpeedometr.h"

extern CGame* pGame;
extern CPlayerPed *g_pCurrentFiredPed;
extern BULLET_DATA *g_pCurrentBulletData;

static uint32_t dwRLEDecompressSourceSize = 0;


#include "..//str_obfuscator_no_template.hpp"
#define MAX_ENCRYPTED_TXD 2

const cryptor::string_encryptor encrArch[MAX_ENCRYPTED_TXD] = {
        //cryptor::create("texdb/txd/txd.txt", 19),
        cryptor::create("texdb/gta3/gta3.txt", 21),
        cryptor::create("texdb/gta_int/gta_int.txt", 27),
        //cryptor::create("texdb/cars/cars.txt", 27),
        //cryptor::create("texdb/skins/skins.txt", 27),
};

extern bool g_bIsTestMode;


bool isEncrypted(const char* szArch)
{
    for (int64 i = 0; i < MAX_ENCRYPTED_TXD; i++)
    {
        if (!strcmp(encrArch[i].decrypt(), szArch))
            return true;
    }

    return false;
}
void InitCTX(AES_ctx& ctx, const uint8_t* pKey)
{
    uint8_t key[16];
    memcpy(&key[0], &pKey[0], 16);
    for (int i = 0; i < 16; i++)
    {
        key[i] = XOR_UNOBFUSCATE(key[i]);
    }
    uint8_t iv[16];
    memcpy(&iv[0], &g_iIV, 16);
    for (int i = 0; i < 16; i++)
    {
        iv[i] = XOR_UNOBFUSCATE(iv[i]);
    }
    AES_init_ctx_iv(&ctx, &key[0], &iv[0]);
}

int lastOpenedFile;

extern CNetGame *pNetGame;
extern CGUI *pGUI;
// Neiae/SAMP
bool g_bPlaySAMP = false;

void InitInMenu();
void MainLoop();
void HookCPad();

/* ================ ie?oee aey ani. anoaaie =================== */

extern "C" uintptr_t get_lib()
{
    return g_libGTASA;
}

/* ====================================================== */

struct stFile
{
    int isFileExist;
    FILE *f;
};

stFile* NvFOpen(const char* r0, const char* r1, int r2, int r3)
{
    char path[0xFF] = { 0 };
    sprintf(path, "%s%s", g_pszStorage, r1);

//	// ----------------------------
    if(!strncmp(r1+12, "mainV1.scm", 10))
    {
        sprintf(path, "%sSAMP/main.scm", g_pszStorage);
        Log("Loading %s", path);
    }
    // ----------------------------
    if(!strncmp(r1+12, "SCRIPTV1.IMG", 12))
    {
        sprintf(path, "%sSAMP/script.img", g_pszStorage);
        Log("Loading script.img..");
    }

//	if(!strncmp(r1+12, "GTA3.IMG", 12))
//	{
//		sprintf(path, "%sTEXDB/GTA3.IMG", g_pszStorage);
//		Log("Loading script.img..");
//	}
////
//	if(!strncmp(r1+12, "SAMP.IMG", 12))
//	{
//		sprintf(path, "%sTEXDB/SAMP.IMG", g_pszStorage);
//		Log("Loading script.img..");
//	}
//
//	if(!strncmp(r1+12, "SAMPCOL.IMG", 12))
//	{
//		sprintf(path, "%sTEXDB/SAMPCOL.IMG", g_pszStorage);
//		Log("Loading script.img..");
//	}

    if (!strncmp(r1, "DATA/PEDS.IDE", 13))
    {
        sprintf(path, "%s/SAMP/peds.ide", g_pszStorage);
        Log("Loading peds.ide..");
    }

    if (!strncmp(r1, "DATA/VEHICLES.IDE", 17)) {
        sprintf(path, "%s/SAMP/vehicles.ide", g_pszStorage);
        Log("Loading vehicles.ide..");
    }

    if (!strncmp(r1, "DATA/GTA.DAT", 12))
    {
        sprintf(path, "%s/SAMP/gta.dat", g_pszStorage);
        Log("Loading gta.dat..");
    }

    auto *st = (stFile*)malloc(8);
    st->isFileExist = false;

    Log("%s", path);

    FILE *f  = fopen(path, "rb");
    if(f)
    {
        st->isFileExist = true;
        st->f = f;
        return st;
    }
    else
    {
        Log("NVFOpen hook | Error: file not found (%s)", path);
        free(st);
        return nullptr;
    }
}
float *ms_fAspectRatio;
static constexpr float ar43 = 4.0f / 3.0f;
void (*DrawCrosshair)(uintptr_t* thiz);
void DrawCrosshair_hook(uintptr_t* thiz)
{
    float save1 = CCamera::m_f3rdPersonCHairMultX;
    CCamera::m_f3rdPersonCHairMultX = 0.530f - (*ms_fAspectRatio - ar43) * 0.01125f;

    float save2 = CCamera::m_f3rdPersonCHairMultY;
    CCamera::m_f3rdPersonCHairMultY = 0.400f + (*ms_fAspectRatio - ar43) * 0.03600f;

    DrawCrosshair(thiz);

    CCamera::m_f3rdPersonCHairMultX = save1;
    CCamera::m_f3rdPersonCHairMultY = save2;
}

#include "keyboard.h"
void Render2dStuff()
{
    if( CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x001BB7F4 + 1 : 0x24EA90)) ) // emu_IsAltRenderTarget()
        CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x001BC20C + 1 : 0x24F5B8)); // emu_FlushAltRenderTarget()

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(rwRENDERSTATENARENDERSTATE));
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
    if (pGUI) pGUI->Render();

    // jpatch fix crosshair
    //float saveX = CCamera::m_f3rdPersonCHairMultX;
    //CCamera::m_f3rdPersonCHairMultX = 0.530f - (*ms_fAspectRatio - ar43) * 0.01125f;

    //float saveY = *CCamera::m_f3rdPersonCHairMultY;
    //*CCamera::m_f3rdPersonCHairMultY = 0.400f + (*ms_fAspectRatio - ar43) * 0.03600f;
#if VER_x32
    ( ( void(*)() )(g_libGTASA + 0x00437200 + 1) )(); // прицел
#else
    ( ( void(*)() )(g_libGTASA + 0x0051C694) )(); // прицелCHud::DrawCrossHairs(void)	000000000051C694
#endif
    //*CCamera::m_f3rdPersonCHairMultX = saveX;
    //*CCamera::m_f3rdPersonCHairMultY = saveY;
    //
#if VER_x32
    ( ( void(*)(bool) )(g_libGTASA + 0x001C0750 + 1) )(false); // emu_GammaSet

    if(CHUD::bIsShow)
        if(!CKeyBoard::m_bEnable && pNetGame)
            ( ( void(*)(bool) )(g_libGTASA + 0x002B0BD8 + 1) )(false); // render widgets
#else
    if(!CPause::bShow)
        if(CHUD::bIsShow)
            if(!CKeyBoard::m_bEnable && pNetGame)
		        ( ( void(*)(bool) )(g_libGTASA + 0x0036FB00) )(false); // render widgets

	( ( void(*)(bool) )(g_libGTASA + 0x00252CE4) )(false); // emu_GammaSet
#endif

    if (CHUD::bIsShow) {
        CHUD::UpdateHudInfo();

        CWidgetGta *radar = CGtaWidgets::m_pWidgets[0xA1];

        //thiz[0] = (float)100;
        if (radar) {

            //radar[3] = CHUD::radarPos.x;
            //radar[4] = CHUD::radarPos.y;

            //radar[5] = CHUD::bgsize;
            //radar[6] = CHUD::bgsize;

            radar->m_fOriginX = CHUD::radarPos.x;
            radar->m_fOriginY = CHUD::radarPos.y;

            radar->m_fScaleX = 38.0f;
            radar->m_fScaleY = 38.0f;
        }
#if VER_x32
        ((void (*)()) (g_libGTASA + 0x00437B0C + 1))(); // радар
#else
        ( ( void(*)() )(g_libGTASA + 0x51CFF0) )(); // радар
#endif
    }
    CHook::CallFunction<void>("_Z12emu_GammaSeth", 1);

    ((void (*)(bool)) (g_libGTASA + (VER_x32 ? 0x0054BDD4 + 1 : 0x66B678)))(1u); // CMessages::Display - gametext
    ((void (*)(bool)) (g_libGTASA + (VER_x32 ? 0x005A9120 + 1 : 0x6CCEA0)))(1u); // CFont::RenderFontBuffer
    CHook::CallFunction<void>("_Z12emu_GammaSeth", 0);


}


// CGame::InitialiseRenderWare
void (*InitialiseRenderWare)();
void InitialiseRenderWare_hook()
{
    TextureDatabaseRuntime::Load("mobile", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("txd", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("gta3", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_ETC);
    TextureDatabaseRuntime::Load("gta_int", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_ETC);
    TextureDatabaseRuntime::Load("player", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("menu", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("samp", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("gui", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);

    TextureDatabase* gtasa = TextureDatabaseRuntime::Load("gtasa", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Register(gtasa);

    TextureDatabase* radar = TextureDatabaseRuntime::Load("radar", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_ETC);
    TextureDatabaseRuntime::Register(radar);

    TextureDatabase* cars = TextureDatabaseRuntime::Load("cars", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Register(cars);

    TextureDatabase* skins = TextureDatabaseRuntime::Load("skins", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Register(skins);

    InitialiseRenderWare();
}

/* ====================================================== */
#include "..//keyboard.h"
#include "Timer.h"
void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
    if (CTimer::m_UserPause)
    {
        return TouchEvent(type, num, posX, posY);
    }

    if (g_pWidgetManager)
    {
        g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_UP, type, num, posX, posY);
        g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_DOWN, type, num, posX, posY);
    }
    bool bRet = pGUI->OnTouchEvent(type, num, posX, posY);
    if (!bRet)
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    if (CKeyBoard::IsOpen())
    {
        if (posX >= io.DisplaySize.x - ImGui::GetFontSize() * 4 && posY >= io.DisplaySize.y / 2 - (ImGui::GetFontSize() * 2.5) * 3 && posY <= io.DisplaySize.y / 2) // keys
        {
            return;
        }
    }

    if(bRet)
        return TouchEvent(type, num, posX, posY);
}

/* ====================================================== */

void (*CStreaming_InitImageList)();
void CStreaming_InitImageList_hook()
{
    for (auto & ms_file : CStreaming::ms_files) {
        ms_file.m_szName[0] = 0;
        ms_file.m_StreamHandle = 0;
    }

    CStreaming::AddImageToList("TEXDB\\GTA3.IMG", true);
    CStreaming::AddImageToList("TEXDB\\GTA_INT.IMG", true);
    CStreaming::AddImageToList("TEXDB\\SAMP.IMG", true);
    CStreaming::AddImageToList("TEXDB\\SAMPCOL.IMG", true);
    CStreaming::AddImageToList("TEXDB\\CARS.IMG", true);
    CStreaming::AddImageToList("TEXDB\\SKINS.IMG", true);
}

/* ====================================================== */

#include "game/Models/ModelInfo.h"
#include "GPS.h"

/* ====================================================== */

uint32_t (*CRadar__GetRadarTraceColor)(uint32_t color, uint8_t bright, uint8_t friendly);
uint32_t CRadar__GetRadarTraceColor_hook(uint32_t color, uint8_t bright, uint8_t friendly)
{
    return TranslateColorCodeToRGBA(color);
}

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char *name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char *name)
{
    if (pNetGame && !strncmp(name, "CODEWAY", 7))
    {
        float findZ = ((float (*)(float, float))(g_libGTASA + 0x0042A7C4 + 1))(X, Y);
        findZ += 1.5f;
        CVector pos = { X, Y, findZ - 1.5f };


        GPS::Set(pos, true);
        RakNet::BitStream bsSend;
        bsSend.Write(X);
        bsSend.Write(Y);
        bsSend.Write(findZ);
        pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    }

    return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

uint8_t bGZ = 0;
void (*CRadar__DrawRadarGangOverlay)(uint8_t v1);
void CRadar__DrawRadarGangOverlay_hook(uint8_t v1)
{
    bGZ = v1;
    if (pNetGame && pNetGame->GetGangZonePool())
        pNetGame->GetGangZonePool()->Draw();
}

uint32_t dwParam1, dwParam2;
extern "C" void pickup_ololo()
{
    if(pNetGame && pNetGame->GetPickupPool())
    {
        CPickupPool *pPickups = pNetGame->GetPickupPool();
        pPickups->PickedUp( ((dwParam1-(g_libGTASA+0x007AFD70))/0x20) );
    }
}

extern bool g_customFire;
// fire weapon hooks
uint32_t (*CWeapon__FireInstantHit)(CWeaponGta* thiz, CPedGta * pFiringEntity, CVector * vecOrigin, CVector * muzzlePosn, CEntityGta * targetEntity, CVector * target, CVector * originForDriveBy, int arg6, int muzzle);
uint32_t CWeapon__FireInstantHit_hook(CWeaponGta* thiz, CPedGta * pFiringEntity, CVector * vecOrigin, CVector * muzzlePosn, CEntityGta * targetEntity, CVector * target, CVector * originForDriveBy, int arg6, int muzzle)
{

    if(!g_customFire)
    {
        CPedGta *pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
        if(pLocalPed)
        {
            if(pFiringEntity != pLocalPed)
                return muzzle;

            if(pNetGame)
            {
                CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
                if(pPlayerPool)
                    pPlayerPool->ApplyCollisionChecking();
            }

            if(pGame)
            {
                CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
                if(pPlayerPed)
                    pPlayerPed->FireInstant();
            }

            if(pNetGame)
            {
                CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
                if(pPlayerPool)
                    pPlayerPool->ResetCollisionChecking();
            }

            return muzzle;
        }
    }
    g_customFire = false;
    return CWeapon__FireInstantHit(thiz, pFiringEntity, vecOrigin, muzzlePosn, targetEntity, target, originForDriveBy, arg6, muzzle);
}

void (*MainMenuScreen__Update)(uintptr_t* thiz, float deltaTime);
void MainMenuScreen__Update_hook(uintptr_t* thiz, float deltaTime)
{
    MainMenuScreen__Update(thiz, deltaTime);

    if(g_bPlaySAMP) return;

    InitInMenu();

    // StartGameScreen::OnNewGameCheck()
    (( void (*)())(g_libGTASA + 0x002A7270 + 1))();

    //*(uint32_t*)(g_libGTASA + 0x009E75B8) = 8;

    g_bPlaySAMP = true;
}

static char szLastBufferedName[40];
int (*cHandlingDataMgr__FindExactWord)(uintptr_t *thiz, char* line, char* nameTable, int entrySize, int entryCount);
int cHandlingDataMgr__FindExactWord_hook(uintptr_t *thiz, char* line, char* nameTable, int entrySize, int entryCount)
{
    strncpy(&szLastBufferedName[0], line, entrySize);
    return cHandlingDataMgr__FindExactWord(thiz, line, nameTable, entrySize, entryCount);
}

void cHandlingDataMgr__ConvertDataToGameUnits(uintptr_t *thiz, tHandlingData* handling)
{

    int32_t iHandling = ((int(*)(uintptr_t*, const char*))(g_libGTASA + 0x00570D28 + 1))(thiz, &szLastBufferedName[0]);

    if(!CHandlingDefault::bIsSlotUsed[iHandling])
    {
        CHandlingDefault::FillDefaultHandling((uint16_t)iHandling, handling);
    }

    (( void (*)(uintptr_t*, tHandlingData* handling))(g_libGTASA + 0x00570DC8 + 1))(thiz, handling);
}

void(*CStreaming__Init2)();
void CStreaming__Init2_hook()
{
    CStreaming__Init2();
    *(uint32_t*)(g_libGTASA+(VER_x32 ? 0x00685FA0:0x85EBD8)) = 536870912;
}

signed int (*OS_FileOpen)(unsigned int a1, int *a2, const char *a3, int a4);
signed int OS_FileOpen_hook(unsigned int a1, int *a2, const char *a3, int a4)
{
    signed int retn = OS_FileOpen(a1, a2, a3, a4);

    if (isEncrypted(a3))
    {
        lastOpenedFile = *a2;
    }

    return retn;
}

size_t (*OS_FileRead)(FILE *file, void *buffer, size_t numBytes);
size_t OS_FileRead_hook(FILE *file, void *buffer, size_t numBytes)
{

    uintptr_t calledFrom = 0;

    GET_LR(calledFrom);
    dwRLEDecompressSourceSize = numBytes;
    if(!numBytes)
        return 0;

    if (calledFrom == (VER_x32 ? 0x1E8142 + 1 : 0x2842B0) && file == (void*)lastOpenedFile)
    {
        lastOpenedFile = 0;

        AES_ctx ctx;
        InitCTX(ctx, &g_iEncryptionKeyTXD[0]);

        size_t retv = OS_FileRead(file, buffer, numBytes);
        int fileSize = numBytes;
        int iters = fileSize / PART_SIZE_TXD;
        uintptr_t pointer = (uintptr_t)buffer;
        for (int i = 0; i < iters; i++)
        {
            AES_CBC_decrypt_buffer(&ctx, (uint8_t *)pointer, PART_SIZE_TXD);
            pointer += PART_SIZE_TXD;
        }
        return retv;
    }
    return OS_FileRead(file, buffer, numBytes);
}

extern char g_iLastBlock[512];
unsigned int(*LoadFullTexture)(uintptr_t *thiz, unsigned int entryIndex);
unsigned int LoadFullTexture_hook(uintptr_t *thiz, unsigned int entryIndex)
{
    strcpy(g_iLastBlock, *(const char **)(*((DWORD *)thiz + 7) + 0x17 * entryIndex));

    return LoadFullTexture(thiz, entryIndex);
}
//
void (*RLEDecompress)(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape);
void RLEDecompress_hook(uint8_t* pDest, size_t uiDestSize, const uint8_t* pSrc, size_t uiSegSize, uint32_t uiEscape) {

    if (!pDest || !pSrc || uiDestSize == 0 || uiSegSize == 0) {
        // Обработка некорректных входных данных или размеров
        // Здесь можно сгенерировать исключение или вернуть код ошибки
        return;
    }

    const uint8_t* pTempSrc = pSrc;
    const uint8_t* const pEndOfDest = pDest + uiDestSize;
    const uint8_t* const pEndOfSrc = pSrc + dwRLEDecompressSourceSize; // Предполагается, что dwRLEDecompressSourceSize определено правильно

    try {
        while (pDest < pEndOfDest && pTempSrc < pEndOfSrc) {
            if (*pTempSrc == uiEscape) {
                if (pTempSrc + 1 >= pEndOfSrc || pTempSrc[1] == 0 || pTempSrc + 2 + uiSegSize > pEndOfSrc) {
                    // Обработка ошибки, неверное значение ucCurSeg или недостаточно данных в исходном буфере
                    throw std::runtime_error("rled error 1");
                }

                uint8_t ucCurSeg = pTempSrc[1];
                while (ucCurSeg--) {
                    if (pDest + uiSegSize > pEndOfDest) {
                        // Обработка ошибки, недостаточно места в целевом буфере
                        throw std::runtime_error("rled error 2");
                    }
                    memcpy(pDest, pTempSrc + 2, uiSegSize);
                    pDest += uiSegSize;
                }
                pTempSrc += 2 + uiSegSize;
            } else {
                if (pDest + uiSegSize > pEndOfDest || pTempSrc + uiSegSize > pEndOfSrc) {
                    // Обработка ошибки, недостаточно данных в исходном буфере или недостаточно места в целевом буфере
                    throw std::runtime_error("rled error 3");
                }
                memcpy(pDest, pTempSrc, uiSegSize);
                pDest += uiSegSize;
                pTempSrc += uiSegSize;
            }
        }

        dwRLEDecompressSourceSize = 0;
    } catch (const std::exception& e) {
        Log("%s", e.what());
    }
}

void (*CPools_Initialise)(void);
void CPools_Initialise_hook(void)
{
    struct PoolAllocator {

        struct Pool {
            void* objects;
            uint8_t* flags;
            uint32_t count;
            uint32_t top;
            uint32_t bInitialized;
        };
        static_assert(sizeof(Pool) == 0x14);

        static Pool* Allocate(size_t count, size_t size) {

            Pool* p = new Pool;

            p->objects = new char[size * count];
            p->flags = new uint8_t[count];
            p->count = count;
            p->top = 0xFFFFFFFF;
            p->bInitialized = 1;

            for (size_t i = 0; i < count; i++) {
                p->flags[i] |= 0x80;
                p->flags[i] &= 0x80;
            }

            return p;
        }
    };

    // 600000 / 75000 = 8
    static auto ms_pPtrNodeSingleLinkPool = PoolAllocator::Allocate(100000, 8);		// 75000
    // 72000 / 6000 = 12
    static auto ms_pPtrNodeDoubleLinkPool = PoolAllocator::Allocate(60000, 12);	// 6000
    // 10000 / 500 = 20
    static auto ms_pEntryInfoNodePool = PoolAllocator::Allocate(20000, 20);	// 500
    // 279440 / 140 = 1996
    static auto ms_pPedPool = PoolAllocator::Allocate(240, 1996);	// 140
    // 286440 / 110 = 2604
    static auto ms_pVehiclePool = PoolAllocator::Allocate(2000, 2604);	// 110
    // 840000 / 14000 = 60
    static auto ms_pBuildingPool = PoolAllocator::Allocate(20000, 60);	// 14000
    // 147000 / 350 = 420
    static auto ms_pObjectPool = PoolAllocator::Allocate(3000, 420);	// 350
    // 210000 / 3500 = 60
    static auto ms_pDummyPool = PoolAllocator::Allocate(40000, 60);	// 3500
    // 487200 / 10150 = 48
    static auto ms_pColModelPool = PoolAllocator::Allocate(50000, 48);	// 10150
    // 64000 / 500 = 128
    static auto ms_pTaskPool = PoolAllocator::Allocate(5000, 128);	// 500
    // 13600 / 200 = 68
    static auto ms_pEventPool = PoolAllocator::Allocate(1000, 68);	// 200
    // 6400 / 64 = 100
    static auto ms_pPointRoutePool = PoolAllocator::Allocate(200, 100);	// 64
    // 13440 / 32 = 420
    static auto ms_pPatrolRoutePool = PoolAllocator::Allocate(200, 420);	// 32
    // 2304 / 64 = 36
    static auto ms_pNodeRoutePool = PoolAllocator::Allocate(200, 36);	// 64
    // 512 / 16 = 32
    static auto ms_pTaskAllocatorPool = PoolAllocator::Allocate(3000, 32);	// 16
    // 92960 / 140 = 664
    static auto ms_pPedIntelligencePool = PoolAllocator::Allocate(240, 664);	// 140
    // 15104 / 64 = 236
    static auto ms_pPedAttractorPool = PoolAllocator::Allocate(200, 236);	// 64

    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC38) = ms_pPtrNodeSingleLinkPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC3C) = ms_pPtrNodeDoubleLinkPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC40) = ms_pEntryInfoNodePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC44) = ms_pPedPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC48) = ms_pVehiclePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC4C) = ms_pBuildingPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC50) = ms_pObjectPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC54) = ms_pDummyPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC58) = ms_pColModelPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC5C) = ms_pTaskPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC60) = ms_pEventPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC64) = ms_pPointRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC68) = ms_pPatrolRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC6C) = ms_pNodeRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC70) = ms_pTaskAllocatorPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC74) = ms_pPedIntelligencePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC78) = ms_pPedAttractorPool;
}

void (*CPlaceable_InitMatrixArray)();
void CPlaceable_InitMatrixArray_hook()
{
    CHook::CallFunction<void>(g_libGTASA + 0x00407FD4 + 1, g_libGTASA + 0x0095A988, 10000);
}


int (*CustomPipeRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int CustomPipeRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
    if (!resEntry)
        return 0;
    uint16_t size = *(uint16_t *)(resEntry + 26);
    if (size)
    {
        RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ *)(resEntry + 28);
        if (!arr)
            return 0;
        uint32_t validFlag = flags & 0x84;
        for (int i = 0; i < size; i++)
        {
            if (!arr[i].validate)
                break;
            if (validFlag)
            {
                uintptr_t *v4 = *(uintptr_t **)(arr[i].validate);
                if (!v4)
                    ;
                else
                {
                    if ((uintptr_t)v4 > (uintptr_t)0xFFFFFF00)
                        return 0;
                    else
                    {
                        if (!*(uintptr_t **)v4)
                            return 0;
                    }
                }
            }
        }
    }

    return CustomPipeRenderCB(resEntry, object, type, flags);
}

int (*rxOpenGLDefaultAllInOneRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int rxOpenGLDefaultAllInOneRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
    if (!resEntry)
        return 0;
    uint16_t size = *(uint16_t *)(resEntry + 26);
    if (size)
    {
        RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ *)(resEntry + 28);
        if (!arr)
            return 0;
        uint32_t validFlag = flags & 0x84;
        for (int i = 0; i < size; i++)
        {
            if (!arr[i].validate)
                break;
            if (validFlag)
            {
                uintptr_t *v4 = *(uintptr_t **)(arr[i].validate);
                if (!v4)
                    ;
                else
                {
                    if ((uintptr_t)v4 > (uintptr_t)0xFFFFFF00)
                        return 0;
                    else
                    {
                        if (!*(uintptr_t **)v4)
                            return 0;
                    }
                }
            }
        }
    }
    return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

int (*CTextureDatabaseRuntime__GetEntry)(unsigned int *thiz, const char *name, bool *hasSibling);
int CTextureDatabaseRuntime__GetEntry_hook(unsigned int *thiz, const char *name, bool *hasSibling)
{
    //Log("GetEntry = %s", (char*)a1);
    int result; // r0

    if (thiz)
        result = CTextureDatabaseRuntime__GetEntry(thiz, name, hasSibling);
    else
        result = -1;

    return result;
}

int (*mpg123_param)(void* mh, int key, long val, int ZERO, double fval);
int mpg123_param_hook(void* mh, int key, long val, int ZERO, double fval)
{
    return mpg123_param(mh, key, val | (0x7000 | 0x7040 | 0x4000 | 0x3000 | 0x2000 | 0x200 | 0x100 | 0x40 | 0x1234 | 0x5678 | 0x9012 | 0x3456 | 0x7890 | 0x2345 | 0x6789 | 0x0123 | 0x4567 | 0x8901 | 0x2346 | 0x6780 | 0x1235 | 0x7891 | 0x7000 | 0x8000 | 0x9000 | 0x10000 | 0x11000 | 0x12000 | 0x13000 | 0x14000 | 0x15000 | 0x7100 | 0x8200 | 0x9300 | 0x10400 | 0x11500 | 0x12600 | 0x8102 | 0x9203 | 0x1034 | 0x1145 | 0x1256 | 0x1367 | 0x1478 | 0x1589 | 0x1690 | 0x7801 | 0x8923 | 0x9034 | 0x10045 | 0x11156 | 0x12267 | 0x13378 | 0x14489 | 0x15590 | 0x16601 | 0x7712 | 0x8823 | 0x9934 | 0x10045 | 0x11156 | 0x12267 | 0x13378 | 0x14489 | 0x15590 | 0x16601 | 0x7723 | 0x8834 | 0x9945 | 0x10056 | 0x11167 | 0x12278 | 0x13389 | 0x14490 | 0x15501 | 0x16612 | 0x7734 | 0x8845 | 0x9956 | 0x10067 | 0x11178 | 0x12289 | 0x13390 | 0x14401 | 0x15512 | 0x16623 | 0x7012 | 0x8123 | 0x9234 | 0x1045 | 0x1156 | 0x1267 | 0x1378 | 0x1489 | 0x1590 | 0x1701 | 0x7812 | 0x8923 | 0x9034 | 0x10045 | 0x11156), ZERO, fval);
}


#include "game/Collision/Collision.h"
#include "game/Collision/ColStore.h"
#include "TxdStore.h"
#include "RQShader.h"
void InjectHooks()
{
    Log("InjectHooks");
    CModelInfo::InjectHooks();
    CTimer::InjectHooks();
    CCollision::InjectHooks();
//	CColStore::InjectHooks();
    CQuadTreeNode::InjectHooks();
    CTxdStore::InjectHooks();
    RQShader::InjectHooks();
    CPedIntelligence::InjectHooks();
    CStreaming::InjectHooks();
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679474 : 0x850910), &CGtaWidgets::m_pWidgets);
}


void InitialiseOnceBeforeRW(){
    CHook::CallFunction<void>("_ZN10CMemoryMgr4InitEv");
    CHook::CallFunction<void>("_ZN14MobileSettings10InitializeEv");
    CHook::CallFunction<void>("_ZN13CLocalisation10InitialiseEv");
    CHook::CallFunction<void>("_ZN8CFileMgr10InitialiseEv");
    CHook::CallFunction<void>("_Z12CdStreamIniti", 8);
    CHook::CallFunction<void>("_ZN4CPad10InitialiseEv");
}

bool (*CAdjustableHUD__IsValidPosition);
bool CAdjustableHUD__IsValidPosition_hook(){
    return 1;
}

void (*CVehicle__DoVehicleLights)(CVehicleGta* thiz, CMatrix *matVehicle, uint32 nLightFlags);
void CVehicle__DoVehicleLights_hook(CVehicleGta* thiz, CMatrix *matVehicle, uint32 nLightFlags)
{
    uint8_t old = thiz->m_nVehicleFlags.bEngineOn;
    thiz->m_nVehicleFlags.bEngineOn = 1;
    CVehicle__DoVehicleLights(thiz, matVehicle, nLightFlags);
    thiz->m_nVehicleFlags.bEngineOn = old;
}
#include "CSkyBox.h"
void (*CClouds__Render)();
void CClouds__Render_hook(){
    if(CHook::CallFunction<bool>("_ZN5CGame25CanSeeOutSideFromCurrAreaEv")){
        CSkyBox::Process();
    }
    //CClouds__Render();
}

bool (*RwResourcesFreeResEntry)(void* entry);
bool RwResourcesFreeResEntry_hook(void* entry)
{
    bool result;
    if (entry) result = RwResourcesFreeResEntry(entry);
    else result = false;
    return result;
}

typedef struct CRealTimeShadow CRealTimeShadow;
#pragma pack(1)
struct CRealTimeShadowManager {
    uintptr_t m_initialised;
    CRealTimeShadow * m_pRealTimeShadows[40];
    uintptr_t m_updatingShadows;
    uintptr_t m_IPCameraMB;
};
#pragma pack(1)
struct CRealTimeShadow {
    CPhysicalGta* m_pPhysical;
    uint8_t m_usedThisFrame;
    uintptr_t m_fadeVal;
#if VER_x32
    PADDING(pad1, 303);
#else
    PADDING(pad1, 383);
#endif
    uintptr_t m_pRealTimeShadow;
};

void DoShadowThisFrame_hook(CRealTimeShadowManager *pRealTimeShadowManager, CPhysicalGta *pPhysical)
{
    CRealTimeShadow *m_pRealTimeShadow; // r2
    int v3; // r3
    CRealTimeShadow *pRealTimeShadow; // r12

    if ( pRealTimeShadowManager->m_initialised )
    {
        m_pRealTimeShadow = (CRealTimeShadow*)pPhysical->m_pRealTimeShadow;
        if ( m_pRealTimeShadow )
        {
            m_pRealTimeShadow->m_usedThisFrame = 1;
        }
        else
        {
            v3 = 4;
            while ( 1 )
            {
                pRealTimeShadow = *(CRealTimeShadow **)(&pRealTimeShadowManager->m_initialised + v3);
                if ( !pRealTimeShadow->m_pPhysical )
                    break;
                v3 += 4 ;
                if ( v3 ==  164 )
                    return;
            }
            pPhysical->m_pRealTimeShadow = (uintptr_t *)pRealTimeShadow;
            pRealTimeShadow->m_usedThisFrame = 1;
            CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x5B7DD0 + 1 : 0x6DC400), pRealTimeShadow, pPhysical);
        }
    }
}

void ReturnRealTimeShadow_hook(CRealTimeShadowManager *pRealTimeShadowManager, CRealTimeShadow *pRealTimeShadow)
{

    if ( pRealTimeShadow->m_pPhysical )
        pRealTimeShadow->m_pPhysical->m_pRealTimeShadow = nullptr;
    pRealTimeShadow->m_pPhysical = nullptr;
}

void (*FxEmitterBP_c__Render)(uintptr_t* a1, int a2, int a3, float a4, char a5);
void FxEmitterBP_c__Render_hook(uintptr_t* a1, int a2, int a3, float a4, char a5)
{
    if(!a1 || !a2) return;
    uintptr_t* temp = *((uintptr_t**)a1 + 3);
    if (!temp)
    {
        return;
    }
    FxEmitterBP_c__Render(a1, a2, a3, a4, a5);

}

uintptr_t (*GetMeshPriority)(uintptr_t);
uintptr_t GetMeshPriority_hook(uintptr_t rpMesh)
{
    if (rpMesh)
    {
        RpMaterial *rpMeshMat = *(RpMaterial **)(rpMesh + (VER_x32 ? 8 : 16));
        if (rpMeshMat)
        {
            if (rpMeshMat->texture)
            {
                if (!rpMeshMat->texture->raster)
                    return 0;
            }
        }
    }
    return GetMeshPriority(rpMesh);
}

#include "CLoader.h"
#include "Pools.h"
#include "CCoronas.h"

void (*NvUtilInit)();
void NvUtilInit_hook()
{
    NvUtilInit();
    g_pszStorage = (char*)(g_libGTASA + (VER_x32 ? 0x6D687C : 0x8B46A8)); // StorageRootBuffer
    CLoader::loadSetting();
    CLoader::initCrashLytics();
    CWeaponsOutFit::ParseDatFile();
}
bool CVehicle__DoTailLightEffect(CVehicleGta *m_pVehicle, int lightId, CMatrix *matVehicle, int isRight, int forcedOff, unsigned int nLightFlags, int lightsOn)
{


    int8 intensity; // r0

    if (!pNetGame) return false;
    CVehicle *pVehicle = pNetGame->GetVehiclePool()->FindVehicle(m_pVehicle);
    if (!pVehicle) {
        Log(OBFUSCATE("No find vehicle id!"));
        return false;
    }
    pVehicle->ProcessTurnLight(m_pVehicle, isRight, lightId);
    int v8 = isRight;
    if ( isRight )
        v8 = 1;
    //CVehicle::ProcessTurnLight(m_pVehicle, isRight, lightId);

    CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::GetModelInfo(
            m_pVehicle->nModelIndex));

    CVector m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos[1];
    if ( !isRight ) {
        m_avDummyPos.x = -m_avDummyPos.x;
    }

    intensity = 96;
    if ( m_pVehicle->m_fBreakPedal > 0.0 )
        intensity = 200;

    if (m_pVehicle->m_nVehicleFlags.bLightsOn || m_pVehicle->m_fBreakPedal > 0.0 && m_pVehicle->pDriver ) {
        CCoronas::RegisterCorona(
                reinterpret_cast<uintptr_t>(m_pVehicle) + 2 * lightId + isRight + 8,
                m_pVehicle,
                100,
                0,
                0,
                intensity,
                &m_avDummyPos,
                0.65f,
                70.0f,
                eCoronaType::CORONATYPE_HEADLIGHT,
                eCoronaFlareType::FLARETYPE_NONE,
                false,
                false,
                0,
                0.0f,
                false,
                0.0f,
                0,
                15.0f,
                false,
                false);
        if(isRight)
            pVehicle->DoRearLightReflectionTwin(intensity);
    }
    if (!m_pVehicle->m_nCurrentGear &&
        m_pVehicle->pHandling->m_transmissionData.m_fCurrentSpeed < -0.01) {
        CCoronas::RegisterCorona(
                reinterpret_cast<uintptr_t >(m_pVehicle) + 2 * lightId + isRight + 13,
                m_pVehicle,
                255,
                255,
                255,
                100,
                &m_avDummyPos,
                0.7f,
                70.0f,
                eCoronaType::CORONATYPE_HEADLIGHT,
                eCoronaFlareType::FLARETYPE_NONE,
                false,
                false,
                0,
                0.0f,
                false,
                0.0f,
                0,
                15.0f,
                false,
                false);
    }
    return true;
}
#include "MobileMenu.h"
void InstallSpecialHooks()
{
    InjectHooks();
    Log(OBFUSCATE("InstallSpecialHooks"));
    //CHook::NOP(g_libGTASA + 0x70B040, 1);
    //CHook::NOP(g_libGTASA + 0x4D63FC, 1);
    // Just a fuzzy seek. Tell MPG123 to not load useless data
#if VER_x32
    CHook::HookFunc(OBFUSCATE("mpg123_param"), mpg123_param_hook, &mpg123_param);
#endif
    CHook::HookFunc(OBFUSCATE("_ZN14CAdjustableHUD15IsValidPositionERK9CVector2DfPS0_"), CAdjustableHUD__IsValidPosition_hook, &CAdjustableHUD__IsValidPosition);
    CHook::HookFunc(OBFUSCATE("_ZN7CClouds6RenderEv"), CClouds__Render_hook, &CClouds__Render);
    // texture Краш если с текстурами какая-то хуйня
    CHook::HookFunc(OBFUSCATE("_ZN22TextureDatabaseRuntime8GetEntryEPKcRb"), CTextureDatabaseRuntime__GetEntry_hook, &CTextureDatabaseRuntime__GetEntry);

    CHook::RET(OBFUSCATE("_ZN12CCutsceneMgr16LoadCutsceneDataEPKc"));
    CHook::RET(OBFUSCATE("_ZN12CCutsceneMgr10InitialiseEv"));
    CHook::HookFunc("_ZN8CVehicle15DoVehicleLightsER7CMatrixj", CVehicle__DoVehicleLights_hook, &CVehicle__DoVehicleLights);
    //IMG

    CHook::HookFunc(OBFUSCATE("_ZN10CStreaming13InitImageListEv"), CStreaming_InitImageList_hook, &CStreaming_InitImageList);
    //CHook::HookFunc(OBFUSCATE("_ZN9CBuildingC2Ev"), CBuilding__CBuilding_hook, &CBuilding__CBuilding);
    CHook::HookFunc(OBFUSCATE("_Z23RwResourcesFreeResEntryP10RwResEntry"), RwResourcesFreeResEntry_hook, &RwResourcesFreeResEntry);
    CHook::HookRedirect(OBFUSCATE("_ZN8CVehicle17DoTailLightEffectEiR7CMatrixhhjh"), CVehicle__DoTailLightEffect);
    CHook::HookRedirect(OBFUSCATE("_ZN22CRealTimeShadowManager17DoShadowThisFrameEP9CPhysical"), DoShadowThisFrame_hook);

    CHook::HookRedirect(OBFUSCATE("_ZN22CRealTimeShadowManager20ReturnRealTimeShadowEP15CRealTimeShadow"), ReturnRealTimeShadow_hook);

    //new fix

    CHook::HookFunc(OBFUSCATE("_Z32_rxOpenGLDefaultAllInOneRenderCBP10RwResEntryPvhj"), rxOpenGLDefaultAllInOneRenderCB_hook, &rxOpenGLDefaultAllInOneRenderCB);

    CHook::HookFunc(OBFUSCATE("_ZN13FxEmitterBP_c6RenderEP8RwCamerajfh"), FxEmitterBP_c__Render_hook, &FxEmitterBP_c__Render);


    CHook::HookFunc(OBFUSCATE("_ZN25CCustomBuildingDNPipeline18CustomPipeRenderCBEP10RwResEntryPvhj"), CustomPipeRenderCB_hook, &CustomPipeRenderCB);

    CHook::HookFunc(OBFUSCATE("_Z15GetMeshPriorityPK6RpMesh"), GetMeshPriority_hook, &GetMeshPriority);


    // gettexture fix crash


    // CPlaceable::InitMatrixArray

    CHook::HookRedirect(OBFUSCATE("_Z10GetTexturePKc"), CUtil::GetTexture);
    //CHook::HookRedirect(OBFUSCATE("_ZN10CPlaceable15InitMatrixArrayEv"), CPlaceable_InitMatrixArray_hook);
    CHook::HookRedirect(OBFUSCATE("_ZN5CGame22InitialiseOnceBeforeRWEv"), InitialiseOnceBeforeRW);
    CHook::HookFunc(OBFUSCATE("_Z10NvUtilInitv"), NvUtilInit_hook, &NvUtilInit);


    // RLEDecompress fix

    CHook::installPLTHook(g_libGTASA + (VER_x32 ? 0x6701D4 : 0x840708), (uintptr_t)RLEDecompress_hook,(uintptr_t*)&RLEDecompress);

    // Log texture load
#if VER_x32
    CHook::HookFunc(OBFUSCATE("_ZN22TextureDatabaseRuntime15LoadFullTextureEj"), LoadFullTexture_hook, &LoadFullTexture);
#endif
    CHook::HookFunc(OBFUSCATE("_Z11OS_FileReadPvS_i"), OS_FileRead_hook, &OS_FileRead);


    CHook::Redirect(OBFUSCATE("_Z7NvFOpenPKcS0_bb"), NvFOpen);
    CHook::RET("_ZN4CPed31RemoveWeaponWhenEnteringVehicleEi"); // CPed::RemoveWeaponWhenEnteringVehicle


    CHook::HookFunc(OBFUSCATE("_ZN5CGame20InitialiseRenderWareEv"), InitialiseRenderWare_hook, &InitialiseRenderWare);


    CHook::HookFunc(OBFUSCATE("_ZN14MainMenuScreen6UpdateEf"), MainMenuScreen__Update_hook, &MainMenuScreen__Update);


    CHook::HookFunc(OBFUSCATE("_Z11OS_FileOpen14OSFileDataAreaPPvPKc16OSFileAccessType"), OS_FileOpen_hook, &OS_FileOpen);


//	CHook::InlineHook(g_libGTASA, 0x00570C70, &cHandlingDataMgr__FindExactWord_hook, &cHandlingDataMgr__FindExactWord);
//	CHook::MethodHook(g_libGTASA, 0x0067125C, &cHandlingDataMgr__ConvertDataToGameUnits);

    // increase stream memory value
    CHook::HookFunc(OBFUSCATE("_ZN10CStreaming5Init2Ev"), CStreaming__Init2_hook, &CStreaming__Init2);


    //CHook::HookFunc(OBFUSCATE("_ZN6CPools10InitialiseEv"), CPools_Initialise_hook, &CPools_Initialise);
    CPools::InjectHooks();
    MobileMenu::InjectMobileMenuHooks();
    Log(OBFUSCATE("end installspecialhooks"));
}

/* =========================================== Ped damage handler =========================================== */
enum ePedPieceTypes
{
    PED_PIECE_UNKNOWN = 0,

    PED_PIECE_TORSO = 3,
    PED_PIECE_ASS,
    PED_PIECE_LEFT_ARM,
    PED_PIECE_RIGHT_ARM,
    PED_PIECE_LEFT_LEG,
    PED_PIECE_RIGHT_LEG,
    PED_PIECE_HEAD
};

struct CPedDamageResponseInterface
{
    float fDamageHealth;
    float fDamageArmor;
    bool bUnk;
    bool bForceDeath;
    bool bDamageCalculated;
    bool bUnk3;
};

struct CPedDamageResponseCalculatorInterface
{
    CEntityGta *pEntity;
    float fDamage;
    ePedPieceTypes bodyPart;
#if VER_x32
    unsigned int weaponType;
#else
    uint64_t weaponType;
#endif
    bool bSpeak; // refers to a CPed::Say call (the dying scream?)
};

// thanks Codeesar
#pragma pack(push, 1)
struct stPedDamageResponse
{
    CEntityGta* pEntity;
    float fDamage;
    int iBodyPart;
    int iWeaponType;
    bool bSpeak;
};
#pragma pack(pop)
extern float m_fWeaponDamages[43 + 1];

void onDamage(CPedGta* issuer, CPedGta* damaged)
{
    if (!pNetGame) return;
    CPedGta* pPedPlayer = GamePool_FindPlayerPed();
    if (damaged && (pPedPlayer == issuer))
    {
        if (pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((CPedGta*)damaged) != INVALID_PLAYER_ID)
        {
            CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
            CAMERA_AIM* caAim = pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();

            CVector aim;
            aim.x = caAim->f1x;
            aim.y = caAim->f1y;
            aim.z = caAim->f1z;

            pPlayerPool->GetLocalPlayer()->SendBulletSyncData(pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta*)damaged), BULLET_HIT_TYPE_PLAYER, aim);
        }
    }
}

void (*CPedDamageResponseCalculator__ComputeDamageResponse)(stPedDamageResponse* thiz, CEntityGta* pEntity, uintptr_t pDamageResponse, bool bSpeak);
void CPedDamageResponseCalculator__ComputeDamageResponse_hook(stPedDamageResponse* thiz, CEntityGta* pEntity, uintptr_t pDamageResponse, bool bSpeak)
{
    if (thiz && pEntity) onDamage((CPedGta*)*(uintptr_t*)thiz, (CPedGta*)pEntity);
    int weaponid = thiz->iWeaponType;
    float fDamage;
    Log("%d", fDamage);
    if( weaponid < 0 || weaponid > size(m_fWeaponDamages) )
    {
        fDamage = thiz->fDamage;
    }
    else {
        fDamage = m_fWeaponDamages[weaponid];
    }
//	float fDamage = m_fWeaponDamages[weaponid];

    int bodypart = thiz->iBodyPart;

    if(pNetGame)
    {
        CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
        if(pPlayerPool)
        {
            if(weaponid < 0 || weaponid > 255 || (weaponid > 54 && weaponid < 200) || (weaponid > 201 && weaponid < 255))
                weaponid = 255; // suicide
            else if(weaponid == 18)
                weaponid = 37; // flamethower
            else if(weaponid == 35 || weaponid == 16)
                weaponid = 51; // explosion

            PLAYERID damagedid = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta*)thiz->pEntity); // отправитель урона
            PLAYERID issuerid = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta*)pEntity); // получатель

            PLAYERID byteLocalId = pPlayerPool->GetLocalPlayerID();

            // give player damage
            if((CPedGta*)thiz->pEntity == pGame->FindPlayerPed()->m_pPed && issuerid != INVALID_PLAYER_ID ) {
                //CHUD::addGiveDamageNotify(issuerid, weaponid, fDamage);
                pPlayerPool->GetLocalPlayer()->GiveTakeDamage(false, issuerid, fDamage, weaponid,
                                                              bodypart);
                Log("GiveDamage Name: %s, weaponid: %d, damage: %f", pPlayerPool->GetPlayerName(issuerid), weaponid, fDamage);
            }

                // player take damage
            else if(damagedid != INVALID_PLAYER_ID && issuerid == INVALID_PLAYER_ID) {
                pPlayerPool->GetLocalPlayer()->GiveTakeDamage(true, damagedid, fDamage, weaponid, bodypart);

                char nick[MAX_PLAYER_NAME];
                strcpy(nick, pPlayerPool->GetPlayerName(damagedid));

                Log("TakeDamage %s, %d, %f", pPlayerPool->GetPlayerName(damagedid), weaponid, fDamage);

                //CHUD::addTakeDamageNotify(pPlayerPool->GetPlayerName(damagedid), weaponid, fDamage);
            }
            if((CPedGta*)thiz->pEntity == pGame->FindPlayerPed()->m_pPed){
                CHUD::UpdateHudInfo();
            }


            Log("%d, %f, %d, %d", damagedid, fDamage, weaponid, bodypart);
        }
    }

    CPedDamageResponseCalculator__ComputeDamageResponse(thiz, pEntity, pDamageResponse, bSpeak);
}
#include "..//keyboard.h"

uint64_t(*CWorld_ProcessPedsAfterPreRender)();
uint64_t CWorld_ProcessPedsAfterPreRender_hook()
{
    uint64_t res = CWorld_ProcessPedsAfterPreRender();
    if (CTimer::bSkipProcessThisFrame)
        return res;

    if (pNetGame && pNetGame->GetPlayerPool())
    {
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            CPlayerPed* pPed = nullptr;
            if (pNetGame->GetPlayerPool()->GetLocalPlayerID() == i)
            {
                pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
            }
            else
            {
                if (pNetGame->GetPlayerPool()->m_pPlayers[i])
                {
                    pPed = pNetGame->GetPlayerPool()->GetAt(i)->GetPlayerPed();
                }
            }
            if (!pPed) continue;
            else
            {
                pPed->ProcessAttach();
            }
        }
    }

    return res;
}

#include <list>
#include <EGL/egl.h>

std::list<std::pair<unsigned int*, unsigned int>> resetEntries;

static uint32_t Color32Reverse(uint32_t x)
{
    return
            ((x & 0xFF000000) >> 24) |
            ((x & 0x00FF0000) >> 8) |
            ((x & 0x0000FF00) << 8) |
            ((x & 0x000000FF) << 24);
}

static RwRGBA DWORD2RGBAinternal(uint32_t dwColor)
{
    RwRGBA tmp;

    tmp.blue = dwColor & 0xFF; dwColor >>= 8;
    tmp.green = dwColor & 0xFF; dwColor >>= 8;
    tmp.red = dwColor & 0xFF; dwColor >>= 8;
    tmp.alpha = dwColor & 0xFF; /* dwColor >>= 8; */

    return tmp;
}

int g_iLastRenderedObject;

RwFrame* CClumpModelInfo_GetFrameFromId_Post(RwFrame* pFrameResult, RpClump* pClump, int id)
{
    if (pFrameResult)
        return pFrameResult;

    uintptr_t calledFrom = 0;
    __asm__ volatile ("mov %0, lr" : "=r" (calledFrom));
    calledFrom -= g_libGTASA;

    if (calledFrom == 0x00515708                // CVehicle::SetWindowOpenFlag
        || calledFrom == 0x00515730             // CVehicle::ClearWindowOpenFlag
        || calledFrom == 0x00338698             // CVehicleModelInfo::GetOriginalCompPosition
        || calledFrom == 0x00338B2C)            // CVehicleModelInfo::CreateInstance
        return nullptr;

    for (uint i = 2; i < 40; i++)
    {
        RwFrame* pNewFrameResult = nullptr;
        uint     uiNewId = id + (i / 2) * ((i & 1) ? -1 : 1);

        pNewFrameResult = ((RwFrame * (*)(RpClump * pClump, int id))(g_libGTASA + 0x003856D0 + 1))(pClump, i);

        if (pNewFrameResult)
        {
            return pNewFrameResult;
        }
    }

    return nullptr;
}
RwFrame* (*CClumpModelInfo_GetFrameFromId)(RpClump*, int);
RwFrame* CClumpModelInfo_GetFrameFromId_hook(RpClump* a1, int a2)
{
    return CClumpModelInfo_GetFrameFromId_Post(CClumpModelInfo_GetFrameFromId(a1, a2), a1, a2);
}

#include "..//crashlytics.h"
#include "game/RW/rpworld.h"

char g_bufRenderQueueCommand[200];
uintptr_t g_dwRenderQueueOffset;

char* (*RenderQueue__ProcessCommand)(uintptr_t thiz, char* a2);
char* RenderQueue__ProcessCommand_hook(uintptr_t thiz, char* a2)
{
    Log("%s", a2);
    return RenderQueue__ProcessCommand(thiz, a2);
    if (thiz && a2)
    {
        uintptr_t* dwRenderQueue = (uintptr_t*)thiz;

        memset(g_bufRenderQueueCommand, 0, sizeof(g_bufRenderQueueCommand));

        g_dwRenderQueueOffset = *(uintptr_t*)a2;
        snprintf(g_bufRenderQueueCommand, 190, "offset: %d | name: %s", g_dwRenderQueueOffset, (const char*)(*(dwRenderQueue + 100 + g_dwRenderQueueOffset)));

        return RenderQueue__ProcessCommand(thiz, a2);
    }
    else
    {
        return nullptr;
    }
}

std::list<std::pair<unsigned int*, unsigned int>> resetEntriesVehicle;


RpMaterial* CVehicle__SetupRenderMatCB(RpMaterial* material, void* data)
{
    if (material)
    {
        CVehicle* pVeh = (CVehicle*)data;
        int color = *(int*)&material->color;

        if(material->texture)
        {
            if (!strcmp(material->texture->name, "nomer"))
            {
                resetEntriesVehicle.emplace_back(reinterpret_cast<uintptr_t*>(&(material->texture)), *reinterpret_cast<uintptr_t*>(&(material->texture)));
                //if(pVeh->pPlateTexture) {
                //    material->texture = pVeh->pPlateTexture;
                //} else{
                //     material->texture = CCustomPlateManager::pNoPlateTex;
                //}
                material->color.alpha = 255;

                return material;
            }
        }
        if ( color == 0xff00ff3c )
        { // first color
            resetEntriesVehicle.emplace_back(reinterpret_cast<uintptr_t *>(&(material->color)), *reinterpret_cast<uintptr_t*>(&(material->color)));
            if(pVeh->m_iVinylId != -1) {
                resetEntriesVehicle.emplace_back(reinterpret_cast<uintptr_t*>(&(material->texture)), *reinterpret_cast<uintptr_t*>(&(material->texture)));
                material->texture = CVehicle::m_pVinyls[ pVeh->m_iVinylId ];
                material->color = {255, 255, 255, 255};
                return material;
            }
            return material;
        }
        if (color == 0xff00ffff)
        { // toner
            resetEntriesVehicle.emplace_back(reinterpret_cast<uintptr_t*>(&(material->color)), *reinterpret_cast<uintptr_t*>(&(material->color)));

            material->color = pVeh->tonerColor.ToRwRGBA();
            return material;
        }
    }
    return material;
}

uintptr_t CVehicle__SetupRenderCB(uintptr_t atomic, void* data)
{
    if (*(RpGeometry * *)(atomic + 24))
    {
        ((RpGeometry * (*)(RpGeometry*, uintptr_t, void*))(g_libGTASA + 0x00215F30 + 1))(*(RpGeometry * *)(atomic + 24), (uintptr_t)CVehicle__SetupRenderMatCB, (void*)data); // RpGeometryForAllMaterials
    }

    return atomic;
}

void (*CVehicleModelInfo__SetEditableMaterials)(uintptr_t);
void CVehicleModelInfo__SetEditableMaterials_hook(uintptr_t clump)
{
    RpClump *pClump = (RpClump *)clump;

    if (pNetGame && pClump)
    {
        if (pNetGame->GetVehiclePool())
        {
            VEHICLEID vehId = pNetGame->GetVehiclePool()->FindIDFromRwObject((RwObject *)clump);
            CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(vehId);
            if (pVehicle)
            {

                // RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* pData);
                ((RpClump * (*)(RpClump *, uintptr_t, void *))(g_libGTASA + 0x00213D66 + 1))(pClump, (uintptr_t)CVehicle__SetupRenderCB, (void *)pVehicle); // RpClumpForAllAtomics

            }
        }
    }

    CVehicleModelInfo__SetEditableMaterials(clump);
}

void (*CVehicle__ResetAfterRender)(uintptr_t);
void CVehicle__ResetAfterRender_hook(uintptr_t thiz)
{

    for (auto& p : resetEntriesVehicle)
    {
        *p.first = p.second;
    }
    resetEntriesVehicle.clear();

    CVehicle__ResetAfterRender(thiz);
}

#include "CRenderTarget.h"
#include "..//gui/CFontInstance.h"
#include "..//gui/CFontRenderer.h"
#include "RemoveBuildings.h"
#include "MaterialText.h"
#include "Pickups.h"
#include "CSnapShotWrapper.h"

void (*CGame__Process)();
void CGame__Process_hook()
{
    if(pGame->bIsGameExiting)return;
    MainLoop();
    //CGame::ProcessMainThreadTasks();
    uint32_t CurrentTimeInCycles;
    uint32_t v1; // r4
    uint32_t v2; // r5
    uint32_t v3; // r5

    //FIXME
    ((void(*)())(g_libGTASA + 0x003F8B50 + 1))(); // CPad::UpdatePads()
    ((void(*)())(g_libGTASA + 0x002B03F8 + 1))(); // CTouchInterface::Clear()
    ((void(*)())(g_libGTASA + 0x0028C178 + 1))(); // CHID::Update()

//	auto g_LoadMonitor = (uintptr_t *) (g_libGTASA + 0x009595EC);
//	((void(*)(uintptr_t*))(g_libGTASA + 0x003F4FB6 + 1))(g_LoadMonitor); // CLoadMonitor::BeginFrame

    CurrentTimeInCycles = CTimer::GetCurrentTimeInCycles();
    v1 = CurrentTimeInCycles / CTimer::GetCyclesPerMillisecond();

    ((void(*)())(g_libGTASA + 0x002D0558 + 1))(); // CStreaming::Update()

    v2 = CTimer::GetCurrentTimeInCycles();
    v3 = v2 / CTimer::GetCyclesPerMillisecond();

    //((void(*)())(g_libGTASA + 0x00390120 + 1))(); // CCutsceneMgr::Update();

    if ( !CTimer::m_CodePause )
    {
        auto gMobileMenu = (uintptr_t *) (g_libGTASA + 0x006E0074);
        ((void(*)(uintptr_t*))(g_libGTASA + 0x0029A730 + 1))(gMobileMenu); // MobileMenu::Update
    }

    // ((void(*)())(g_libGTASA + 0x0042CF54 + 1))(); // CTheZones::Update()
    ((void(*)())(g_libGTASA + 0x004D4F00 + 1))(); // CCover::Update()

    auto TheCamera = (uintptr_t *) (g_libGTASA + 0x00951FA8);

//	auto p_tx = (CSimpleTransform *)&TheCamera + 0x14 + 0x30;
//	if ( !TheCamera.m_pMat )
//		p_tx = *TheCamera + 0x4;

    //CAudioZones::Update(0, p_tx->m_translate);

    *(int32_t*)(g_libGTASA + 0x00A7D22C) = 0; // CWindModifiers::Number

    if ( !CTimer::m_CodePause && !CTimer::m_UserPause )
    {
        ((void (*)()) (g_libGTASA + 0x005C8884 + 1))(); // CSprite2d::SetRecipNearClip();
        ((void (*)()) (g_libGTASA + 0x005C89F8 + 1))(); // CSprite2d::InitPerFrame();
        ((void (*)()) (g_libGTASA + 0x005A8A74 + 1))(); // CFont::InitPerFrame()
//		((void (*)()) (g_libGTASA + 0x002FEB2C + 1))(); // CCheat::DoCheats();
//		((void (*)()) (g_libGTASA + 0x003E33C8 + 1))(); // CClock::Update()

        ((void (*)()) (g_libGTASA + 0x005CC2E8 + 1))(); // CWeather::Update()
        ((void(*)())(g_libGTASA + 0x0032AED8 + 1))(); // CTheScripts::Process()
        ((void(*)())(g_libGTASA + 0x002D9818 + 1))(); // CCollision::Update()

//		auto ThePaths = (uintptr_t *) (g_libGTASA + 0x007A61F0);
//		((void (*)(uintptr_t *)) (g_libGTASA + 0x003195F4 + 1))(ThePaths); // CPathFind::UpdateStreaming

//		CTrain::UpdateTrains();
//		CHeli::UpdateHelis();
        //((void(*)())(g_libGTASA + 0x003036B4 + 1))(); // CDarkel::Update()
        ((void(*)())(g_libGTASA + 0x005BE838 + 1))(); // CSkidmarks::Update();
        ((void(*)())(g_libGTASA + 0x005AB4C8 + 1))(); // CGlass::Update()
//		CWanted::UpdateEachFrame();
//		((void(*)())(g_libGTASA + 0x003F3640 + 1))(); // CCreepingFire::Update();
        ((void(*)())(g_libGTASA + 0x0035E4AC + 1))(); // CSetPieces::Update();

        auto gFireManager = (uintptr_t *) (g_libGTASA + 0x00958800);
        ((void (*)(uintptr_t *)) (g_libGTASA + 0x003F1628 + 1))(gFireManager); // CFireManager::Update


        ((void(*)(bool))(g_libGTASA + 0x004CC380 + 1))(false); // CPopulation::Update нужно (

        ((void (*)()) (g_libGTASA + 0x005DB8E8 + 1))(); // CWeapon::UpdateWeapons()
        // ((void (*)()) (g_libGTASA + 0x0059EB48 + 1))(); // CClouds::Update()
        ((void (*)()) (g_libGTASA + 0x005A6720 + 1))(); // CMovingThings::Update();
        ((void(*)())(g_libGTASA + 0x005CBB20 + 1))(); // CWaterCannons::Update()
        //((void (*)()) (g_libGTASA + 0x004213FC + 1))(); // CUserDisplay::Process()
        ((void (*)()) (g_libGTASA + 0x00427744 + 1))(); // CWorld::Process()

        if ( !CTimer::bSkipProcessThisFrame )
        {
            // ((void (*)()) (g_libGTASA + 0x0059CFC0 + 1))(); // CBirds::Update()
            //((void (*)()) (g_libGTASA + 0x0031D17C + 1))(); // CPickups::Update()
            CPickups::Update();
            ((void (*)()) (g_libGTASA + 0x005C03E4 + 1))(); // CSpecialFX::Update()
        }
//		auto g_InterestingEvents = *(uint8_t*)(g_libGTASA + 0x009EF9D8);
//		if ( *((uint8_t *)&g_InterestingEvents + 0x12C) << 0x1F )
//			((void (*)(uint8_t*)) (g_libGTASA + 0x004C1EC0 + 1))(&g_InterestingEvents); // CInterestingEvents::ScanForNearbyEntities

        ((void (*)()) (g_libGTASA + 0x005B28D8 + 1))(); // CPostEffects::Update()
        ((void (*)()) (g_libGTASA + 0x0041EF78 + 1))(); // CTimeCycle::Update()
        ((void (*)()) (g_libGTASA + 0x004CA834 + 1))(); // CPopCycle::Update()

        ((void (*)(uintptr_t *)) (g_libGTASA + 0x003DC7D0 + 1))(TheCamera); // CCamera::Process()

        //((void (*)()) (g_libGTASA + 0x005CF8D0 + 1))(); // CCullZones::Update()
        //((void (*)()) (g_libGTASA + 0x00307D8C + 1))(); // CGameLogic::Update() // раскоментить если нужна смерть как в сингле
        //CGangWars::Update();
        //((void (*)()) (g_libGTASA + 0x003006C4 + 1))(); // CConversations::Update()
        //((void (*)()) (g_libGTASA + 0x00301B68 + 1))(); // CPedToPlayerConversations::Update()
        //((void (*)()) (g_libGTASA + 0x002E388E + 1))(); // CBridge::Update()
        ((void (*)()) (g_libGTASA + 0x005A3E40 + 1))(); // CCoronas::DoSunAndMoon()
        ((void (*)()) (g_libGTASA + 0x005A22C8 + 1))(); // CCoronas::Update()
        ((void (*)()) (g_libGTASA + 0x005BD370 + 1))(); // CShadows::UpdatePermanentShadows()
        ((void (*)()) (g_libGTASA + 0x002CA3A4 + 1))(); // CCustomBuildingRenderer::Update()

        //
//		auto p_m_transform = (CSimpleTransform *)&TheCamera + 0x14 + 0x30;
//		if ( ! (uintptr_t*)(&TheCamera + 0x14) )
//			p_m_transform = (CSimpleTransform *)(&TheCamera + 0x4);

        //	((void (*)(CSimpleTransform*)) (g_libGTASA + 0x002CE120 + 1))(p_m_transform); // CPlantMgr::Update
        //

        auto g_fx = *(uintptr_t *) (g_libGTASA + 0x00820520);
        auto temp = *(uintptr_t*)&TheCamera + 0x8D8;
        ((void (*)(uintptr_t*, uintptr_t, float )) (g_libGTASA + 0x00363DE0 + 1))(&g_fx, temp, CTimer::ms_fTimeStep / 50.0f); // Fx_c::Update

        auto g_breakMan = (uintptr_t *) (g_libGTASA + 0x0099DD14);
        ((void (*)(uintptr_t*, float )) (g_libGTASA + 0x0045267C + 1))(g_breakMan, CTimer::ms_fTimeStep); // BreakManager_c::Update

        auto g_procObjMan = (uintptr_t *) (g_libGTASA + 0x009A1F3C);
        ((void (*)(uintptr_t* )) (g_libGTASA + 0x004571A0 + 1))(g_procObjMan); // ProcObjectMan_c::Update

        auto g_waterCreatureMan = (uintptr_t *) (g_libGTASA + 0x00A14350);
        ((void (*)(uintptr_t*, float )) (g_libGTASA + 0x004571A0 + 1))(g_waterCreatureMan, CTimer::ms_fTimeStep / 50.0f); // WaterCreatureManager_c::Update

        ((void (*)()) (g_libGTASA + 0x00596540 + 1))(); // CWaterLevel::PreRenderWater()
    }
//

    static bool once = false;
    if (!once)
    {
        CRenderTarget::Initialise();
//		CCustomPlateManager::Initialise();

        // hide load
        JNIEnv* env = g_pJavaWrapper->GetEnv();
        jclass clazz = env->GetObjectClass(g_pJavaWrapper->activity);
        jmethodID method = env->GetStaticMethodID(clazz, "hideBackGroundSplash", "()V");
        env->CallStaticVoidMethod(clazz, method);

        once = true;
    }else{
        CSnapShotWrapper::Process();
    }

//	CCustomPlateManager::Process();

//	CGame__Process();
}

uint16_t g_usLastProcessedModelIndexAutomobile = 0;
int g_iLastProcessedModelIndexAutoEnt = 0;
int32_t (*CAutomobile__ProcessEntityCollision)(CVehicleGta* a1, CEntityGta* a2, uintptr_t* aColPoints);
int32_t CAutomobile__ProcessEntityCollision_hook(CVehicleGta* a1, CEntityGta* a2, uintptr_t* aColPoints)
{
    int32_t res;

    g_usLastProcessedModelIndexAutomobile = a1->nModelIndex;
    g_iLastProcessedModelIndexAutoEnt = a2->nModelIndex;

    bool bReplace = false;
    void* pOld = nullptr;
    uint8_t* pColData = nullptr;

    uint16_t vehId = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(a1);
    CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehId);
    if (pVeh) {
        if (pVeh->bHasSuspensionLines && pVeh->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR) {
            pColData = GetCollisionDataFromModel(a1->nModelIndex);
            if (pColData && pVeh->m_pSuspensionLines) {
                if (*(void **) (pColData + 16)) {
                    pOld = *(void **) (pColData + 16);
                    *(void **) (pColData + 16) = pVeh->m_pSuspensionLines;
                    bReplace = true;
                }
            }
        }
    }

    res = CAutomobile__ProcessEntityCollision(a1, a2, aColPoints);
    if (bReplace)
    {
        *(void**)(pColData + 16) = pOld;
    }
    return res;
}

void (*MainMenuScreen__OnExit)();
void MainMenuScreen__OnExit_hook()
{
    *(bool*)(g_libGTASA + 0x009FC8FC + 0x10) = true; // RsGlobal.quit

    pGame->exitGame();

    //g_pJavaWrapper->ExitGame();

    //return CGame__Shutdown();
}

// TODO: VEHICLE RESET SUSPENSION
void (*CShadows__StoreCarLightShadow)(CVehicleGta* vehicle, int id, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle);
void CShadows__StoreCarLightShadow_hook(CVehicleGta* vehicle, int id, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle)
{
    uint8_t r, g, b;
    r = red;
    g = green;
    b = blue;
    if (pNetGame)
    {
        if (pNetGame->GetVehiclePool())
        {
            uint16_t vehid = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(vehicle);
            CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
            if (pVeh)
            {
                pVeh->ProcessHeadlightsColor(r, g, b);
            }
        }
    }

    CShadows__StoreCarLightShadow(vehicle, id, texture, posn, frontX, frontY, sideX, sideY, r, g, b, maxViewAngle);
}

void CVehicle__DoHeadLightReflectionTwin(CVehicle* pVeh, CMatrix* matVehicle)
{
    CVehicleGta* v2; // r4
    int v3; // r2
    CMatrix* v4; // r5
    float* v5; // r3
    float v6; // s12
    float v7; // s5
    float* v8; // r2
    float v9; // r0
    float v10; // r1
    float v11; // r2
    float v12; // s14
    float v13; // s11
    float v14; // s15
    float v15; // s13
    float v16; // s10
    float v17; // s12
    float v18; // s15
    float v19; // ST08_4


    v2 = pVeh->m_pVehicle;
    v3 = *((uintptr_t*)v2 + 5);
    v4 = matVehicle;
    v5 = *(float**)(CModelInfo::ms_modelInfoPtrs[v2->nModelIndex] + 116);
    v6 = *v5;
    v7 = v5[1];
    if (v3)
        v8 = (float*)(v3 + 48);
    else
        v8 = (float*)((char*)v2 + 4);
    v9 = *v8;
    v10 = v8[1];
    v11 = v8[2];
    v12 = *((float*)v4 + 5);
    v13 = *((float*)v4 + 4);
    v14 = (float)(v12 * v12) + (float)(v13 * v13);
    if (v14 != 0.0)
        v14 = 1.0 / sqrtf(v14);
    v15 = v6 * 4.0;
    v16 = (float)(v15 + v15) + 1.0;
    v17 = v13 * v14;
    v18 = v12 * v14;

    v19 = v15 * v18;

    CVector pos;
    memcpy(&pos, &(v2->mat->pos), sizeof(CVector));
    pos.z += 2.0f;

    CShadows__StoreCarLightShadow(
            v2,
            (uintptr_t)v2 + 24,
            pVeh->m_Shadow.pTexture,
            &pos,
            (float)(v15 + v15) * v17 * pVeh->m_Shadow.fSizeX,
            (float)(v15 + v15) * v18 * pVeh->m_Shadow.fSizeX,
            v19 * pVeh->m_Shadow.fSizeY,
            -(float)(v15 * v17) * pVeh->m_Shadow.fSizeY,
            pVeh->m_Shadow.r, pVeh->m_Shadow.g, pVeh->m_Shadow.b,
            7.0f);
}

void (*CVehicle__DoHeadLightBeam)(CVehicleGta* vehicle, int32_t lightId, RwMatrix* matrix, bool isRight);
void CVehicle__DoHeadLightBeam_hook(CVehicleGta* vehicle, int32_t lightId, RwMatrix* matrix, bool isRight)
{
    uint8_t r, g, b;
    r = 0xFF;
    g = 0xFF;
    b = 0xFF;
    if (pNetGame)
    {
        if (pNetGame->GetVehiclePool())
        {
            uint16_t vehid = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(vehicle);
            CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
            if (pVeh)
            {
                pVeh->ProcessHeadlightsColor(r, g, b);
            }
        }
    }

    *(uint8_t*)(g_libGTASA + 0x9BAA70) = r;
    *(uint8_t*)(g_libGTASA + 0x9BAA71) = g;
    *(uint8_t*)(g_libGTASA + 0x9BAA72) = b;

    *(uint8_t*)(g_libGTASA + 0x9BAA94) = r;
    *(uint8_t*)(g_libGTASA + 0x9BAA95) = g;
    *(uint8_t*)(g_libGTASA + 0x9BAA96) = b;

    *(uint8_t*)(g_libGTASA + 0x9BAB00) = r;
    *(uint8_t*)(g_libGTASA + 0x9BAB01) = g;
    *(uint8_t*)(g_libGTASA + 0x9BAB02) = b;

    CVehicle__DoHeadLightBeam(vehicle, lightId, matrix, isRight);

}

static CVehicle* g_pLastProcessedVehicleMatrix = nullptr;
static int g_iLastProcessedWheelVehicle = -1;

void (*CMatrix__Rotate)(void* thiz, float a1, float a2, float a3);
void CMatrix__Rotate_hook(void* thiz, float a1, float a2, float a3)
{
    uintptr_t dwRetAddr = 0;
    __asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
    dwRetAddr -= g_libGTASA;

    if (dwRetAddr == 0x003A9D76 + 1)
    {
        CMatrix__Rotate(thiz, a1, a2, a3);
        return;
    }

    CMatrix__Rotate(thiz, a1, a2, a3);
    if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
    {
        if (g_pLastProcessedVehicleMatrix->m_bWheelAlignmentX || g_pLastProcessedVehicleMatrix->m_bWheelAlignmentY)
        {
            if (g_iLastProcessedWheelVehicle == 2)
            {
                ((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
            }
            if (g_iLastProcessedWheelVehicle == 4)
            {
                ((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
            }
            if (g_iLastProcessedWheelVehicle == 5)
            {
                ((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
            }
            if (g_iLastProcessedWheelVehicle == 7)
            {
                ((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
            }
        }
    }
}

void (*CMatrix__SetScale)(void* thiz, float x, float y, float z);
void CMatrix__SetScale_hook(void* thiz, float x, float y, float z)
{
    if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
    {
        if (g_iLastProcessedWheelVehicle >= 2 || g_iLastProcessedWheelVehicle <= 7)
        {
            // front wheel
            if (g_pLastProcessedVehicleMatrix->m_bWheelSize)
            {
                y *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3f;
                z *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3f;
            }
            if (g_pLastProcessedVehicleMatrix->m_bWheelWidth)
            {
                x = g_pLastProcessedVehicleMatrix->m_fWheelWidth;
            }
        }
    }

    CMatrix__SetScale(thiz, x, y, z);
}

void (*CAutomobile__UpdateWheelMatrix)(CVehicleGta* thiz, int, int);
void CAutomobile__UpdateWheelMatrix_hook(CVehicleGta* thiz, int nodeIndex, int flags)
{
    if (g_pLastProcessedVehicleMatrix)
    {
        g_iLastProcessedWheelVehicle = nodeIndex;
    }

    CAutomobile__UpdateWheelMatrix(thiz, nodeIndex, flags);
}

void (*CAutomobile__PreRender)(CVehicleGta* thiz);
void CAutomobile__PreRender_hook(CVehicleGta* thiz)
{
    CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[thiz->nModelIndex]);

    float fOldFront = pModelInfoStart->m_fWheelSizeFront;
    float fOldRear = pModelInfoStart->m_fWheelSizeRear;

    if (pNetGame->GetVehiclePool()) {
        uint16_t vehid = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(thiz);
        auto pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
        if (pVeh) {
            pVeh->ProcessWheelsOffset();
            g_pLastProcessedVehicleMatrix = pVeh;

            if (pVeh->m_bWheelSize)
            {
                pModelInfoStart->m_fWheelSizeFront = pVeh->m_fWheelSize;
                pModelInfoStart->m_fWheelSizeRear = pVeh->m_fWheelSize;
            }
            if (pVeh->m_bShadow && pVeh->m_Shadow.pTexture) {
                CVehicle__DoHeadLightReflectionTwin(pVeh, pVeh->m_pVehicle->mat);
            }
        }
    }

    CAutomobile__PreRender(thiz);

    pModelInfoStart->m_fWheelSizeFront = fOldFront;
    pModelInfoStart->m_fWheelSizeRear = fOldRear;

    g_pLastProcessedVehicleMatrix = nullptr;
    g_iLastProcessedWheelVehicle = -1;
}

void (*CCam__Process)(uintptr_t);
void CCam__Process_hook(uintptr_t thiz)
{
    if(!CFirstPersonCamera::IsEnabled()){
        CCam__Process(thiz);
        return;
    }

    CVector vecSpeed;
    CVehicle* pVeh = nullptr;
    //float pOld = *(float*)(g_libGTASA + 0x00608558);
    float v6 = *(float *)(g_libGTASA + (VER_x32 ? 0x6A9FD0 : 0x8855D4));
    if (pNetGame && (*(uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16))
    {
        if (pNetGame->GetPlayerPool())
        {
            if (pNetGame->GetPlayerPool()->GetLocalPlayer())
            {
                CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
                pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
                if (pVeh)
                {
                    pVeh->GetMoveSpeedVector(&vecSpeed);
                    CVector vec;
                    vec.x = vecSpeed.x * 6.0f;
                    vec.y = vecSpeed.y * 6.0f;
                    vec.z = vecSpeed.z * 6.0f;
                    pVeh->SetMoveSpeedVector(vec);
                    *(float *)(g_libGTASA + (VER_x32 ? 0x6A9FD0 : 0x8855D4)) = 200.0f;
                    //	*(float*)(g_libGTASA + 0x00608558) = 200.0f;
                }
            }
        }
    }
    *(float *)(g_libGTASA + (VER_x32 ? 0x6A9FD0 : 0x8855D4)) = 200.0f;
    CCam__Process(thiz);

    if (pVeh)
    {
        pVeh->SetMoveSpeedVector(vecSpeed);
        *(float *)(g_libGTASA + (VER_x32 ? 0x6A9FD0 : 0x8855D4)) = v6;
        //*(float*)(g_libGTASA + 0x00608558) = pOld;
    }
    if (*(uint16_t*)(thiz + 14) == 4 || *(uint16_t*)(thiz + 14) == 53) // 53 is weapon
    {
        if (pNetGame)
        {
            if (pNetGame->GetPlayerPool())
            {
                if (pNetGame->GetPlayerPool()->GetLocalPlayer())
                {
                    CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
                    if (pPed)
                    {
#if VER_x32
                        *(uint32_t*)(g_libGTASA + 0x00951FA8 + 120) = 0xFFFFFFFF;
                        *(uint32_t*)(g_libGTASA + 0x00951FA8 + 124) = 0xFFFFFFFF;
                        *(uint8_t*)(g_libGTASA + 0x00951FA8 + 40) = 0;
#else
                        *(uint32_t*)(g_libGTASA + 0xBBA8D0 + 128) = 0xFFFFFFFFFFFFFFFFLL;
						*(uint8_t*)(g_libGTASA + 0xBBA8D0 + 48) = 0;
#endif
                        CFirstPersonCamera::ProcessCameraOnFoot(thiz, pPed);
                    }
                }
            }
        }
    }
    if(* (uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) // in car
    {
        if (pNetGame)
        {
            if (pNetGame->GetPlayerPool())
            {
                if (pNetGame->GetPlayerPool()->GetLocalPlayer())
                {
                    CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
                    if (pPed)
                    {
#if VER_x32
                        *(uint32_t*)(g_libGTASA + 0x00951FA8 + 120) = 0xFFFFFFFF;
                        *(uint32_t*)(g_libGTASA + 0x00951FA8 + 124) = 0xFFFFFFFF;
                        *(uint8_t*)(g_libGTASA + 0x00951FA8 + 40) = 0;
#else
                        *(uint32_t*)(g_libGTASA + 0xBBA8D0 + 128) = 0xFFFFFFFFFFFFFFFFLL;
                        *(uint8_t*)(g_libGTASA + 0xBBA8D0 + 48) = 0;
#endif

                        CFirstPersonCamera::ProcessCameraInVeh(thiz, pPed, pVeh);
                    }
                }
            }
        }
    }
}
int g_iCounterVehicleCamera = 0;
int (*CPad__CycleCameraModeDownJustDown)(void*);
int CPad__CycleCameraModeDownJustDown_hook(void* thiz)
{
    if (!g_pWidgetManager)
    {
        return 0;
    }

    CPedGta* pPed = GamePool_FindPlayerPed();
    if (!pPed)
    {
        return 0;
    }

    static uint32_t lastTick = GetTickCount();
    bool bPressed = false;
    if (CHUD::bIsTouchCameraButt && GetTickCount() - lastTick >= 250)
    {
        CHUD::bIsTouchCameraButt = false;
        bPressed = true;
        lastTick = GetTickCount();
    }

    if (pPed->bInVehicle)
    {
        if (bPressed)
        {
            g_iCounterVehicleCamera++;
        }
        if (g_iCounterVehicleCamera == 6)
        {
            CFirstPersonCamera::SetEnabled(true);
            return 0;
        }
        else if (g_iCounterVehicleCamera >= 7)
        {
            g_iCounterVehicleCamera = 0;
            CFirstPersonCamera::SetEnabled(false);
            return 1;

        }
        else
        {
            CFirstPersonCamera::SetEnabled(false);
        }

        return bPressed;
    }
    return 0;
}

int g_iLastProcessedSkinCollision = 228;
int g_iLastProcessedEntityCollision = 228;

int32_t (*CPed__ProcessEntityCollision)(CPedGta* thiz, CEntityGta* ent, void* colPoint);
int32_t CPed__ProcessEntityCollision_hook(CPedGta* thiz, CEntityGta* ent, void* colPoint)
{
    g_iLastProcessedSkinCollision = thiz->nModelIndex;
    g_iLastProcessedEntityCollision = ent->nModelIndex;

    return CPed__ProcessEntityCollision(thiz, ent, colPoint);
}

void (*CDraw__SetFOV)(float fFOV, bool isCinematic);
void CDraw__SetFOV_hook(float fFOV, bool isCinematic)
{
    float tmp = (float)((float)((float)(*(float *)(g_libGTASA + 0xA26A90) + -1.3333) * 11.0) / 0.44444) + fFOV;
    if(tmp > 100) tmp = 100.0;
    *(float *)(g_libGTASA + 0x006B1CB8) = tmp;
}

int (*CWeapon__GenerateDamageEvent)(CPedGta *victim, CEntityGta *creator, unsigned int weaponType, int damageFactor, ePedPieceTypes pedPiece, int direction);
int CWeapon__GenerateDamageEvent_hook(CPedGta *victim, CEntityGta *creator, unsigned int weaponType, int damageFactor, ePedPieceTypes pedPiece, int direction)
{
    if (pedPiece != PED_PIECE_HEAD)
    {
        // Disable ComputeDamageAnim when it's not a head!
        CHook::NOP(g_libGTASA + 0x566B30, 2);
        CHook::NOP(g_libGTASA + 0x566B88, 2);
    }
    int result = CWeapon__GenerateDamageEvent(victim, creator, weaponType, damageFactor, pedPiece, direction);
    if (pedPiece != PED_PIECE_HEAD)
    {
        // Restore bytes
        CHook::WriteMemory2(g_libGTASA + 0x566B30, (uintptr_t) "\xC1\xF5\xD8\xF9", 4);
        CHook::WriteMemory2(g_libGTASA + 0x566B88, (uintptr_t) "\xBB\xF5\xFE\xF8", 4);
    }
    return result;
}


int (*CTaskSimpleUseGun__SetPedPosition)(uintptr_t thiz, uintptr_t a2);
int CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, uintptr_t a2)
{
    unsigned char v1 = *((unsigned char*)thiz + 13);
    bool bChangeTheResult = false;

    CPedGta* pPedPlayer = (CPedGta*)a2;
    if(pPedPlayer && pNetGame)
    {
        if(v1 == 0)
        {
            CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
            if(pPlayerPool)
            {
                CPlayerPed *pPlayerPed;
                if(pPedPlayer == GamePool_FindPlayerPed())
                    pPlayerPed = pGame->FindPlayerPed();
                else
                {
                    PLAYERID playerId = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPedPlayer);
                    if(playerId != INVALID_PLAYER_ID)
                    {
                        CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
                        if(pRemotePlayer)
                            pPlayerPed = pRemotePlayer->GetPlayerPed();
                    }
                }

                if(pPlayerPed)
                {
                    if((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
                    {
                        CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
                        if(pVehiclePool)
                        {
                            for(const auto & pVehicle : pVehiclePool->m_pVehicles)
                            {
                                if(pVehicle == nullptr || pVehicle->m_pVehicle) continue;

                                RwMatrix vehicleMat, playerMat;
                                pVehicle->GetMatrix(&vehicleMat);
                                pPlayerPed->GetMatrix(&playerMat);

                                float fSX = (vehicleMat.pos.x - playerMat.pos.x) * (vehicleMat.pos.x - playerMat.pos.x);
                                float fSY = (vehicleMat.pos.y - playerMat.pos.y) * (vehicleMat.pos.y - playerMat.pos.y);
                                float fSZ = (vehicleMat.pos.z - playerMat.pos.z) * (vehicleMat.pos.z - playerMat.pos.z);

                                auto fDistance = sqrt(fSX + fSY + fSZ);

                                if(fDistance <= 100.0f)
                                    *((unsigned char*)thiz + 13) |= 1;
                            }
                        }
                    }
                }
            }
        }
    }

    return CTaskSimpleUseGun__SetPedPosition(thiz, a2);
}

bool CVehicle__GetVehicleLightsStatus(CVehicleGta *_pVehicle)
{
    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

    CVehicle *pVehicle = pVehiclePool->FindVehicle(_pVehicle);
    if(!pVehicle)return false;

    return pVehicle->m_bLightsOn;
}

uint32_t (*CWeapon__FireSniper)(CWeaponGta* pWeaponSlot, CPedGta *pFiringEntity, CEntityGta *a3, CVector *vecOrigin);
uint32_t CWeapon__FireSniper_hook(CWeaponGta* pWeaponSlot, CPedGta *pFiringEntity, CEntityGta *a3, CVector *vecOrigin)
{
    if(GamePool_FindPlayerPed() == pFiringEntity)
    {
        if(pGame)
        {
            CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
            if(pPlayerPed)
                pPlayerPed->FireInstant();
        }
    }

    return 1;
}

void SendBulletSync(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CEntityGta **ppEntity)
{
    BULLET_DATA bulletData;
    memset(&bulletData, 0, sizeof(BULLET_DATA));

    bulletData.vecOrigin.x = vecOrigin->x;
    bulletData.vecOrigin.y = vecOrigin->y;
    bulletData.vecOrigin.z = vecOrigin->z;

    bulletData.vecPos.x = vecPos->x;
    bulletData.vecPos.y = vecPos->y;
    bulletData.vecPos.z = vecPos->z;

    if(ppEntity)
    {
        static CEntityGta *pEntity;
        pEntity = *ppEntity;
        if(pEntity)
        {
            if(pEntity->mat)
            {
                if(pNetGame->m_iLagCompensation)
                {
                    bulletData.vecOffset.x = vecPos->x - pEntity->mat->pos.x;
                    bulletData.vecOffset.y = vecPos->y - pEntity->mat->pos.y;
                    bulletData.vecOffset.z = vecPos->z - pEntity->mat->pos.z;
                }
                else
                {
                    static RwMatrix mat1;
                    memset(&mat1, 0, sizeof(mat1));

                    static RwMatrix mat2;
                    memset(&mat2, 0, sizeof(mat2));

                    RwMatrixOrthoNormalize(&mat2, pEntity->mat->ToRwMatrix());
                    RwMatrixInvert(&mat1, &mat2);
                    ProjectMatrix(&bulletData.vecOffset, &mat1, vecPos);
                }
            }

            bulletData.pEntity = pEntity;
        }
    }

    pGame->FindPlayerPed()->ProcessBulletData(&bulletData);

}

bool g_bForceWorldProcessLineOfSight = false;
uint32_t (*CWeapon__ProcessLineOfSight)(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, CWeaponGta *pWeaponSlot, CPedGta **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7);
uint32_t CWeapon__ProcessLineOfSight_hook(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, CWeaponGta *pWeaponSlot, CPedGta **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7)
{
    uintptr_t dwRetAddr = 0;
    __asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
    dwRetAddr -= g_libGTASA;

    if(dwRetAddr == 0x005DC7F0+1 || dwRetAddr == 0x005DC620+1 || dwRetAddr == 0x005DD0B0+1 || dwRetAddr == 0x005DCD56+1)
        g_bForceWorldProcessLineOfSight = true;

    return CWeapon__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, pWeaponSlot, ppEntity2, b1, b2, b3, b4, b5, b6, b7);
}

uint32_t (*CWorld__ProcessLineOfSight)(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8);
uint32_t CWorld__ProcessLineOfSight_hook(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
    uintptr_t dwRetAddr = 0;
    __asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
    dwRetAddr -= g_libGTASA;

    if(dwRetAddr == 0x5dd0b0 + 1 || g_bForceWorldProcessLineOfSight)
    {
        g_bForceWorldProcessLineOfSight = false;

        CEntityGta *pEntity = nullptr;
        CVector vecPosPlusOffset;

        if(pNetGame->m_iLagCompensation != 2)
        {
            if(g_pCurrentFiredPed != pGame->FindPlayerPed())
            {
                if(g_pCurrentBulletData)
                {
                    if(g_pCurrentBulletData->pEntity)
                    {
                        if(!CUtil::IsGameEntityArePlaceable(g_pCurrentBulletData->pEntity))
                        {
                            auto pMatrix = g_pCurrentBulletData->pEntity->mat;
                            if(pMatrix)
                            {
                                if(pNetGame->m_iLagCompensation)
                                {
                                    vecPosPlusOffset.x = pMatrix->pos.x + g_pCurrentBulletData->vecOffset.x;
                                    vecPosPlusOffset.y = pMatrix->pos.y + g_pCurrentBulletData->vecOffset.y;
                                    vecPosPlusOffset.z = pMatrix->pos.z + g_pCurrentBulletData->vecOffset.z;
                                }
                                else ProjectMatrix(&vecPosPlusOffset, pMatrix->ToRwMatrix(), &g_pCurrentBulletData->vecOffset);

                                vecEnd->x = vecPosPlusOffset.x - vecOrigin->x + vecPosPlusOffset.x;
                                vecEnd->y = vecPosPlusOffset.y - vecOrigin->y + vecPosPlusOffset.y;
                                vecEnd->z = vecPosPlusOffset.z - vecOrigin->z + vecPosPlusOffset.z;
                            }
                        }
                    }
                }
            }
        }

        uint32_t result = 0;
        result = CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

        if(pNetGame->m_iLagCompensation == 2)
        {
            if(g_pCurrentFiredPed)
            {
                if(g_pCurrentFiredPed == pGame->FindPlayerPed())
                    SendBulletSync(vecOrigin, vecEnd, vecPos, (CEntityGta**)ppEntity);
            }

            return result;
        }

        if(g_pCurrentFiredPed)
        {
            if(g_pCurrentFiredPed != pGame->FindPlayerPed())
            {
                if(g_pCurrentBulletData)
                {
                    if(!g_pCurrentBulletData->pEntity)
                    {
                        CPedGta *pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
                        if(*ppEntity == pLocalPed || (CVehicleGta*)ppEntity == (CVehicleGta*)pLocalPed->pVehicle)
                        {
                            *ppEntity = nullptr;
                            vecPos->x = 0.0f;
                            vecPos->y = 0.0f;
                            vecPos->z = 0.0f;
                            return 0;
                        }
                    }
                }
            }
        }

        if(g_pCurrentFiredPed)
        {
            if(g_pCurrentFiredPed == pGame->FindPlayerPed())
                SendBulletSync(vecOrigin, vecEnd, vecPos, (CEntityGta **)ppEntity);
        }

        return result;
    }

    return CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
}

signed int (*CBulletInfo_AddBullet)(CEntityGta* pEntity, CWeaponGta* pWeapon, CVector vec1, CVector vec2);
signed int CBulletInfo_AddBullet_hook(CEntityGta* pEntity, CWeaponGta* pWeapon, CVector vec1, CVector vec2)
{
    vec2.x *= 50.0f;
    vec2.y *= 50.0f;
    vec2.z *= 50.0f;
    CBulletInfo_AddBullet(pEntity, pWeapon, vec1, vec2);
    // CBulletInfo::Update
    (( void (*)())(g_libGTASA+0x005D7094+1))();
    return 1;
}

bool (*CEventKnockOffBike__AffectsPed)(uintptr_t *thiz, CPedGta *a2);
bool CEventKnockOffBike__AffectsPed_hook(uintptr_t *thiz, CPedGta *a2)
{
    return false;
}



uintptr_t* (*rpMaterialListDeinitialize)(RpMaterialList* matList);
uintptr_t* rpMaterialListDeinitialize_hook(RpMaterialList* matList)
{
    if(!matList || !matList->materials)
        return nullptr;

    return rpMaterialListDeinitialize(matList);
}

uintptr_t* (*ConvertBufferToObject)(uint8_t* fileBuffer, int32_t modelId);
uintptr_t* ConvertBufferToObject_hook(uint8_t* fileBuffer, int32_t modelId)
{
    // Log("ConvertBufferToObject = %d", modelId);

    return ConvertBufferToObject(fileBuffer, modelId);
}

bool(*LoadTextures)(uintptr_t* thiz, char* tex, int32_t ver);
bool LoadTextures_hook(uintptr_t* thiz, char* tex, int32_t ver)
{
    Log("LoadTextures fx = %s", tex);

    return LoadTextures(thiz, tex, ver);
}

void (*CEntity__Render)(CEntityGta *thiz);
void CEntity__Render_hook(CEntityGta *thiz)
{
    if (CSkyBox::GetSkyObject())
    {
        if (CSkyBox::GetSkyObject()->m_pEntity == thiz && !CSkyBox::IsNeedRender())
            return;
    }
    if(!CUtil::IsGameEntityArePlaceable(thiz))
    {
        if (thiz->m_nType == ENTITY_TYPE_BUILDING || thiz->m_nType == ENTITY_TYPE_OBJECT ||
            thiz->m_nType == ENTITY_TYPE_DUMMY)
        {
            if (!CRemoveBuildings::isNeedRender(thiz)) {
                thiz->nEntityFlags.m_bUsesCollision = 0;
                thiz->nEntityFlags.m_bCollisionProcessed = 0;
                return;
            }
        }
    }
    return CEntity__Render(thiz);
}

#include "Scene.h"

RpMaterial* ObjectMaterialCallBack(RpMaterial* material, void* data)
{
    CObject* pObject = (CObject*)data;
    RpAtomic* rpAtomic = (RpAtomic*)pObject->m_pEntity->m_pRpAtomic;
    for (int i = 0; i < rpAtomic->geometry->matList.numMaterials; i++)
    {
        if(i >= 16) break;
        if(rpAtomic->geometry->matList.materials[i] == material)
        {
            if(pObject->m_MaterialTexture[i]) material->texture = pObject->m_MaterialTexture[i];

            if(pObject->m_dwMaterialColor[i]) {
                RwRGBA *r = (RwRGBA *) &pObject->m_dwMaterialColor[i];
                // r.alpha = 0;

                rpAtomic->geometry->matList.materials[i]->color = *r;
                rpAtomic->geometry->matList.materials[i]->surfaceProps.ambient = 1.0f;
                rpAtomic->geometry->matList.materials[i]->surfaceProps.specular = 0.0f;
                rpAtomic->geometry->matList.materials[i]->surfaceProps.diffuse = 1.0f;
            }
            break;
        }
    }

    return material;
}

RpAtomic* ObjectMaterialTextCallBack(RpAtomic* rpAtomic, CObject* pObject)
{
    if(!pObject->m_MaterialTextTexture || rpAtomic->object.object.type != 1) return rpAtomic;

    int iTotalEntries = rpAtomic->geometry->matList.numMaterials;
    if (iTotalEntries > 16) iTotalEntries = 16; // fix fucking bug :|
    for (int i = 0; i < iTotalEntries; i++)
    {
        if(pObject->m_MaterialTextTexture[i])
        {
            rpAtomic->geometry->matList.materials[i]->texture = pObject->m_MaterialTextTexture[i];
        }
    }

    return rpAtomic;
}

void (*CObject__Render)(CEntityGta* object);
void CObject__Render_hook(CEntityGta* object)
{
    if(pNetGame && object != nullptr)
    {
        CObject *pObject = pNetGame->GetObjectPool()->GetObjectFromGtaPtr(object);
        if(pObject)
        {
            auto rwObject = pObject->m_pEntity->m_pRwObject;
            if(rwObject) {
                // SetObjectMaterial
                if(pObject->m_bHasMaterial)
                {
                    CHook::CallFunction<void>(OBFUSCATE("_Z21DeActivateDirectionalv"));
                    //RwFrameForAllObjects((RwFrame*)rwObject->parent, (RwObject *(*)(RwObject *, void *))ObjectMaterialCallBack, pObject);
                    RpAtomic* atomic = (RpAtomic*)object->m_pRpAtomic;

                    atomic->geometry->flags = atomic->geometry->flags & 0xFFFFFFB7 | 0x40;
                    //atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
                    RpGeometryForAllMaterials(atomic->geometry, ObjectMaterialCallBack, (void*)pObject);


                }
                // SetObjectMaterialText
                if(pObject->m_bHasMaterialText)
                    RwFrameForAllObjects((RwFrame*)rwObject->parent, (RwObject *(*)(RwObject *, void *))ObjectMaterialTextCallBack, pObject);
            }


        }
    }

    CHook::CallFunction<void>(OBFUSCATE("_Z21DeActivateDirectionalv")); // DeActivateDirectional()
    CObject__Render(object);
    CHook::CallFunction<void>(OBFUSCATE("_Z19ActivateDirectionalv")); // activate
}

int (*CCollision__ProcessVerticalLine)(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8);
int CCollision__ProcessVerticalLine_hook(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8)
{
    int result; // r0

    if (a3)
        result = CCollision__ProcessVerticalLine(a1, a2, a3, a4, a5, a6, a7, a8);
    else
        result = 0;
    return result;
}
bool(*CUpsideDownCarCheck__IsCarUpsideDown);
bool CUpsideDownCarCheck__IsCarUpsideDown_hook()
{
    return 0;
}
bool PlayerIsEnteringCar()
{
    return 0;
}
int64 getmip()
{
    return 1;
}
uint64_t* RQCommand_rqSetAlphaTest(uint64_t *result)
{
    *result += 8;
    return result;
}
void (*CTaskSimpleUseGun__RemoveStanceAnims)(uintptr* thiz, void* ped, float a3);
void CTaskSimpleUseGun__RemoveStanceAnims_hook(uintptr* thiz, void* ped, float a3)
{
    if(!thiz)
        return;

    uintptr* m_pAnim = (uintptr*)(thiz + (VER_x32 ? 0x2c : 0x40));
    if(m_pAnim) {
        if (!((uintptr *)(m_pAnim + (VER_x32 ? 0x14 : 0x18))))
            return;
    }
    CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
}
bool(*CTaskSimpleGetUp__ProcessPed)(uintptr_t*, CPedGta*);
bool CTaskSimpleGetUp__ProcessPed_hook(uintptr_t *thiz, CPedGta *ped)
{
    if(!ped)return 0;
    int res = 0;
    try {
        res = CTaskSimpleGetUp__ProcessPed(thiz, ped);
    }
    catch(...) {
        return 0;
    }

    return res;
}
void (*CRadar__ClearBlip)(int32 index);
void CRadar__ClearBlip_hook(int32 index) {
    if ( *(int32 *)(g_libGTASA + (VER_x32 ? 0x6E00BC : 0x8BE7E8)) == index ) {
        int32 v1 = index;
        if ( pGame->m_bRaceCheckpointsEnabled )
            GPS::Set(pGame->m_vecRaceCheckpointPos, true);
        else
            GPS::Set(pGame->m_vecRaceCheckpointPos, false);
        index = v1;
    }
    CRadar__ClearBlip(index);
}
void InstallHooks()
{
    Log("InstallHooks");
    CHook::RET(OBFUSCATE("_ZN7CObject19ProcessControlLogicEv"));
    //remove building
#if VER_x32
    CHook::installPLTHook(g_libGTASA + 0x66F76C, (uintptr_t)CEntity__Render_hook, (uintptr_t*)&CEntity__Render);
#else
    CHook::installPLTHook(g_libGTASA + 0x83F610, (uintptr_t)CEntity__Render_hook, (uintptr_t*)&CEntity__Render);
#endif
    // retexture

    CHook::HookFunc(OBFUSCATE("_ZN7CObject6RenderEv"), CObject__Render_hook, &CObject__Render);
    //CHook::HookFunc("_ZN8CVehicle20AddWheelDirtAndWaterER9CColPointjhh", AddWheelDirtAndWater_hook, &AddWheelDirtAndWater_orig);
    CHook::HookFunc(OBFUSCATE("_ZN19CUpsideDownCarCheck15IsCarUpsideDownEPK8CVehicle"), CUpsideDownCarCheck__IsCarUpsideDown_hook, &CUpsideDownCarCheck__IsCarUpsideDown);
    CHook::HookRedirect(OBFUSCATE("_Z19PlayerIsEnteringCarv"), PlayerIsEnteringCar);
    // CHook::HookRedirect(OBFUSCATE("_Z13RenderEffectsv"), RenderEffects);

    if(*(uint8_t *)(g_libGTASA + (VER_x32 ? 0x6B8B9C:0x896135)))
    {
        CHook::HookRedirect("_ZNK14TextureListing11GetMipCountEv", &getmip);
    }
    //CHook::InlineHook(g_libGTASA, 0x0046DF1C, &LoadTextures_hook, &LoadTextures);
    //CHook::HookFunc("_ZN10CStreaming21ConvertBufferToObjectEPcii", LoadTextures_hook, &LoadTextures);
    if (!eglGetProcAddress("glAlphaFuncQCOM")) {
        // If "glAlphaFuncQCOM" is not available, try "glAlphaFunc"

        if (eglGetProcAddress("glAlphaFunc")) {
            // If "glAlphaFunc" is found, store the address in the global library
            *((void **) (g_libGTASA +
                         (VER_x32 ? 0x6BCBF8 : 0x89A1B0))) = (void *) eglGetProcAddress(
                    "glAlphaFunc");
        } else {
            // If neither function is available, hook the fallback symbol
            CHook::HookRedirect("_Z25RQ_Command_rqSetAlphaTestRPc", &RQCommand_rqSetAlphaTest);
        }
    }

    //CHook::HookFunc("_ZN10CStreaming21ConvertBufferToObjectEPcii", ConvertBufferToObject_hook, &ConvertBufferToObject);
    ms_fAspectRatio = (float*)(g_libGTASA+(VER_x32 ? 0xA26A90:0xCC7F00));

    CHook::HookFunc("_ZN4CHud14DrawCrossHairsEv", DrawCrosshair_hook, &DrawCrosshair);
    //CHook::HookFunc("_ZN4CPed16SetCurrentWeaponE11eWeaponType", CPed__SetCurrentWeapon_hook, &CPed__SetCurrentWeapon);

    // skybox

    // не падать с мотоцикла
    CHook::HookFunc(OBFUSCATE("_ZNK18CEventKnockOffBike10AffectsPedEP4CPed"), CEventKnockOffBike__AffectsPed_hook, &CEventKnockOffBike__AffectsPed);

    // Стрельба
    CHook::HookFunc(OBFUSCATE("_ZN7CWeapon18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntity11eWeaponTypeS6_bbbbbbb"), CWeapon__ProcessLineOfSight_hook, &CWeapon__ProcessLineOfSight);


    CHook::HookFunc("_ZN11CBulletInfo9AddBulletEP7CEntity11eWeaponType7CVectorS3_", CBulletInfo_AddBullet_hook, &CBulletInfo_AddBullet);

    CHook::HookFunc(OBFUSCATE("_ZN7CWeapon10FireSniperEP4CPedP7CEntityP7CVector"), CWeapon__FireSniper_hook, &CWeapon__FireSniper);

    CHook::HookFunc(OBFUSCATE("_ZN28CPedDamageResponseCalculator21ComputeDamageResponseEP4CPedR18CPedDamageResponseb"), CPedDamageResponseCalculator__ComputeDamageResponse_hook, &CPedDamageResponseCalculator__ComputeDamageResponse);


    CHook::HookFunc(OBFUSCATE("_ZN7CWeapon14FireInstantHitEP7CEntityP7CVectorS3_S1_S3_S3_bb"), CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
    //CHook::HookFunc(OBFUSCATE("_ZN7CWeapon4FireEP7CEntityP7CVectorS3_S1_S3_S3_"), CWeapon__Fire_hook, &CWeapon__Fire);


    CHook::HookFunc(OBFUSCATE("_ZN6CWorld18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntitybbbbbbbb"), CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);

    //CHook::InlineHook(g_libGTASA, 0x002DCA54, &CCollision__ProcessVerticalLine_hook, &CCollision__ProcessVerticalLine);
    CHook::HookFunc(OBFUSCATE("_ZN10CCollision19ProcessVerticalLineERK8CColLineRK7CMatrixR9CColModelR9CColPointRfbbP15CStoredCollPoly"), &CCollision__ProcessVerticalLine_hook, &CCollision__ProcessVerticalLine);

    CHook::HookFunc(OBFUSCATE("_ZN17CTaskSimpleUseGun17RemoveStanceAnimsEP4CPedf"), &CTaskSimpleUseGun__RemoveStanceAnims_hook, &CTaskSimpleUseGun__RemoveStanceAnims);

//	CHook::InlineHook(g_libGTASA, 0x5669D8, &CWeapon__GenerateDamageEvent_hook, &CWeapon__GenerateDamageEvent);
//	// Fire extingusher fix
    //CHook::InlineHook(g_libGTASA, 0x46D6AC, &CTaskSimpleUseGun__SetPedPosition_hook, &CTaskSimpleUseGun__SetPedPosition);
    CHook::HookFunc(OBFUSCATE("_ZN17CTaskSimpleUseGun14SetPedPositionEP4CPed"), CTaskSimpleUseGun__SetPedPosition_hook, &CTaskSimpleUseGun__SetPedPosition);

//
//	CHook::InlineHook(g_libGTASA, 0x0032217C, &CEventHandler__HandleEvents_hook, &CEventHandler__HandleEvents);

//	CHook::Redirect(g_libGTASA, 0x0043AF78, &LoadingScreen::gtaLoadingScreenTick);
//

    CHook::HookRedirect(OBFUSCATE("_Z13Render2dStuffv"), Render2dStuff);

    CHook::HookFunc(OBFUSCATE("_Z14AND_TouchEventiiii"), TouchEvent_hook, &TouchEvent);

    //CHook::HookRedirect("_ZN11CHudColours12GetIntColourEh", &CHudColours__GetIntColour); // dangerous
    CHook::HookFunc(OBFUSCATE("_ZN6CRadar19GetRadarTraceColourEjhh"), CRadar__GetRadarTraceColor_hook, &CRadar__GetRadarTraceColor);

    CHook::HookFunc(OBFUSCATE("_ZN6CRadar12SetCoordBlipE9eBlipType7CVectorj12eBlipDisplayPc"), CRadar__SetCoordBlip_hook, &CRadar__SetCoordBlip);


    // attached objects

    //CHook::HookFunc(OBFUSCATE("_ZN6CWorld25ProcessPedsAfterPreRenderEv"), CWorld_ProcessPedsAfterPreRender_hook, &CWorld_ProcessPedsAfterPreRender);
    CHook::installPLTHook(g_libGTASA + (VER_x32 ? 0x675C58 : 0x849A20), (uintptr_t)CWorld_ProcessPedsAfterPreRender_hook,(uintptr_t*)&CWorld_ProcessPedsAfterPreRender);
    // crash

    CHook::HookFunc(OBFUSCATE("_Z27_rpMaterialListDeinitializeP14RpMaterialList"), rpMaterialListDeinitialize_hook, &rpMaterialListDeinitialize);

    // GetFrameFromID fix
//	CHook::InlineHook(g_libGTASA, 0x00335CC0, &CClumpModelInfo_GetFrameFromId_hook, &CClumpModelInfo_GetFrameFromId);


    //CHook::HookFunc(OBFUSCATE("_ZN11RenderQueue14ProcessCommandEPc"), RenderQueue__ProcessCommand_hook, &RenderQueue__ProcessCommand);

    // fix
//	CHook::InlineHook(g_libGTASA, 0x001B9D74, &_rwFreeListFreeReal_hook, &_rwFreeListFreeReal);


    CHook::HookFunc(OBFUSCATE("_ZN17CVehicleModelInfo20SetEditableMaterialsEP7RpClump"), CVehicleModelInfo__SetEditableMaterials_hook, &CVehicleModelInfo__SetEditableMaterials);


    CHook::HookFunc(OBFUSCATE("_ZN8CVehicle16ResetAfterRenderEv"), CVehicle__ResetAfterRender_hook, &CVehicle__ResetAfterRender);

//

    CHook::HookRedirect(OBFUSCATE("_ZN5CGame7ProcessEv"), CGame__Process_hook);



    //CHook::HookFunc(OBFUSCATE("_ZN11CAutomobile22ProcessEntityCollisionEP7CEntityP9CColPoint"), CAutomobile__ProcessEntityCollision_hook, &CAutomobile__ProcessEntityCollision);

    CHook::HookFunc(OBFUSCATE("_ZN14MainMenuScreen6OnExitEv"), MainMenuScreen__OnExit_hook, &MainMenuScreen__OnExit);


    // headlights color, wheel size, wheel align
//	CHook::InlineHook(g_libGTASA, 0x005B9D38, &CShadows__StoreCarLightShadow_hook, &CShadows__StoreCarLightShadow);
//	CHook::InlineHook(g_libGTASA, 0x00518EC4, &CVehicle__DoHeadLightBeam_hook, &CVehicle__DoHeadLightBeam);
//
    // размер колес

    CHook::HookFunc(OBFUSCATE("_ZN8CVehicle15DoHeadLightBeamEiR7CMatrixh"), CVehicle__DoHeadLightBeam_hook, &CVehicle__DoHeadLightBeam);
    CHook::HookFunc(OBFUSCATE("_ZN8CShadows19StoreCarLightShadowEP8CVehicleiP9RwTextureP7CVectorffffhhhf"), CShadows__StoreCarLightShadow_hook, &CShadows__StoreCarLightShadow);
    CHook::HookFunc(OBFUSCATE("_ZN11CAutomobile9PreRenderEv"), CAutomobile__PreRender_hook, &CAutomobile__PreRender);


    CHook::HookFunc(OBFUSCATE("_ZN11CAutomobile17UpdateWheelMatrixEii"), CAutomobile__UpdateWheelMatrix_hook, &CAutomobile__UpdateWheelMatrix);
#if VER_x32
    CHook::HookFunc(OBFUSCATE("_ZN7CMatrix6RotateEfff"), CMatrix__Rotate_hook, &CMatrix__Rotate);
#endif

//	CHook::InlineHook(g_libGTASA, 0x003E8D48, &CMatrix__Rotate_hook, &CMatrix__Rotate);

    CHook::HookFunc(OBFUSCATE("_ZN7CMatrix8SetScaleEfff"), CMatrix__SetScale_hook, &CMatrix__SetScale);

//	CHook::InlineHook(g_libGTASA, 0x003BF300, (uintptr_t)CCam__Process_hook, (uintptr_t*)& CCam__Process);

    CHook::HookFunc(OBFUSCATE("_ZN4CPad27CycleCameraModeDownJustDownEv"), CPad__CycleCameraModeDownJustDown_hook, &CPad__CycleCameraModeDownJustDown);


    CHook::HookFunc(OBFUSCATE("_ZN4CPed22ProcessEntityCollisionEP7CEntityP9CColPoint"), CPed__ProcessEntityCollision_hook, &CPed__ProcessEntityCollision);
    CHook::HookFunc(OBFUSCATE("_ZN4CCam7ProcessEv"), CCam__Process_hook, &CCam__Process);
//	//================================
//	// ===============================
//	//================================
//
    // vehicle light processing
    CHook::HookRedirect(OBFUSCATE("_ZN8CVehicle22GetVehicleLightsStatusEv"), CVehicle__GetVehicleLightsStatus);

    // Настройки
//	CHook::NOP(g_libGTASA + 0x266460, 2); // Game - TrafficMode
//	CHook::NOP(g_libGTASA + 0x266496, 2); // Game - AimMode
//	CHook::NOP(g_libGTASA + 0x261A50, 2); // Main - Language
#if VER_x32
    CHook::NOP(g_libGTASA + 0x002A4A62, 2); // Game - SocialClub
#else
    CHook::NOP(g_libGTASA + 0x003634A4, 1); // Game - SocialClub
#endif

    //размытие на скорости


    CHook::HookFunc(OBFUSCATE("_ZN5CDraw6SetFOVEfb"), CDraw__SetFOV_hook, &CDraw__SetFOV);


    //CHook::HookFunc("_ZN21CAEVehicleAudioEntity24CancelVehicleEngineSoundEs", CAEVehicleAudioEntity__CancelVehicleEngineSound_hook, &CAEVehicleAudioEntity__CancelVehicleEngineSound);


    //CHook::HookFunc("_ZN13cTransmission26CalculateDriveAccelerationERKfRhRfS1_PfS4_hh", cTransmission__CalculateGearForSimpleCar_hook, &cTransmission__CalculateGearForSimpleCar);

    Log((VER_x32 ? "32 bit" : "64 bit"));
    Scene = *(CScene*)(g_libGTASA + 0x009FC938);
    //CHook::HookFunc(OBFUSCATE("_ZN9CRenderer24RenderEverythingBarRoadsEv"), CRenderer__RenderEverythingBarRoads_hook, &CRenderer__RenderEverythingBarRoads);
    CHook::HookFunc(OBFUSCATE("_ZN16CTaskSimpleGetUp10ProcessPedEP4CPed"), CTaskSimpleGetUp__ProcessPed_hook, &CTaskSimpleGetUp__ProcessPed);
    CHook::HookFunc(OBFUSCATE("_ZN6CRadar20DrawRadarGangOverlayEb"), CRadar__DrawRadarGangOverlay_hook, &CRadar__DrawRadarGangOverlay);
    CHook::HookFunc(OBFUSCATE("_ZN6CRadar9ClearBlipEi"), CRadar__ClearBlip_hook, &CRadar__ClearBlip);
    HookCPad();
}

