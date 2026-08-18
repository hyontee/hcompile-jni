#include "../main.h"
#include "common.h"
#include "CPlayerInfoGta.h"
#include "../CSettings.h"
#include "..//CDebugInfo.h"
#include "game.h"
#include "CExtendedCarColors.h"
#include "patch.h"
#include "chatwindow.h"
#include "CWorld.h"

VehicleAudioPropertiesStruct VehicleAudioProperties[20000];

void DisableAutoAim()
{
    CHook::RET("_ZN10CPlayerPed22FindWeaponLockOnTargetEv");
    CHook::RET("_ZN10CPlayerPed26FindNextWeaponLockOnTargetEP7CEntityb");
    CHook::RET("_ZN4CPed21SetWeaponLockOnTargetEP7CEntity");
}
void ApplyPausePathes() {

    CHook::RET(OBFUSCATE("_ZN6CRadar10DrawLegendEiii")); // CRadar::DrawLegend
#if VER_x32
    CHook::NOP(g_libGTASA + 0x2AB4A6, 2);
#else
    CHook::NOP(g_libGTASA + 0x36A190, 1);
#endif
}
void ApplyFPSPatch(uint8_t fps)
{
    if(fps > CSettings::maxFps)
        fps = CSettings::maxFps;
#if VER_x32
    CHook::WriteMemory(g_libGTASA + 0x005E49E0, (uintptr_t)fps, 1);
    CHook::WriteMemory(g_libGTASA + 0x005E492E, (uintptr_t)fps, 1);
    CHook::WriteMemory(g_libGTASA + 0x005E49E6, (uintptr_t)fps, 1);
    CHook::WriteMemory(g_libGTASA + 0x005E49C8, (uintptr_t)fps, 1);
#else
    // CHook::WriteMemory2(g_libGTASA + 0x70A38C, "\xE9\x0F\x1E\x32", 4);
   // CHook::WriteMemory(g_libGTASA + 0x70A43C, (uintptr_t)fps, 1);
   // CHook::WriteMemory(g_libGTASA + 0x70A458, (uintptr_t)fps, 1);
   CHook::NOP(g_libGTASA + 0x70A474, 2);
   CHook::NOP(g_libGTASA + 0x70A398, 2);
   CHook::UnFuck(g_libGTASA + 0xC9B320);
    ((RsGlobalType*)(g_libGTASA + 0xC9B320))->maxFPS = fps;
#endif
    Log("New fps limit = %d", fps);
}

extern void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1);

void ApplyPatches_level0()
{
    Log("ApplyPatches_level0");
    CHook::RET(OBFUSCATE("_ZN7CClouds22RenderBottomFromHeightEv"));

    //CWorld::Players = new CPlayerInfoGta[MAX_PLAYERS]{nullptr};
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006783C0 : 0x84E7A8), &CWorld::Players);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679B5C : 0x8516D8), &CWorld::PlayerInFocus);
    CHook::HookRedirect("_ZN6CWorld28FindPlayerSlotWithPedPointerEPv", CUtil::FindPlayerSlotWithPedPointer);

    // From CGame::Init2
    CHook::RET("_ZN8CClothes4InitEv"); // CClothes::Init
    CHook::RET("_ZN8CClothes13RebuildPlayerEP10CPlayerPedb"); // CClothes::RebuildPlayer

    CHook::RET("_ZN6CTrain10InitTrainsEv"); // CTrain::InitTrains()
    CHook::RET("_ZN14CLoadingScreen15DisplayPCScreenEv"); // Loading screen

    CHook::HookFunc("_Z12ANDRunThreadPv", ANDRunThread_hook, &ANDRunThread);


    CHook::RET("_ZN10Interior_c10AddPickupsEv"); // Interior_c::AddPickups

    CHook::RET("_ZNK35CPedGroupDefaultTaskAllocatorRandom20AllocateDefaultTasksEP9CPedGroupP4CPed");
    CHook::RET("_ZN6CGlass4InitEv");
    CHook::RET("_ZN8CGarages17Init_AfterRestartEv");
    CHook::RET("_ZN6CGangs10InitialiseEv");
    CHook::RET("_ZN5CHeli9InitHelisEv");
    CHook::RET("_ZN9CPedGroup7ProcessEv");
    CHook::RET("_ZN11CFileLoader10LoadPickupEPKc");
