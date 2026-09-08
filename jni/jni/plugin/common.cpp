#include "common.h"
#include "netgame.h"
#include "plugin.h"
#include "xorstr.h"

extern RakClientInterface* pRakClient;

bool g_bInitGameProcess = false;

namespace
{
constexpr uint16_t kMaxNativeRemotePlayers = 1004;
constexpr uintptr_t kGetRemotePlayerOffset = 0x2AC4B4;
constexpr uintptr_t kCreateRemotePlayerOffset = 0x2AC4D4;

bool IsLikelyValidRemotePlayerPtr(CRemotePlayer* remotePlayer)
{
    return reinterpret_cast<uintptr_t>(remotePlayer) >= 0x10000;
}

CRemotePlayer* GetNativeRemotePlayer(CPlayerPool* playerPool, uint16_t playerId)
{
    if(!playerPool || playerId >= kMaxNativeRemotePlayers) {
        return nullptr;
    }

    const uintptr_t base = CGameAPI::GetBase();
    if(!base) {
        return playerPool->GetAt(playerId);
    }

    auto getRemotePlayer = reinterpret_cast<CRemotePlayer*(*)(CPlayerPool*, uint16_t)>(base + kGetRemotePlayerOffset);
    return getRemotePlayer ? getRemotePlayer(playerPool, playerId) : playerPool->GetAt(playerId);
}

bool EnsureRemotePlayerExists(uint16_t playerId, const char* name)
{
    if(playerId >= kMaxNativeRemotePlayers) {
        return false;
    }

    CPlayerPool* playerPool = CNetGame::GetPlayerPool();
    if(!playerPool || reinterpret_cast<uintptr_t>(playerPool) < 0x10000) {
        return false;
    }

    CRemotePlayer* remotePlayer = GetNativeRemotePlayer(playerPool, playerId);
    if(IsLikelyValidRemotePlayerPtr(remotePlayer)) {
        return true;
    }

    const uintptr_t base = CGameAPI::GetBase();
    if(!base) {
        return false;
    }

    auto createRemotePlayer = reinterpret_cast<void(*)(CPlayerPool*, const char*, uint16_t)>(base + kCreateRemotePlayerOffset);
    if(!createRemotePlayer) {
        return false;
    }

    createRemotePlayer(playerPool, (name && name[0]) ? name : "", playerId);
    remotePlayer = GetNativeRemotePlayer(playerPool, playerId);
    return IsLikelyValidRemotePlayerPtr(remotePlayer);
}
}

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
    if(value == BR_RPC_ShowDialogEx) { return RPC_ScrDialogBox; }
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
    if(value == BR_RPC_ScrSetPlayerColor) { return RPC_ScrSetPlayerColor; }
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
    
    return -1;
}

PacketReliability ConvertBRToSampReliability(BRPacketReliability reliability)
{
	if(reliability == BR_RELIABILITY_UNRELIABLE) { return UNRELIABLE; }
	if(reliability == BR_RELIABILITY_UNRELIABLE_SEQUENCED) { return UNRELIABLE_SEQUENCED; }
	if(reliability == BR_RELIABILITY_RELIABLE) { return RELIABLE; }
	if(reliability == BR_RELIABILITY_RELIABLE_ORDERED) { return RELIABLE_ORDERED; }
	if(reliability == BR_RELIABILITY_RELIABLE_SEQUENCED) { return RELIABLE_SEQUENCED; }
	return RELIABLE;
}

