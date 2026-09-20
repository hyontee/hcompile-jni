#include "netrpc.h"
#include "xorstr.h"

#include "plugin.h"
#include "common.h"
#include "features/br_features.h"

#include <cstring>

void DialogBoxRPC(RPCParameters* rpcParams)
{
	const uintptr_t fn = CGameAPI::GetBase(xorstr("RPC::DialogBox"));
	if (!fn || !rpcParams) return;
	reinterpret_cast<void(*)(RPCParameters*)>(fn)(rpcParams);
}


namespace
{
    using RpcHandler = void (*)(RPCParameters*);
    RpcHandler g_mapIconHandler = nullptr;
    RpcHandler g_mapIconRemoveHandler = nullptr;
    RpcHandler g_pickupHandler = nullptr;
    RpcHandler g_pickupRemoveHandler = nullptr;
    RpcHandler g_waypointHandler = nullptr;
    RpcHandler g_waypointRemoveHandler = nullptr;
    RpcHandler g_checkpointHandler = nullptr;
    RpcHandler g_checkpointRemoveHandler = nullptr;

    bool GetRpcPayload(RPCParameters* rpcParams, unsigned int& bytes)
    {
        if (!rpcParams || !rpcParams->input || rpcParams->numberOfBitsOfData == 0) return false;
        const uint64_t byteCount = (static_cast<uint64_t>(rpcParams->numberOfBitsOfData) + 7u) / 8u;
        if (byteCount == 0 || byteCount > 4096u) return false;
        bytes = static_cast<unsigned int>(byteCount);
        return true;
    }

    void BR_RPC_SetMapIcon(RPCParameters* rpcParams)
    {
        unsigned int bytes = 0;
        if (GetRpcPayload(rpcParams, bytes))
        {
            RakNet::BitStream bs(reinterpret_cast<unsigned char*>(rpcParams->input), bytes, false);
            uint8_t id = 0, type = 0, style = 0;
            uint32_t color = 0;
            CVector pos{};
            if (bs.Read(id) && bs.Read(pos.x) && bs.Read(pos.y) && bs.Read(pos.z) && bs.Read(type) && bs.Read(color))
            {
                if (bs.GetNumberOfUnreadBits() >= 8) bs.Read(style);
                br::FeatureState::Instance().SetMapIcon(id, pos, type, color, style);
            }
        }
        if (g_mapIconHandler) g_mapIconHandler(rpcParams);
    }

    void BR_RPC_RemoveMapIcon(RPCParameters* rpcParams)
    {
        uint8_t id = 0;
        unsigned int bytes = 0;
        if (GetRpcPayload(rpcParams, bytes))
        {
            RakNet::BitStream bs(reinterpret_cast<unsigned char*>(rpcParams->input), bytes, false);
            if (bs.Read(id)) br::FeatureState::Instance().RemoveMapIcon(id);
        }
        if (g_mapIconRemoveHandler) g_mapIconRemoveHandler(rpcParams);
    }

    void BR_HandlePickup(RPCParameters* rpcParams)
    {
        unsigned int bytes = 0;
        if (GetRpcPayload(rpcParams, bytes))
        {
            RakNet::BitStream bs(reinterpret_cast<unsigned char*>(rpcParams->input), bytes, false);
            uint32_t pickupId = 0, model = 0, type = 0;
            CVector pos{};
            if (bs.Read(pickupId) && bs.Read(model) && bs.Read(pos.x) && bs.Read(pos.y) && bs.Read(pos.z) && bs.Read(type) && pickupId < 4096u)
                br::FeatureState::Instance().SetPickup(static_cast<uint16_t>(pickupId), static_cast<int32_t>(model), pos, static_cast<int32_t>(type));
        }
        if (g_pickupHandler) g_pickupHandler(rpcParams);
    }

    void BR_HandleDestroyPickup(RPCParameters* rpcParams)
    {
        uint32_t pickupId = 0;
        unsigned int bytes = 0;
        if (GetRpcPayload(rpcParams, bytes))
        {
            RakNet::BitStream bs(reinterpret_cast<unsigned char*>(rpcParams->input), bytes, false);
            if (bs.Read(pickupId) && pickupId < 4096u) br::FeatureState::Instance().RemovePickup(static_cast<uint16_t>(pickupId));
        }
        if (g_pickupRemoveHandler) g_pickupRemoveHandler(rpcParams);
    }

