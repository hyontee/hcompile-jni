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
    pInterface->RegisterAsRemoteProcedureCall(&RPC_Pickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8A268)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA878C8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA91EE4)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8B26C)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA88E34)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8E298)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8D460)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8A3B0)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA89E80)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA98174)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8D5B8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA97A84)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8B0D8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8B5A8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C438)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA97D70)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA9293C)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA91478)));
   // pInterface->RegisterAsRemoteProcedureCall(&RPC_EditObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8A840)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8B4AC)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA89C70)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWorldBounds, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA97564)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8F9FC)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA9171C)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8CC7C)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA91790)));
  //  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA90B28)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8BF10)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveComponent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C6F8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA886C8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8BED8)));
   // pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA90560)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8D864)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C7F0)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8AD54)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8AEFC)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C334)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayerUpdate, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA930B0)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C9F8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA88294)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8E738)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA97BC0)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C184)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_Weather, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8E848)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8CAF0)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8A0A4)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA9825C)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA98420)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8BFF4)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C0BC)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8E664)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8E958)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8A080)));
  //  pInterface->RegisterAsRemoteProcedureCall(&RPC_SvrStats, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8D038)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8AFC8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8B3AC)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA89F50)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA912BC)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA89BA0)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8E1D0)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C8E4)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA98024)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA88050)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C334)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8B6F4)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetGravity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8F218)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8D6E4)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkLevel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8CD64)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C5D4)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA89D8C)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8812C)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA8C268)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA882B8)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA93700)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA87E6C)));
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0xA930B0)));
#endif
}