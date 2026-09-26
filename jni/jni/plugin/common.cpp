#include "common.h"
#include "netgame.h"
#include "plugin.h"
#include "xorstr.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

extern RakClientInterface* pRakClient;

bool g_bInitGameProcess = false;

static constexpr unsigned int kWorldPlayerAddPacketBytes = 28;
static constexpr unsigned int kWorldPlayerAddLegacyPacketBytes = 27;
static constexpr unsigned int kWorldVehicleAddLegacyPacketBytes = 63;
static constexpr unsigned int kWorldVehicleAddPacketBytes = 172;
static constexpr uintptr_t kCreateRemotePlayerOffset = 0x2AC4D4;
static constexpr uintptr_t kGetRemotePlayerOffset = 0x2AC4B4;
static constexpr uint16_t kMaxRemotePlayers = 1004;
static constexpr uint16_t kMaxRemoteVehicles = 2000;
static void (*g_customizeVehicleHandler)(RPCParameters*) = nullptr;

void SetCustomizeVehicleHandler(void (*handler)(RPCParameters*))
{
	g_customizeVehicleHandler = handler;
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
    bs->WriteBits((const unsigned char *)&data.lrAnalogLeftStick, 16);
    bs->WriteBits((const unsigned char *)&data.udAnalogLeftStick, 16);
    bs->WriteBits((const unsigned char *)&data.wKeys, 16);
    bs->WriteBits((const unsigned char *)&data.vecPos, 96);
    bs->WriteBits((const unsigned char *)&data.quatw, 32);
    bs->WriteBits((const unsigned char *)&data.quatx, 32);
    bs->WriteBits((const unsigned char *)&data.quaty, 32);
    bs->WriteBits((const unsigned char *)&data.quatz, 32);
    bs->WriteBits((const unsigned char *)&data.health, 8);
    bs->WriteBits((const unsigned char *)&data.armour, 8);
    bs->WriteBits((const unsigned char *)&data.byteCurrentWeapon, 8);
    bs->WriteBits((const unsigned char *)&data.byteSpecialAction, 8);
    bs->WriteBits((const unsigned char *)&data.vecMoveSpeed, 96);
    bs->WriteBits((const unsigned char *)&data.vecSurfOffsets, 96);
    bs->WriteBits((const unsigned char *)&data.wSurfInfo, 16);
    bs->WriteBits((const unsigned char *)&data.dwAnimation, 32);
}

void ConvertBRInCarSyncToSampSync(RakNet::BitStream* bs, BRInCarSyncData data)
{
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
    bs->WriteBits((const unsigned char *)&data.playerHealth, 8);
    bs->WriteBits((const unsigned char *)&data.playerArmour, 8);
    bs->WriteBits((const unsigned char *)&data.byteCurrentWeapon, 8);
    bs->WriteBits((const unsigned char *)&data.byteSirenOn, 8);
    bs->WriteBits((const unsigned char *)&data.byteLandingGearState, 8);
    bs->WriteBits((const unsigned char *)&data.TrailerID, 16);
	float fTrainSpeed = 0;
	bs->WriteBits((const unsigned char *)&fTrainSpeed, 32);
	
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
	if(rpcId == RPC_ScrDialogBox) {
		return true;
	}
	if(rpcId == RPC_ServerJoin) { return true; }
	if(rpcId == RPC_ScrSetSpawnInfo) {
		return true;
	}
	return false;
}

static bool IsFiniteInRange(float value, float minValue, float maxValue)
{
	return std::isfinite(value) && value >= minValue && value <= maxValue;
}

static float ClampOrFallback(float value, float minValue, float maxValue, float fallback)
{
	if (!std::isfinite(value) || value < minValue || value > maxValue) {
		return fallback;
	}
	return value;
}

static bool IsValidWorldPlayerAddCandidate(
	uint16_t playerId,
	uint32_t skinId,
	const CVector& vecPos,
	float fHeading)
{
	(void)skinId;
	return playerId < kMaxRemotePlayers &&
		IsFiniteInRange(vecPos.x, -50000.0f, 50000.0f) &&
		IsFiniteInRange(vecPos.y, -50000.0f, 50000.0f) &&
		IsFiniteInRange(vecPos.z, -50000.0f, 50000.0f) &&
		IsFiniteInRange(fHeading, -3600.0f, 3600.0f);
}

