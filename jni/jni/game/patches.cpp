#include "../main.h"
#include "../util/armhook.h"
#include "../util/patch.h"

#include "common.h"
#include "../settings.h"
#include "..//debug.h"
#include "game.h"
#include "CExtendedCarColors.h"

char* PLAYERS_REALLOC = nullptr;
extern CSettings* pSettings;

void WriteUnVerified0();
void InitInGame();

VehicleAudioPropertiesStruct VehicleAudioProperties[20000];

int test_pointsArray[1000];
int test_pointersLibArray[1000];

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
	sprintf(buffer, "%sSAMP/vehicleAudioSettings.cfg", g_pszStorage);
	pFile = fopen(buffer, "r");
	if (!pFile)
	{
		Log("Cannot read vehicleAudioSettings.cfg");
		return;
	}

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

extern void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1);

void ApplyPatches_level0()
{
	// НОВЫЕ ПАТЧИ (УРЕЗАНИЕ РАЗМЕРА ИГРЫ)
	WriteMemory(g_libGTASA + 0x573640, (int)"dxt", 3);
	WriteMemory(g_libGTASA + 0x573668, (int)"dxt", 3);
	WriteMemory(g_libGTASA + 0x57367C, (int)"dxt", 3);
	WriteMemory(g_libGTASA + 0x573690, (int)"dxt", 3);
	WriteMemory(g_libGTASA + 0x5736C8, (int)"dxt", 3);
	WriteMemory(g_libGTASA + 0x5736D8, (int)"dxt", 3);
	WriteMemory(g_libGTASA + 0x5736E8, (int)"dxt", 3);


	PLAYERS_REALLOC = ((char* (*)(uint32_t))(g_libGTASA + 0x179B40))(404 * 257 * sizeof(char));
	memset(PLAYERS_REALLOC, 0, 404 * 257);
	UnFuck(g_libGTASA + 0x5D021C);
	*(char**)(g_libGTASA + 0x5D021C) = PLAYERS_REALLOC;

	// 3 touch begin
	UnFuck(g_libGTASA + 0x005D1E8C);
	memset(test_pointsArray, 0, 999 * sizeof(int));
	*(int**)(g_libGTASA + 0x005D1E8C) = &test_pointsArray[0];

	UnFuck(g_libGTASA + 0x0063D4F0);
	memset(test_pointersLibArray, 0, 999 * sizeof(int));
	*(int**)(g_libGTASA + 0x0063D4F0) = &test_pointersLibArray[0];

	WriteMemory(g_libGTASA + 0x00238232, (uintptr_t)"\x03\x20", 2);
	WriteMemory(g_libGTASA + 0x0025C522, (uintptr_t)"\x02\x2C", 2);
	// 3 touch end

	// col links limits!!!
	WriteMemory(g_libGTASA + 0x1A7DE8, (uintptr_t)"\x01\x22", 2);
	//sizeof(CCollisionLink) = 12 (0xC)
	// we need 200 col links(MALO LI!!!)

	// col links limits end

	CExtendedCarColors::ApplyPatches_level0();

	uint8_t fps = 90;
	if (pSettings)
	{
		fps = pSettings->GetReadOnly().iFPS;
	}
	WriteMemory(g_libGTASA + 0x463FE8, (uintptr_t)&fps, 1);
	WriteMemory(g_libGTASA + 0x56C1F6, (uintptr_t)&fps, 1);
	WriteMemory(g_libGTASA + 0x56C126, (uintptr_t)&fps, 1);
	WriteMemory(g_libGTASA + 0x95B074, (uintptr_t)&fps, 1);

	SetUpHook(g_libGTASA + 0x0023768C, (uintptr_t)ANDRunThread_hook, (uintptr_t*)&ANDRunThread);

	//UnFuck(g_libGTASA + 0x0056CA68);
	//*(uint8_t*)(g_libGTASA + 0x0056CA68) = 1;

	//NOP(g_libGTASA + 0x001A869C, 2);

	// nop random pickups
	NOP(g_libGTASA + 0x00402472, 2);
	NOP(g_libGTASA + 0x003E1AF0, 2);

	// osMutexStuff
	WriteMemory(g_libGTASA + 0x001A7ECE, (uintptr_t)"\x4F\xF0\x01\x00\x00\x46", 6);

	// CdStreamInit(6);
	WriteMemory(g_libGTASA + 0x3981EC, (uintptr_t)"\x06\x20", 2);

	uintptr_t g_libSCAnd = FindLibrary("libSCAnd.so");
	if (g_libSCAnd)
	{
		UnFuck(g_libSCAnd + 0x001E1738);
		UnFuck(g_libSCAnd + 0x001E16DC);
		UnFuck(g_libSCAnd + 0x001E080C);
		strcpy((char*)(g_libSCAnd + 0x001E16DC), "com/rockstargames/hal/andViewManager");
		strcpy((char*)(g_libSCAnd + 0x001E1738), "staticExitSocialClub");
		strcpy((char*)(g_libSCAnd + 0x001E080C), "()V");
	}



	// fix hook crash
	WriteMemory(g_libGTASA + 0x00336618, (uintptr_t)"\x4F\xF4\x6A\x71", 4);
	WriteMemory(g_libGTASA + 0x33661C, (uintptr_t)"\x1A\x4B", 2);
	WriteMemory(g_libGTASA + 0x00274AB4, (uintptr_t)"\x00\x46", 2);

	//WriteMemory(g_libGTASA + 0x001A7EB6, (uintptr_t)"\x00\x23", 2); // MOVS            R3, #0
	//WriteMemory(g_libGTASA + 0x001A7ED2, (uintptr_t)"\x00\x20", 2); // MOVS				R0, #0

	Log("%f", *(float*)(g_libGTASA + 0x8B08F4));
	*(float*)(g_libGTASA + 0x8B08F4) = 300.00001;//delete limit col upper 300m


	WriteUnVerified0();
}
void ApplyShadowPatch()
{
}

