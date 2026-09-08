#include "netrpc.h"
#include "xorstr.h"

#include "plugin.h"
#include "common.h"

static void IgnoreRemotePlayerRPC(RPCParameters*)
{
}

void DialogBoxRPC(RPCParameters* rpcParams)
{
	reinterpret_cast<void(*)(RPCParameters*)>(CGameAPI::GetBase(xorstr("RPC::DialogBox")))(rpcParams);
}

void RegisterRPCs(RakClientInterface* pInterface)
{
#ifdef __arm__
  // 32 bit - ВОССТАНОВЛЕННЫЕ оффсеты с +1
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20E2A0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209208 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209D14 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2087BC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B6A4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20C2E8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208854 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A6B4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20E04C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2089A8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20BBC4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x207FF4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20E5AC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20E198 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x207570 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x207288 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2092C8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Weather, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209404 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208EE4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208CC8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A2A0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2090B8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209018 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20AF58 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2098F8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208D3C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B980 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20E3C8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20AB78 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, IgnoreRemotePlayerRPC);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B4D4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B404 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208F90 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A448 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20E224 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x207D64 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209658 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A324 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209EA4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20DEE8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2097D0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x206DD4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208014 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20E4E4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2094F0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208098 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208E6C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209E34 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20AA04 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209370 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B980 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20781C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A160 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208EF8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x207310 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2088E4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B638 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20840C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldTime, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20947C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20C0A4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A8B0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x207CE4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B068 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, IgnoreRemotePlayerRPC);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x207E20 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B84C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Pickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208124 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSelectTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20860C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A928 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208C38 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2084D8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209AB4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209868 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_HideActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209760 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A564 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208554 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208DB0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208218 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208748 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x208C0C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x207F38 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2081A4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A764 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209BFC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20A60C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2071C4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20E11C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, IgnoreRemotePlayerRPC);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2073B0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2099F8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209D9C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20B9EC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_GiveActorDamage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x1C4A08 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20C50C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x20C9E8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x209F4C + 1)));
#endif
}
