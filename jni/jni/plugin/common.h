#pragma once

#include <cstdint>
#include <string>

#include "vendor/RakNet/BitStream.h"
#include "vendor/RakNet/SAMP/SAMPRPC.h"
#include "vendor/RakNet/PacketPriority.h"

#include "game/math/vector.h"

enum BRPacketEnumeration : int32_t
{
    BR_ID_USER_INTERFACE_SYNC = 0xD6,   // custom GUI sync (Black Russia)
    BR_ID_AIM_SYNC = 0xCB,              // ID_AIM_SYNC
    BR_ID_WEAPONS_UPDATE = 0xCC,        // ID_WEAPONS_UPDATE
    BR_ID_STATS_UPDATE = 0xCD,          // ID_STATS_UPDATE
    BR_ID_PASSENGER_SYNC = 0xC9,        // ID_PASSENGER_SYNC
    BR_ID_TRAILER_SYNC = 0xD2,          // ID_TRAILER_SYNC
    BR_ID_PLAYER_SYNC = 0xCF,           // ID_PLAYER_SYNC
    BR_ID_UNOCCUPIED_SYNC = 0xD3,       // ID_UNOCCUPIED_SYNC
    BR_ID_SPECTATOR_SYNC = 0xCA,        // ID_SPECTATOR_SYNC
    BR_ID_BULLET_SYNC = 0xCE,           // ID_BULLET_SYNC
    BR_ID_VEHICLE_SYNC = 0xC8,          // ID_VEHICLE_SYNC
    BR_ID_TURNLIGHTS_SYNC = 0xD4,       // custom turn signals
    BR_ID_USER_PACKET_ENUM = 0x80,
    BR_ID_CONNECTION_ATTEMPT_FAILED = 0x81,
    BR_ID_DISCONNECTION_NOTIFICATION = 0x82,
    BR_ID_NO_FREE_INCOMING_CONNECTIONS = 0x83,
    BR_ID_CONNECTION_REQUEST_ACCEPTED = 0x84,
    BR_ID_TIMESTAMP = 0x85,
    BR_ID_PONG = 0x86,
    BR_ID_CONNECTION_BANNED = 0x87,
    BR_ID_CONNECTION_LOST = 0x88,
    BR_ID_MODIFIED_PACKET = 0x89,
    BR_ID_NEW_INCOMING_CONNECTION = 0x8A,
    BR_ID_INVALID_PASSWORD = 0x8B,
    BR_ID_QUEUE_UPDATE = 0x8C,
    BR_ID_QUEUE_LEFT = 0x8D,
    BR_ID_RPC = 0x8E,
    BR_ID_DETECT_LOST_CONNECTIONS = 0x8F,
    BR_ID_OPEN_CONNECTION_REPLY = 0x90,
    BR_ID_CONNECTED_PONG = 0x91,
    BR_ID_PING_OPEN_CONNECTIONS = 0x92,
    BR_ID_AUTH_KEY = 0x93,
    BR_ID_PING = 0x94,
    BR_ID_INTERNAL_PING = 0x95,
    BR_ID_OPEN_CONNECTION_REQUEST = 0x96,
    BR_ID_CONNECTION_REQUEST = 0x97,
    BR_ID_REQUEST_STATIC_DATA = 0x98,
    BR_ID_SECURED_CONNECTION_RESPONSE = 0x99,
    BR_ID_SECURED_CONNECTION_CONFIRMATION = 0x9A,
    BR_ID_RPC_MAPPING = 0x9B,
    BR_ID_RPC_REPLY = 0x9C,
    BR_ID_BROADCAST_PINGS = 0x9D,
    BR_ID_SET_RANDOM_NUMBER_SEED = 0x9E,
    BR_ID_RSA_PUBLIC_KEY_MISMATCH = 0x9F,
    BR_ID_RECEIVED_STATIC_DATA = 0xA0,
    BR_ID_REMOTE_DISCONNECTION_NOTIFICATION = 0xA1,
    BR_ID_REMOTE_CONNECTION_LOST = 0xA2,
    BR_ID_REMOTE_NEW_INCOMING_CONNECTION = 0xA3,
    BR_ID_REMOTE_EXISTING_CONNECTION = 0xA4,
    BR_ID_REMOTE_STATIC_DATA = 0xA5,
    BR_ID_ADVERTISE_SYSTEM = 0xA6,
    BR_ID_OPEN_CONNECTION_COOKIE = 0xA7,
};

