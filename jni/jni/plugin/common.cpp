#include "common.h"
#include "netgame.h"
#include "plugin.h"
#include "xorstr.h"

#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>

extern RakClientInterface* pRakClient;

bool g_bInitGameProcess = false;

int ConvertBRIDToSampID(BRRpcIds value)
{
    if(value == BR_RPC_ClientJoin) { return RPC_ClientJoin; }
    if(value == BR_RPC_InitGame) { return RPC_InitGame; }
	if(value == BR_RPC_ConnectionRejected) { return RPC_ConnectionRejected; }
    if(value == BR_RPC_ServerJoin) { return RPC_ServerJoin; }
    if(value == BR_RPC_ServerQuit) { return RPC_ServerQuit; }
    if(value == BR_RPC_ServerCommand) { return RPC_ServerCommand; }
    if(value == BR_RPC_RequestClass) { return RPC_RequestClass; }
    if(value == BR_RPC_RequestSpawn) { return RPC_RequestSpawn; }
    if(value == BR_RPC_Spawn) { return RPC_Spawn; }
    if(value == BR_RPC_ScrSetSpawnInfo) { return RPC_ScrSetSpawnInfo; }
    if(value == BR_RPC_Chat) { return RPC_Chat; }
    if(value == BR_RPC_ChatBubble) { return RPC_ChatBubble; }
    if(value == BR_RPC_ClientMessage) { return RPC_ClientMessage; }
    if(value == BR_RPC_ShowDialog) { return RPC_ScrDialogBox; }
    if(value == BR_RPC_WorldTime) { return RPC_WorldTime; }
    if(value == BR_RPC_SetTimeEx) { return RPC_SetTimeEx; }
    if(value == BR_RPC_Weather) { return RPC_Weather; }
    if(value == BR_RPC_ScrSetInterior) { return RPC_ScrSetInterior; }
    if(value == BR_RPC_ScrHaveSomeMoney) { return RPC_ScrHaveSomeMoney; }
    if(value == BR_RPC_ScrResetMoney) { return RPC_ScrResetMoney; }
    if(value == BR_RPC_Pickup) { return RPC_Pickup; }
    if(value == BR_RPC_DestroyPickup) { return RPC_DestroyPickup; }
    if(value == BR_RPC_PickedUpPickup) { return RPC_PickedUpPickup; }
    if(value == BR_RPC_SetInteriorId) { return RPC_SetInteriorId; }
    if(value == BR_RPC_ScmEvent) { return RPC_ScmEvent; }
    if(value == BR_RPC_Death) { return RPC_Death; }
    if(value == BR_RPC_SetCheckpoint) { return RPC_SetCheckpoint; }
    if(value == BR_RPC_DisableCheckpoint) { return RPC_DisableCheckpoint; }
    if(value == BR_RPC_SetRaceCheckpoint) { return RPC_SetRaceCheckpoint; }
    if(value == BR_RPC_DisableRaceCheckpoint) { return RPC_DisableRaceCheckpoint; }
    if(value == BR_RPC_WorldActorAdd) { return RPC_ShowActor; }
    if(value == BR_RPC_WorldActorRemove) { return RPC_HideActor; }
    if(value == BR_RPC_ScrSetActorPos) { return RPC_SetActorPos; }
    if(value == BR_RPC_ScrSetActorFacingAngle) { return RPC_SetActorFacingAngle; }
    if(value == BR_RPC_ScrSetActorHealth) { return RPC_SetActorHealth; }
    if(value == BR_RPC_ScrApplyActorAnimation) { return RPC_ScrApplyActorAnimation; }
    if(value == BR_RPC_ScrClearActorAnimations) { return RPC_ScrClearActorAnimations; }
    if(value == BR_RPC_ActorGiveDamage) { return RPC_GiveActorDamage; }
    if(value == BR_RPC_WorldPlayerDeath) { return RPC_WorldPlayerDeath; }
    if(value == BR_RPC_WorldPlayerAdd) { return RPC_WorldPlayerAdd; }
    if(value == BR_RPC_WorldPlayerRemove) { return RPC_WorldPlayerRemove; }
    if(value == BR_RPC_ScrShowNameTag) { return RPC_ScrShowNameTag; }
    if(value == BR_RPC_ScrSetPlayerName) { return RPC_ScrSetPlayerName; }
    if(value == BR_RPC_ScrSetPlayerPos) { return RPC_ScrSetPlayerPos; }
    if(value == BR_RPC_ScrSetPlayerPosFindZ) { return RPC_ScrSetPlayerPosFindZ; }
    if(value == BR_RPC_ScrTogglePlayerControllable) { return RPC_ScrTogglePlayerControllable; }
    if(value == BR_RPC_ScrSetPlayerHealth) { return RPC_ScrSetPlayerHealth; }
    if(value == BR_RPC_ScrSetPlayerArmour) { return RPC_ScrSetPlayerArmour; }
    if(value == BR_RPC_ScrSetFightingStyle) { return RPC_ScrSetFightingStyle; }
    if(value == BR_RPC_ScrGivePlayerWeapon) { return RPC_ScrGivePlayerWeapon; }
    if(value == BR_RPC_ScrResetPlayerWeapons) { return RPC_ScrResetPlayerWeapons; }
    if(value == BR_RPC_ScrSetArmedWeapon) { return RPC_SetArmedWeapon; }
    if(value == BR_RPC_ScrSetWeaponAmmo) { return RPC_ScrSetWeaponAmmo; }
    if(value == BR_RPC_ScrSetPlayerVelocity) { return RPC_ScrSetPlayerVelocity; }
    if(value == BR_RPC_ScrSetPlayerColor) { return RPC_ScrSetPlayerColor; }
    if(value == BR_RPC_ScrSetPlayerSkin) { return RPC_ScrSetPlayerSkin; }
    if(value == BR_RPC_ScrSetPlayerAttachedObject) { return RPC_ScrSetPlayerAttachedObject; }
    if(value == BR_RPC_ScrAttachObjectToPlayer) { return RPC_ScrAttachObjectToPlayer; }
    if(value == BR_RPC_ScrSetPlayerWantedLevel) { return RPC_ScrSetPlayerWantedLevel; }
    if(value == BR_RPC_ScrSetPlayerFacingAngle) { return RPC_ScrSetPlayerFacingAngle; }
    if(value == BR_RPC_ScrSetPlayerDrunkLevel) { return RPC_ScrSetPlayerDrunkLevel; }
    if(value == BR_RPC_ScrApplyAnimation) { return RPC_ScrApplyPlayerAnimation; }
    if(value == BR_RPC_ScrClearAnimations) { return RPC_ScrClearPlayerAnimations; }
    if(value == BR_RPC_ScrPutPlayerInVehicle) { return RPC_ScrPutPlayerInVehicle; }
    if(value == BR_RPC_ScrRemovePlayerFromVehicle) { return RPC_ScrRemovePlayerFromVehicle; }
    if(value == BR_RPC_ScrSetCameraBehindPlayer) { return RPC_ScrSetCameraBehindPlayer; }
    if(value == BR_RPC_ScrSetCameraLookAt) { return RPC_ScrSetCameraLookAt; }
    if(value == BR_RPC_ScrSetCameraPos) { return RPC_ScrSetCameraPos; }
    if(value == BR_RPC_ScrInterpolateCamera) { return RPC_ScrInterpolateCamera; }
    if(value == BR_RPC_EnterVehicle) { return RPC_EnterVehicle; }
    if(value == BR_RPC_ExitVehicle) { return RPC_ExitVehicle; }
    if(value == BR_RPC_ScrSetMapIcon) { return RPC_ScrSetMapIcon; }
    if(value == BR_RPC_ScrDisableMapIcon) { return RPC_ScrDisableMapIcon; }
    if(value == BR_RPC_ScrTogglePlayerSpectating) { return RPC_ScrTogglePlayerSpectating; }
    if(value == BR_RPC_ScrPlayerSpectatePlayer) { return RPC_ScrPlayerSpectatePlayer; }
    if(value == BR_RPC_ScrPlayerSpectateVehicle) { return RPC_ScrPlayerSpectateVehicle; }
    if(value == BR_RPC_ScrAddGangZone) { return RPC_ScrAddGangZone; }
    if(value == BR_RPC_ScrFlashGangZone) { return RPC_ScrFlashGangZone; }
    if(value == BR_RPC_ScrStopFlashGangZone) { return RPC_ScrStopFlashGangZone; }
    if(value == BR_RPC_ScrRemoveGangZone) { return RPC_ScrRemoveGangZone; }
    if(value == BR_RPC_ScrSetSpecialAction) { return RPC_ScrSetSpecialAction; }
    if(value == BR_RPC_ScrAttachTrailerToVehicle) { return RPC_ScrAttachTrailerToVehicle; }
    if(value == BR_RPC_ScrDetachTrailerFromVehicle) { return RPC_ScrDetachTrailerFromVehicle; }
    if(value == BR_RPC_ScrCreateObject) { return RPC_ScrCreateObject; }
    if(value == BR_RPC_ScrDestroyObject) { return RPC_ScrDestroyObject; }
    if(value == BR_RPC_ScrSetObjectRotation) { return RPC_ScrSetObjectRotation; }
    if(value == BR_RPC_ScrMoveObject) { return RPC_ScrMoveObject; }
    if(value == BR_RPC_ScrStopObject) { return RPC_ScrStopObject; }
    if(value == BR_RPC_WorldVehicleAdd) { return RPC_WorldVehicleAdd; }
    if(value == BR_RPC_WorldVehicleRemove) { return RPC_WorldVehicleRemove; }
    if(value == BR_RPC_VehicleDestroyed) { return RPC_VehicleDestroyed; }
    if(value == BR_RPC_ScrSetVehicleHealth) { return RPC_ScrSetVehicleHealth; }
    if(value == BR_RPC_ScrSetVehiclePos) { return RPC_ScrSetVehiclePos; }
    if(value == BR_RPC_ScrSetVehicleVelocity) { return RPC_ScrSetVehicleVelocity; }
    if(value == BR_RPC_ScrVehicleParams) { return RPC_ScrVehicleParams; }
    if(value == BR_RPC_SetVehicleParamsEx) { return RPC_ScrVehicleParamsEx; }
    if(value == BR_RPC_ScrSetVehicleZAngle) { return RPC_ScrSetVehicleZAngle; }
    if(value == BR_RPC_ScrLinkVehicleToInterior) { return RPC_ScrLinkVehicle; }
    if(value == BR_RPC_ScrDisplayGameText) { return RPC_ScrDisplayGameText; }
    if(value == BR_RPC_ScrSelectTextDraw) { return RPC_ScrSelectTextDraw; }
    if(value == BR_RPC_ClickTextDraw) { return RPC_ClickTextDraw; }
    if(value == BR_RPC_ScrShowTextDraw) { return RPC_ScrShowTextDraw; }
    if(value == BR_RPC_ScrEditTextDraw) { return RPC_ScrEditTextDraw; }
    if(value == BR_RPC_ScrHideTextDraw) { return RPC_ScrHideTextDraw; }
    if(value == BR_RPC_ScrCreateExplosion) { return RPC_ScrCreateExplosion; }
    if(value == BR_RPC_DialogResponse) { return RPC_DialogResponse; }
    if(value == BR_RPC_MapMarker) { return RPC_MapMarker; }
    if(value == BR_RPC_UpdateScoresPingsIPs) { return RPC_UpdateScoresPingsIPs; }
    if(value == BR_RPC_PlayerGiveTakeDamage) { return RPC_PlayerGiveTakeDamage; }
    if(value == BR_RPC_ScrPlaySound) { return RPC_ScrPlaySound; }
    if(value == BR_RPC_ScrPlayAudioStream) { return RPC_PlayAudioStream; }
    if(value == BR_RPC_ScrStopAudioStream) { return RPC_StopAudioStream; }
    if(value == BR_RPC_Create3DTextLabel) { return RPC_ScrCreate3DTextLabel; }
    if(value == BR_RPC_ScrCustomizeVehicle) { return RPC_ScrCustomizeVehicle; }
    if(value == BR_RPC_ScrCommonStuff) { return RPC_ScrCommonStuff; }
    if(value == BR_RPC_CreateWaypoint) { return RPC_ScrCreateWayPoint; }
    if(value == BR_RPC_DestroyWaypoint) { return RPC_ScrRemoveWayPoint; }
    
    return -1;
}