static void EnsureRemotePlayerExists(uint16_t playerId, const char* nickname)
{
	CPlayerPool* pool = CNetGame::GetPlayerPool();
	if (!pool || playerId >= kMaxRemotePlayers) {
		return;
	}

	if (reinterpret_cast<CRemotePlayer*(*)(CPlayerPool*, uint16_t)>(
			CGameAPI::m_address + kGetRemotePlayerOffset)(pool, playerId)) {
		return;
	}

	const char* safeNickname = (nickname && nickname[0]) ? nickname : "Player";
	reinterpret_cast<void(*)(CPlayerPool*, const char*, uint16_t)>(CGameAPI::m_address + kCreateRemotePlayerOffset)(pool, safeNickname, playerId);
}

template <typename T>
static T ReadUnalignedValue(const unsigned char* data, size_t offset)
{
	T value{};
	std::memcpy(&value, data + offset, sizeof(T));
	return value;
}

template <typename T>
static void WriteUnalignedValue(unsigned char* data, size_t offset, const T& value)
{
	std::memcpy(data + offset, &value, sizeof(T));
}

static bool IsValidLegacyVehicleAdd(const BRLegacyVehicleAdd& packet)
{
	return packet.VehicleID < kMaxRemoteVehicles &&
		packet.iVehicleType > 0 &&
		packet.iVehicleType <= 65535 &&
		IsFiniteInRange(packet.vecPos.x, -50000.0f, 50000.0f) &&
		IsFiniteInRange(packet.vecPos.y, -50000.0f, 50000.0f) &&
		IsFiniteInRange(packet.vecPos.z, -50000.0f, 50000.0f) &&
		IsFiniteInRange(packet.fRotation, -3600.0f, 3600.0f) &&
		IsFiniteInRange(packet.fHealth, 0.0f, 50000.0f);
}

static uint32_t SanitizeRemotePlayerSkin(uint32_t skinId)
{
	return skinId <= 311 ? skinId : 7;
}

static uint8_t SanitizeRemotePlayerByte(uint8_t value, uint8_t fallback)
{
	return value <= 32 ? value : fallback;
}

static void SanitizeVehicleVisuals(BRNewVehiclePacked* packet)
{
	if (!packet) {
		return;
	}

	packet->fSuspensionForce = ClampOrFallback(packet->fSuspensionForce, 0.75f, 1.75f, 1.0f);
	packet->fSuspensionCenter = ClampOrFallback(packet->fSuspensionCenter, -0.08f, 0.08f, 0.0f);
	packet->fWheelSize = ClampOrFallback(packet->fWheelSize, 0.85f, 1.15f, 1.0f);
	packet->fSteeringLock = ClampOrFallback(packet->fSteeringLock, 10.0f, 80.0f, 35.0f);
	packet->fWheelsAngle = ClampOrFallback(packet->fWheelsAngle, -90.0f, 90.0f, packet->fWheelsAngle);
	packet->fWheelsAngleRear = ClampOrFallback(packet->fWheelsAngleRear, -90.0f, 90.0f, packet->fWheelsAngleRear);
	packet->fWheelsOffset = ClampOrFallback(packet->fWheelsOffset, -0.03f, 0.05f, 0.0f);
	packet->fWheelsOffsetRear = ClampOrFallback(packet->fWheelsOffsetRear, -0.03f, 0.05f, 0.0f);
	packet->fWheelScale = ClampOrFallback(packet->fWheelScale, 0.85f, 1.15f, 1.0f);
	packet->fWheelScaleRear = ClampOrFallback(packet->fWheelScaleRear, 0.85f, 1.15f, 1.0f);
}