enum BRRpcIds : int32_t
{
    BR_RPC_ClientJoin = 0x19,
    BR_RPC_ScrSetPlayerPos = 0xC,
    BR_RPC_WorldActorRemove = 0x36,
    BR_RPC_ScrSetActorAttachedObject = 0x84,
    BR_RPC_ScrTogglePlayerSpectating = 0x7C,
    BR_RPC_Spawn = 0x34,
    BR_RPC_ScrClearActorAnimations = 0xAE,
    BR_RPC_ScrSetMapIcon = 0x38,
    BR_RPC_EnterVehicle = 0x1A,
    BR_RPC_ScrSetSpecialAction = 0x58,
    BR_RPC_ScrFlashGangZone = 0x79,
    BR_RPC_Chat = 0x65,
    BR_RPC_ScrDetachTrailerFromVehicle = 0x95,
    BR_RPC_ClientInfo = 0x8C,
    BR_RPC_ScrSetSpawnInfo = 0x44,
    BR_RPC_ScrGivePlayerWeapon = 0x16,
    BR_RPC_ScrSetActorHealth = 0xFF,
    BR_RPC_SelectObject = 0x66,
    BR_RPC_RequestClass = 0x80,
    BR_RPC_PickedUpPickup = 0x83,
    BR_RPC_ShowDialogEx = 0x28,
    BR_RPC_ServerQuit = 0x8A,
    BR_RPC_ScrSetPlayerArmour = 0x42,
    BR_RPC_ScrSetPlayerPosFindZ = 0xD,
    BR_RPC_ScrSetObjectRotation = 0x2E,
    BR_RPC_QueueGame = 0x61,
    BR_RPC_ChatBubble = 0x3B,
    BR_RPC_ScrLinkVehicleToInterior = 0x1E,
    BR_RPC_ScrSetInterior = 0x9C,
    BR_RPC_ScrDisableMapIcon = 0x90,
    BR_RPC_ScrCreateObject = 0x2C,
    BR_RPC_ScrDestroyObject = 0x2F,
    BR_RPC_ScrSetVehicleZAngle = 0xA0,
    BR_RPC_WorldPlayerDeath = 0xA6,
    BR_RPC_DisableCheckpoint = 0x25,
    BR_RPC_ScrRemovePlayerFromVehicle = 0x47,
    BR_RPC_ScrAttachCameraToObject = 0xFF,
    BR_RPC_ScrPlaySound = 0x10,
    BR_RPC_SetVehicleParamsEx = 0x29,
    BR_RPC_ObjectCollision = 0x2A,
    BR_RPC_ScrSelectTextDraw = 0x53,
    BR_RPC_DialogResponse = 0x3E,
    BR_RPC_ScrSetActorFacingAngle = 0x17,
    BR_RPC_ScrClientAction = 0x45,
    BR_RPC_EditAttachedObject = 0x11,
    BR_RPC_ScrPlayerSpectatePlayer = 0x7E,
    BR_RPC_WorldVehicleAdd = 0xA4,
    BR_RPC_Death = 0x35,
    BR_RPC_ScrSetVehicleVelocity = 0x5B,
    BR_RPC_ScrDisplayGameText = 0x49,
    BR_RPC_ConnectionRejected = 0x82,
    BR_RPC_SetPlayerSkillLevel = 0xFF,
    BR_RPC_ScrSetVehicleHealth = 0x93,
    BR_RPC_MapMarker = 0x77,
    BR_RPC_ScrRemoveGangZone = 0x78,
    BR_RPC_ScrResetPlayerWeapons = 0x15,
    BR_RPC_ScrSetPlayerVelocity = 0x5A,
    BR_RPC_ScrHideTextDraw = 0x87,
    BR_RPC_ScrSetMaxHealth = 0x41,
    BR_RPC_ScrInterpolateCamera = 0x52,
    BR_RPC_UpdateScoresPingsIPs = 0x9B,
    BR_RPC_ShowDialog = 0x37,
    BR_RPC_ScrPlayAudioStream = 0x92,
    BR_RPC_SetGlobalTimer = 0x7B,
    BR_RPC_ScrSetObjectMaterial = 0xFF,
    BR_RPC_DisableRaceCheckpoint = 0x27,
    BR_RPC_ScrClearAnimations = 0x39,
    BR_RPC_ServerCommand = 0x32,
    BR_RPC_VehicleDestroyed = 0x88,
    BR_RPC_WorldPlayerAdd = 0x20,
    BR_RPC_ScrStopFlashGangZone = 0x55,
    BR_RPC_ScrSetPlayerColor = 0x48,
    BR_RPC_WorldTime = 0x5E,
    BR_RPC_WorldVehicleRemove = 0xA5,
    BR_RPC_ClickTextDraw = 0x53,
    BR_RPC_ScrSetPlayerSkin = 0x99,
    BR_RPC_DestroyWaypoint = 0x4A,
    BR_RPC_InitGame = 0x8B,
    BR_RPC_ScrStopObject = 0x7A,
    BR_RPC_ScrSetArmedWeapon = 0x4C,
    BR_RPC_ClientMessage = 0x5D,
    BR_RPC_ScrRemoveVehicleComponent = 0x4D,
    BR_RPC_SetCheckpoint = 0x6B,
    BR_RPC_CreateWaypoint = 0x4E,
    BR_RPC_ScrSetMaxArmour = 0x56,
    BR_RPC_ScrSetPlayerAttachedObject = 0x71,
    BR_RPC_CancelEdit = 0x57,
    BR_RPC_ScrSetPlayerName = 0xB,
    BR_RPC_WorldActorAdd = 0x68,
    BR_RPC_ScrPlayerSpectateVehicle = 0x7F,
    BR_RPC_ScrCreateExplosion = 0x4F,
    BR_RPC_DamageVehicle = 0x6A,
    BR_RPC_ScrEditTextDraw = 0x69,
    BR_RPC_WorldPlayerRemove = 0xA3,
    BR_RPC_ScrAttachObjectToPlayer = 0x4B,
    BR_RPC_ScrAttachTrailerToVehicle = 0x94,
    BR_RPC_ScrPutPlayerInVehicle = 0x46,
    BR_RPC_ScrShowTextDraw = 0x86,
    BR_RPC_ScmEvent = 0x60,
    BR_RPC_ScrApplyActorAnimation = 0xAD,
    BR_RPC_SetRaceCheckpoint = 0x26,
    BR_RPC_ScrSetVehiclePos = 0x9F,
    BR_RPC_Weather = 0x98,
    BR_RPC_ScrSetPlayerHealth = 0xE,
    BR_RPC_DestroyPickup = 0x3F,
    BR_RPC_Pickup = 0x5F,
    BR_RPC_ScrSetObjectPos = 0x2D,
    BR_RPC_EditObject = 0x75,
    BR_RPC_ScrAddGangZone = 0x6C,
    BR_RPC_RequestSpawn = 0x81,
    BR_RPC_ScrApplyAnimation = 0xFF,
    BR_RPC_ScrResetMoney = 0x14,
    BR_RPC_ScrTogglePlayerControllable = 0xF,
    BR_RPC_SetTimeEx = 0x1D,
    BR_RPC_ScrSetWeaponAmmo = 0x91,
    BR_RPC_Create3DTextLabel = 0xFF,
    BR_RPC_Destroy3DTextLabel = 0x3D,
    BR_RPC_PlayerGiveTakeDamage = 0x73,
    BR_RPC_ScrSetCameraLookAt = 0x9E,
    BR_RPC_ServerJoin = 0x89,
    BR_RPC_ScrStopAudioStream = 0x3C,
    BR_RPC_ScrSetCameraBehindPlayer = 0xA2,
    BR_RPC_ScrMoveObject = 0x63,
    BR_RPC_ScrSetActorPos = 0x24,
    BR_RPC_ScrHaveSomeMoney = 0x12,
    BR_RPC_ActorGiveDamage = 0x3A,
    BR_RPC_ScrSetEntityOutline = 0x18,
    BR_RPC_SetInteriorId = 0x76,
    BR_RPC_ScrSetPlayerWantedLevel = 0x85,
    BR_RPC_ScrSetFightingStyle = 0x59,
    BR_RPC_ScrSetPlayerDrunkLevel = 0x23,
    BR_RPC_ScrSetPlayerFacingAngle = 0x13,
    BR_RPC_ScrSetCameraPos = 0x9D,
    BR_RPC_ScrShowNameTag = 0x50,
    BR_RPC_ExitVehicle = 0x9A,
    BR_RPC_ScrVehicleParams = 0xA1,
    BR_RPC_ProjectileHit = 0xAB,
    BR_RPC_ScrPlayRadioStream = 0xAC,
    BR_RPC_ScrCustomizeVehicle = 0xA7,
    BR_RPC_ScrCommonStuff = 0xA8
};