    void BR_RPC_CreateWaypoint(RPCParameters* rpcParams)
    {
        CVector pos{};
        unsigned int bytes = 0;
        if (GetRpcPayload(rpcParams, bytes))
        {
            RakNet::BitStream bs(reinterpret_cast<unsigned char*>(rpcParams->input), bytes, false);
            if (bs.Read(pos.x) && bs.Read(pos.y) && bs.Read(pos.z)) br::FeatureState::Instance().SetWaypoint(pos);
        }
        if (g_waypointHandler) g_waypointHandler(rpcParams);
    }

    void BR_RPC_RemoveWaypoint(RPCParameters* rpcParams)
    {
        br::FeatureState::Instance().ClearWaypoint();
        if (g_waypointRemoveHandler) g_waypointRemoveHandler(rpcParams);
    }

    void BR_HandleSetCheckpoint(RPCParameters* rpcParams)
    {
        CVector pos{}; float size = 0.0f;
        unsigned int bytes = 0;
        if (GetRpcPayload(rpcParams, bytes))
        {
            RakNet::BitStream bs(reinterpret_cast<unsigned char*>(rpcParams->input), bytes, false);
            if (bs.Read(pos.x) && bs.Read(pos.y) && bs.Read(pos.z) && bs.Read(size))
                br::FeatureState::Instance().SetCheckpoint(pos, size);
        }
        if (g_checkpointHandler) g_checkpointHandler(rpcParams);
    }

    void BR_HandleDisableCheckpoint(RPCParameters* rpcParams)
    {
        br::FeatureState::Instance().ClearCheckpoint();
        if (g_checkpointRemoveHandler) g_checkpointRemoveHandler(rpcParams);
    }

    void RegisterTracked(RakClientInterface* pInterface, int* rpcId, RpcHandler original, RpcHandler wrapper, RpcHandler& slot)
    {
        slot = original;
        pInterface->RegisterAsRemoteProcedureCall(rpcId, wrapper);
    }
}

void RegisterRPCs(RakClientInterface* pInterface)
{
#ifdef __arm__
  // 32 bit - ВОССТАНОВЛЕННЫЕ оффсеты с +1
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x339948 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3326CC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333160 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331628 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x335388 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3361AC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3316C0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333C64 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3396F4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331820 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3359A0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x330850 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x33A304 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x339840 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32E050 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32D9B8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x33278C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Weather, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3328C8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3321FC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331FE0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3338EC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3323D0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332330 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x334CE4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332DBC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332054 + 1)));
  RegisterTracked(pInterface, &RPC_ScrDisableMapIcon, ((RpcHandler)(CGameAPI::m_address + 0x335674 + 1)), BR_RPC_RemoveMapIcon, g_mapIconRemoveHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x33A198 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3348E8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x330710 + 1)));
  RegisterTracked(pInterface, &RPC_SetCheckpoint, ((RpcHandler)(CGameAPI::m_address + 0x335214 + 1)), BR_HandleSetCheckpoint, g_checkpointHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x335154 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3322A8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333A94 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3398CC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3305BC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332B1C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333970 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333348 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x339590 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332C94 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32D47C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x330870 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x33A23C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3329B4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3308F4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332184 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3332D8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333FC0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332834 + 1)));
  RegisterTracked(pInterface, &RPC_ScrSetMapIcon, ((RpcHandler)(CGameAPI::m_address + 0x335540 + 1)), BR_RPC_SetMapIcon, g_mapIconHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32E4B0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3337AC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332210 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32DA40 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331750 + 1)));
  RegisterTracked(pInterface, &RPC_DisableCheckpoint, ((RpcHandler)(CGameAPI::m_address + 0x335338 + 1)), BR_HandleDisableCheckpoint, g_checkpointRemoveHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3310D0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldTime, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332940 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x335F68 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333E6C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x330010 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x334DA4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32DE20 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x330678 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3354F0 + 1)));
  RegisterTracked(pInterface, &RPC_Pickup, ((RpcHandler)(CGameAPI::m_address + 0x330980 + 1)), BR_HandlePickup, g_pickupHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSelectTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x33136C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333EE4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331F50 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331238 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332FA0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332D2C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_HideActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332C24 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333B14 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3312B4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3320C8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x330AD8 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331528 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331F24 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x330790 + 1)));
  RegisterTracked(pInterface, &RPC_DestroyPickup, ((RpcHandler)(CGameAPI::m_address + 0x330A64 + 1)), BR_HandleDestroyPickup, g_pickupRemoveHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333D14 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x33304C + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333BBC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32D884 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3397C4 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32DB20 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x32DB00 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x332EBC + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x333240 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x335710 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_GiveActorDamage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x331008 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x3363D0 + 1)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x336900 + 1)));
