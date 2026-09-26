#include "netrpc.h"
#include "xorstr.h"

#include "plugin.h"
#include "common.h"

static void (*g_RPC_InitGame_Handler)(RPCParameters*) = nullptr;
static void (*g_RPC_ScrDialogBox_Handler)(RPCParameters*) = nullptr;
static void (*g_RPC_ScrSetSpawnInfo_Handler)(RPCParameters*) = nullptr;
static void (*g_RPC_ServerJoin_Handler)(RPCParameters*) = nullptr;
static void (*g_RPC_WorldVehicleAdd_Handler)(RPCParameters*) = nullptr;
static void (*g_RPC_WorldPlayerAdd_Handler)(RPCParameters*) = nullptr;
static void (*g_RPC_ScrSetPlayerAttachedObject_Handler)(RPCParameters*) = nullptr;
static void (*g_RPC_ScrCustomizeVehicle_Handler)(RPCParameters*) = nullptr;

static void Hook_RPC_InitGame(RPCParameters* rpcParams)
{
	if (g_RPC_InitGame_Handler) {
		FixBrokenRPC(RPC_InitGame, rpcParams, g_RPC_InitGame_Handler);
	}
}

static void Hook_RPC_ScrDialogBox(RPCParameters* rpcParams)
{
	if (g_RPC_ScrDialogBox_Handler) {
		FixBrokenRPC(RPC_ScrDialogBox, rpcParams, g_RPC_ScrDialogBox_Handler);
	}
}

static void Hook_RPC_ScrSetSpawnInfo(RPCParameters* rpcParams)
{
	if (g_RPC_ScrSetSpawnInfo_Handler) {
		FixBrokenRPC(RPC_ScrSetSpawnInfo, rpcParams, g_RPC_ScrSetSpawnInfo_Handler);
	}
}

static void Hook_RPC_ServerJoin(RPCParameters* rpcParams)
{
	if (g_RPC_ServerJoin_Handler) {
		FixBrokenRPC(RPC_ServerJoin, rpcParams, g_RPC_ServerJoin_Handler);
	}
}

static void Hook_RPC_WorldPlayerAdd(RPCParameters* rpcParams)
{
	if (g_RPC_WorldPlayerAdd_Handler) {
		FixBrokenRPC(RPC_WorldPlayerAdd, rpcParams, g_RPC_WorldPlayerAdd_Handler);
	}
}

static void Hook_RPC_WorldVehicleAdd(RPCParameters* rpcParams)
{
	if (g_RPC_WorldVehicleAdd_Handler) {
		FixBrokenRPC(RPC_WorldVehicleAdd, rpcParams, g_RPC_WorldVehicleAdd_Handler);
	}
}

static void Hook_RPC_ScrSetPlayerAttachedObject(RPCParameters* rpcParams)
{
	(void)rpcParams;
	// BR sends a different attached-object payload here; letting the SA-MP-style
	// native handler read it crashes during remote-player stream-in. Ignore it
	// until the packet layout is mapped correctly.
}

static void Hook_RPC_ScrCustomizeVehicle(RPCParameters* rpcParams)
{
	if (g_RPC_ScrCustomizeVehicle_Handler) {
		SetCustomizeVehicleHandler(g_RPC_ScrCustomizeVehicle_Handler);
		FixBrokenRPC(RPC_ScrCustomizeVehicle, rpcParams, g_RPC_ScrCustomizeVehicle_Handler);
	}
}

void DialogBoxRPC(RPCParameters* rpcParams)
{
	reinterpret_cast<void(*)(RPCParameters*)>(CGameAPI::GetBase(xorstr("RPC::DialogBox")))(rpcParams);
}

void RegisterRPCs(RakClientInterface* pInterface)
{
#ifdef __aarch64__
   pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B550C))); //
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF43C))); //
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B07E8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AE760)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B21FC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B301C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AE850)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B13AC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B5194)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AEA60)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B2964)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADCCC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B597C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B537C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AD248)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ACD00)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF54C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Weather, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF73C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF088)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AED6C)));
  // RPC_ScrStopObject: RPC id 122 has no registered handler in this client build
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF328)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF240)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B1E30)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AFE64)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AEE18)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B2634)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B56E4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B1A2C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADAF4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B1F78)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B0178)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF184)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B101C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B5450)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AD90C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AFA94)));
  // RPC_ScrSetObjectRotation: RPC id 46 has no registered handler in this client build
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B0A60)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B4F64)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AFCAC)));
   g_RPC_InitGame_Handler = (void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AC67C);
   pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, Hook_RPC_InitGame);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADCF0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B5850)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF890)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADDB4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AEFD0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B0994)));
  
  g_RPC_ScrDialogBox_Handler = (void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B1868);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, Hook_RPC_ScrDialogBox);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF650)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B24FC)));
  g_RPC_WorldVehicleAdd_Handler = (void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AD308);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, Hook_RPC_WorldVehicleAdd);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B0E40)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF0A4)));
  
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ACDC4))); //
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AE938)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B2168)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AE280)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldTime, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AF7E8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B3018)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B1684)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AD854)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AFFDC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AD190)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADA14)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B2468)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Pickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADE7C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSelectTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AE544)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B1734)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AEC9C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AE388)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B0560)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AFD88)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_HideActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AFC0C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B11BC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AE444)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AEEC4)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADFE8)));//
  g_RPC_ScrSetSpawnInfo_Handler = (void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AE6B4);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, Hook_RPC_ScrSetSpawnInfo);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AEC64)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADBAC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADF3C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B14B4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B0688)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B12B4)));
  g_RPC_ServerJoin_Handler = (void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ACBE4);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, Hook_RPC_ServerJoin);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B52C0)));
  g_RPC_WorldPlayerAdd_Handler = (void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ACFE8);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, Hook_RPC_WorldPlayerAdd);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ACFC8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B0204)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B08B0)));
  g_RPC_ScrSetPlayerAttachedObject_Handler = (void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B26D4);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, Hook_RPC_ScrSetPlayerAttachedObject);
  g_RPC_ScrCustomizeVehicle_Handler = (void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B3380);
  SetCustomizeVehicleHandler(g_RPC_ScrCustomizeVehicle_Handler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, Hook_RPC_ScrCustomizeVehicle);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B3A20)));
  // RPC_GiveActorDamage: RPC id 177 has no registered handler in this client build
#endif
}
