#include "../main.h"
#include "../chatwindow.h"
#include "..//CSettings.h"

#include "game.h"
#include "CRadar.h"

#include "../util/armhook.h"

#include "../graphics/CSkyBox.h"

void ApplyPatches();
void ApplyInGamePatches();
void InstallHooks();
void InitScripting();

uint16_t *szGameTextMessage;
bool bUsedPlayerSlots[PLAYER_PED_SLOTS];
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;

void CGame::RemoveModel(int iModel, bool bFromStreaming)
{
	if (iModel < 0 || iModel > 20000) return;
	
	Log("RemoveModel");
	if (bFromStreaming)
	{
		if (!GetModelReferenceCount(iModel) && IsModelLoaded(iModel))
			((void(*)(int))(SA_ADDR(0x290C4C + 1)))(iModel);
	}
	else
	{
		//if (ScriptCommand(&is_model_available, iModel))
		if (IsModelLoaded(iModel))
			ScriptCommand(&release_model, iModel);
	}
	Log("RemoveModel done!");
}

CGame::CGame()
{
	for (int i = 0; i < HUD_MAX; i++)
	{
		aToggleStatusHUD[i] = true;
	}
	m_pGameCamera = new CCamera();
	m_pGamePlayer = nullptr;

	m_bClockEnabled = true;
	memset(&m_checkpointData, 0, sizeof(m_checkpointData));

	m_bRaceCheckpointsEnabled = 0;
	m_dwRaceCheckpointHandle = 0;
	m_dwRaceCheckpointMarker = 0;

	memset(&bUsedPlayerSlots[0], 0, PLAYER_PED_SLOTS);
}

// 0.3.7
uint8_t CGame::FindFirstFreePlayerPedSlot()
{
	uint8_t x = 2;
	while(x != PLAYER_PED_SLOTS)
	{
		if(!bUsedPlayerSlots[x]) return x;
		x++;
	}

	return 0;
}

// 0.3.7
CPlayerPed* CGame::NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, uint8_t byteCreateMarker)
{
	uint8_t bytePlayerNum = FindFirstFreePlayerPedSlot();
	if(!bytePlayerNum) return nullptr;

	CPlayerPed* pPlayerNew = new CPlayerPed(bytePlayerNum, iSkin, fX, fY, fZ, fRotation);
	if(pPlayerNew && pPlayerNew->m_pPed)
		bUsedPlayerSlots[bytePlayerNum] = true;

	//if(byteCreateMarker) (no xrefs ;( )
	return pPlayerNew;
}

// 0.3.7
void CGame::RemovePlayer(CPlayerPed* pPlayer)
{
	if(pPlayer)
	{
		delete pPlayer;
	}
	bUsedPlayerSlots[pPlayer->m_bytePlayerNumber] = false;
}

// 0.3.7
CVehicle* CGame::NewVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bAddSiren)
{
	CVehicle *pVehicleNew = new	CVehicle(iType, fPosX, fPosY, fPosZ, fRotation, bAddSiren);
	return pVehicleNew;
}

CObject *CGame::NewObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance)
{
	CObject *pObjectNew = new CObject(iModel, fPosX, fPosY, fPosZ, vecRot, fDrawDistance);
	return pObjectNew;
}

uint32_t CGame::CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int* unk)
{
	Log(OBFUSCATE("CreatePickup(%d, %d, %4.f, %4.f, %4.f)"), iModel, iType, fX, fY, fZ);

	if (iModel > 0 && iModel < 20000)
	{
		uintptr_t *dwModelArray = (uintptr_t*)(SA_ADDR(0x87BF48));
    	if (dwModelArray[iModel] == 0)
    		iModel = 18631;
	}
	else iModel = 18631;
	
	if (!IsModelLoaded(iModel))
	{
		RequestModel(iModel);
		LoadRequestedModels();
		while(!IsModelLoaded(iModel))
			usleep(1);
	}

	CVector vecPos = {fX, fY, fZ};
	uint32_t hnd;
	hnd = ((uint32_t (*)(CVector, unsigned int, unsigned char, unsigned int, unsigned int, bool, char *))(SA_ADDR(0x2D8744 + 1)))(vecPos, iModel, iType, 0, 0, false, nullptr); // CPickups::GenerateNewOne
	//ScriptCommand(&create_pickup, iModel, iType, fX, fY, fZ, &hnd);

	int lol = 32 * (uint16_t)hnd;
	if(lol) lol /= 32;
	if(unk) *unk = lol;

	return hnd;
}