static void BuildExpandedVehicleAddPacket(const BRLegacyVehicleAdd& legacyPacket, RakNet::BitStream* output)
{
	unsigned char expandedBytes[kWorldVehicleAddPacketBytes] = {};
	auto* expanded = reinterpret_cast<BRNewVehiclePacked*>(expandedBytes);
	expanded->VehicleID = legacyPacket.VehicleID;
	expanded->iVehicleType = legacyPacket.iVehicleType;
	expanded->vecPos = legacyPacket.vecPos;
	expanded->fRotation = legacyPacket.fRotation;
	expanded->aColor1 = legacyPacket.aColor1;
	expanded->aColor2 = legacyPacket.aColor2;
	expanded->fHealth = legacyPacket.fHealth;
	expanded->byteInterior = legacyPacket.byteInterior;
	expanded->dwDoorDamageStatus = legacyPacket.dwDoorDamageStatus;
	expanded->dwPanelDamageStatus = legacyPacket.dwPanelDamageStatus;
	expanded->byteLightDamageStatus = legacyPacket.byteLightDamageStatus;
	expanded->byteTireDamageStatus = legacyPacket.byteTireDamageStatus;
	expanded->byteAddSiren = legacyPacket.byteAddSiren;
	expanded->cColor1 = legacyPacket.cColor1;
	expanded->cColor2 = legacyPacket.cColor2;
	expanded->fSuspensionForce = 1.0f;
	expanded->fSuspensionCenter = 0.0f;
	expanded->fWheelSize = 1.0f;
	expanded->fMaxSpeed = 160.0f;
	expanded->fEngineAcceleration = 8.0f;
	expanded->fSteeringLock = 35.0f;
	expanded->fWheelsAngle = 0.0f;
	expanded->fWheelsAngleRear = 0.0f;
	expanded->fWheelsOffset = 0.0f;
	expanded->fWheelsOffsetRear = 0.0f;
	expanded->fWheelScale = 1.0f;
	expanded->fWheelScaleRear = 1.0f;
	expanded->bUseSiren = legacyPacket.byteAddSiren ? 1 : 0;
	expanded->bEnabledSiren = expanded->bUseSiren;
	SanitizeVehicleVisuals(expanded);

	output->Write(reinterpret_cast<const char*>(expandedBytes), sizeof(expandedBytes));
	output->Write(static_cast<uint8_t>(0));
	output->Write(static_cast<uint8_t>(0)); // let the client normalize legacy style itself
}