void ConvertBROnFootSyncToSampSync(RakNet::BitStream* bs, BROnFootSyncData data)
{
    uint8_t health = data.health > 255 ? 255 : static_cast<uint8_t>(data.health);
    uint8_t armour = data.armour > 255 ? 255 : static_cast<uint8_t>(data.armour);

    bs->WriteBits((const unsigned char *)&data.lrAnalogLeftStick, 16);
    bs->WriteBits((const unsigned char *)&data.udAnalogLeftStick, 16);
    bs->WriteBits((const unsigned char *)&data.wKeys, 16);
    bs->WriteBits((const unsigned char *)&data.vecPos, 96);
    bs->WriteBits((const unsigned char *)&data.quatw, 32);
    bs->WriteBits((const unsigned char *)&data.quatx, 32);
    bs->WriteBits((const unsigned char *)&data.quaty, 32);
    bs->WriteBits((const unsigned char *)&data.quatz, 32);
    bs->WriteBits((const unsigned char *)&health, 8);
    bs->WriteBits((const unsigned char *)&armour, 8);
    bs->WriteBits((const unsigned char *)&data.byteCurrentWeapon, 8);
    bs->WriteBits((const unsigned char *)&data.byteSpecialAction, 8);
    bs->WriteBits((const unsigned char *)&data.vecMoveSpeed, 96);
    bs->WriteBits((const unsigned char *)&data.vecSurfOffsets, 96);
    bs->WriteBits((const unsigned char *)&data.wSurfInfo, 16);
    bs->WriteBits((const unsigned char *)&data.dwAnimation, 32);
}

void ConvertBRInCarSyncToSampSync(RakNet::BitStream* bs, BRInCarSyncData data)
{
    uint8_t playerHealth = data.playerHealth > 255 ? 255 : static_cast<uint8_t>(data.playerHealth);
    uint8_t playerArmour = data.playerArmour > 255 ? 255 : static_cast<uint8_t>(data.playerArmour);
    float trainSpeed = 0.0f;

    bs->WriteBits((const unsigned char *)&data.VehicleID, 16);
    bs->WriteBits((const unsigned char *)&data.lrAnalogLeftStick, 16);
    bs->WriteBits((const unsigned char *)&data.udAnalogLeftStick, 16);
    bs->WriteBits((const unsigned char *)&data.wKeys, 16);
    bs->WriteBits((const unsigned char *)&data.quatw, 32);
    bs->WriteBits((const unsigned char *)&data.quatx, 32);
    bs->WriteBits((const unsigned char *)&data.quaty, 32);
    bs->WriteBits((const unsigned char *)&data.quatz, 32);
    bs->WriteBits((const unsigned char *)&data.vecPos.x, 32);
	bs->WriteBits((const unsigned char *)&data.vecPos.y, 32);
	bs->WriteBits((const unsigned char *)&data.vecPos.z, 32);
    bs->WriteBits((const unsigned char *)&data.vecMoveSpeed.x, 32);
	bs->WriteBits((const unsigned char *)&data.vecMoveSpeed.y, 32);
	bs->WriteBits((const unsigned char *)&data.vecMoveSpeed.z, 32);
    bs->WriteBits((const unsigned char *)&data.fCarHealth, 32);
    bs->WriteBits((const unsigned char *)&playerHealth, 8);
    bs->WriteBits((const unsigned char *)&playerArmour, 8);
    bs->WriteBits((const unsigned char *)&data.byteCurrentWeapon, 8);
    bs->WriteBits((const unsigned char *)&data.byteSirenOn, 8);
    bs->WriteBits((const unsigned char *)&data.byteLandingGearState, 8);
    bs->WriteBits((const unsigned char *)&data.TrailerID, 16);
    bs->WriteBits((const unsigned char *)&trainSpeed, 32);
}

void ConvertBRPassengerSyncToSampSync(RakNet::BitStream* bs, BRPassengerSyncData data)
{
    bs->WriteBits((const unsigned char *)&data.VehicleID, 16);
	uint8_t combinedByte = (data.byteSeatFlags & 0x7F) | (data.byteDriveBy << 7);
    bs->WriteBits((const unsigned char *)&combinedByte, 8);
    bs->WriteBits((const unsigned char *)&data.byteCurrentWeapon, 8);
    bs->WriteBits((const unsigned char *)&data.playerHealth, 8);
    bs->WriteBits((const unsigned char *)&data.playerArmour, 8);
    bs->WriteBits((const unsigned char *)&data.lrAnalog, 16);
    bs->WriteBits((const unsigned char *)&data.udAnalog, 16);
    bs->WriteBits((const unsigned char *)&data.wKeys, 16);
	bs->WriteBits((const unsigned char *)&data.vecPos, 96);
}