void CGame::InitInMenu()
{
	Log(OBFUSCATE("CGame: InitInMenu"));
	
	// Disable shadows
	*(int32_t *)(g_libGTASA + 0x0063E424) = 0;
	
	ApplyPatches();
	InstallHooks();
	GameAimSyncInit();

	szGameTextMessage = new uint16_t[1076];
}

void CGame::InitInGame()
{
	Log(OBFUSCATE("CGame: InitInGame"));

	ApplyInGamePatches();
	InitScripting();
	
	GameResetRadarColors();
}

void CGame::ToggleHUDElement(int iID, bool bToggle)
{
	if (iID < 0 || iID >= HUD_MAX)
	{
		return;
	}
	aToggleStatusHUD[iID] = bToggle;
}

bool CGame::IsToggledHUDElement(int iID)
{
	if (iID < 0 || iID >= HUD_MAX)
	{
		return 1;
	}
	return aToggleStatusHUD[iID];
}

void CGame::HandleChangedHUDStatus()
{
	DisplayHUD(aToggleStatusHUD[HUD_ELEMENT_HUD]);
	ToggleRadar(aToggleStatusHUD[HUD_ELEMENT_MAP]);
	DisplayWidgets(aToggleStatusHUD[HUD_ELEMENT_WIDGETS]);
}

void CGame::SetEnabledPCMoney(bool bEnabled)
{
	if (bEnabled)
		WriteMemory(SA_ADDR(0x599510), (uintptr_t)"$%08d", 6);
	else WriteMemory(SA_ADDR(0x599510), (uintptr_t)"$%d", 6);
}

float CGame::FindGroundZForCoord(float x, float y, float z)
{
	float fGroundZ;
	ScriptCommand(&get_ground_z, x, y, z, &fGroundZ);
	return fGroundZ;
}

// 0.3.7
uint32_t checkpoint_Sphere = 0;
const SCRIPT_COMMAND create_sphere = { 0x03BC, "ffffv" }; // x, y, z, radius, var
const SCRIPT_COMMAND destroy_sphere = { 0x03BD, "i" }; // var
void CGame::SetCheckpoint(VECTOR *pos, VECTOR *extent)
{
	if (!pos || !extent || m_dwRaceCheckpointMarker) return;

	if (m_checkpointData.m_bIsActive)
	{
		DisableCheckpoint();
	}

	m_checkpointData.m_vecPosition.X = pos->X;
	m_checkpointData.m_vecPosition.Y = pos->Y;
	m_checkpointData.m_vecPosition.Z = pos->Z;

	m_checkpointData.m_vecExtent.X = extent->X;
	m_checkpointData.m_vecExtent.Y = extent->Y;
	m_checkpointData.m_vecExtent.Z = extent->Z;

	m_checkpointData.m_dwMarkerId = CreateRadarMarkerIcon(0, pos->X, pos->Y, pos->Z, 1005, 4);
	ScriptCommand(&create_sphere, pos->X, pos->Y, pos->Z, extent->X, &checkpoint_Sphere);
	
	m_checkpointData.m_bIsActive = true;
}

void CGame::DisableCheckpoint()
{
	DisableMarker(m_checkpointData.m_dwMarkerId);
	ScriptCommand(&destroy_sphere, checkpoint_Sphere);
	m_checkpointData.m_dwMarkerId = 0;
	checkpoint_Sphere = 0;
	m_checkpointData.m_bIsActive = false;
}