PacketReliability ConvertBRToSampReliability(BRPacketReliability reliability)
{
	if(reliability == BR_RELIABILITY_UNRELIABLE) { return UNRELIABLE; }
	if(reliability == BR_RELIABILITY_UNRELIABLE_SEQUENCED) { return UNRELIABLE_SEQUENCED; }
	if(reliability == BR_RELIABILITY_RELIABLE) { return RELIABLE; }
	if(reliability == BR_RELIABILITY_RELIABLE_ORDERED) { return RELIABLE_ORDERED; }
	if(reliability == BR_RELIABILITY_RELIABLE_SEQUENCED) { return RELIABLE_SEQUENCED; }
	return UNRELIABLE;
}

namespace
{
    uint8_t CompressHealthArmour(uint16_t health, uint16_t armour)
    {
        const uint8_t h = health >= 100 ? 0x0F : static_cast<uint8_t>(std::min<uint16_t>(15u, health / 7u));
        const uint8_t a = armour >= 100 ? 0x0F : static_cast<uint8_t>(std::min<uint16_t>(15u, armour / 7u));
        return static_cast<uint8_t>((h << 4) | a);
    }

    uint16_t ClampVehicleHealth(float health)
    {
        if (!std::isfinite(health) || health <= 0.0f) return 0;
        if (health >= 65535.0f) return 65535;
        return static_cast<uint16_t>(health);
    }
}

