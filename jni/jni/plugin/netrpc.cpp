#include "netrpc.h"
#include "xorstr.h"

#include "plugin.h"
#include "common.h"

void DialogBoxRPC(RPCParameters* rpcParams)
{
	reinterpret_cast<void(*)(RPCParameters*)>(CGameAPI::GetBase(xorstr("RPC::DialogBox")))(rpcParams);
}


// слито в @derixtoncrmp
void RegisterRPCs(RakClientInterface* pInterface)
{
#ifdef __arm__
  // 32 bit - ВОССТАНОВЛЕННЫЕ оффсеты с +1
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E3230 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC6CC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DD130 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB5AC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DF8F8 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E0744 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB644 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DDC2C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E2FDC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB7A0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DFF14 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA7B4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E34B8 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E3128 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D92E0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D8C8C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC764 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Weather, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC8A0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC3DC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC1F0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DD8A0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC578 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC4EC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DF1C8 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DCD8C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC264 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DFBE4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E3358 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DED70 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA674 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DF784 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DF6C4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC480 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DDA5C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E31B4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA520 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DCAEC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DD924 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DD318 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E2E78 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DCC64 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D8750 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA7D4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E3428 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC98C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA858 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC364 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DD2A8 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DE338 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC80C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DFAB0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D9760 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DD758 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC3F0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D8D14 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB6D4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DF8A8 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB044 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldTime, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC918 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E04DC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DE1C8 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA4A0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DF2B0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D90C0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA5DC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DFA60 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Pickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA8E4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSelectTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB2E8 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DE25C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC160 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB1B4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DCF70 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DCCFC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_HideActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DCBF4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DDADC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB230 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC2D8 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DAA44 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DB4A4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DC134 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA6F4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DA9D0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DDCD0 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DD01C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DDB84 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D8B58 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E30AC + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D8E00 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3D8DD4 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DCE8C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DD210 + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3DFC80 + 1))); // yes
  // pInterface->RegisterAsRemoteProcedureCall(&RPC_GiveActorDamage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331008 + 1))); // no - BR 0x1AA is not registered as incoming RPC in target 16.34
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E098C + 1))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3E0DC8 + 1))); // yes
#endif
#ifdef __aarch64__
  // 64 bit - ЗАМЕНЕННЫЕ оффсеты для нового билда
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x605078))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB720))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FC79C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FA1D4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x60016C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x601600))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FA2D4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FD7BC))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x604CC0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FA514))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x600A58))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F8D9C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x605498))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x604EC8))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F7614))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F6EA0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB814))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Weather, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FBA2C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB264))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FAFEC))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FD254))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB504))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB41C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FF748))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FC20C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FAF24))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x6005EC))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x605260))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FF16C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F8B98))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FFF3C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FFE1C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB368))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FD4C8))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x604FAC))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F8988))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FBDE4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FD324))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FCAD8))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x604A80))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FC02C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F6704))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F8DC0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x6053B0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FBBB8))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F8EA0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB198))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FCA14))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FE3B0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB928))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x600410))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F7B20))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FD048))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB280))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F6F7C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FA3D0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x6000F8))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F9AA0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldTime, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FBAF4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x601254))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FE174))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F88B8))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FF890))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F7400))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F8AA4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x60039C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Pickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F8F84))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSelectTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F9E2C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FE26C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FAE40))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F9C50))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FC4F0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FC11C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_HideActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FBF70))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FD59C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F9D1C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FB0B4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F91A0))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FA088))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FAE08))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F8C68))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F90D8))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FD8C4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FC61C))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FD6AC))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F6CBC))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x604DFC))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F70D8))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5F70B4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FC398))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FC914))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x6006E4))); // yes
  // pInterface->RegisterAsRemoteProcedureCall(&RPC_GiveActorDamage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4EEE68))); // no - BR 0x1AA is not registered as incoming RPC in target 16.34
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x6019A4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x601FF4))); // yes
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5FCBF0))); // yes
#endif
}