// 0.3.7
void CGame::SetRaceCheckpointInformation(uint8_t byteType, VECTOR *pos, VECTOR *next, float fSize)
{
	if (m_checkpointData.m_bIsActive)
	{
		DisableCheckpoint();
	}
	
	memcpy(&m_vecRaceCheckpointPos, pos, sizeof(VECTOR));
	memcpy(&m_vecRaceCheckpointNext, next, sizeof(VECTOR));
	m_fRaceCheckpointSize = fSize;
	m_byteRaceType = byteType;

	if (m_dwRaceCheckpointMarker)
	{
		DisableMarker(m_dwRaceCheckpointMarker);

		m_dwRaceCheckpointMarker = NULL;

		m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.X,
			m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z, 1005, 4);
	}

	MakeRaceCheckpoint();
}

// 0.3.7
void CGame::MakeRaceCheckpoint()
{
	DisableRaceCheckpoint();

	ScriptCommand(&create_racing_checkpoint, (int)m_byteRaceType,
				m_vecRaceCheckpointPos.X, m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z,
				m_vecRaceCheckpointNext.X, m_vecRaceCheckpointNext.Y, m_vecRaceCheckpointNext.Z,
				m_fRaceCheckpointSize, &m_dwRaceCheckpointHandle);

	m_bRaceCheckpointsEnabled = true;
}

// 0.3.7
void CGame::DisableRaceCheckpoint()
{
	if (m_dwRaceCheckpointHandle)
	{
		ScriptCommand(&destroy_racing_checkpoint, m_dwRaceCheckpointHandle);
		m_dwRaceCheckpointHandle = NULL;
	}
	m_bRaceCheckpointsEnabled = false;
}

// 0.3.7
void CGame::UpdateCheckpoints()
{
	if(m_checkpointData.m_bIsActive) 
	{
		CPlayerPed *pPlayerPed = this->FindPlayerPed();
		if(pPlayerPed)
		{
			MATRIX4X4 playerMatrix;
			pPlayerPed->GetMatrix(&playerMatrix);
			if (GetDistanceBetween3DPoints(&m_checkpointData.m_vecPosition, &playerMatrix.pos) < 3)
			{
				DisableMarker(m_checkpointData.m_dwMarkerId);
				ScriptCommand(&destroy_sphere, checkpoint_Sphere);
				m_checkpointData.m_dwMarkerId = 0;
				checkpoint_Sphere = 0;
			}
			
			if (!checkpoint_Sphere)
			{
				m_checkpointData.m_dwMarkerId = CreateRadarMarkerIcon(0, m_checkpointData.m_vecPosition.X, m_checkpointData.m_vecPosition.Y, m_checkpointData.m_vecPosition.Z, 1005, 4);
				ScriptCommand(&create_sphere, m_checkpointData.m_vecPosition.X, m_checkpointData.m_vecPosition.Y, m_checkpointData.m_vecPosition.Z, m_checkpointData.m_vecExtent.X, &checkpoint_Sphere);
			}
		}
	}
	else if (m_checkpointData.m_dwMarkerId && checkpoint_Sphere)
	{
		DisableCheckpoint();
	}
	
	if(m_bRaceCheckpointsEnabled) 
	{
		CPlayerPed *pPlayerPed = this->FindPlayerPed();
		if(pPlayerPed)
		{
			if (!m_dwRaceCheckpointMarker)
			{
				m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.X,
					m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z, 1005, 4);
			}
		}
	}
	else if(m_dwRaceCheckpointMarker) 
	{
		DisableMarker(m_dwRaceCheckpointMarker);
		DisableRaceCheckpoint();
		m_dwRaceCheckpointMarker = 0;
	}
}

extern int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char* name);

