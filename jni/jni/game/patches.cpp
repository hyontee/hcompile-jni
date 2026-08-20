#include "../main.h"
#include "../util/armhook.h"
#include "../util/patch.h"

#include "common.h"
#include "../settings.h"
#include "game.h"
#include "CExtendedCarColors.h"

char* PLAYERS_REALLOC = nullptr;
extern CSettings* pSettings;

void WriteUnVerified0();
void InitInGame();

VehicleAudioPropertiesStruct VehicleAudioProperties[20000];

int test_pointsArray[1000];
int test_pointersLibArray[1000];

// TODO:
//                 ms_lodDistScale
//


extern void (*ANDRunThread)(void* a1);
//void ANDRunThread_hook(void* a1);

void ApplyDXT()
{
    Log(OBFUSCATE("Installing DXTpatches..."));

    CHook::WriteMemory(g_libGTASA + 0x573640, (int)"dxt", 3);
    CHook::WriteMemory(g_libGTASA + 0x573668, (int)"dxt", 3);
    CHook::WriteMemory(g_libGTASA + 0x57367C, (int)"dxt", 3);
    CHook::WriteMemory(g_libGTASA + 0x573690, (int)"dxt", 3);
    CHook::WriteMemory(g_libGTASA + 0x5736C8, (int)"dxt", 3);
    CHook::WriteMemory(g_libGTASA + 0x5736D8, (int)"dxt", 3);
    CHook::WriteMemory(g_libGTASA + 0x5736E8, (int)"dxt", 3);
}

void ApplySKY()
{
    Log(OBFUSCATE("Installing SKYpatches..."));
}

void ApplyFPS()
{
    Log(OBFUSCATE("Installing FPSpatches..."));

    uint8_t fps = 120;
    if(pSettings) fps = pSettings->GetReadOnly().iFPS;

    CHook::WriteMemory(g_libGTASA + 0x463FE8, (uintptr_t)& fps, 1);
    CHook::WriteMemory(g_libGTASA + 0x56C1F6, (uintptr_t)& fps, 1);
    CHook::WriteMemory(g_libGTASA + 0x56C126, (uintptr_t)& fps, 1);
    CHook::WriteMemory(g_libGTASA + 0x95B074, (uintptr_t)& fps, 1);
}

void ApplyShadowFix()
{
    Log(OBFUSCATE("Installing SHADOWpatches..."));
    CHook::NOP(g_libGTASA + 0x0039B2C0, 2);

    CHook::UnFuck(g_libGTASA + 0x0039B2C4);
    CHook::NOP(g_libGTASA + 0x0039B2C4, 2);
}

void ApplySC()
{
    Log(OBFUSCATE("Installing SCpatches.."));

    uintptr_t g_libSCAnd = FindLibrary("libSCAnd.so");
    if (g_libSCAnd)
    {
        CHook::UnFuck(g_libSCAnd + 0x001E1738);
        CHook::UnFuck(g_libSCAnd + 0x001E16DC);
        CHook::UnFuck(g_libSCAnd + 0x001E080C);
        strcpy((char*)(g_libSCAnd + 0x001E16DC), "com/rockstargames/hal/andViewManager");
        strcpy((char*)(g_libSCAnd + 0x001E1738), "staticExitSocialClub");
        strcpy((char*)(g_libSCAnd + 0x001E080C), "()V");
    }
}

void ApplyMemory()
{
    Log(OBFUSCATE("Installing WEATHERpatches.."));
    CHook::WriteMemory(g_libGTASA+0x52DD38, (uintptr_t)"\x00\x20\x70\x47", 4);
    CHook::NOP(g_libGTASA + 0x39AD14, 1);

    Log(OBFUSCATE("Installing MEMORYpatches.."));
    memcpy((uint32_t*)(g_libGTASA+0x5DE734), "0x10000000", 10);
    //memcpy((uint32_t*)(g_libGTASA + 0x5DE734), "0x20000000", 10); // 500.0 megabytes

    Log(OBFUSCATE("Installing MATRIXpatches.."));
    *(float*)(g_libGTASA + 0x608558) = 200.0f;
}