void ConvertBROnFootSyncToSampSync(RakNet::BitStream* bs, BROnFootSyncData data)
{
    if (!bs) return;

    // SA-MP OnFootSync uses optional LR/UD fields, a normalized quaternion,
    // compressed velocity and optional surfing/animation sections.
    const bool hasLr = data.lrAnalogLeftStick != 0;
    const bool hasUd = data.udAnalogLeftStick != 0;
    bs->Write(hasLr);
    if (hasLr) bs->Write(data.lrAnalogLeftStick);
    bs->Write(hasUd);
    if (hasUd) bs->Write(data.udAnalogLeftStick);
    bs->Write(data.wKeys);
    bs->Write((char*)&data.vecPos, sizeof(data.vecPos));
    bs->WriteNormQuat(data.quatw, data.quatx, data.quaty, data.quatz);
    const uint8_t healthArmour = CompressHealthArmour(data.health, data.armour);
    bs->Write(healthArmour);
    bs->Write(data.byteCurrentWeapon);
    bs->Write(data.byteSpecialAction);
    bs->WriteVector(data.vecMoveSpeed.x, data.vecMoveSpeed.y, data.vecMoveSpeed.z);

    const bool hasSurf = data.wSurfInfo != 0 && data.wSurfInfo != 0xFFFF;
    bs->Write(hasSurf);
    if (hasSurf)
    {
        bs->Write(data.wSurfInfo);
        bs->Write((char*)&data.vecSurfOffsets, sizeof(data.vecSurfOffsets));
    }

    const uint16_t animationId = static_cast<uint16_t>(data.dwAnimation & 0xFFFFu);
    const uint16_t animationFlags = static_cast<uint16_t>((data.dwAnimation >> 16) & 0xFFFFu);
    const bool hasAnimation = animationId != 0 || animationFlags != 0;
    bs->Write(hasAnimation);
    if (hasAnimation)
    {
        bs->Write(animationId);
        bs->Write(animationFlags);
    }
}