enum BRPacketReliability : int32_t
{
	BR_RELIABILITY_UNRELIABLE = 0x0,
	BR_RELIABILITY_UNRELIABLE_SEQUENCED = 0x1,
	BR_RELIABILITY_RELIABLE = 0x2,
	BR_RELIABILITY_RELIABLE_ORDERED = 0x3,
	BR_RELIABILITY_RELIABLE_SEQUENCED = 0x4,
};

int ConvertBRIDToSampID(BRRpcIds value);
PacketReliability ConvertBRToSampReliability(BRPacketReliability reliability);

#pragma pack(push, 1);
typedef struct _BROnFootSyncData
{
	int16_t lrAnalogLeftStick;
	int16_t udAnalogLeftStick;
	uint16_t wKeys;
	CVector vecPos;
	float quatw;
	float quatx;
	float quaty;
	float quatz;
	uint16_t health;
	uint16_t armour;
	uint8_t byteCurrentWeapon;
	uint8_t byteSpecialAction;
	CVector vecMoveSpeed;
	CVector vecSurfOffsets;
	uint16_t wSurfInfo;
	uint32_t dwAnimation;
} BROnFootSyncData;
#pragma pack(pop);

#pragma pack(push, 1);
struct BRInCarSyncData
{
	uint16_t VehicleID;
	uint16_t lrAnalogLeftStick;
	uint16_t udAnalogLeftStick;
	uint16_t wKeys;
	float quatw;
	float quatx;
	float quaty;
	float quatz;
	CVector vecPos;
	CVector vecMoveSpeed;
	float fCarHealth;
	uint16_t playerHealth;
	uint16_t playerArmour;
	uint8_t byteCurrentWeapon;
	uint8_t byteSirenOn;
	uint8_t byteLandingGearState;
	uint16_t TrailerID;
	int16_t lrAnalogRightStick;
	int16_t udAnalogRightStick;
};
#pragma pack(pop);