void ApplyBRPatches()
{
    // BYTE, WORD, DWORD - uint8_t
    // WORD - uint16_t
    // DWORD - uint32_t
    // PSTR - char*

    Log(OBFUSCATE("Installing REVERSEDpatches.."));

    // ����� � ������� ������

    // -----------------------------------------------------------------
    // ���� ������ ����/���� ��� ������� ���� ��� ���
    // CBike::ProcessAI
    CHook::UnFuck(g_libGTASA + 0x4EE200);
    *(uint8_t*)(g_libGTASA + 0x4EE200) = 0x9B;

    // CAutomobile::ProcessAI
//                  CHook::UnFuck(g_libGTASA + 0x004E5A22);
//	*(uint8_t*)(g_libGTASA + 0x004E5A22) = 0x9B;
    // ------------------------------------------------------------------

//                  CHook::NOP(g_libGTASA + 0x63E424, 100); // ���� ��������� ����� (��� ���� �����)
//                  *(uint32_t *)(g_libGTASA + 0x63E424) = 0;
}

void ApplyFixes()
{
    Log(OBFUSCATE("Installing FIXpatches.."));

    // ����� ��� ������������
    CHook::NOP(g_libGTASA + 0x0056CD4A, 2); // 0x56CD4A

    // ����� ������ ���� (gta 1.08)
    CHook::WriteMemory(g_libGTASA + 0x005197E0, (uintptr_t)"\x04", 1); // 0x5197E0
    CHook::WriteMemory(g_libGTASA + 0x005197EA, (uintptr_t)"\xFA", 1); // 0x5197EA

    // CPopulation::AddPed(ePedType,uint,CVector const&,bool)	0x45F1A4
    CHook::RET(g_libGTASA + 0x0045F1A4); // CPopulation::AddPed

    // CPlayerInfo::FindObjectToSteal(CPed *)	0x3AC114
    CHook::RET(g_libGTASA + 0x003AC114); // CPlayerInfo::FindObjectToSteal

    // CEntryExit::GenerateAmbientPeds(CVector const&)	0x2C1CB0
    CHook::RET(g_libGTASA + 0x002C1CB0); // CEntryExit::GenerateAmbientPeds

    // CHud::SetHelpMessage(char const*,ushort *,bool,bool,bool,uint)	0x3D4244
    // CHook::RET(g_libGTASA + 0x003D4244); // CHud::SetHelpMessage

    // CPlaceName::Process(void)	0x3BF8B4
    CHook::RET(g_libGTASA + 0x003BF8B4); // CPlaceName::Process

    // CPedGroupDefaultTaskAllocatorRandom::AllocateDefaultTasks(CPedGroup *,CPed *)	0x443EF4
    CHook::RET(g_libGTASA + 0x00443EF4); // CPedGroupDefaultTaskAllocatorRandom::AllocateDefaultTasks | crash

    // CVehicleRecording::Load(RwStream *,int,int)	0x2DC8E0
    // CHook::RET(g_libGTASA + 0x002DC8E0); // CVehicleRecording::Load

    // CPedGroups::Process(void)	0x448B38
    CHook::RET(g_libGTASA + 0x00448B38); // CPedGroups::Process() | crash

    // CPedGroupIntelligence::Process(void)	0x448850
    CHook::RET(g_libGTASA + 0x00448850); // CPedGroupIntelligence::Process | crash

    // CPedGroupMembership::SetLeader(CPed *)	0x444B18
    CHook::RET(g_libGTASA + 0x00444B18); // CPedGroupMembership::SetLeader | crash

    // CPedGroupIntelligence::Flush(void)	0x444FDC
    CHook::RET(g_libGTASA + 0x004B3156); // CPedGroupIntelligence::Flush | crash

    // WaterCreatureManager_c::Update(float)	0x51DEC4
    CHook::RET(g_libGTASA + 0x0051DEC4); // �������� � ����

    // CBirds::Render(void)	0x5286EC
    CHook::RET(g_libGTASA + 0x5286EC); // �����

    // CMotionBlurStreaks::Update
    CHook::NOP(g_libGTASA + 0x0054D720, 2);

    // CCamera::RenderMotionBlur(void)
    CHook::NOP(g_libGTASA + 0x0039B394, 2);

    // CStreaming::ms_memoryAvailable	0x5DE734
    // CHook::NOP(g_libGTASA + 0x005DE734, 2);	// CStreaming::ms_memoryAvailable = (int)v24

    // CPlayerPed::AnnoyPlayerPed(bool)	0x457644
    CHook::RET(g_libGTASA + 0x00457644	); // CPlayerPed::AnnoyPlayerPed | N� �������� � �����
}