void FixBrokenRPC(int rpcId, RPCParameters* rpcParams, void (*staticFunc)(RPCParameters*))
{
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
    const unsigned int dataBytes = rpcParams->numberOfBitsOfData / 8;
	if(rpcId == RPC_InitGame) {
		g_bInitGameProcess = true;
		staticFunc(rpcParams);
		g_bInitGameProcess = false;
		CNetGame::SetGameState(GAMESTATE_CONNECTED);
		return;
	}
	if(rpcId == RPC_ScrDialogBox) {
		 bsData.Read(CNetGame::m_nLastSAMPDialogID);
	}
	if(rpcId == RPC_ScrSetSpawnInfo) {
		staticFunc(rpcParams);
		RakNet::BitStream bsSpawnRequest;
		if (pRakClient) {
			pRakClient->RPC(&RPC_RequestSpawn, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
		}
		return;
	}
	if(rpcId == RPC_WorldPlayerAdd) {
		if (dataBytes < kWorldPlayerAddLegacyPacketBytes) {
			return;
		}

		const unsigned char* inputData = reinterpret_cast<const unsigned char*>(rpcParams->input);
		const uint16_t playerId = ReadUnalignedValue<uint16_t>(inputData, 0);
		if (playerId >= kMaxRemotePlayers) {
			return;
		}

		const bool isLegacyPacket = (dataBytes == kWorldPlayerAddLegacyPacketBytes);
		const bool isCanonicalNativePacket = (dataBytes == kWorldPlayerAddPacketBytes);
		const size_t skinOffset = isLegacyPacket ? 2u : 3u;
		const size_t posOffset = isLegacyPacket ? 6u : 7u;
		const size_t headingOffset = isLegacyPacket ? 18u : 19u;
		const size_t extraOffset = isLegacyPacket ? 22u : 23u;
		const size_t finalByteOffset = isLegacyPacket ? 26u : 27u;
		const uint32_t skinId = ReadUnalignedValue<uint32_t>(inputData, skinOffset);
		const CVector vecPos = ReadUnalignedValue<CVector>(inputData, posOffset);
		const float fHeading = ReadUnalignedValue<float>(inputData, headingOffset);

		if (!IsValidWorldPlayerAddCandidate(playerId, skinId, vecPos, fHeading)) {
			return;
		}

		// Unknown extended payloads are forwarded untouched to avoid corrupting
		// BR-specific fields and crashing stream-in code.
		if (!isLegacyPacket && !isCanonicalNativePacket) {
			EnsureRemotePlayerExists(playerId, "Player");
			staticFunc(rpcParams);
			return;
		}

		RakNet::BitStream bsRepair;
		if (isCanonicalNativePacket) {
			unsigned char rebuilt[kWorldPlayerAddPacketBytes] = {};
			std::memcpy(rebuilt, inputData, kWorldPlayerAddPacketBytes);
			WriteUnalignedValue<uint32_t>(rebuilt, 3, SanitizeRemotePlayerSkin(skinId));
			bsRepair.Write(reinterpret_cast<const char*>(rebuilt), kWorldPlayerAddPacketBytes);
		} else {
			unsigned char rebuilt[kWorldPlayerAddPacketBytes] = {};
			WriteUnalignedValue<uint16_t>(rebuilt, 0, playerId);
			WriteUnalignedValue<uint8_t>(rebuilt, 2, 0);
			WriteUnalignedValue<uint32_t>(rebuilt, 3, SanitizeRemotePlayerSkin(skinId));
			WriteUnalignedValue<CVector>(rebuilt, 7, vecPos);
			WriteUnalignedValue<float>(rebuilt, 19, fHeading);
			WriteUnalignedValue<uint32_t>(rebuilt, 23, ReadUnalignedValue<uint32_t>(inputData, extraOffset));
			WriteUnalignedValue<uint8_t>(rebuilt, 27, inputData[finalByteOffset]);
			bsRepair.Write(reinterpret_cast<const char*>(rebuilt), kWorldPlayerAddPacketBytes);
		}

		rpcParams->input = bsRepair.GetData();
		rpcParams->numberOfBitsOfData = bsRepair.GetNumberOfBitsUsed();
		EnsureRemotePlayerExists(playerId, "Player");
		staticFunc(rpcParams);
		return;
	}
	if(rpcId == RPC_WorldVehicleAdd) {
	        if (dataBytes < kWorldVehicleAddLegacyPacketBytes) {
	            return;
	        }

		if (dataBytes == kWorldVehicleAddLegacyPacketBytes) {
			BRLegacyVehicleAdd legacyPacket{};
			std::memcpy(&legacyPacket, rpcParams->input, sizeof(legacyPacket));
			if (!IsValidLegacyVehicleAdd(legacyPacket)) {
				return;
			}
			RakNet::BitStream bsRepair;
			BuildExpandedVehicleAddPacket(legacyPacket, &bsRepair);
			rpcParams->input = bsRepair.GetData();
			rpcParams->numberOfBitsOfData = bsRepair.GetNumberOfBitsUsed();
			staticFunc(rpcParams);
			return;
		}

		if (dataBytes < kWorldVehicleAddPacketBytes) {
			return;
		}

		const BRNewVehiclePacked* packet = reinterpret_cast<const BRNewVehiclePacked*>(rpcParams->input);
		if (packet->VehicleID >= kMaxRemoteVehicles || packet->iVehicleType <= 0 || packet->iVehicleType > 65535) {
			return;
		}
		staticFunc(rpcParams);
		return;
	}
	if (rpcId == RPC_ScrCustomizeVehicle) {
		staticFunc(rpcParams);
		return;
	}
	if(rpcId == RPC_ServerJoin) {
		staticFunc(rpcParams);
        return;
    }
	
	staticFunc(rpcParams);
}