#endif
#ifdef __aarch64__
  // 64 bit - ЗАМЕНЕННЫЕ оффсеты для нового билда
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x506A60)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FCCF8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FDDAC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FB628)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x500E20)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x50233C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FB728)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FEE04)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5066A8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FB968)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x501734)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FA1F4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x50767C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5068B0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F7644)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F6E38)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FCE18)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_Weather, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD030)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC62C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC2B4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FE89C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC914)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC818)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x500454)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD81C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC37C)));
  RegisterTracked(pInterface, &RPC_ScrDisableMapIcon, ((RpcHandler)(CGameAPI::m_address + 0x5012B8)), BR_RPC_RemoveMapIcon, g_mapIconRemoveHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x507430)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FFEA0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F9FF0)));
  RegisterTracked(pInterface, &RPC_SetCheckpoint, ((RpcHandler)(CGameAPI::m_address + 0x500BF0)), BR_HandleSetCheckpoint, g_checkpointHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x500AD0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC73C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FEB10)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x506994)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F9DE0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD3F4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FE96C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FE0E8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x506468)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD63C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F669C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FA218)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x507540)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD1BC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FA2F8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC560)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FE024)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FF30C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FCF30)));
  RegisterTracked(pInterface, &RPC_ScrSetMapIcon, ((RpcHandler)(CGameAPI::m_address + 0x5010C4)), BR_RPC_SetMapIcon, g_mapIconHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F7B58)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FE690)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC648)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F6F14)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FB824)));
  RegisterTracked(pInterface, &RPC_DisableCheckpoint, ((RpcHandler)(CGameAPI::m_address + 0x500DAC)), BR_HandleDisableCheckpoint, g_checkpointRemoveHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FAEE4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldTime, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD0F8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x501FB4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FF0FC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F96A4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x500578)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F7408)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F9EFC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x501050)));
  RegisterTracked(pInterface, &RPC_Pickup, ((RpcHandler)(CGameAPI::m_address + 0x4FA3DC)), BR_HandlePickup, g_pickupHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSelectTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FB270)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FF1C8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC1D0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FB094)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FDB00)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD72C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_HideActor, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD580)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FEBE4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FB160)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC444)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FA5F8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FB4DC)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FC198)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FA0C0)));
  RegisterTracked(pInterface, &RPC_DestroyPickup, ((RpcHandler)(CGameAPI::m_address + 0x4FA530)), BR_HandleDestroyPickup, g_pickupRemoveHandler);
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FEF20)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FDC2C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FECF4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F6C54)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5067E4)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F7070)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4F704C)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FD9A8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FDF24)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5013B0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_GiveActorDamage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FADC8)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5026B0)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x502F30)));
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FE200)));
  RegisterTracked(pInterface, &RPC_ScrCreateWayPoint, ((RpcHandler)(CGameAPI::m_address + 0x4FA7A4)), BR_RPC_CreateWaypoint, g_waypointHandler);
  RegisterTracked(pInterface, &RPC_ScrRemoveWayPoint, ((RpcHandler)(CGameAPI::m_address + 0x4FA904)), BR_RPC_RemoveWaypoint, g_waypointRemoveHandler);
#endif
}