void ApplyPatches_level0()
{
    //ApplyDXT();

    // -------------------------------------------
    PLAYERS_REALLOC = ((char* (*)(uint32_t))(g_libGTASA + 0x179B40))(404 * 257 * sizeof(char));
    memset(PLAYERS_REALLOC, 0, 404 * 257);
    CHook::UnFuck(g_libGTASA + 0x5D021C);
    *(char**)(g_libGTASA + 0x5D021C) = PLAYERS_REALLOC;
    // -------------------------------------------

    // -------------------------------------------
    CHook::UnFuck(g_libGTASA + 0x005D1E8C);
    memset(test_pointsArray, 0, 999 * sizeof(int));
    *(int**)(g_libGTASA + 0x005D1E8C) = &test_pointsArray[0];

    /*
                                ** 3 TOUCHS **
                                                                                       */

    CHook::UnFuck(g_libGTASA + 0x0063D4F0);
    memset(test_pointersLibArray, 0, 999 * sizeof(int));
    *(int**)(g_libGTASA + 0x0063D4F0) = &test_pointersLibArray[0];

    CHook::WriteMemory(g_libGTASA + 0x00238232, (uintptr_t)"\x03\x20", 2);
    CHook::WriteMemory(g_libGTASA + 0x0025C522, (uintptr_t)"\x02\x2C", 2);
    // -------------------------------------------

    // col links limits
    CHook::WriteMemory(g_libGTASA + 0x1859FC, (uintptr_t)"\x01\x22", 2);
    // -------------------------------------------

    //CExtendedCarColors::ApplyPatches_level0();

    //ApplyFPS();

    //SetUpHook(g_libGTASA + 0x0023768C, (uintptr_t)ANDRunThread_hook, (uintptr_t*)& ANDRunThread);

    // �������� ���� �� ������� � ����� (��� ������ ������ �����)
    //CHook::UnFuck(g_libGTASA + 0x0056CA68);
    //*(uint8_t*)(g_libGTASA + 0x0056CA68) = 1;

    //CODE XREF: GraphicsThread(void *) Pause
    //NOP(g_libGTASA + 0x001A869C, 2);

    // CHook::NOP random pickups
    CHook::NOP(g_libGTASA + 0x00402472, 2);
    CHook::NOP(g_libGTASA + 0x003E1AF0, 2);

    // osMutexStuff
    CHook::WriteMemory(g_libGTASA + 0x001A7ECE, (uintptr_t)"\x4F\xF0\x01\x00\x00\x46", 6);

    // CdStreamInit(6);
    CHook::WriteMemory(g_libGTASA + 0x3981EC, (uintptr_t)"\x06\x20", 2);

    //ApplySC();

    // fix hook crash
    CHook::WriteMemory(g_libGTASA + 0x00336618, (uintptr_t)"\x4F\xF4\x6A\x71", 4);
    CHook::WriteMemory(g_libGTASA + 0x0033661C, (uintptr_t)"\x1A\x4B", 2);
    CHook::WriteMemory(g_libGTASA + 0x00274AB4, (uintptr_t)"\x00\x46", 2);

    //WriteUnVerified0();
}

uint32_t* CStreaming__msFiles;