void ConvertBRInCarSyncToSampSync(RakNet::BitStream* bs, BRInCarSyncData data)
{
    if (!bs) return;

    // SA-MP InCarSync is compressed on the wire. Do not emit the custom BR
    // 32-bit vehicle-health/raw-vector representation directly.
    bs->Write(data.VehicleID);
    bs->Write(data.lrAnalogLeftStick);
    bs->Write(data.udAnalogLeftStick);
    bs->Write(data.wKeys);
    bs->WriteNormQuat(data.quatw, data.quatx, data.quaty, data.quatz);
    bs->Write((char*)&data.vecPos, sizeof(data.vecPos));
    bs->WriteVector(data.vecMoveSpeed.x, data.vecMoveSpeed.y, data.vecMoveSpeed.z);
    const uint16_t vehicleHealth = ClampVehicleHealth(data.fCarHealth);
    bs->Write(vehicleHealth);
    const uint8_t healthArmour = CompressHealthArmour(data.playerHealth, data.playerArmour);
    bs->Write(healthArmour);
    bs->Write(data.byteCurrentWeapon);
    bs->Write(data.byteSirenOn != 0);
    bs->Write(data.byteLandingGearState != 0);

    // BR carries a trailer id but no separate train speed field here.
    bs->Write(false); // has train speed
    const bool hasTrailer = data.TrailerID != 0xFFFF;
    bs->Write(hasTrailer);
    if (hasTrailer) bs->Write(data.TrailerID);
}