bool IsRPCNeedFix(int rpcId)
{
    if(rpcId == RPC_InitGame) {
        return true;
    }
	if(rpcId == RPC_WorldPlayerAdd) {
		return true;
	}
	if(rpcId == RPC_WorldPlayerRemove) {
		return true;
	}
	if(rpcId == RPC_ScrDialogBox) {
		return true;
	}
	if(rpcId == RPC_WorldVehicleAdd) { return true; }
	if(rpcId == RPC_ServerJoin) { return true; }
	if(rpcId == RPC_ScrSetSpawnInfo) {
		return true;
	}
	return false;
}

void FixBrokenRPC(int rpcId, RPCParameters* rpcParams, void (*staticFunc)(RPCParameters*))
{
    if(!rpcParams || !staticFunc) {
        return;
    }

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
	if(rpcId == RPC_InitGame) {
		g_bInitGameProcess = true;
		staticFunc(rpcParams);
		g_bInitGameProcess = false;
        CNetGame::SetGameState(GAMESTATE_CONNECTED);
		return;
	}
	if(rpcId == RPC_ScrDialogBox) {
		 bsData.Read(CNetGame::m_nLastSAMPDialogID);
         CNetGame::SetDialogActive(true);
	}
	if(rpcId == RPC_ScrSetSpawnInfo) {
		staticFunc(rpcParams);
        if(pRakClient) {
		    RakNet::BitStream bsSpawnRequest;
		    pRakClient->RPC(&RPC_RequestSpawn, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
        }
		return;
	}
	if(rpcId == RPC_WorldPlayerAdd) {
		uint16_t playerId = 0;
        uint8_t teamId = 0;
		uint32_t skinId = 0;
		uint32_t dwColor = 0;
		CVector vecPos = {0.0f, 0.0f, 0.0f};
		float fHeading = 0.0f;
		uint8_t byteFightingStyle = 4;

		bool ok = true;
        ok = ok && bsData.Read(playerId);
        ok = ok && bsData.Read(teamId);
		ok = ok && bsData.Read(skinId);
		ok = ok && bsData.Read(vecPos.x);
		ok = ok && bsData.Read(vecPos.y);
		ok = ok && bsData.Read(vecPos.z);
		ok = ok && bsData.Read(fHeading);
		ok = ok && bsData.Read(dwColor);

		if(!ok) {
            staticFunc(rpcParams);
			return;
		}

        // Native setter indexes a model table by skin id, so keep it in valid bounds.
        if(skinId > 624) {
            skinId = 0;
        }

        bool hasFightingStyle = bsData.Read(byteFightingStyle);
        EnsureRemotePlayerExists(playerId, "");

		RakNet::BitStream bsRepair;
		bsRepair.Write(playerId);
        bsRepair.Write(teamId);
		bsRepair.Write(skinId);
		bsRepair.Write(vecPos.x);
		bsRepair.Write(vecPos.y);
		bsRepair.Write(vecPos.z);
		bsRepair.Write(fHeading);
		bsRepair.Write(dwColor);
        if(hasFightingStyle) {
		    bsRepair.Write(byteFightingStyle);
        }

		RPCParameters repairedParams = *rpcParams;
		repairedParams.input = bsRepair.GetData();
		repairedParams.numberOfBitsOfData = bsRepair.GetNumberOfBitsUsed();

		staticFunc(&repairedParams);
		CNetGame::SetRemotePlayerReady(playerId, true);
		return;
	}
	if(rpcId == RPC_WorldPlayerRemove) {
		uint16_t playerId;
		if(bsData.Read(playerId)) {
			CNetGame::SetRemotePlayerReady(playerId, false);
		}
		staticFunc(rpcParams);
		return;
	}
	if(rpcId == RPC_WorldVehicleAdd) {
		BRNewVehiclePacked vehicleData = {};

		bool ok = true;
		ok = ok && bsData.Read(vehicleData.VehicleID);
		ok = ok && bsData.Read(vehicleData.iVehicleType);
		ok = ok && bsData.Read(vehicleData.vecPos.x);
		ok = ok && bsData.Read(vehicleData.vecPos.y);
		ok = ok && bsData.Read(vehicleData.vecPos.z);
		ok = ok && bsData.Read(vehicleData.fRotation);
		ok = ok && bsData.Read(vehicleData.aColor1);
		ok = ok && bsData.Read(vehicleData.aColor2);
		ok = ok && bsData.Read(vehicleData.fHealth);
		if(!ok) {
			return;
		}

		// Extended BR tuning block (optional in some packets).
		bsData.Read(vehicleData.byteInterior);
		bsData.Read(vehicleData.dwDoorDamageStatus);
		bsData.Read(vehicleData.dwPanelDamageStatus);
		bsData.Read(vehicleData.byteLightDamageStatus);
		bsData.Read(vehicleData.byteTireDamageStatus);
		bsData.Read(vehicleData.byteAddSiren);
		bsData.Read(vehicleData.byteModSlots[0]);
		bsData.Read(vehicleData.byteModSlots[1]);
		bsData.Read(vehicleData.cColor1);
		bsData.Read(vehicleData.cColor2);
		bsData.Read(vehicleData.dwWindowsColor);
		bsData.Read(vehicleData.dwWindowsColor2);
		bsData.Read(vehicleData.dwWindowsColor3);
		bsData.Read(vehicleData.dwWindowsColor4);
		bsData.Read(vehicleData.fSuspensionForce);
		bsData.Read(vehicleData.fSuspensionCenter);
		bsData.Read(vehicleData.fWheelSize);
		bsData.Read(vehicleData.fMaxSpeed);
		bsData.Read(vehicleData.fEngineAcceleration);
		bsData.Read(vehicleData.fSteeringLock);
		bsData.Read(vehicleData.dwLightsColor);
		bsData.Read(vehicleData.dwNeonColor);
		bsData.Read(vehicleData.dwNeonColor2);
		bsData.Read(vehicleData.dwNeonColor3);
		bsData.Read(vehicleData.fWheelsAngle);
		bsData.Read(vehicleData.fWheelsAngleRear);
		bsData.Read(vehicleData.dwWheelsWidth);
		bsData.Read(vehicleData.bDriftMode);
		bsData.Read(vehicleData.fWheelsOffset);
		bsData.Read(vehicleData.fWheelsOffsetRear);
		bsData.Read(vehicleData.fWheelScale);
		bsData.Read(vehicleData.fWheelScaleRear);
		bsData.Read(vehicleData.dwColor[0]);
		bsData.Read(vehicleData.dwColor[1]);
		bsData.Read(vehicleData.dwColor[2]);
		bsData.Read(vehicleData.dwColor[3]);
		bsData.Read(vehicleData.bHornTon);
		bsData.Read(vehicleData.dwStrobesMode);
		bsData.Read(vehicleData.dwChipMode);
		bsData.Read(vehicleData.bHydauliks);
		bsData.Read(vehicleData.bFar);
		bsData.Read(vehicleData.wExhaust);
		bsData.Read(vehicleData.bUseSiren);
		bsData.Read(vehicleData.bEnabledSiren);

        uintptr_t base = CGameAPI::GetBase(xorstr("CNetVehiclePool::New"));
        uintptr_t vehPoolAddr = CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool"));
        if (base && vehPoolAddr) {
            uintptr_t vehPool = *(uintptr_t *)(vehPoolAddr);
            if (vehPool) {
                reinterpret_cast<void(*)(uintptr_t, BRNewVehiclePacked *, int)>(base)(vehPool, &vehicleData, 0);
                return;
            }
        }
        staticFunc(rpcParams);
        return;
	}
	if(rpcId == RPC_ServerJoin) {
        uint16_t playerId = 0;
        uint32_t ignoredColor = 0;
        uint8_t ignoredFlag = 0;
        uint8_t nameLen = 0;
        char playerName[32] = {};
        bool parsed = bsData.Read(playerId) &&
                      bsData.Read(ignoredColor) &&
                      bsData.Read(ignoredFlag) &&
                      bsData.Read(nameLen);

        if(parsed) {
            if(nameLen > 24) {
                nameLen = 24;
            }
            if(nameLen > 0) {
                parsed = bsData.Read(playerName, nameLen);
            }
            playerName[nameLen] = '\0';
        }

		staticFunc(rpcParams);
        if(parsed) {
            EnsureRemotePlayerExists(playerId, playerName);
            CNetGame::SetRemotePlayerReady(playerId, true);
        }
        return;
    }
	
	staticFunc(rpcParams);
}