#if VER_x32
    CHook::WriteMemory2(g_libGTASA + 0x5A2C04, (uintptr_t)"\x00\x20\x70\x47", 4);//отр
    CHook::NOP(g_libGTASA + 0x3F62D8, 1);//отр

#endif
    ApplyPausePathes();
    DisableAutoAim();

#if VER_x32
    CHook::WriteMemory(g_libGTASA + 0x002D9724, (uintptr_t)0x6061F44F, 4); // allocate memory to 300 * sizeof(CCollisionLink)
#endif

#if VER_x32
    CHook::WriteMemory2(g_libGTASA + 0x001AE8DE, (uintptr_t)"\x01\x22", 2);
#else
    CHook::WriteMemory2(g_libGTASA + 0x23FDE0, (uintptr_t)"\x22\x00\x80\x52", 4);
#endif

    CExtendedCarColors::ApplyPatches_level0();

}

void ApplyShadowPatch()
{

}

void ApplyDefaultPlatePatch()
{
//                CCustomCarPlateMgr
//                NOP ????????? ??????
//	CHook::RET(g_libGTASA + 0x0052FF5C); //CCustomCarPlateMgr::GeneratePlateText
//
//	CHook::RET(g_libGTASA + 0x0052FE90);
//	CHook::RET(g_libGTASA + 0x00530098);
//	CHook::RET(g_libGTASA + 0x00530104);
//	CHook::RET(g_libGTASA + 0x0053012C);
//	CHook::RET(g_libGTASA + 0x00530158);
//	CHook::RET(g_libGTASA + 0x00530190);
//
//	CHook::RET(g_libGTASA + 0x0053021C);
//	CHook::RET(g_libGTASA + 0x0053038C);
//	CHook::RET(g_libGTASA + 0x00530420);
//
//	CHook::RET(g_libGTASA + 0x0050E37C);
}