// 0.3.7
uint32_t CGame::CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor, int iStyle)
{
	uint32_t dwMarkerID = 0;
	bool needMarker = false;

	if (iStyle == 1) 
		ScriptCommand(&create_marker_icon, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else if (iStyle == 2) 
		ScriptCommand(&create_radar_marker_icon, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else if (iStyle == 3)
	{
		ScriptCommand(&create_icon_marker_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);
	}
	else if (iStyle == 4)
	{
		//ScriptCommand(&create_radar_marker_without_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);
		
		needMarker = true;
		dwMarkerID = CRadar__SetCoordBlip(4, fX, fY, fZ, TranslateColorCodeToRGBA(1005), 3, "");
		((void(*)(int, int))(SA_ADDR(0x3DB2F8 + 1)))(dwMarkerID, 0);
		((void(*)(int, int))(SA_ADDR(0x3DB290 + 1)))(dwMarkerID, 3); // CRadar::ChangeBlipScale
		((void(*)(int, int))(SA_ADDR(0x3DB158 + 1)))(dwMarkerID, TranslateColorCodeToRGBA(1005)); // CRadar::ChangeBlipColour
	}
	else
	{
		needMarker = true;
		dwMarkerID = CRadar__SetCoordBlip(4, fX, fY, fZ, 5, 3, "");
		((void(*)(int, int))(SA_ADDR(0x3DB2F8 + 1)))(dwMarkerID, iMarkerType);
	}

	if (!needMarker && iMarkerType == 0)
	{
		if (iColor >= 1004)
		{
			ScriptCommand(&set_marker_color, dwMarkerID, iColor);
			ScriptCommand(&show_on_radar, dwMarkerID, 3);
		}
		else
		{
			ScriptCommand(&set_marker_color, dwMarkerID, iColor);
			ScriptCommand(&show_on_radar, dwMarkerID, 2);
		}
	}
	
	return dwMarkerID;
}

// 0.3.7
uint8_t CGame::GetActiveInterior()
{
	uint32_t dwRet;
	ScriptCommand(&get_active_interior, &dwRet);
	return (uint8_t)dwRet;
}

// 0.3.7
void CGame::SetWorldTime(int iHour, int iMinute)
{
	*(uint8_t*)(SA_ADDR(0x8B18A4)) = (uint8_t)iMinute;
	*(uint8_t*)(SA_ADDR(0x8B18A5)) = (uint8_t)iHour;
	
	((void (*)(unsigned char, unsigned char, unsigned char))(SA_ADDR(0x38C058) + 1))(iHour, iMinute, 0); //  CClock::SetGameClock
}

// 0.3.7
void CGame::SetWorldWeather(unsigned char byteWeatherID)
{
	*(unsigned char*)(SA_ADDR(0x9DB98E)) = byteWeatherID;

	if(!m_bClockEnabled)
	{
		*(uint16_t*)(SA_ADDR(0x9DB990)) = byteWeatherID;
		*(uint16_t*)(SA_ADDR(0x9DB992)) = byteWeatherID;
	}
}

void CGame::ToggleThePassingOfTime(bool bOnOff)
{
	if(bOnOff)
		WriteMemory(SA_ADDR(0x38C154), (uintptr_t)"\x2D\xE9", 2);
	else WriteMemory(SA_ADDR(0x38C154), (uintptr_t)"\xF7\x46", 2);
}

// 0.3.7
void CGame::EnableClock(bool bEnable)
{
	char byteClockData[] = { '%', '0', '2', 'd', ':', '%', '0', '2', 'd', 0 };
	unProtect(SA_ADDR(0x599504));

	if(bEnable)
	{
		ToggleThePassingOfTime(true);
		m_bClockEnabled = true;
		memcpy((void*)(SA_ADDR(0x599504)), byteClockData, 10);
	}
	else
	{
		ToggleThePassingOfTime(false);
		m_bClockEnabled = false;
		memset((void*)(SA_ADDR(0x599504)), 0, 10);
	}
}

// 0.3.7
void CGame::EnableZoneNames(bool bEnable)
{
	ScriptCommand(&enable_zone_names, bEnable);
}

void CGame::DisplayWidgets(bool bDisp)
{
	if(bDisp)
		*(uint16_t*)(SA_ADDR(0x8B82A0 + 0x10C)) = 0;
	else *(uint16_t*)(SA_ADDR(0x8B82A0 + 0x10C)) = 1;
}

// допилить
void CGame::PlaySound(int iSound, float fX, float fY, float fZ)
{
	ScriptCommand(&play_sound, fX, fY, fZ, iSound);
}

void CGame::ToggleRadar(bool iToggle)
{
	*(uint8_t*)(SA_ADDR(0x8EF36B)) = (uint8_t)!iToggle;
}

void CGame::DisplayHUD(bool bDisp)
{
	if(bDisp)
	{	
		// CTheScripts11bDisplayHud
		*(uint8_t*)(SA_ADDR(0x7165E8)) = 1;
		ToggleRadar(true);
	}
	else
	{
		*(uint8_t*)(SA_ADDR(0x7165E8)) = 0;
		ToggleRadar(false);
	}
}

// 0.3.7
/*void CGame::RequestModel(unsigned int iModelID, int iLoadingStream)
{
	ScriptCommand(&request_model, iModelID);
}*/

char g_iLastModelRequest[128];
char g_iLastModelRequestLoad[128];

void CGame::RequestModel(unsigned int iModelID, int iLoadingStream)
{
	sprintf(g_iLastModelRequest, "[RequestModel] start");

	//CStreaming::RequestModel(int,int) 0x0028EB10
	uintptr_t *dwModelArray = (uintptr_t*)(g_libGTASA + 0x87BF48);
	uintptr_t dwModelInfo = dwModelArray[iModelID];
	if (dwModelInfo)
	{
		((void (*)(int32_t, int32_t))(g_libGTASA + 0x0028EB10 + 1))(iModelID, iLoadingStream);
	}
	sprintf(g_iLastModelRequest, "[RequestModel] done");
}

// 0.3.7
void CGame::LoadRequestedModels()
{
	//ScriptCommand(&load_requested_models);
	
	sprintf(g_iLastModelRequestLoad, "[LoadRequestedModels] start");

	//CStreaming::LoadAllRequestedModels(bool) 0x00294CB4
	((void (*)(bool))(g_libGTASA + 0x00294CB4 + 1))(false);

	sprintf(g_iLastModelRequestLoad, "[LoadRequestedModels] done");
}

int g_iLastModelLoaded;
char g_iLastModelStatus[128];

// 0.3.7
uint8_t CGame::IsModelLoaded(unsigned int iModelID)
{
    if(iModelID < 0 || iModelID > 20000) return 0;
	
	g_iLastModelLoaded = iModelID;
	uintptr_t *dwModelArray = (uintptr_t*)(g_libGTASA + 0x87BF48);

	uintptr_t dwModelInfo = dwModelArray[iModelID];
	if (dwModelInfo && *(uintptr_t*)(dwModelInfo + 0x34))
	{
		sprintf(g_iLastModelStatus, "[IsModelLoaded] true");
		return 1;
	}
	sprintf(g_iLastModelStatus, "[IsModelLoaded] false");
	return 0;
	//return ScriptCommand(&is_model_available, iModelID);
}

void CGame::RequestVehicleUpgrade(uint16_t iModelId, uint8_t iLoadingStream) 
{
	if (iModelId >= 1000 && iModelId <= 1193)
	{
		Log("RequestVehicleUpgrade");
		// CStreaming::RequestVehicleUpgrade
		((void (*)(int32_t, int32_t))(g_libGTASA + 0x28F2F8 + 1))(iModelId, iLoadingStream);
		Log("RequestVehicleUpgrade done!");
	}
}

uint8_t CGame::IsModelLoadedScript(unsigned int iModelID)
{
	Log("IsModelLoadedScript");
	return ScriptCommand(&is_model_available, iModelID);
}

// 0.3.7
void CGame::RefreshStreamingAt(float x, float y)
{
	ScriptCommand(&refresh_streaming_at, x, y);
}

// 0.3.7
void CGame::DisableTrainTraffic()
{
	ScriptCommand(&enable_train_traffic, 0);
}

// 0.3.7
void CGame::SetMaxStats()
{
	// CCheat::VehicleSkillsCheat
	(( int (*)())(SA_ADDR(0x2BAED0 + 1)))();

	// CCheat::WeaponSkillsCheat
	(( int (*)())(SA_ADDR(0x2BAE68 + 1)))();

	// CStats::SetStatValue nop
	WriteMemory(SA_ADDR(0x3B9074), (uintptr_t)"\xF7\x46", 2);
}

void CGame::SetWantedLevel(uint8_t byteLevel)
{
	WriteMemory(SA_ADDR(0x27D8D2), (uintptr_t)&byteLevel, 1);
}

uint8_t CGame::GetWantedLevel()
{
	return *(uint8_t*)(g_libGTASA + 0x27D8D2);
}

int CGame::IsAnimationLoaded(char *szAnimFile)
{
	Log(OBFUSCATE("IsAnimationLoaded"));
	uintptr_t pAnimBlock = ((uintptr_t(*)(const char*))(SA_ADDR(0x33DB7C + 1)))(szAnimFile);
	if (!pAnimBlock)
	{
		Log("IsAnimationLoaded done!");
		return -1;
	}

	uint8_t bLoaded = *((uint8_t*)pAnimBlock + 16);
	Log(OBFUSCATE("IsAnimationLoaded bLoaded: %d"), bLoaded);
	return bLoaded;
	//return ScriptCommand(&is_animation_loaded, szAnimFile);
}

void CGame::RequestAnimation(char *szAnimFile)
{
	Log(OBFUSCATE("RequestAnimation"));
	uintptr_t pAnimBlock = ((uintptr_t(*)(const char*))(SA_ADDR(0x33DBCC + 1)))(szAnimFile);
	if(!pAnimBlock)
	{
		Log(OBFUSCATE("RequestAnimation done!"));
		return;
	}

	RequestModel(pAnimBlock + 0x63E7, 0x4);
	//ScriptCommand(&request_animation, szAnimFile);
	Log(OBFUSCATE("RequestAnimation good!"));
}

// 0.3.7
void CGame::DisplayGameText(char* szStr, int iTime, int iType)
{
	ScriptCommand(&text_clear_all);
	CFont::AsciiToGxtChar(szStr, szGameTextMessage);

	// CMessages::AddBigMesssage
	(( void (*)(uint16_t*, int, int))(SA_ADDR(0x4D18C0 + 1)))(szGameTextMessage, iTime, iType);
}

// 0.3.7
void CGame::SetGravity(float fGravity)
{
	unProtect(SA_ADDR(0x3A0B64));
	*(float*)(SA_ADDR(0x3A0B64)) = fGravity;
}

void CGame::ToggleCJWalk(bool bUseCJWalk)
{
	if(bUseCJWalk)
		WriteMemory(SA_ADDR(0x45477E), (uintptr_t)"\xC4\xF8\xDC\x64", 4);
	else makeNOP(SA_ADDR(0x45477E), 2);
}

void CGame::DisableMarker(uint32_t dwMarkerID)
{
	ScriptCommand(&disable_marker, dwMarkerID);
}

// 0.3.7
#include "CWorld.h"
extern char* PLAYERS_REALLOC;
int CGame::GetLocalMoney()
{
	if (pSettings->GetReadOnly().iHUD == 1) return m_bPlayerMoney;
	else
	{
		return *(int*)(PLAYERS_REALLOC+0xB8);
	}
}

// 0.3.7
void CGame::AddToLocalMoney(int iAmmount)
{
	ScriptCommand(&add_to_player_money, 0, iAmmount);
	m_bPlayerMoney += iAmmount;
}

// 0.3.7
void CGame::ResetLocalMoney()
{
	int iMoney = GetLocalMoney();
	if(!iMoney) return;

	if(iMoney < 0)
		AddToLocalMoney(abs(iMoney));
	else
		AddToLocalMoney(-(iMoney));
}

// 0.3.7
void CGame::DisableInteriorEnterExits()
{
	uintptr_t addr = *(uintptr_t*)(SA_ADDR(0x700120));
	int count = *(uint32_t*)(addr + 8);
	Log(OBFUSCATE("Count = %d"), count);

	addr = *(uintptr_t*)addr;

	for(int i=0; i<count; i++)
	{
		*(uint16_t*)(addr + 0x30) = 0;
		addr += 0x3C;
	}
}

extern uint8_t bGZ;
void CGame::DrawGangZone(float fPos[], uint32_t dwColor)
{
    (( void (*)(float*, uint32_t*, uint8_t))(SA_ADDR(0x3DE7F8 + 1)))(fPos, &dwColor, bGZ);
}

uint8_t CGame::IsGamePaused()
{
	return *(uint8_t*)(SA_ADDR(0x8C9BA3));
}