void ConvertBRPassengerSyncToSampSync(RakNet::BitStream* bs, BRPassengerSyncData data)
{
    if (!bs) return;
    // Keep the exact 24-byte SA-MP wire layout. The high bit of the first
    // flags byte is the passenger cuffed flag.
    bs->WriteBits(reinterpret_cast<const unsigned char*>(&data.VehicleID), 16);
    bs->WriteBits(&data.byteSeatAndFlags, 8);
    bs->WriteBits(&data.byteWeaponAndSpecial, 8);
    bs->WriteBits(&data.playerHealth, 8);
    bs->WriteBits(&data.playerArmour, 8);
    bs->WriteBits(reinterpret_cast<const unsigned char*>(&data.lrAnalog), 16);
    bs->WriteBits(reinterpret_cast<const unsigned char*>(&data.udAnalog), 16);
    bs->WriteBits(reinterpret_cast<const unsigned char*>(&data.wKeys), 16);
    bs->WriteBits(reinterpret_cast<const unsigned char*>(&data.vecPos), 96);
}

void ConvertBRTurnSignalToSampSync(RakNet::BitStream* bs, BRTurnSignalData data)
{
    if (!bs) return;
    bs->Write((unsigned short)data.VehicleID);
    bs->Write((unsigned char)data.leftSignal);
    bs->Write((unsigned char)data.rightSignal);
    bs->Write((unsigned char)data.hazard);
}