void ApplyPatches()
{
    Log("Installing patches..");

    CHook::RET("_ZN15CWidgetHelpText7ProcessEv"); // ???????? ??? ????? ???????? ?? ???-?? ???.


    // ?????? ????
    //CHook::WriteMemory(g_libGTASA + 0x00591074, (uintptr_t)0x04, 1);
    //CHook::WriteMemory(g_libGTASA + 0x00591086, (uintptr_t)0xFA, 1);

#if VER_x32



    // ?????? ??????
    CHook::WriteMemory2(g_libGTASA + 0x00442120, (uintptr_t)"\x2C\xE0", 2); // B 0x44217c
    CHook::WriteMemory2(g_libGTASA + 0x0044217C, (uintptr_t)"\x30\x46", 2); // mov r0, r6

    /*// CRadar::DrawEntityBlip (translate color)
    CHook::WriteMemory(g_libGTASA + 0x004404C0, (uintptr_t)"\x3A\xE0", 2); // B 0x440538
    CHook::WriteMemory(g_libGTASA + 0x00440538, (uintptr_t)"\x30\x46", 2); // mov r0, r6

    // CRadar::DrawCoordBlip (translate color)
    CHook::WriteMemory(g_libGTASA + 0x0043FB5E, (uintptr_t)"\x12\xE0", 2); // B 0x43fb86
    CHook::WriteMemory(g_libGTASA + 0x0043FB86, (uintptr_t)"\x48\x46", 2); // mov r0, r9
    CHook::WriteMemory(g_libGTASA + 0x002AB5C6, (uintptr_t)"\x00\x21", 2);*/
#else
    // ?????? ??????
    CHook::WriteMemory2(g_libGTASA + 0x52737C, (uintptr_t)"\x1E\x00\x00\x14", 4); // B 0x5273F4
    CHook::WriteMemory2(g_libGTASA + 0x5273F4, (uintptr_t)"\xE1\x03\x14\x2A", 4); // mov w1, w20

    /*// CRadar::DrawEntityBlip (translate color)
    CHook::WriteMemory(g_libGTASA + 0x5258D8, (uintptr_t)"\x22\x00\x00\x14", 4); // B 0x525960
    CHook::WriteMemory(g_libGTASA + 0x525960, (uintptr_t)"\xE1\x03\x16\x2A", 4); // mov w1, W22

    // CRadar::DrawCoordBlip (translate color)
    CHook::WriteMemory(g_libGTASA + 0x524F58, (uintptr_t)"\xCC\xFF\xFF\x17", 4); // B 0x524E88
    CHook::WriteMemory(g_libGTASA + 0x524E88, (uintptr_t)"\xE1\x03\x16\x2A", 4); // mov w1, W22*/

    // crash legend
    CHook::NOP(g_libGTASA + 0x36A690, 1);
#endif
    // CHook::WriteMemory(g_libGTASA + 0x2AB5C6, (uintptr_t)0x2100, 2);
    CHook::RET("_ZN11CPopulation6AddPedE8ePedTypejRK7CVectorb"); // CPopulation::AddPed
    CHook::RET("_ZN11CPlayerInfo17FindObjectToStealEP4CPed"); // CPlayerInfo::FindObjectToSteal
    CHook::RET("_ZN10CEntryExit19GenerateAmbientPedsERK7CVector"); // CEntryExit::GenerateAmbientPeds
    CHook::RET("_ZN17CWidgetPlayerInfo4DrawEv"); // ????????, ?????, ???? ?????? ? ??
    CHook::RET("_ZN4CHud14SetHelpMessageEPKcPtbbbj"); // CHud::SetHelpMessage
    CHook::RET("_ZN12CAudioEngine16StartLoadingTuneEv"); // ???? ???????????? ??????


    CHook::RET("_ZN10CPedGroups7ProcessEv"); // CPedGroups::Process() crash
    CHook::RET("_ZN21CPedGroupIntelligence7ProcessEv"); // CPedGroupIntelligence::Process crash
    CHook::RET("_ZN19CPedGroupMembership9SetLeaderEP4CPed"); // CPedGroupMembership::SetLeader crash
    CHook::RET("_ZN21CPedGroupIntelligence5FlushEv"); // CPedGroupIntelligence::Flush crash!111111111

    //CHook::NOP(g_libGTASA + 0x005E54FA, 2); // ?? ????????? ??? ????????????. ?????? ?????

    CHook::NOP(g_libGTASA + (VER_x32 ? 0x0043FE5A : 0x52522C), 2);
    CHook::NOP(g_libGTASA + (VER_x32 ? 0x004409AE : 0x525E14), 2);
//	CHook::NOP(g_libGTASA + 0x002AB400, 85); // ???????? ??? ??? ????????? ?????

    //ApplyShadowPatch();

    // DefaultPCSaveFileName
    char* DefaultPCSaveFileName = (char*)(g_libGTASA + (VER_x32 ? 0x006B012C : 0x88CB08));
    memcpy(DefaultPCSaveFileName, "GTASAMP", 8);

#if VER_x32
    CHook::NOP(g_libGTASA + 0x003F61B6, 2);	// CCoronas::RenderSunReflection crash
    CHook::NOP(g_libGTASA + 0x00584884, 2);	// ?? ?????? ??? ??? ?????? ?? ????? 	( ??????, ???????? and etc )
    CHook::NOP(g_libGTASA + 0x00584850, 2);	// ?? ?????? ??? ??? ?????? ?? ?????	( ??????, ???????? and etc )
#else
    CHook::NOP(g_libGTASA + 0x004D8700, 1);  // CCoronas::RenderSunReflection crash
    CHook::NOP(g_libGTASA + 0x006A852C, 1);  // ?? ?????? ??? ??? ?????? ?? ?????   ( ??????, ???????? and etc )
    CHook::NOP(g_libGTASA + 0x006A84E0, 1);  // ?? ?????? ??? ??? ?????? ?? ?????  ( ??????, ???????? and etc )
#endif

    CHook::NOP(g_libGTASA + (VER_x32 ? 0x0046BE88 : 0x55774C), 2);	// CStreaming::ms_memoryAvailable = (int)v24
#if VER_x32
    //CHook::WriteMemory(g_libGTASA + 0x001D16EA, "\x4F\xF4\x00\x10\x4F\xF4\x80\x06", 8); 	// RenderQueue::RenderQueue
    uintptr_t v106 = g_libGTASA + 0x1D16EA;
    if ( mprotect((void *)((g_libGTASA + 0x1D16EA) & 0xFFFFF000), 8u, 7) )
        mprotect((void *)(v106 & 0xFFFFF000), 8u, 3);
    *(uintptr_t *)(g_libGTASA + 0x1D16EA) = 0x1000F44F;
    *(uintptr_t *)(v106 + 4) = 0x680F44F;
    cacheflush(v106, v106 + 8, 0);

    CHook::WriteMemory(g_libGTASA + 0x001D193A, 0x1600F44F, 4); 	// RenderQueue::RenderQueue
#endif




    CHook::RET("_ZN17CVehicleModelInfo17SetCarCustomPlateEv");
    CHook::RET("_Z16SaveGameForPause10eSaveTypesPc");
    CHook::RET("_ZN17CVehicleRecording4LoadEP8RwStreamii");
    CHook::RET("_ZN18CMotionBlurStreaks6UpdateEv");
    CHook::RET("_ZN7CCamera16RenderMotionBlurEv");
    CHook::RET("_ZN26CAEGlobalWeaponAudioEntity21ServiceAmbientGunFireEv");
    CHook::RET("_ZN30CWidgetRegionSteeringSelection4DrawEv");
    CHook::RET("_ZN23CTaskSimplePlayerOnFoot18PlayIdleAnimationsEP10CPlayerPed");
    CHook::RET("_ZN13CCarEnterExit17SetPedInCarDirectEP4CPedP8CVehicleib");
    CHook::RET("_ZN6CRadar10DrawLegendEiii");
    CHook::RET("_ZN6CRadar19AddBlipToLegendListEhi");
    CHook::RET("_ZN11CAutomobile35CustomCarPlate_BeforeRenderingStartEP17CVehicleModelInfo");
    CHook::RET("_ZN11CAutomobile33CustomCarPlate_AfterRenderingStopEP17CVehicleModelInfo");
    CHook::RET("_ZN7CCamera8CamShakeEffff");
    CHook::RET("_ZN7CEntity23PreRenderForGlassWindowEv");
    CHook::RET("_ZN8CMirrors16RenderReflBufferEb");
    CHook::RET("_ZN4CHud23DrawBustedWastedMessageEv");
    CHook::RET("_ZN4CHud24SetHelpMessageStatUpdateEhtff");
    CHook::RET("_ZN6CCheat16ProcessCheatMenuEv");
    CHook::RET("_ZN6CCheat13ProcessCheatsEv");
    CHook::RET("_ZN6CCheat16AddToCheatStringEc");
    CHook::RET("_ZN6CCheat12WeaponCheat1Ev");
    CHook::RET("_ZN6CCheat12WeaponCheat2Ev");
    CHook::RET("_ZN6CCheat12WeaponCheat3Ev");
    CHook::RET("_ZN6CCheat12WeaponCheat4Ev");
    CHook::RET("_ZN8CGarages14TriggerMessageEPcsts");
    CHook::RET("_ZN6CPlane27DoPlaneGenerationAndRemovalEv");
    CHook::RET("_ZN8CCarCtrl31GenerateOneEmergencyServicesCarEj7CVector");
    CHook::RET("_ZN11CPopulation17AddPedAtAttractorEiP9C2dEffect7CVectorP7CEntityi");
    CHook::RET("_ZN7CDarkel26RegisterCarBlownUpByPlayerEP8CVehiclei");
    CHook::RET("_ZN7CDarkel25ResetModelsKilledByPlayerEi");
    CHook::RET("_ZN7CDarkel25QueryModelsKilledByPlayerEii");
    CHook::RET("_ZN7CDarkel27FindTotalPedsKilledByPlayerEi");
    CHook::RET("_ZN7CDarkel20RegisterKillByPlayerEPK4CPed11eWeaponTypebi");
    CHook::RET("_ZN10CPlayerPed14AnnoyPlayerPedEb");
    CHook::RET("_ZN11CPopulation15AddToPopulationEffff");
    CHook::RET("_ZN23CAEPedSpeechAudioEntity11AddSayEventEisjfhhh");
#if VER_x32
    CHook::NOP(g_libGTASA + (g_libGTASA ? 0x0040BF26 : 0x3AC8B2), 2); 	// CMessages::AddBigMessage from CPlayerInfo::KillPlayer

    CHook::NOP(g_libGTASA + 0x004C5902, 2);  // CCamera::ClearPlayerWeaponMode from CPedSamp::ClearWeaponTarget
    //CHook::NOP(g_libGTASA + 0x2FEE76, 2);	// CGarages::RespraysAreFree = true in CRunningScript::ProcessCommands800To899
    CHook::NOP(g_libGTASA + (g_libGTASA ? 0x003F395E : 0x39840A), 2);	// CStreaming::Shutdown from CGame::Shutdown

    //	CHook::WriteMemory(g_libGTASA + 0x2C3868, "\x00\x20\x70\x47", 4); 					// CGameLogic::IsCoopGameGoingOn

    //CHook::WriteMemory(g_libGTASA + 0x001D16EA, "\x4F\xF4\x00\x10\x4F\xF4\x80\x06", 8); 	// RenderQueue::RenderQueue
    //CHook::WriteMemory(g_libGTASA + 0x001D193A, "\x4F\xF4\x00\x16", 4); 	// RenderQueue::RenderQueue

    CHook::WriteMemory2(g_libGTASA + 0x003F4138, "\x03", 1); // RE3: Fix R* optimization that prevents peds to spawn
#else
    CHook::NOP(g_libGTASA + 0x5C3258, 1);  // CCamera::ClearPlayerWeaponMode from CPlayerPed::ClearWeaponTarget
    //CHook::WriteMemory(g_libGTASA + 0x266FC8, "\xF5\x03\x08\x32", 4); //  RenderQueue::RenderQueue
    CHook::WriteMemory2(g_libGTASA + 0x4D644C, "\x1F\x0D\x00\x71", 4); // RE3: Fix R* optimization that prevents peds to spawn
#endif

}
void ApplyInGamePatches()
{
    Log("Installing patches (ingame)..");
    /* ????????????? ????? */
    // CTheZones::ZonesVisited[100]
    memset((void*)(g_libGTASA + (VER_x32 ? 0x0098D252 : 0xC1BF92)), 1, 100);
    // CTheZones::ZonesRevealed
    *(uint32_t*)(g_libGTASA + (VER_x32 ? 0x0098D2B8 : 0xC1BFF8)) = 100;


#if VER_x32
    CHook::WriteMemory2(g_libGTASA + 0x004C36E2, (uintptr_t)"\xE0", 1);
#else
    CHook::WriteMemory2(g_libGTASA + 0x5C0BC4, (uintptr_t)"\x34\x00\x80\x52", 4);
#endif

    CHook::NOP(g_libGTASA + (VER_x32 ? 0x2ABA08 : 0x36A6E8), 2); // ????? ??????? ?????
    CHook::NOP(g_libGTASA + (VER_x32 ? 0x2ABA14 : 0x36A6F8), 2); // ?????? ???????
    CHook::NOP(g_libGTASA + (VER_x32 ? 0x2AB4A6 : 0x36A190), 2); // ???????? ?????????


    CHook::RET("_ZN20CAERadioTrackManager7ServiceEi");
    CHook::NOP(g_libGTASA + (VER_x32 ? 0x00553E96 : 0x674610), 2);
    CHook::NOP(g_libGTASA + (VER_x32 ? 0x00561AC2 : 0x682C1C), 2);
    CHook::NOP(g_libGTASA + (VER_x32 ? 0x0056BED4 : 0x68DD0C), 2);;

    CHook::NOP(g_libGTASA + 0x2ABA08, 2);
    CHook::NOP(g_libGTASA + 0x2AB990, 2);



    // Stop it trying to load tracks2.dat
    //CHook::NOP(g_libGTASA + 0x0057CBD6, 2);

    // Stop it trying to load tracks4.dat
    //CHook::NOP(g_libGTASA + 0x0057CC06, 2);

}


void readVehiclesAudioSettings()
{
    char vehicleModel[50];
    int16_t pIndex = 0;

    FILE* pFile;

    char line[300];

    // Zero VehicleAudioProperties
    memset(VehicleAudioProperties, 0x00, sizeof(VehicleAudioProperties));

    VehicleAudioPropertiesStruct CurrentVehicleAudioProperties;

    memset(&CurrentVehicleAudioProperties, 0x0, sizeof(VehicleAudioPropertiesStruct));

    char buffer[0xFF];
    sprintf(buffer, "%sSAMP/vehicleAudioSettings.cfg", g_pszStorage);
    pFile = fopen(buffer, "r");
    if (!pFile)
    {
        Log("Cannot read vehicleAudioSettings.cfg");
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