#pragma pack(push, 1);
struct BRPassengerSyncData
{
	uint16_t VehicleID;
	uint8_t byteSeatFlags : 7;
	uint8_t byteDriveBy : 1;
	uint8_t byteCurrentWeapon;
	uint16_t playerHealth;
	uint16_t playerArmour;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	CVector vecPos;
};
#pragma pack(pop);

#pragma pack(push, 1);
struct OnFootSyncData
{
	int16_t lrAnalog;
	int16_t udAnalog;
	uint16_t wKeys;
	CVector vecPos;
	float quatw;
	float quatx;
	float quaty;
	float quatz;
	uint8_t byteHealth;
	uint8_t byteArmour;
	uint8_t byteCurrentWeapon;
	uint8_t byteSpecialAction;
	CVector vecMoveSpeed;
	CVector vecSurfOffsets;
	uint16_t wSurfInfo;
	uint32_t dwAnimation;
};
#pragma pack(pop);

#pragma pack(push, 1);
struct InCarSyncData
{
	uint16_t VehicleID;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	float quatw;
	float quatx;
	float quaty;
	float quatz;
	CVector vecPos;
	CVector vecMoveSpeed;
	float fCarHealth;
	uint8_t bytePlayerHealth;
	uint8_t bytePlayerArmour;
	uint8_t byteCurrentWeapon;
	uint8_t byteSirenOn;
	uint8_t byteLandingGearState;
	uint16_t TrailerID;
	float fTrainSpeed;
};
#pragma pack(pop);