bool IsRPCNeedFix(int rpcId)
{
	if(rpcId == RPC_WorldPlayerAdd) {
		return true;
	}
	if(rpcId == RPC_ScrDialogBox) {
		return true;
	}
	if(rpcId == RPC_WorldVehicleAdd) { return true; }
	if(rpcId == RPC_ServerJoin) { return true; }
	if(rpcId == RPC_ScrSetSpawnInfo) {
		RakNet::BitStream bs;
		pRakClient->RPC(&RPC_RequestSpawn, &bs, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
		return true;
	}
	return false;
}

struct NewVehicleFix
{
	char VehicleID[2];
	char iVehicleType[4];
	char pos[12];
	char fRotation[4];
	char color[2];
	char health[4];
};

void FixBrokenRPC(int rpcId, RPCParameters* rpcParams, void (*staticFunc)(RPCParameters*))
{
    if (!rpcParams || !staticFunc || !rpcParams->input)
        return;

    const size_t inputBytes = (rpcParams->numberOfBitsOfData + 7u) / 8u;
    if (inputBytes == 0)
        return;

    RakNet::BitStream bsData(rpcParams->input, inputBytes, false);
    thread_local std::vector<unsigned char> repairedRpcBuffer;

    if (rpcId == RPC_InitGame)
    {
        g_bInitGameProcess = true;
        staticFunc(rpcParams);
        g_bInitGameProcess = false;
        return;
    }

    if (rpcId == RPC_ScrDialogBox)
    {
        if (bsData.GetNumberOfUnreadBits() < 16 || !bsData.Read(CNetGame::m_nLastSAMPDialogID))
            return;
    }

    if (rpcId == RPC_WorldPlayerAdd)
    {
        uint16_t playerId = 0;
        uint32_t skinId = 0;
        uint32_t dwColor = 0;
        CVector vecPos = {0.0f, 0.0f, 0.0f};
        float fHeading = 0.0f;
        uint8_t byteFightingStyle = 4;

        if (!bsData.Read(playerId) || bsData.GetNumberOfUnreadBits() < 8)
            return;
        bsData.IgnoreBits(8);
        if (!bsData.Read(skinId) || !bsData.Read(vecPos.x) || !bsData.Read(vecPos.y) ||
            !bsData.Read(vecPos.z) || !bsData.Read(fHeading) || !bsData.Read(dwColor) ||
            !bsData.Read(byteFightingStyle))
            return;

        RakNet::BitStream bsRepair;
        bsRepair.Write(playerId);
        bsRepair.Write(skinId);
        bsRepair.Write(vecPos.x);
        bsRepair.Write(vecPos.y);
        bsRepair.Write(vecPos.z);
        bsRepair.Write(fHeading);
        bsRepair.Write(dwColor);
        bsRepair.Write(byteFightingStyle);
        const float maxHAvalue = 100.0f;
        bsRepair.Write(maxHAvalue);
        bsRepair.Write(maxHAvalue);

        const size_t repairedBytes = static_cast<size_t>(bsRepair.GetNumberOfBytesUsed());
        repairedRpcBuffer.resize(repairedBytes);
        if (repairedBytes > 0)
            std::memcpy(repairedRpcBuffer.data(), bsRepair.GetData(), repairedBytes);
        rpcParams->input = repairedRpcBuffer.data();
        rpcParams->numberOfBitsOfData = bsRepair.GetNumberOfBitsUsed();
    }

    if (rpcId == RPC_WorldVehicleAdd)
    {
        uint16_t VehicleID = 0;
        int iVehicleType = 0;
        float vecPos[3] = {0.0f, 0.0f, 0.0f};
        float fRotation = 0.0f;
        float fHealth = 0.0f;
        uint16_t color = 0;

        if (!bsData.Read(VehicleID) || !bsData.Read(iVehicleType) ||
            !bsData.Read(vecPos[0]) || !bsData.Read(vecPos[1]) || !bsData.Read(vecPos[2]) ||
            !bsData.Read(fRotation) || !bsData.Read(color) || !bsData.Read(fHealth))
            return;

        NewVehicleFix newVehBuff = {0};
        std::memcpy(&newVehBuff.VehicleID, &VehicleID, 2);
        std::memcpy(&newVehBuff.iVehicleType, &iVehicleType, 4);
        std::memcpy(&newVehBuff.pos, &vecPos, 12);
        std::memcpy(&newVehBuff.fRotation, &fRotation, 4);
        std::memcpy(&newVehBuff.color, &color, 2);
        std::memcpy(&newVehBuff.health, &fHealth, 4);

        const uintptr_t base = CGameAPI::GetBase(xorstr("CNetVehiclePool::New"));
        const uintptr_t vehPoolAddr = CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool"));
        if (base && vehPoolAddr && VehicleID < 2000)
        {
            const uintptr_t vehPool = *reinterpret_cast<uintptr_t*>(vehPoolAddr);
            if (vehPool)
                reinterpret_cast<void(*)(uintptr_t, NewVehicleFix*, int)>(base)(vehPool, &newVehBuff, 0);
        }
        return;
    }

    if (rpcId == RPC_ServerJoin)
    {
        staticFunc(rpcParams);
        uint8_t nickNameLen = 0;
        char nickName[25] = {0};
        uint16_t playerId = 0;
        if (!bsData.Read(playerId) || bsData.GetNumberOfUnreadBits() < 40)
            return;
        bsData.IgnoreBits(40);
        if (!bsData.Read(nickNameLen))
            return;
        const uint8_t safeLen = nickNameLen > 24 ? 24 : nickNameLen;
        if (bsData.GetNumberOfUnreadBits() < static_cast<unsigned>(safeLen) * 8u ||
            (safeLen > 0 && !bsData.Read(nickName, safeLen)))
            return;
        nickName[safeLen] = 0;

        CPlayerPool* pool = CNetGame::GetPlayerPool();
        if (pool)
        {
            CRemotePlayer* remote_player = pool->GetAt(playerId);
            if (remote_player)
            {
                std::memcpy(remote_player->m_szName, nickName, safeLen);
                remote_player->m_szName[safeLen] = 0;
            }
        }
        return;
    }

    staticFunc(rpcParams);
}