void ApplyPatches()
{
    Log(OBFUSCATE("Installing patches.."));

    // CWorld::FindPlayerSlotWithPedPointer
    CHook::NOP(g_libGTASA + 0x0045A4C8, 11);
    CHook::NOP(g_libGTASA + 0x0045A4E0, 3);

    //DATA XREF: ToggleDebugFPS(void)
    //CHook::UnFuck(g_libGTASA + 0x008B8018);
    //*(uint8_t*)(g_libGTASA + 0x008B8018) = 1;

    // CAudioEngine::StartLoadingTune
    CHook::NOP(g_libGTASA + 0x56C150, 2);

    // CHook::NOP CFileLoader::loadPickups
    //NOP(g_libGTASA + 0x402472, 2);

    ApplyShadowFix();

    // DefaultPCSaveFileName
    char* DefaultPCSaveFileName = (char*)(g_libGTASA + 0x60EAE8);
    memcpy((char*)DefaultPCSaveFileName, "GTASAMP", 8);

    // CWidgetRegionSteeringSelection::Draw
    CHook::WriteMemory(g_libGTASA + 0x284BB8, (uintptr_t)"\xF7\x46", 2);

    // CHud::SetHelpMessage
    CHook::WriteMemory(g_libGTASA + 0x3D4244, (uintptr_t)"\xF7\x46", 2);
    // CHud::SetHelpMessageStatUpdate
    CHook::WriteMemory(g_libGTASA + 0x3D42A8, (uintptr_t)"\xF7\x46", 2);
    // CVehicleRecording::Load
    CHook::WriteMemory(g_libGTASA + 0x2DC8E0, (uintptr_t)"\xF7\x46", 2);

    CHook::WriteMemory(g_libGTASA + 0x00458D68, (uintptr_t)"\x00\x46\x00\x46", 4); // cFire::extinguish CHook::NOPped

    ApplyFPS();

    //CStreaming::DeleteRwObjectsAfterDeath(CVector const&)

    Log("Install STREAMINGpatch...");
    //CHook::UnFuck(g_libGTASA + 0x002C3B82);
    //NOP(g_libGTASA + 0x002C3B82, 2);

    CHook::UnFuck(g_libGTASA + 0x00454950);
    CHook::NOP(g_libGTASA + 0x00454950, 17); // CAnimManager::RemoveAnimBlockRef

    // CPed pool (old: 140, new: 210)
    CHook::WriteMemory(g_libGTASA + 0x3AF2D0, (uintptr_t)"\x45\xF6\xC8\x60\xC0\xF2\x06\x00", 8); // MOV  R0, #0x65EC8 | size=0x7C4 (old: 0x43F30)
    CHook::WriteMemory(g_libGTASA + 0x3AF2DE, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
    CHook::WriteMemory(g_libGTASA + 0x3AF2E4, (uintptr_t)"\xD2\x22", 2); // MOVS R2, #0xD2
    CHook::WriteMemory(g_libGTASA + 0x3AF310, (uintptr_t)"\xD2\x2B", 2); // CMP  R3, #0xD2

    // CPedIntelligence pool (old: 140, new: 210)
    CHook::WriteMemory(g_libGTASA + 0x3AF7E6, (uintptr_t)"\x42\xF2\xB0\x00\xC0\xF2\x02\x00\x00\x46", 10); // MOVS R0, #0x220B0 | size=0x298 (old: 0x16B20)
    CHook::WriteMemory(g_libGTASA + 0x3AF7F6, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
    CHook::WriteMemory(g_libGTASA + 0x3AF7FC, (uintptr_t)"\xD2\x22", 2); // MOVS R2, #0xD2
    CHook::WriteMemory(g_libGTASA + 0x3AF824, (uintptr_t)"\xD2\x2B", 2); // CMP  R3, 0xD2

    // Task pool (old: 500, new: 1524 (1536))
    CHook::WriteMemory(g_libGTASA + 0x3AF4EA, (uintptr_t)"\x4F\xF4\x40\x30", 4); // MOV.W R0, #30000 | size = 0x80 (old: 0xFA00)
    CHook::WriteMemory(g_libGTASA + 0x3AF4F4, (uintptr_t)"\x4F\xF4\xC0\x60", 4); // MOV.W R0, #0x600
    CHook::WriteMemory(g_libGTASA + 0x3AF4FC, (uintptr_t)"\x4F\xF4\xC0\x62", 4); // MOV.W R2, #0x600
    CHook::WriteMemory(g_libGTASA + 0x3AF52A, (uintptr_t)"\xB3\xF5\xC0\x6F", 4); // CMP.W R3, #0x600

    // TxdStore pool (old: 5000, new: 20000)
    CHook::WriteMemory(g_libGTASA + 0x0055BA9A, (uintptr_t)"\x4f\xf4\xb8\x50\xc0\xf2\x11\x00", 8); //  MOV.W	R0, #0x1700 | MOVT	R0, #0x11
    CHook::WriteMemory(g_libGTASA + 0x0055BAA8, (uintptr_t)"\x44\xf6\x20\x60", 4); // MOVW	R0, #0x4E20
    CHook::WriteMemory(g_libGTASA + 0x0055BAB0, (uintptr_t)"\x44\xf6\x20\x62", 4); // MOVW	R2, #0x4E20

    // Event pool (old: 200, new: 512)
    // Object pool (old: 350, new: 2992)
    CHook::WriteMemory(g_libGTASA + 0x003AF3D6, (uintptr_t)"\x4f\xf4\x7e\x40\xc0\xf2\x12\x00", 8); // MOV.W	R0, #0x5900 | MOVT	R0, #0x6
    CHook::WriteMemory(g_libGTASA + 0x003AF3E4, (uintptr_t)"\x4f\xf4\x3b\x60", 4); // MOV.W R0, #0xBB0
    CHook::WriteMemory(g_libGTASA + 0x003AF3EC, (uintptr_t)"\x4f\xf4\x3b\x62", 4); // MOV.W R2, #0xBB0
    CHook::WriteMemory(g_libGTASA + 0x003AF41A, (uintptr_t)"\xb3\xf5\x3b\x6f", 4); // CMP.W R3, #0xBB0

    // Building pool (old: 14000, new: 29585)
    CHook::WriteMemory(g_libGTASA + 0x003AF378, (uintptr_t)"\x4F\xF4\x90\x40\xC0\xF2\x19\x00", 8); // mov.w r0, #0x4800 movt r0, #0x19
    CHook::WriteMemory(g_libGTASA + 0x003AF386, (uintptr_t)"\x47\xF2\x91\x30", 4); // mov r0, #0x7391
    CHook::WriteMemory(g_libGTASA + 0x003AF38E, (uintptr_t)"\x47\xF2\x91\x32", 4); //mov r2, #0x7391

    // Dummys pool (old: 3500, new: 4000)
    CHook::WriteMemory(g_libGTASA + 0x003AF430, (uintptr_t)"\x4f\xf4\xd6\x40\xc0\xf2\x03\x00", 8); // MOV.W	R0, #0x6B00 | MOVT	R0, #0x3
    CHook::WriteMemory(g_libGTASA + 0x003AF43E, (uintptr_t)"\x40\xf6\xa0\x70", 4); // MOVW            R0, #0xFA0
    CHook::WriteMemory(g_libGTASA + 0x003AF446, (uintptr_t)"\x40\xf6\xa0\x72", 4); // MOVW            R2, #0xFA0

    // PtrNode Single pool (old: 75000, new: 100000)
    CHook::WriteMemory(g_libGTASA + 0x003AF1BC, (uintptr_t)"\x4f\xf4\x54\x50\xc0\xf2\x0c\x00", 8); // MOV.W	R0, #0x3500 | MOVT	R0, #0xC
    CHook::WriteMemory(g_libGTASA + 0x003AF1D6, (uintptr_t)"\x48\xf2\xa0\x62", 4); // MOVW	R2, #0x86A0
    CHook::WriteMemory(g_libGTASA + 0x003AF1B0, (uintptr_t)"\x48\xf2\xa0\x66", 4); // MOVW	R6, #0x86A0
    CHook::WriteMemory(g_libGTASA + 0x003AF1CA, (uintptr_t)"\x48\xf2\xa0\x60\xc0\xf2\x01\x00", 8); // MOV.W R0, #0x86A0 | MOVT R0, #1

    //PtrNode Double pool (old: 6000, new: 8000)
    CHook::WriteMemory(g_libGTASA + 0x003AF21C, (uintptr_t)"\x4f\xf4\xee\x40\xc0\xf2\x01\x00", 8); // MOV.W	R0, #0x7700 | MOVT	R0, #1
    CHook::WriteMemory(g_libGTASA + 0x003AF22A, (uintptr_t)"\x41\xf6\x40\x70", 4); // MOVW            R0, #8000
    CHook::WriteMemory(g_libGTASA + 0x003AF232, (uintptr_t)"\x41\xf6\x40\x72", 4); // MOVW            R2, #8000

    // Entry Node Info pool (old: 500, new: 5120)
    CHook::WriteMemory(g_libGTASA + 0x003AF27A, (uintptr_t)"\x4f\xf4\xc8\x30", 4); // MOV.W	R0, #0x19000 | size = 0x14
    CHook::WriteMemory(g_libGTASA + 0x003AF284, (uintptr_t)"\x4f\xf4\xa0\x50", 4); // MOV.W R0, #0x1400
    CHook::WriteMemory(g_libGTASA + 0x003AF28C, (uintptr_t)"\x4f\xf4\xa0\x52", 4); // MOV.W R2, #0x1400
    CHook::WriteMemory(g_libGTASA + 0x003AF2BA, (uintptr_t)"\xb3\xf5\xa0\x5f", 4); // CMP.W R3, #0x1400

    // ColModel pool (old:10150, new: 32511)
    CHook::WriteMemory(g_libGTASA + 0x3AF48E, (uintptr_t)"\x4C\xF6\xD0\x70\xC0\xF2\x17\x00", 8); // MOV R0, #0x17CFD0 | size=0x30 (old: 0x76F20)
    CHook::WriteMemory(g_libGTASA + 0x3AF49C, (uintptr_t)"\x47\xF6\xFF\x60", 4); // MOVW R0, #0x7EFF
    CHook::WriteMemory(g_libGTASA + 0x3AF4A4, (uintptr_t)"\x47\xF6\xFF\x62", 4); // MOVW R2, #0x7EFF

    // VehicleStruct increase (0x32C*0x50 = 0xFDC0)
    CHook::WriteMemory(g_libGTASA + 0x405338, (uintptr_t)"\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
    CHook::WriteMemory(g_libGTASA + 0x405342, (uintptr_t)"\x50\x20", 2);			// MOVS R0, #0x50
    CHook::WriteMemory(g_libGTASA + 0x405348, (uintptr_t)"\x50\x22", 2);			// MOVS R2, #0x50
    CHook::WriteMemory(g_libGTASA + 0x405374, (uintptr_t)"\x50\x2B", 2);			// CMP  R3, #0x50

    // Increase matrix count in CPlaceable::InitMatrixArray
    CHook::WriteMemory(g_libGTASA + 0x3ABB0A, (uintptr_t)"\x4F\xF4\x7A\x61", 4); // MOV.W R1, #4000

    // stop assign 0xFFFFFF to beam color!
    CHook::NOP(g_libGTASA + 0x00519116, 2);
    CHook::NOP(g_libGTASA + 0x0051911C, 4);
    CHook::NOP(g_libGTASA + 0x00519198, 4);
    CHook::NOP(g_libGTASA + 0x00519198, 2);
    CHook::NOP(g_libGTASA + 0x005191D0, 4);
    CHook::NOP(g_libGTASA + 0x005191DC, 2);

    ApplyFixes();

}
void ApplyInGamePatches()
{
    Log("");

    ApplyMemory();

    // CTheZones::ZonesVisited[100]
    memset((void*)(g_libGTASA + 0x8EA7B0), 1, 100);
    // CTheZones::ZonesRevealed
    *(uint32_t*)(g_libGTASA + 0x8EA7A8) = 100;

    // CTaskSimplePlayerOnFoot::PlayIdleAnimations
    CHook::WriteMemory(g_libGTASA + 0x4BDB18, (uintptr_t)"\x70\x47", 2);

    // CarCtl::GenerateRandomCars nulled from CGame::Process
    CHook::UnFuck(g_libGTASA + 0x398A3A);
    CHook::NOP(g_libGTASA + 0x398A3A, 2);

    // CTheCarGenerators::Process nulled from CGame::Process
    CHook::UnFuck(g_libGTASA + 0x398A34);
    CHook::NOP(g_libGTASA + 0x398A34, 2);

    // ��������� ��� MaxHealth
    CHook::UnFuck(g_libGTASA + 0x3BAC68);
    *(float*)(g_libGTASA + 0x3BAC68) = 176.0f;
    // ��������� ��� Armour
    CHook::UnFuck(g_libGTASA + 0x27D884);
    *(float*)(g_libGTASA + 0x27D884) = 176.0;

    // CEntryExit::GenerateAmbientPeds nulled from CEntryExit::TransitionFinished
    CHook::UnFuck(g_libGTASA + 0x2C2C22);
    CHook::NOP(g_libGTASA + 0x2C2C22, 4);

    CHook::NOP(g_libGTASA + 0x003D6FDC, 2);
    CHook::NOP(g_libGTASA + 0x0026B214, 2);

    // ���������
    CHook::WriteMemory(g_libGTASA + 0x3D62FC, (uintptr_t)"\xF7\x46", 2);

    // CPlayerPed::CPlayerPed task fix
    CHook::WriteMemory(g_libGTASA + 0x458ED1, (uintptr_t)"\xE0", 1);

    // ReapplyPlayerAnimation (�� �����)
    CHook::NOP(g_libGTASA + 0x45477E, 5);

    // radar draw blips
    CHook::UnFuck(g_libGTASA + 0x3DCA90);
    CHook::NOP(g_libGTASA + 0x3DCA90, 2);
    CHook::UnFuck(g_libGTASA + 0x3DD4A4);
    CHook::NOP(g_libGTASA + 0x3DD4A4, 2);

    // CCamera::CamShake from CExplosion::AddExplosion
    CHook::NOP(g_libGTASA + 0x55EFB8, 2);
    CHook::NOP(g_libGTASA + 0x55F8F8, 2);

    // camera_on_actor path
    CHook::UnFuck(g_libGTASA + 0x2F7B68);
    *(uint8_t*)(g_libGTASA + 0x2F7B6B) = 0xBE;

    // CPed::RemoveWeaponWhenEnteringVehicle (GetPlayerInfoForThisPlayerPed)
    CHook::UnFuck(g_libGTASA + 0x434D94);
    CHook::NOP(g_libGTASA + 0x434D94, 6);

    // CWidgetPlayerInfo::DrawWanted
    CHook::WriteMemory(g_libGTASA + 0x27D8D0, (uintptr_t)"\x4F\xF0\x00\x08", 4);

    // no vehicle audio processing
    CHook::UnFuck(g_libGTASA + 0x4E31A6);
    CHook::NOP(g_libGTASA + 0x4E31A6, 2);
    CHook::UnFuck(g_libGTASA + 0x4EE7D2);
    CHook::NOP(g_libGTASA + 0x4EE7D2, 2);
    CHook::UnFuck(g_libGTASA + 0x4F741E);
    CHook::NOP(g_libGTASA + 0x4F741E, 2);
    CHook::UnFuck(g_libGTASA + 0x50AB4A);
    CHook::NOP(g_libGTASA + 0x50AB4A, 2);

    CHook::NOP(g_libGTASA + 0x00398768, 2);
    CHook::NOP(g_libGTASA + 0x003987DC, 2);

    CHook::NOP(g_libGTASA + 0x003688EC, 2);

    ApplyBRPatches();
}

void readVehiclesAudioSettings()
{
    char vehicleModel[50];
    int16_t pIndex = 0;
    FILE* pFile;
    char line[300];
    memset(VehicleAudioProperties, 0x00, sizeof(VehicleAudioProperties));

    VehicleAudioPropertiesStruct CurrentVehicleAudioProperties;

    memset(&CurrentVehicleAudioProperties, 0x0, sizeof(VehicleAudioPropertiesStruct));

    char buffer[0xFF];
    sprintf(buffer, OBFUSCATE("%sSAMP/vehicleAudioSettings.cfg"), g_pszStorage);
    pFile = fopen(buffer, "r");
    if (!pFile)
    {
        Log(OBFUSCATE("Cannot read vehicleAudioSettings.cfg"));
        return;
    }

    // File exists
    while (fgets(line, sizeof(line), pFile))
    {
        if (strncmp(line, ";the end", 8) == 0)
            break;

        if (line[0] == ';')
            continue;

        sscanf(line, "%s %d %d %d %d %f %f %d %f %d %d %d %d %f",
               vehicleModel,
               &CurrentVehicleAudioProperties.VehicleType,
               &CurrentVehicleAudioProperties.EngineOnSound,
               &CurrentVehicleAudioProperties.EngineOffSound,
               &CurrentVehicleAudioProperties.field_4,
               &CurrentVehicleAudioProperties.field_5,
               &CurrentVehicleAudioProperties.field_6,
               &CurrentVehicleAudioProperties.HornTon,
               &CurrentVehicleAudioProperties.HornHigh,
               &CurrentVehicleAudioProperties.DoorSound,
               &CurrentVehicleAudioProperties.RadioNum,
               &CurrentVehicleAudioProperties.RadioType,
               &CurrentVehicleAudioProperties.field_14,
               &CurrentVehicleAudioProperties.field_16);

        int result = ((int (*)(const char* thiz, int16_t* a2))(g_libGTASA + 0x00336110 + 1))(vehicleModel, &pIndex);
        memcpy(&VehicleAudioProperties[pIndex-400], &CurrentVehicleAudioProperties, sizeof(VehicleAudioPropertiesStruct));
    }
    fclose(pFile);
}