#pragma pack(push, 1);
struct PassengerSyncData
{
	uint16_t VehicleID;
	uint8_t byteSeatFlags : 7;
	uint8_t byteDriveBy : 1;
	uint8_t byteCurrentWeapon;
	uint8_t bytePlayerHealth;
	uint8_t bytePlayerArmour;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	CVector vecPos;
};
#pragma pack(pop);

#pragma pack(push, 1)
struct BRNewVehiclePacked
{
	uint16_t VehicleID;
	int iVehicleType;
	CVector vecPos;
	float fRotation;
	uint8_t aColor1;
	uint8_t aColor2;
	float fHealth;
	uint8_t byteInterior;
	uint32_t dwDoorDamageStatus;
	uint32_t dwPanelDamageStatus;
	uint8_t byteLightDamageStatus;
	uint8_t byteTireDamageStatus;
	uint8_t byteAddSiren;
	uint16_t byteModSlots[2];
	int32_t cColor1;
	int32_t cColor2;
	uint32_t dwWindowsColor;
	uint32_t dwWindowsColor2;
	uint32_t dwWindowsColor3;
	uint32_t dwWindowsColor4;
	float fSuspensionForce;
	float fSuspensionCenter;
	float fWheelSize;
	float fMaxSpeed;
	float fEngineAcceleration;
	float fSteeringLock;
	uint32_t dwLightsColor;
	uint32_t dwNeonColor;
	uint32_t dwNeonColor2;
	uint32_t dwNeonColor3;
	float fWheelsAngle;
	float fWheelsAngleRear;
	uint32_t dwWheelsWidth;
	uint8_t bDriftMode;
	float fWheelsOffset;
	float fWheelsOffsetRear;
	float fWheelScale;
	float fWheelScaleRear;
	uint32_t dwColor[4];
	uint8_t bHornTon;
	uint32_t dwStrobesMode;
	uint32_t dwChipMode;
	uint8_t bHydauliks;
	uint8_t bFar;
	int16_t wExhaust;
	uint8_t bUseSiren;
	uint8_t bEnabledSiren;
};
#pragma pack(pop)

void ConvertBROnFootSyncToSampSync(RakNet::BitStream*, BROnFootSyncData);
void ConvertBRInCarSyncToSampSync(RakNet::BitStream*, BRInCarSyncData);
void ConvertBRPassengerSyncToSampSync(RakNet::BitStream*, BRPassengerSyncData);

bool IsRPCNeedFix(int rpcId);
void FixBrokenRPC(int rpcId, RPCParameters* rpcParams, void (*staticFunc)(RPCParameters*));
