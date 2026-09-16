#include "netrpc.h"
#include "xorstr.h"

#include "plugin.h"
#include "common.h"

void DialogBoxRPC(RPCParameters* rpcParams)
{
  reinterpret_cast<void(*)(RPCParameters*)>(CGameAPI::GetBase(xorstr("RPC::DialogBox")))(rpcParams);
}

void RegisterRPCs(RakClientInterface* pInterface)
{
#ifdef __aarch64__
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4AC20))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3FC3C))); // нашел но есть подозрение
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40CB0))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3E734))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA44980))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA45FE4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3E834))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA41D68))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4A868))); // + (возможно не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3EA7C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA45270))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D408))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4B040))); // нашел но возможно не верный
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4AA70))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3BD5C))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3B44C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3FD30))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Weather, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3FF3C))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F780))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F440))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA417F4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3FA20))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F938))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA43F80))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40720))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F508))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA44DA8))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4AE08))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA43980))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D208))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA44750))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA44630))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F884))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA41A74))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4AB54))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3CFF8))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA402F4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA418C4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA41090))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4A628))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40540))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3ACC4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D42C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4AF58))); // нашел, но возможно не верный
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA400C8))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D50C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F6B4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40F3C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA42E1C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3FE44))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA44C24))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3C1E4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA415E0))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F79C))); // нашел но есть немного сомнение
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3B528))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3E930))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4490C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3E0DC))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldTime, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40004))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA45C4C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA42BE0))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3CF28))); // нашел, но может быть не верный
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA440CC))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3BA98))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D114))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA44BB0))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Pickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D5F0))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSelectTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3E460))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA42CD8))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F35C))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3E284))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40A04))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40630))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_HideActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40484))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA41B48))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3E350))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F5D0))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D800))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrUpdate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3DFC0))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3E5F4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3F324))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D2D8))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3D738))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA41E70))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40B30))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA41C58))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3B268))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA4A9A4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3B6B4))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DamageVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA401B0))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA3B690))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA408AC))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA40E28))); // + (мб не верный)
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA44EF8))); // нашел, но есть сомнение что неправильный
  pInterface->RegisterAsRemoteProcedureCall(&RPC_GiveActorDamage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x676EA4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA46A10))); // +
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA463C0))); // +
#endif
}