void ApplyDefaultPlatePatch()
{
	//CCustomCarPlateMgr all meth
	// убрать дефолтные номера
}
uint32_t* CStreaming__msFiles;
void ApplyPatchesNew() {

	//  NOP(g_libGTASA + 0x40C646, 2); //CPostEffects::Initialise(void)
	//  NOP(g_libGTASA + 0x4087E8, 2); //CGlass::Init
	//  NOP(g_libGTASA + 0x3985A2, 2); //CGlass::Init
	//  NOP(g_libGTASA + 0x39AE5A, 2); //CGlass::Render
	//  NOP(g_libGTASA + 0x39A922, 2); //CGlass::Render
	NOP(g_libGTASA + 0x40E54E, 2); //CGarages::Init
	NOP(g_libGTASA + 0x4FEB1A, 2); //CHeli::SendDownSwat
	NOP(g_libGTASA + 0x4FEA74, 2); //CHeli::FindSwatPositionRelativeToHeli
	NOP(g_libGTASA + 0x4FE8F4, 2); //CHeli::FindSwatPositionRelativeToHeli
	NOP(g_libGTASA + 0x4FEAC4, 2); //CHeli::FindSwatPositionRelativeToHeli
	NOP(g_libGTASA + 0x39AE50, 2); //CRopes::Render
	NOP(g_libGTASA + 0x39A92E, 2); //CRopes::Render
	NOP(g_libGTASA + 0x51AB26, 2); //CCarFXRenderer::RemapDirt
	NOP(g_libGTASA + 0x408800, 2); //CGangs::Initialise
	NOP(g_libGTASA + 0x40881A, 2); //CCranes::InitCranes
	NOP(g_libGTASA + 0x408846, 2); //CSpecialFX::Init*/
	NOP(g_libGTASA + 0x405386, 2); //veh dirt tex init
	//NOP(g_libGTASA + 0x405304, 12);

	//NOP(g_libGTASA + 0x405304, 20);


	//RET(g_libGTASA + 0x398302);
	//NOP(g_libGTASA + 0x408706, 2); //cclothes::init 
	//CGame::Init : \/
   /* NOP(g_libGTASA + 0x4086F2, 2); //load player skin
	NOP(g_libGTASA + 0x4086C2, 2); //post zones init
	NOP(g_libGTASA + 0x4086C6, 2); //post entry exit manager
	NOP(g_libGTASA + 0x4086BE, 2); //loading screen
	NOP(g_libGTASA + 0x408742, 2); //loading screen 2
	NOP(g_libGTASA + 0x40C538, 2); //validate version*/
	//NOP(g_libGTASA + 0x2435D6, 2);
	//NOP(g_libGTASA + 0x3982E6, 2);

	//NOP(g_libGTASA + 0x40E454, 2); //CPools:Init
  //  NOP(g_libGTASA + 0x39B156, 2); //MobileSettings::RRender
  //  NOP(g_libGTASA + 0x39B436, 2);
   // NOP(g_libGTASA + 0x39B3AE, 2);
   // NOP(g_libGTASA + 0x39B3AA, 2); 
}
void applyBlackRussiaPatches() {
	// start reversed br patches
	WriteMemory(g_libGTASA + 0x3981EC, (uintptr_t)"\x06\x20", 2); // no img limit

	NOP(g_libGTASA + 0x56C342, 2);
	NOP(g_libGTASA + 0x4BE6FE, 4);
	NOP(g_libGTASA + 0x339240, 7);
	NOP(g_libGTASA + 0x39B1E4, 2);


	NOP(g_libGTASA + 0x3D373A, 2);
	NOP(g_libGTASA + 0x3DDA6A, 2);
	NOP(g_libGTASA + 0x4E55E2, 2);
	NOP(g_libGTASA + 0x519AC8, 2);

	NOP(g_libGTASA + 0x51013E, 1);
	NOP(g_libGTASA + 0x510166, 1);
	NOP(g_libGTASA + 0x4BEBE6, 2);
	NOP(g_libGTASA + 0x39A844 + 0x518, 2);
	NOP(g_libGTASA + 0x39A844 + 0x207E14, 2);

	NOP(g_libGTASA + 0x39A844 + 0x540, 2);
	// NOP(g_libGTASA + 0x53E4EE, 2); //??? graphics black
	// NOP(g_libGTASA + 0x53E35E, 2); //??? graphics black 2
	NOP(g_libGTASA + 0x2467AA, 1);
	NOP(g_libGTASA + 0x4874E0, 5);

	if (*(uint8_t*)(g_libGTASA + 0x207E58 + 0x130))
	{
		UnFuck(g_libGTASA + 0x63E3A4);
		*(uint8_t*)((char*)g_libGTASA + 0x63E3A4) = 0x64;
		NOP(g_libGTASA + 0x39B1E4, 2);
	}

	/*  UnFuck(g_libGTASA + 0x5D097C);
	  *(uint8_t *)((char *)g_libGTASA + 0x5D097C) = 0x72F31C; //register kills
  */
	UnFuck(g_libGTASA + 0x63D4F0);
	*(uint8_t*)((char*)g_libGTASA + 0x63D4F0) = 0x739F00;

	UnFuck(g_libGTASA + 0x5D1E8C);
	*(uint8_t*)((char*)g_libGTASA + 0x5D1E8C) = 0x738F60;

	NOP(g_libGTASA + 0x45477E, 2);
	NOP(g_libGTASA + 0x405398, 0xE);

	NOP(g_libGTASA + 0x556914, 2);

	UnFuck(g_libGTASA + 0x67E1A8);
	*(uint8_t*)(g_libGTASA + 0x67E1A8) = 0;

	UnFuck(g_libGTASA + 0x67E1AC);
	*(uint8_t*)(g_libGTASA + 0x67E1AC) = 0;

	UnFuck(g_libGTASA + 0x67E1B0);
	*(uint8_t*)(g_libGTASA + 0x67E1B0) = 0;

	UnFuck(g_libGTASA + 0x67E1B4);
	*(uint8_t*)(g_libGTASA + 0x67E1B4) = 0;

	UnFuck(g_libGTASA + 0x67E1B8);
	*(uint8_t*)(g_libGTASA + 0x67E1B8) = 0;

	UnFuck(g_libGTASA + 0x67E1BC);
	*(uint8_t*)(g_libGTASA + 0x67E1BC) = 0;

	WriteMemory(g_libGTASA + 0x51E0AA, (uintptr_t)"\x20\x00\x20\x00", 4u);
	WriteMemory(g_libGTASA + 0x4BEF44, (uintptr_t)"\xE0\x0A", 2u);
	WriteMemory(g_libGTASA + 0x1A7EF2, (uintptr_t)"\x10\x40\xF4\x4F", 8u);
	WriteMemory(g_libGTASA + 0x351700, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x289BB8, (uintptr_t)"\x40", 1);
	// WriteMemory(g_libGTASA + 0x541AC4, (uintptr_t)"\xF7\x46", 2);
	//NOP(g_libGTASA + 0x391E6E, 2);

	WriteMemory(g_libGTASA + 0x2BC08C + 0x1C0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BC08C, (uintptr_t)"\xF7\x46", 2); //cheat asi loader off
	WriteMemory(g_libGTASA + 0x2BA68C + 0x84, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BA68C + 0x2A0, (uintptr_t)"\xF7\x46", 2); //cheat, cheat
	WriteMemory(g_libGTASA + 0x2BAB20, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x2BA68C, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BA68C, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BCDB4, (uintptr_t)"\xF7\x46", 2); //process cheats
	WriteMemory(g_libGTASA + 0x2BC08C + 0xC80, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x284BB8, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x3D4244, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3D4244 + 0x64, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2DC8E0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3474E0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3BF8B4, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x3D541C, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2C1CB0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x401BAC, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x4F90AC, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x504DB8, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x45F1A4, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x4FDC78, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3B67F8, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x494FE4, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2B5C24, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x3E3F38, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3E17F0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3E3F38 + 0x3A8, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x34A380, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x34A380 + 0xAC0, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x34AE58, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x34C6DC, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x34CAE8, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x34D39C, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x34DD88, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x34D39C + 0x3EC, (uintptr_t)"\xF7\x46", 2);

	ApplyPatchesNew();
	// end reversed br patches
}
void applyMordorPatches() {
	// start reversed mordor patches

	NOP(g_libGTASA + 0x51013E, 1u);
	NOP(g_libGTASA + 0x510166, 1u);
	NOP(g_libGTASA + 0x508F36, 2u);
	NOP(g_libGTASA + 0x508F54, 2u);
	NOP(g_libGTASA + 0x39840A, 2u);

	NOP(g_libGTASA + 0x39844E, 2u);
	NOP(g_libGTASA + 0x39845E, 2u);
	NOP(g_libGTASA + 0x3983C6, 2u);
	NOP(g_libGTASA + 0x4E55E2, 2u);

	NOP(g_libGTASA + 0x4E31A6, 2u);
	NOP(g_libGTASA + 0x4EE7D2, 2u);
	NOP(g_libGTASA + 0x4F741E, 2u);
	NOP(g_libGTASA + 0x50AB4A, 2u);
	NOP(g_libGTASA + 0x3688DA, 2u);

	NOP(g_libGTASA + 0x368DF0, 2u);
	NOP(g_libGTASA + 0x369072, 2u);
	NOP(g_libGTASA + 0x369168, 2u);
	NOP(g_libGTASA + 0x519198 - 0x82, 2u);
	NOP(g_libGTASA + 0x519198 - 0x7C, 4u);

	NOP(g_libGTASA + 0x519198, 4u);
	NOP(g_libGTASA + 0x519198, 2u);
	NOP(g_libGTASA + 0x519198 + 0x38, 4u);
	NOP(g_libGTASA + 0x519198 + 0x44, 2u);
	NOP(g_libGTASA + 0x405398, 0xEu);

	NOP(g_libGTASA + 0x266460, 2u);
	NOP(g_libGTASA + 0x2659AA + 0x7C0, 2u);
	NOP(g_libGTASA + 0x266460 + 0x36, 2u);
	NOP(g_libGTASA + 0x2659AA + 0x150, 2u);
	NOP(g_libGTASA + 0x2659AA, 2u);

	NOP(g_libGTASA + 0x2659AA + 0x48, 2u);
	NOP(g_libGTASA + 0x2659AA + 0x86, 2u);

	UnFuck(g_libGTASA + 0x60EAE8);
	*(uint8_t*)((char*)g_libGTASA + 0x60EAE8) = 0x53415447;

	*((uint8_t*)g_libGTASA + 0x60EAE8 + 1) = 0x504D41;

	UnFuck(g_libGTASA + 0x608558);
	*(uint8_t*)((char*)g_libGTASA + 0x608558) = 0x43480000;

	UnFuck(g_libGTASA + 0x3BAC68);
	*(uint8_t*)((char*)g_libGTASA + 0x3BAC68) = 0x43300000;

	UnFuck(g_libGTASA + 0x27D884);
	*(uint8_t*)((char*)g_libGTASA + 0x27D884) = 0x43300000;

	UnFuck(g_libGTASA + 0x8EA7B0);
	memset((char*)g_libGTASA + 0x8EA7B0, 1, 0x64u);

	UnFuck(g_libGTASA + 0x8EA7A8);
	*(uint8_t*)((char*)g_libGTASA + 0x8EA7A8) = 0x64;

	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x120, (uintptr_t)"\x45", 8u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x12E, (uintptr_t)"\x20\xD2", 2u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x134, (uintptr_t)"\x22\xD2", 2u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x160, (uintptr_t)"\x2B\xD2", 2u);
	WriteMemory(g_libGTASA + 0x3AF7E6, (uintptr_t)"\x42", 10u);

	WriteMemory(g_libGTASA + 0x3AF7E6 + 0x10, (uintptr_t)"\x20\xD2", 2u);
	WriteMemory(g_libGTASA + 0x3AF7E6 + 0x16, (uintptr_t)"\x22\xD2", 2u);
	WriteMemory(g_libGTASA + 0x3AF7E6 + 0x3E, (uintptr_t)"\x2B\xD2", 2u);
	WriteMemory(g_libGTASA + 0x3AF3D6 + 0x114, (uintptr_t)"\x4F", 4u);
	WriteMemory(g_libGTASA + 0x3AF3D6 + 0x16, (uintptr_t)"\x4F", 4u);

	WriteMemory(g_libGTASA + 0x3AF3D6 + 0x44, (uintptr_t)"\xB3", 4u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x1C8, (uintptr_t)"\x4F", 8u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x1D6, (uintptr_t)"\x47", 4u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x1DE, (uintptr_t)"\x47", 4u);
	WriteMemory(g_libGTASA + 0x3AF3D6 + 0x5A, (uintptr_t)"\x4F", 8u);

	WriteMemory(g_libGTASA + 0x3AF3D6 + 0x68, (uintptr_t)"\x40", 4u);
	WriteMemory(g_libGTASA + 0x3AF3D6 + 0x70, (uintptr_t)"\x40", 4u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0xC, (uintptr_t)"\x4F", 8u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x26, (uintptr_t)"\x48", 4u);
	WriteMemory(g_libGTASA + 0x3AF1B0, (uintptr_t)"\x48", 4u);

	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x1A, (uintptr_t)"\x48", 8u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x6C, (uintptr_t)"\x4F", 8u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x7A, (uintptr_t)"\x41", 4u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x82, (uintptr_t)"\x41", 4u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0xCA, (uintptr_t)"\x4F", 4u);

	WriteMemory(g_libGTASA + 0x3AF1B0 + 0xD4, (uintptr_t)"\x4F", 4u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0xDC, (uintptr_t)"\x4F", 4u);
	WriteMemory(g_libGTASA + 0x3AF1B0 + 0x10A, (uintptr_t)"\xB3", 4u);
	WriteMemory(g_libGTASA + 0x3AF3D6 + 0xB8, (uintptr_t)"\x4C", 8u);
	WriteMemory(g_libGTASA + 0x3AF3D6 + 0xC6, (uintptr_t)"\x47", 4u);

	WriteMemory(g_libGTASA + 0x3AF3D6 + 0xCE, (uintptr_t)"\x47", 4u);
	WriteMemory(g_libGTASA + 0x405338, (uintptr_t)"\x4F", 4u);
	WriteMemory(g_libGTASA + 0x1A64CE, (uintptr_t)"\x72\x80\xF0\x4F", 8u);
	WriteMemory(g_libGTASA + 0x56C42C, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x28BB46 + 0x12, (uintptr_t)"\x42\xC0\xF4\x4F", 4u);

	WriteMemory(g_libGTASA + 0x3DA500, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3DBB30, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3B67F8, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x494FE4, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x3ECD14 + 0x4A0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3ECD14, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x38FE34, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x5286EC + 0xBE0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x5366D0, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x5286EC, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x52CDCC, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3D4244, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3D4244 + 0x64, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3D62FC, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x2C4694, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2C1CB0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2B0B70, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2B055C, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x461334, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x2BC08C + 0xC80, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BCDB4, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BC08C, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BC08C + 0x1C0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BA68C + 0x84, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x2BA68C + 0x2A0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BAB20, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2BA68C, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3BF8B4, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3B9598, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x284BB8, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2DC8E0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x509408 + 0x150, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x509408, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x3E17F0, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x3474E0, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x2B5C24, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x4F90AC, (uintptr_t)"\xF7\x46", 2);
	WriteMemory(g_libGTASA + 0x56CD78, (uintptr_t)"\x22\x00", 2u);

	// change 0x80000 to 0x1000000
	WriteMemory(g_libGTASA + 0x1A7EF2, (uintptr_t)"\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
	//	WriteMemory(g_libGTASA + 0x3B3C90, (uintptr_t)"\x00\x00\x20\x41", 4);
	WriteMemory(g_libGTASA + 0x1A7F34, (uintptr_t)"\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
	// end reversed mordor patches
}
void ApplyPatches()
{

	//CHook::RET(g_libGTASA + 0x001A6680);
	/*NOP(g_libGTASA + 0x1A6756, 4);
	NOP(g_libGTASA + 0x1A6760, 4);
	NOP(g_libGTASA + 0x185A92, 2);
	CHook::RET(g_libGTASA + 0x1A6770);
	CHook::RET(g_libGTASA + 0x1A8530);*/

	Log("max ipls: %d, sizeof %d", *(int*)(g_libGTASA + 0x608558), sizeof(*(int*)(g_libGTASA + 0x608558)));

	applyMordorPatches();
	applyBlackRussiaPatches();

	//CHook::WriteMemory(0x4E6B2E + g_libGTASA, (uintptr_t)"\x40\xF2\x90\x15", 6); //blackrussia пробив колеса


   // задние фары
	CHook::WriteMemory(g_libGTASA + 0x005197E0, (uintptr_t)"\x04", 1);
	CHook::WriteMemory(g_libGTASA + 0x005197BC, (uintptr_t)"\xFA", 1);

	//CHook::WriteMemory(g_libGTASA + 0x00518D44, (uintptr_t)"\x04", 1);
	//CHook::WriteMemory(g_libGTASA + 0x00518D54, (uintptr_t)"\xFA", 1);
	// fix invalid death id
	CHook::NOP(g_libGTASA + 0x2FEAE6, 2);
	CHook::NOP(g_libGTASA + 0x2FFAFC, 2);
	CHook::NOP(g_libGTASA + 0x300040, 2);
	CHook::NOP(g_libGTASA + 0x2FFB4E, 2);
	CHook::NOP(g_libGTASA + 0x26A2C4, 2);
	//	CHook::NOP(g_libGTASA + 0x4052CC, 2);
		//CHook::RET(g_libGTASA + 0x259E50);

	CHook::NOP(g_libGTASA + 0x046154, 2);
	CHook::NOP(g_libGTASA + 0x2D0A5C, 2);

	//CHook::NOP(g_libGTASA + 0x294ABC, 2);

	//CHook::NOP(g_libGTASA + 0x1A7B00, 1);
	//CHook::NOP(g_libGTASA + 0x3B1AF0, 2);
	//Log(OBFUSCATE("Installing patches..."));


	CDebugInfo::ApplyDebugPatches();

	// CWorld::FindPlayerSlotWithPedPointer
	NOP(g_libGTASA + 0x0045A4C8, 11);
	NOP(g_libGTASA + 0x0045A4E0, 3);

	//DATA XREF: ToggleDebugFPS(void)
//UnFuck(g_libGTASA + 0x008B8018);
//*(uint8_t*)(g_libGTASA + 0x008B8018) = 1;

// CAudioEngine::StartLoadingTune
	NOP(g_libGTASA + 0x56C150, 2);

	// Nop CFileLoader::loadPickups
	//NOP(g_libGTASA + 0x402472, 2);


	// DefaultPCSaveFileName
	char* DefaultPCSaveFileName = (char*)(g_libGTASA + 0x60EAE8);
	memcpy((char*)DefaultPCSaveFileName, "GTASAMP", 8);

	// CWidgetRegionSteeringSelection::Draw
	WriteMemory(g_libGTASA + 0x284BB8, (uintptr_t)"\xF7\x46", 2);

	// CHud::SetHelpMessage
	WriteMemory(g_libGTASA + 0x3D4244, (uintptr_t)"\xF7\x46", 2);
	// CHud::SetHelpMessageStatUpdate
	WriteMemory(g_libGTASA + 0x3D42A8, (uintptr_t)"\xF7\x46", 2);
	// CVehicleRecording::Load
	WriteMemory(g_libGTASA + 0x2DC8E0, (uintptr_t)"\xF7\x46", 2);

	WriteMemory(g_libGTASA + 0x00458D68, (uintptr_t)"\x00\x46\x00\x46", 4); // cFire::extinguish nopped


				  //CStreaming::DeleteRwObjectsAfterDeath(CVector const&)

	//Log("Install STREAMINGpatch...");
				  //UnFuck(g_libGTASA + 0x002C3B82);
	//NOP(g_libGTASA + 0x002C3B82, 2);

	UnFuck(g_libGTASA + 0x00454950);
	NOP(g_libGTASA + 0x00454950, 17); // CAnimManager::RemoveAnimBlockRef

	// CPed pool (old: 140, new: 210)
	WriteMemory(g_libGTASA + 0x3AF2D0, (uintptr_t)"\x45\xF6\xC8\x60\xC0\xF2\x06\x00", 8); // MOV  R0, #0x65EC8 | size=0x7C4 (old: 0x43F30)
	WriteMemory(g_libGTASA + 0x3AF2DE, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
	WriteMemory(g_libGTASA + 0x3AF2E4, (uintptr_t)"\xD2\x22", 2); // MOVS R2, #0xD2
	WriteMemory(g_libGTASA + 0x3AF310, (uintptr_t)"\xD2\x2B", 2); // CMP  R3, #0xD2

	// CPedIntelligence pool (old: 140, new: 210)
	WriteMemory(g_libGTASA + 0x3AF7E6, (uintptr_t)"\x42\xF2\xB0\x00\xC0\xF2\x02\x00\x00\x46", 10); // MOVS R0, #0x220B0 | size=0x298 (old: 0x16B20)
	WriteMemory(g_libGTASA + 0x3AF7F6, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
	WriteMemory(g_libGTASA + 0x3AF7FC, (uintptr_t)"\xD2\x22", 2); // MOVS R2, #0xD2
	WriteMemory(g_libGTASA + 0x3AF824, (uintptr_t)"\xD2\x2B", 2); // CMP  R3, 0xD2

	// Task pool (old: 500, new: 1524 (1536))
	WriteMemory(g_libGTASA + 0x3AF4EA, (uintptr_t)"\x4F\xF4\x40\x30", 4); // MOV.W R0, #30000 | size = 0x80 (old: 0xFA00)
	WriteMemory(g_libGTASA + 0x3AF4F4, (uintptr_t)"\x4F\xF4\xC0\x60", 4); // MOV.W R0, #0x600
	WriteMemory(g_libGTASA + 0x3AF4FC, (uintptr_t)"\x4F\xF4\xC0\x62", 4); // MOV.W R2, #0x600
	WriteMemory(g_libGTASA + 0x3AF52A, (uintptr_t)"\xB3\xF5\xC0\x6F", 4); // CMP.W R3, #0x600

	// TxdStore pool (old: 5000, new: 20000)
	WriteMemory(g_libGTASA + 0x0055BA9A, (uintptr_t)"\x4f\xf4\xb8\x50\xc0\xf2\x11\x00", 8); //  MOV.W	R0, #0x1700 | MOVT	R0, #0x11
	WriteMemory(g_libGTASA + 0x0055BAA8, (uintptr_t)"\x44\xf6\x20\x60", 4); // MOVW	R0, #0x4E20
	WriteMemory(g_libGTASA + 0x0055BAB0, (uintptr_t)"\x44\xf6\x20\x62", 4); // MOVW	R2, #0x4E20

	// Event pool (old: 200, new: 512)
	// Object pool (old: 350, new: 2992)
	WriteMemory(g_libGTASA + 0x003AF3D6, (uintptr_t)"\x4f\xf4\x7e\x40\xc0\xf2\x12\x00", 8); // MOV.W	R0, #0x5900 | MOVT	R0, #0x6
	WriteMemory(g_libGTASA + 0x003AF3E4, (uintptr_t)"\x4f\xf4\x3b\x60", 4); // MOV.W R0, #0xBB0
	WriteMemory(g_libGTASA + 0x003AF3EC, (uintptr_t)"\x4f\xf4\x3b\x62", 4); // MOV.W R2, #0xBB0
	WriteMemory(g_libGTASA + 0x003AF41A, (uintptr_t)"\xb3\xf5\x3b\x6f", 4); // CMP.W R3, #0xBB0

	// Building pool (old: 14000, new: 29585)
	WriteMemory(g_libGTASA + 0x003AF378, (uintptr_t)"\x4F\xF4\x90\x40\xC0\xF2\x19\x00", 8); // mov.w r0, #0x4800 movt r0, #0x19
	WriteMemory(g_libGTASA + 0x003AF386, (uintptr_t)"\x47\xF2\x91\x30", 4); // mov r0, #0x7391
	WriteMemory(g_libGTASA + 0x003AF38E, (uintptr_t)"\x47\xF2\x91\x32", 4); //mov r2, #0x7391

	// Dummys pool (old: 3500, new: 4000)
	WriteMemory(g_libGTASA + 0x003AF430, (uintptr_t)"\x4f\xf4\xd6\x40\xc0\xf2\x03\x00", 8); // MOV.W	R0, #0x6B00 | MOVT	R0, #0x3
	WriteMemory(g_libGTASA + 0x003AF43E, (uintptr_t)"\x40\xf6\xa0\x70", 4); // MOVW            R0, #0xFA0
	WriteMemory(g_libGTASA + 0x003AF446, (uintptr_t)"\x40\xf6\xa0\x72", 4); // MOVW            R2, #0xFA0

	// PtrNode Single pool (old: 75000, new: 100000)
	WriteMemory(g_libGTASA + 0x003AF1BC, (uintptr_t)"\x4f\xf4\x54\x50\xc0\xf2\x0c\x00", 8); // MOV.W	R0, #0x3500 | MOVT	R0, #0xC
	WriteMemory(g_libGTASA + 0x003AF1D6, (uintptr_t)"\x48\xf2\xa0\x62", 4); // MOVW	R2, #0x86A0
	WriteMemory(g_libGTASA + 0x003AF1B0, (uintptr_t)"\x48\xf2\xa0\x66", 4); // MOVW	R6, #0x86A0
	WriteMemory(g_libGTASA + 0x003AF1CA, (uintptr_t)"\x48\xf2\xa0\x60\xc0\xf2\x01\x00", 8); // MOV.W R0, #0x86A0 | MOVT R0, #1

	//PtrNode Double pool (old: 6000, new: 8000)
	WriteMemory(g_libGTASA + 0x003AF21C, (uintptr_t)"\x4f\xf4\xee\x40\xc0\xf2\x01\x00", 8); // MOV.W	R0, #0x7700 | MOVT	R0, #1
	WriteMemory(g_libGTASA + 0x003AF22A, (uintptr_t)"\x41\xf6\x40\x70", 4); // MOVW            R0, #8000
	WriteMemory(g_libGTASA + 0x003AF232, (uintptr_t)"\x41\xf6\x40\x72", 4); // MOVW            R2, #8000

	// Entry Node Info pool (old: 500, new: 5120)
	WriteMemory(g_libGTASA + 0x003AF27A, (uintptr_t)"\x4f\xf4\xc8\x30", 4); // MOV.W	R0, #0x19000 | size = 0x14
	WriteMemory(g_libGTASA + 0x003AF284, (uintptr_t)"\x4f\xf4\xa0\x50", 4); // MOV.W R0, #0x1400
	WriteMemory(g_libGTASA + 0x003AF28C, (uintptr_t)"\x4f\xf4\xa0\x52", 4); // MOV.W R2, #0x1400
	WriteMemory(g_libGTASA + 0x003AF2BA, (uintptr_t)"\xb3\xf5\xa0\x5f", 4); // CMP.W R3, #0x1400

	// ColModel pool (old:10150, new: 32511)
	WriteMemory(g_libGTASA + 0x3AF48E, (uintptr_t)"\x4C\xF6\xD0\x70\xC0\xF2\x17\x00", 8); // MOV R0, #0x17CFD0 | size=0x30 (old: 0x76F20)
	WriteMemory(g_libGTASA + 0x3AF49C, (uintptr_t)"\x47\xF6\xFF\x60", 4); // MOVW R0, #0x7EFF
	WriteMemory(g_libGTASA + 0x3AF4A4, (uintptr_t)"\x47\xF6\xFF\x62", 4); // MOVW R2, #0x7EFF

	// VehicleStruct increase (0x32C*0x50 = 0xFDC0)
	WriteMemory(g_libGTASA + 0x405338, (uintptr_t)"\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
	WriteMemory(g_libGTASA + 0x405342, (uintptr_t)"\x50\x20", 2);			// MOVS R0, #0x50
	WriteMemory(g_libGTASA + 0x405348, (uintptr_t)"\x50\x22", 2);			// MOVS R2, #0x50
	WriteMemory(g_libGTASA + 0x405374, (uintptr_t)"\x50\x2B", 2);			// CMP  R3, #0x50

	// Increase matrix count in CPlaceable::InitMatrixArray 
	WriteMemory(g_libGTASA + 0x3ABB0A, (uintptr_t)"\x4F\xF4\x7A\x61", 4); // MOV.W R1, #4000

	// stop assign 0xFFFFFF to beam color!
	NOP(g_libGTASA + 0x00519116, 2);
	NOP(g_libGTASA + 0x0051911C, 4);
	NOP(g_libGTASA + 0x00519198, 4);
	NOP(g_libGTASA + 0x00519198, 2);
	NOP(g_libGTASA + 0x005191D0, 4);
	NOP(g_libGTASA + 0x005191DC, 2);


	CHook::NOP(g_libGTASA + 0x28E7EE, 2);
	CHook::NOP(g_libGTASA + 0x28E7F4, 2);
	*((uint8_t*)g_libGTASA + 0x28E7EC) = 10;

	// Prevent another ped from answering when collision happens (or damage)
	//CHook::RET(g_libGTASA + 0x00457644);

	// CPed::Say
	CHook::NOP(g_libGTASA + 0x43E288, 2);

	/*WriteMemory(g_libGTASA + 0x1A6684, (uintptr_t)"\xF7\x46", 2);

WriteMemory(g_libGTASA + 0x1A656A, (uintptr_t)"\x2C\x08", 2);
WriteMemory(g_libGTASA + 0x1A7D04, (uintptr_t)"\x2C\x08", 2);
WriteMemory(g_libGTASA + 0x1A7DE8, (uintptr_t)"\x2C\x08", 2);
WriteMemory(g_libGTASA + 0x1A76D0, (uintptr_t)"\x2C\x08", 2);
WriteMemory(g_libGTASA + 0x1A68D0, (uintptr_t)"\x2C\x08", 2);
/*	//MORDOR
	NOP(g_libGTASA + 0x51013E, 1);
  NOP(g_libGTASA + 0x510166, 1);
  NOP(g_libGTASA + 0x508F36, 2);
  NOP(g_libGTASA + 0x508F54, 2);
  NOP(g_libGTASA + 0x39840A, 2);
  NOP(g_libGTASA + 0x39844E, 2);
  NOP(g_libGTASA + 0x39845E, 2);
  NOP(g_libGTASA + 0x3983C6, 2);
  NOP(g_libGTASA + 0x4E55E2, 2);
  NOP(g_libGTASA + 0x408AAA, 2);
  NOP(g_libGTASA + 0x3AC8B2, 2);
  NOP(g_libGTASA + 0x3A019C, 2);
  NOP(g_libGTASA + 0x50A140, 2);
  NOP(g_libGTASA + 0x2C3EA2, 2);
  NOP(g_libGTASA + 0x4E31A6, 2);
  NOP(g_libGTASA + 0x4EE7D2, 2);
  NOP(g_libGTASA + 0x4F741E, 2);
  NOP(g_libGTASA + 0x3DCA90, 2);
  NOP(g_libGTASA + 0x4E31A6, 2);
  NOP(g_libGTASA + 0x4EE7D2, 2);
  NOP(g_libGTASA + 0x4F741E, 2);
  NOP(g_libGTASA + 0x50AB4A, 2);
  NOP(g_libGTASA + 0x3688DA, 2);
  NOP(g_libGTASA + 0x368DF0, 2);
  NOP(g_libGTASA + 0x369072, 2);
  NOP(g_libGTASA + 0x369168, 2);


  CHook::RET(0x3693E4 + g_libGTASA);
  CHook::RET(0x19981C + g_libGTASA); //opasno


  CHook::RET(g_libGTASA + 0x3AC114);
  CHook::RET(g_libGTASA + 0x3DA500);
  CHook::RET(0x3DBB30 + g_libGTASA);
  CHook::RET(0x3B67F8 + g_libGTASA);
  CHook::RET(0x494FE4 + g_libGTASA);
  CHook::RET(0x3ECD14 + g_libGTASA + 0x4A0);
  CHook::RET(0x3ECD14 + g_libGTASA);
  CHook::RET(0x38FE34 + g_libGTASA);
  CHook::RET(0x5286EC + g_libGTASA + 0xBE0);
  CHook::RET(0x5366D0 + g_libGTASA);
  CHook::RET(0x5286EC + g_libGTASA);
  CHook::RET(0x52CDCC + g_libGTASA);
  CHook::RET(0x3D4244 + g_libGTASA);
  CHook::RET(0x3D4244 + g_libGTASA + 0x64);
  CHook::RET(0x3D62FC + g_libGTASA);
  CHook::RET(0x2C4694 + g_libGTASA);
  CHook::RET(0x2C1CB0 + g_libGTASA);
  CHook::RET(g_libGTASA + 0x2B0B70);
  CHook::RET(0x2B055C + g_libGTASA);
  CHook::RET(0x461334 + g_libGTASA);
  CHook::RET(0x2BC08C + g_libGTASA + 0xC80);
  CHook::RET(0x2BCDB4 + g_libGTASA);
  CHook::RET(0x2BC08C + g_libGTASA);
  CHook::RET(0x2BC08C + g_libGTASA + 0x1C0);
  CHook::RET(0x2BA68C + g_libGTASA + 0x84);
  CHook::RET(0x2BA68C + g_libGTASA + 0x2A0);
  CHook::RET(0x2BAB20 + g_libGTASA);
  CHook::RET(0x2BA68C + g_libGTASA);
  CHook::RET(0x3BF8B4 + g_libGTASA);
  CHook::RET(0x3B9598 + g_libGTASA);
  CHook::RET(0x284BB8 + g_libGTASA);
  CHook::RET(g_libGTASA + 0x2DC8E0);
  CHook::RET(0x509408 + g_libGTASA + 0x150);
  CHook::RET(0x509408 + g_libGTASA);
  CHook::RET(0x3E17F0 + g_libGTASA);
  CHook::RET(0x3474E0 + g_libGTASA);
  CHook::RET(0x2B5C24 + g_libGTASA);
  CHook::RET(0x4F90AC + g_libGTASA);


  NOP(g_libGTASA + 0x45477E, 5);
  NOP(0x43E288 + g_libGTASA, 2);
  NOP(0x434D94 + g_libGTASA, 6);
  NOP(g_libGTASA + 0x3DCA90, 2);
  NOP(0x3DD4A4 + g_libGTASA, 2);
  NOP(0x55EFB8 + g_libGTASA, 2);
  NOP(0x55EFB8 + g_libGTASA + 0x940, 2);
  NOP(0x519198 + g_libGTASA - 0x7C, 4);
  NOP(0x519198 + g_libGTASA, 4);
  NOP(0x519198 + g_libGTASA, 2);
  NOP(0x39A1D8 + g_libGTASA, 2);
  CHook::RET(g_libGTASA + 0x56C42C);

	//END MORDOR

	//BR
	CHook::RET(0x2BC08C + g_libGTASA + 0x1C0);
  CHook::RET(0x2BC08C + g_libGTASA);
  CHook::RET(0x2BA68C + g_libGTASA + 0x84);
  CHook::RET(0x2BA68C + g_libGTASA + 0x2A0);
  CHook::RET(0x2BAB20 + g_libGTASA);
  CHook::RET(0x2BA68C + g_libGTASA);
  CHook::RET(0x2BA68C + g_libGTASA);
  CHook::RET(0x2BCDB4 + g_libGTASA);
  CHook::RET(0x2BC08C + g_libGTASA + 0xC80);
  CHook::RET(0x284BB8 + g_libGTASA);
  CHook::RET(0x3D4244 + g_libGTASA);
  CHook::RET(g_libGTASA + 0x2DC8E0);
  CHook::RET(0x3474E0 + g_libGTASA);
  CHook::RET(0x3BF8B4 + g_libGTASA);
  CHook::RET(0x3D541C + g_libGTASA);
  CHook::RET(0x2C1CB0 + g_libGTASA);
  CHook::RET(0x401BAC + g_libGTASA);
  CHook::RET(0x4F90AC + g_libGTASA);
  CHook::RET(0x504DB8 + g_libGTASA);
  CHook::RET(0x45F1A4 + g_libGTASA);
  CHook::RET(0x4FDC78 + g_libGTASA);
  CHook::RET(0x3B67F8 + g_libGTASA);
  CHook::RET(0x494FE4 + g_libGTASA);
  CHook::RET(0x2B5C24 + g_libGTASA);
  CHook::RET(0x3E3F38 + g_libGTASA);
  CHook::RET(0x3E17F0 + g_libGTASA);
  CHook::RET(0x3E3F38 + g_libGTASA + 0x3A8);
  CHook::RET(0x34A380 + g_libGTASA);
  CHook::RET(0x34A380 + g_libGTASA + 0xAC0);
  CHook::RET(0x34AE58 + g_libGTASA);
  CHook::RET(0x34C6DC + g_libGTASA);
  CHook::RET(0x34CAE8 + g_libGTASA);
  CHook::RET(0x34D39C + g_libGTASA);
  CHook::RET(0x34DD88 + g_libGTASA);


  CHook::RET(g_libGTASA + 0x351700);

  CHook::RET(0x34D39C + g_libGTASA + 0x3EC);




 /* WriteMemory(g_libGTASA + 0x1A6684, (uintptr_t)"\xF7\x46", 2);

WriteMemory(g_libGTASA + 0x1A656A, (uintptr_t)"\x2C\x08", 2);
WriteMemory(g_libGTASA + 0x1A7D04, (uintptr_t)"\x2C\x08", 2);
WriteMemory(g_libGTASA + 0x1A7DE8, (uintptr_t)"\x2C\x08", 2);
WriteMemory(g_libGTASA + 0x1A76D0, (uintptr_t)"\x2C\x08", 2);
WriteMemory(g_libGTASA + 0x1A68D0, (uintptr_t)"\x2C\x08", 2);*/






/* NOP(0x398A3A + g_libGTASA, 2);
 NOP(0x398A34 + g_libGTASA, 2);
 NOP(g_libGTASA + 0x2C2C22, 4);
 NOP(g_libGTASA + 0x45477E, 5);
 NOP(g_libGTASA + 0x3DCA90, 2);
 NOP(0x3DD4A4 + g_libGTASA, 2);
 NOP(0x55EFB8 + g_libGTASA, 2);
 NOP(g_libGTASA + 0x4E31A6, 2);
 NOP(g_libGTASA + 0x4EE7D2, 2);
 NOP(g_libGTASA + 0x4F741E, 2);
 NOP(0x50AB4A + g_libGTASA, 2);

 NOP(0x4874E0 + g_libGTASA, 5);

 NOP(0x39B2C0 + g_libGTASA + 4, 2);

 NOP(g_libGTASA + 0x3A019C, 2);
 NOP(0x39B2C0 + g_libGTASA, 2);

 NOP(g_libGTASA + 0x4045D2, 1);
 NOP(0x56C150 + g_libGTASA, 2);
 NOP(g_libGTASA + 0x2665EE, 2);


 NOP(0x3981EC + g_libGTASA + 0x1DA, 2);
 NOP(g_libGTASA + 0x4E55E2, 2);
 NOP(0x519AC8 + g_libGTASA, 2);
 NOP(g_libGTASA + 0x51013E, 1);
 NOP(g_libGTASA + 0x510166, 1);

 CHook::RET(0x4BDB18 + g_libGTASA);

 //END BR



   */
	uint8_t fps_ = 120;
	float fps_timeStep = (fps_ / 428.5714);
	float timeStep = ((int)((100 * fps_timeStep)) / 100.0);

	UnFuck(g_libGTASA + 0x46C03C, 2);
	*(float*)(g_libGTASA + 0x46C03C) = timeStep;
}


void ApplyInGamePatches()
{

	Log("Installing patches (ingame)..");

	//copebar patches
	WriteMemory(g_libGTASA + 0x52DD38, (uintptr_t)"\x00\x20\x70\x47", 4);
	NOP(g_libGTASA + 0x39AD14, 1);
	memcpy((uint32_t*)(g_libGTASA + 0x5DE734), "0x10000000", 10);
	*(float*)(g_libGTASA + 0x608558) = 0.33f;

	Log("max ipls: %d, sizeof %d", *(int*)(g_libGTASA + 0x608558), sizeof(*(int*)(g_libGTASA + 0x608558)));

	*(float*)(g_libGTASA + 0x5DEE7C) = 2.0f;

	/* Разблокировка карты */
	// CTheZones::ZonesVisited[100]
	memset((void*)(g_libGTASA + 0x8EA7B0), 1, 100);
	// CTheZones::ZonesRevealed
	*(uint32_t*)(g_libGTASA + 0x8EA7A8) = 100;
	//*(uint32_t*)(g_libGTASA + 0x40484A) = 7;

	// CTaskSimplePlayerOnFoot::PlayIdleAnimations 
	WriteMemory(g_libGTASA + 0x4BDB18, (uintptr_t)"\x70\x47", 2);


	// CarCtl::GenerateRandomCars nulled from CGame::Processf
	UnFuck(g_libGTASA + 0x398A3A);
	NOP(g_libGTASA + 0x398A3A, 2);

	// CTheCarGenerators::Process nulled from CGame::Process
	UnFuck(g_libGTASA + 0x398A34);
	NOP(g_libGTASA + 0x398A34, 2);

	// множитель для MaxHealth
	UnFuck(g_libGTASA + 0x3BAC68);
	*(float*)(g_libGTASA + 0x3BAC68) = 176.0f;
	// множитель для Armour
	UnFuck(g_libGTASA + 0x27D884);
	*(float*)(g_libGTASA + 0x27D884) = 176.0;

	// CEntryExit::GenerateAmbientPeds nulled from CEntryExit::TransitionFinished
	UnFuck(g_libGTASA + 0x2C2C22);
	NOP(g_libGTASA + 0x2C2C22, 4);

	NOP(g_libGTASA + 0x003D6FDC, 2);
	NOP(g_libGTASA + 0x0026B214, 2);

	// ПОТРАЧЕНО
	WriteMemory(g_libGTASA + 0x3D62FC, (uintptr_t)"\xF7\x46", 2);

	// CPlayerPed::CPlayerPed task fix
	WriteMemory(g_libGTASA + 0x458ED1, (uintptr_t)"\xE0", 1);

	// ReapplyPlayerAnimation (хз зачем)
	NOP(g_libGTASA + 0x45477E, 5);

	// radar draw blips
	UnFuck(g_libGTASA + 0x3DCA90);
	NOP(g_libGTASA + 0x3DCA90, 2);
	UnFuck(g_libGTASA + 0x3DD4A4);
	NOP(g_libGTASA + 0x3DD4A4, 2);
	// CCamera::CamShake from CExplosion::AddExplosion
	NOP(g_libGTASA + 0x55EFB8, 2);
	NOP(g_libGTASA + 0x55F8F8, 2);

	// camera_on_actor path
	UnFuck(g_libGTASA + 0x2F7B68);
	*(uint8_t*)(g_libGTASA + 0x2F7B6B) = 0xBE;

	// CPed::RemoveWeaponWhenEnteringVehicle (GetPlayerInfoForThisPlayerPed)
	UnFuck(g_libGTASA + 0x434D94);
	NOP(g_libGTASA + 0x434D94, 6);

	// CBike::ProcessAI
	UnFuck(g_libGTASA + 0x4EE200);
	*(uint8_t*)(g_libGTASA + 0x4EE200) = 0x9B;

	// CWidgetPlayerInfo::DrawWanted
	WriteMemory(g_libGTASA + 0x27D8D0, (uintptr_t)"\x4F\xF0\x00\x08", 4);


	NOP(g_libGTASA + 0x00398768, 2); // nop police helis
	NOP(g_libGTASA + 0x003987DC, 2);

	NOP(g_libGTASA + 0x003688EC, 2); // nop ServiceAmbientGunFire

	//todo CPlayerPed::ProcessAnimGroups in the end
}