#include "netgame.h"
#include "xorstr.h"
#include "plugin.h"
#include "features/br_features.h"
#include "features/br_sync_state.h"

#include <cstring>
#include <array>

#define NETGAME_VERSION 4057

extern RakClientInterface* pRakClient;

uint16_t CNetGame::m_nLastSAMPDialogID;

uint8_t GetPacketID(Packet *p)
{
    if (!p || !p->data || p->length == 0)
        return 255;

    if (p->data[0] != ID_TIMESTAMP)
        return static_cast<uint8_t>(p->data[0]);

    constexpr size_t kTimestampHeader = sizeof(uint8_t) + sizeof(RakNetTime);
    if (p->length <= kTimestampHeader)
        return 255;

    return static_cast<uint8_t>(p->data[kTimestampHeader]);
}

namespace
{
    constexpr uint16_t kMaxPlayers = 1504;

    bool HasBits(const RakNet::BitStream& bs, size_t bits)
    {
        return bs.GetNumberOfUnreadBits() >= static_cast<int>(bits);
    }

    bool ReadPacketHeader(RakNet::BitStream& bs, uint8_t& packetId)
    {
        return HasBits(bs, 8) && bs.ReadBits(&packetId, 8);
    }

    bool ReadTimestampedHeader(RakNet::BitStream& bs, uint8_t& packetId, uint32_t& timestamp)
    {
        return HasBits(bs, 8) && bs.ReadBits(&packetId, 8) &&
               HasBits(bs, 32u) &&
               bs.ReadBits(reinterpret_cast<unsigned char*>(&timestamp), 32u);
    }
}

CPlayerPool* CNetGame::GetPlayerPool()
{
    const uintptr_t addr = CGameAPI::GetBase(xorstr("CNetGame::m_pPlayerPool"));
    if (!addr) return nullptr;
    return *reinterpret_cast<CPlayerPool **>(addr);
}

void CNetGame::ProcessNetwork()
{
    if (!pRakClient)
        return;

    Packet* pkt = nullptr;
    uint8_t packetIdentifier = 255;
    while ((pkt = pRakClient->Receive()) != nullptr)
    {
        packetIdentifier = GetPacketID(pkt);
        switch (packetIdentifier)
        {
            case ID_FAILED_INITIALIZE_ENCRIPTION:
                CChat::AddDebugMessage(xorstr("Failed to initialize encryption."));
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                CChat::AddDebugMessage(xorstr("Сервер не отвечает. Переподключение..."));
                SetGameState(GAMESTATE_WAIT_CONNECT);
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                CChat::AddDebugMessage(xorstr("Сервер полон. Переподключение..."));
                SetGameState(GAMESTATE_WAIT_CONNECT);
                pRakClient->Disconnect(0, 0);
                break;
            case ID_CONNECTION_BANNED:
                CChat::AddDebugMessage(xorstr("Вы были заблокированы на этом сервере."));
                break;
            case ID_INVALID_PASSWORD:
                CChat::AddDebugMessage(xorstr("Wrong server password."));
                pRakClient->Disconnect(0);
                break;
            case ID_AUTH_KEY:
                Packet_AuthKey(pkt);
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                Packet_ConnectionSucceeded(pkt);
                break;
            case ID_CONNECTION_LOST:
                CChat::AddDebugMessage(xorstr("Переподключение через 15 секунд..."));
                Packet_ConnectionLost(pkt);
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                CChat::AddDebugMessage(xorstr("Переподключение через 15 секунд..."));
                pRakClient->Disconnect(2000, 0);
                break;
            case ID_AIM_SYNC:
                Packet_AimSync(pkt);
                break;
            case ID_PLAYER_SYNC:
                Packet_PlayerSync(pkt);
                break;
            case ID_VEHICLE_SYNC:
                Packet_VehicleSync(pkt);
                break;
            case ID_PASSENGER_SYNC:
                Packet_PassengerSync(pkt);
                break;
            case ID_BULLET_SYNC:
                Packet_BulletSync(pkt);
                break;
            case ID_UNOCCUPIED_SYNC:
                Packet_UnoccupiedSync(pkt);
                break;
            case ID_TRAILER_SYNC:
                Packet_TrailerSync(pkt);
                break;
            case ID_SPECTATOR_SYNC:
                Packet_SpectatorSync(pkt);
                break;
            case ID_MARKERS_SYNC:
                Packet_MarkersSync(pkt);
                break;
            case BR_ID_TURNLIGHTS_SYNC:
                Packet_Turnlights(pkt);
                break;
            case 252:
                Packet_GUI(pkt);
                break;
            default:
                break;
        }
        pRakClient->DeallocatePacket(pkt);
    }
}

int CNetGame::GetGameState()
{
    const uintptr_t addr = CGameAPI::GetBase(xorstr("CNetGame::m_iGameState"));
    return addr ? *reinterpret_cast<int*>(addr) : GAMESTATE_NONE;
}

void CNetGame::SetGameState(int state)
{
    const uintptr_t addr = CGameAPI::GetBase(xorstr("CNetGame::m_iGameState"));
    if (addr)
        *reinterpret_cast<int*>(addr) = state;
}

#ifdef __arm__
void gen_auth_key(char buf[260], char* auth_in);
void CNetGame::Packet_AuthKey(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length < 2 || !pRakClient)
        return;

    RakNet::BitStream bsAuth(reinterpret_cast<unsigned char *>(pkt->data), pkt->length, false);
    uint8_t byteAuthLen = 0;
    char szAuth[260] = {0};

    bsAuth.IgnoreBits(8);
    if (!bsAuth.Read(byteAuthLen) ||
        byteAuthLen > static_cast<uint8_t>(sizeof(szAuth) - 1u) ||
        bsAuth.GetNumberOfUnreadBits() < static_cast<int>(byteAuthLen) * 8)
        return;
    if (byteAuthLen > 0 && !bsAuth.Read(szAuth, byteAuthLen))
        return;
    szAuth[byteAuthLen] = '\0';

    char szAuthKey[260] = {0};
    gen_auth_key(szAuthKey, szAuth);

    const size_t keyLenRaw = strnlen(szAuthKey, sizeof(szAuthKey));
    if (keyLenRaw > 255)
        return;
    const uint8_t byteAuthKeyLen = static_cast<uint8_t>(keyLenRaw);

    RakNet::BitStream bsKey;
    bsKey.Write(static_cast<uint8_t>(ID_AUTH_KEY));
    bsKey.Write(byteAuthKeyLen);
    if (byteAuthKeyLen > 0)
        bsKey.Write(szAuthKey, byteAuthKeyLen);
    pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);
}
#endif

#ifdef __aarch64__
extern char AuthKeyTable[512][2][128];
void CNetGame::Packet_AuthKey(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length < 3 || !pRakClient)
        return;

    const char* incomingKey = reinterpret_cast<const char*>(pkt->data) + 2;
    const size_t incomingMax = pkt->length - 2;
    const size_t incomingLen = strnlen(incomingKey, incomingMax);
    if (incomingLen == incomingMax || incomingLen >= 128)
        return;

    const char* auth_key = nullptr;
    for (int x = 0; x < 512; ++x)
    {
        const char* candidate = AuthKeyTable[x][0];
        const size_t candidateLen = strnlen(candidate, 128);
        if (candidateLen == incomingLen && memcmp(candidate, incomingKey, incomingLen) == 0)
        {
            auth_key = AuthKeyTable[x][1];
            break;
        }
    }

    if (!auth_key)
        return;

    const size_t keyLenRaw = strnlen(auth_key, 128);
    if (keyLenRaw > 255)
        return;

    RakNet::BitStream bsKey;
    bsKey.Write(static_cast<uint8_t>(ID_AUTH_KEY));
    bsKey.Write(static_cast<uint8_t>(keyLenRaw));
    if (keyLenRaw > 0)
        bsKey.Write(auth_key, static_cast<unsigned int>(keyLenRaw));
    pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);
}
#endif

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
    br::FeatureState::Instance().Reset();
    br::SyncRegistry::Instance().Reset();
    const uintptr_t fn = CGameAPI::GetBase(xorstr("CNetGame::Packet_ConnectionLost"));
    if (fn)
        reinterpret_cast<void(*)(Packet*)>(fn)(pkt);
}

void CNetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
    br::FeatureState::Instance().Reset();
    br::SyncRegistry::Instance().Reset();
    if (!pkt || !pkt->data || pkt->length < 13 || !pRakClient)
        return;

    RakNet::BitStream bsSuccAuth(reinterpret_cast<unsigned char *>(pkt->data), pkt->length, false);
    uint8_t packetId = 0;
    uint16_t myPlayerID = 0;
    unsigned int uiChallenge = 0;

    if (!bsSuccAuth.Read(packetId) || packetId != ID_CONNECTION_REQUEST_ACCEPTED ||
        bsSuccAuth.GetNumberOfUnreadBits() < 32 + 16 + 16 + 32)
        return;
    bsSuccAuth.IgnoreBits(32);
    bsSuccAuth.IgnoreBits(16);
    if (!bsSuccAuth.Read(myPlayerID) || !bsSuccAuth.Read(uiChallenge))
        return;

    CPlayerPool* playerPool = GetPlayerPool();
    CLocalPlayer* localPlayer = playerPool ? playerPool->GetLocalPlayer() : nullptr;
    if (!localPlayer)
        return;
    localPlayer->SetLocalPlayerID(myPlayerID);

    int iVersion = NETGAME_VERSION;
    char byteMod = 0x01;
    unsigned int uiClientChallengeResponse = uiChallenge ^ static_cast<unsigned int>(iVersion);
    const char* sampVersion = xorstr("0.3.7");
    const char* auth_bs = xorstr("15121F6F18550C00AC4B4F8A167D0379BB0ACA99043");
    const char* localPlayerName = reinterpret_cast<const char*>(localPlayer->GetLocalPlayerName());
    if (!localPlayerName)
        return;

    const size_t nameLenRaw = strnlen(localPlayerName, 255);
    if (nameLenRaw > 255)
        return;
    const char byteAuthBSLen = static_cast<char>(strlen(auth_bs));
    const char byteNameLen = static_cast<char>(nameLenRaw);
    const char byteClientverLen = static_cast<char>(strlen(sampVersion));

    RakNet::BitStream bsSend;
    bsSend.Write(iVersion);
    bsSend.Write(byteMod);
    bsSend.Write(byteNameLen);
    if (byteNameLen > 0) bsSend.Write(localPlayerName, byteNameLen);
    bsSend.Write(uiClientChallengeResponse);
    bsSend.Write(byteAuthBSLen);
    bsSend.Write(auth_bs, byteAuthBSLen);
    bsSend.Write(byteClientverLen);
    bsSend.Write(sampVersion, byteClientverLen);
    pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    SetGameState(GAMESTATE_AWAIT_JOIN);
}

void CNetGame::Packet_AimSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0; uint8_t payload[31] = {0};
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) || playerId >= kMaxPlayers ||
        !HasBits(bsData, sizeof(payload) * 8u) || !bsData.ReadBits(payload, sizeof(payload) * 8u)) return;
    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptTimestamp(playerId, br::SyncChannel::Aim, timestamp)) return;
    syncRegistry.RecordAim(playerId, payload, timestamp);
    CPlayerPool* pool = GetPlayerPool(); if (!pool) return;
    if (CRemotePlayer* remote = pool->GetAt(playerId)) remote->StoreAimSyncData(payload, timestamp);
}

void CNetGame::Packet_PlayerSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0;
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) || playerId >= kMaxPlayers) return;

    int16_t lrAnalog = 0, udAnalog = 0;
    uint16_t wKeys = 0, wSurfInfo = 0;
    CVector vecPos = {0,0,0}, vecMoveSpeed = {0,0,0}, vecSurfOffsets = {0,0,0};
    float tw = 1.0f, tx = 0.0f, ty = 0.0f, tz = 0.0f;
    uint8_t healthArmour = 0, currentWeapon = 0, specialAction = 0;
    uint16_t animationId = 0, animationFlags = 0;

    bool present = false;
    if (!bsData.Read(present) || (present && !bsData.ReadBits(reinterpret_cast<unsigned char*>(&lrAnalog), 16))) return;
    if (!bsData.Read(present) || (present && !bsData.ReadBits(reinterpret_cast<unsigned char*>(&udAnalog), 16))) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&wKeys), 16) ||
        !bsData.Read(reinterpret_cast<char*>(&vecPos), sizeof(vecPos)) ||
        !bsData.ReadNormQuat<float>(tw, tx, ty, tz) ||
        !bsData.ReadBits(&healthArmour, 8) ||
        !bsData.ReadBits(&currentWeapon, 8) ||
        !bsData.ReadBits(&specialAction, 8) ||
        !bsData.ReadVector<float>(vecMoveSpeed.x, vecMoveSpeed.y, vecMoveSpeed.z)) return;

    if (!bsData.Read(present)) return;
    if (present)
    {
        if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&wSurfInfo), 16) ||
            !bsData.Read(reinterpret_cast<char*>(&vecSurfOffsets), sizeof(vecSurfOffsets))) return;
    }

    // Animation ID/flags are another optional section. The old parser stopped
    // before it, silently dropping remote animation state.
    if (!bsData.Read(present)) return;
    if (present)
    {
        if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&animationId), 16) ||
            !bsData.ReadBits(reinterpret_cast<unsigned char*>(&animationFlags), 16)) return;
    }

    const uint8_t armourNibble = healthArmour & 0x0F;
    const uint8_t healthNibble = healthArmour >> 4;
    BROnFootSyncData sync = {};
    sync.lrAnalogLeftStick = lrAnalog;
    sync.udAnalogLeftStick = udAnalog;
    sync.wKeys = wKeys;
    sync.vecPos = vecPos;
    sync.quatw = tw;
    sync.quatx = tx;
    sync.quaty = ty;
    sync.quatz = tz;
    sync.health = healthNibble == 0xF ? 100 : static_cast<uint8_t>(healthNibble * 7);
    sync.armour = armourNibble == 0xF ? 100 : static_cast<uint8_t>(armourNibble * 7);
    sync.byteCurrentWeapon = currentWeapon;
    sync.byteSpecialAction = specialAction;
    sync.vecMoveSpeed = vecMoveSpeed;
    sync.vecSurfOffsets = vecSurfOffsets;
    sync.wSurfInfo = wSurfInfo;
    sync.dwAnimation = static_cast<uint32_t>(animationId) | (static_cast<uint32_t>(animationFlags) << 16);

    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptTimestamp(playerId, br::SyncChannel::OnFoot, timestamp)) return;
    if (!syncRegistry.NormalizeOnFoot(sync)) return;
    syncRegistry.RecordOnFoot(playerId, sync, timestamp);

    CPlayerPool* pool = GetPlayerPool(); if (!pool) return;
    if (CRemotePlayer* remote = pool->GetAt(playerId)) remote->StoreSyncData(&sync, timestamp);
}

void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0;
    BRInCarSyncData sync = {};
    uint16_t carHealth = 0; uint8_t healthArmour = 0, currentWeapon = 0;
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) || playerId >= kMaxPlayers ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&sync.VehicleID), 16) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&sync.lrAnalogLeftStick), 16) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&sync.udAnalogLeftStick), 16) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&sync.wKeys), 16) ||
        !bsData.ReadNormQuat<float>(sync.quatw, sync.quatx, sync.quaty, sync.quatz) ||
        !bsData.Read(reinterpret_cast<char*>(&sync.vecPos), sizeof(sync.vecPos)) ||
        !bsData.ReadVector<float>(sync.vecMoveSpeed.x, sync.vecMoveSpeed.y, sync.vecMoveSpeed.z) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&carHealth), 16) ||
        !bsData.Read(healthArmour) || !bsData.Read(currentWeapon)) return;
    const uint8_t armourNibble = healthArmour & 0x0F, healthNibble = healthArmour >> 4;
    sync.fCarHealth = static_cast<float>(carHealth);
    sync.playerHealth = healthNibble == 0xF ? 100 : static_cast<uint16_t>(healthNibble * 7);
    sync.playerArmour = armourNibble == 0xF ? 100 : static_cast<uint16_t>(armourNibble * 7);
    sync.byteCurrentWeapon = currentWeapon;
    bool flag = false;
    if (!bsData.ReadCompressed(flag)) return; sync.byteSirenOn = flag ? 1 : 0;
    if (!bsData.ReadCompressed(flag)) return; sync.byteLandingGearState = flag ? 1 : 0;

    // Standard vehicle sync carries optional train speed and trailer sections.
    bool hasTrainSpeed = false;
    if (!bsData.Read(hasTrainSpeed)) return;
    if (hasTrainSpeed)
    {
        float trainSpeed = 0.0f;
        if (!bsData.Read(trainSpeed)) return;
        (void)trainSpeed;
    }

    bool hasTrailer = false;
    sync.TrailerID = 0xFFFF;
    if (!bsData.Read(hasTrailer)) return;
    if (hasTrailer && !bsData.Read(sync.TrailerID)) return;

    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptTimestamp(playerId, br::SyncChannel::InCar, timestamp)) return;
    if (!syncRegistry.NormalizeInCar(sync)) return;
    syncRegistry.RecordInCar(playerId, sync, timestamp);
    br::FeatureState::Instance().SetVehicleHealth(sync.VehicleID, sync.fCarHealth);

    CPlayerPool* pool = GetPlayerPool(); if (!pool) return;
    if (CRemotePlayer* remote = pool->GetAt(playerId)) remote->StoreInCarSyncData(&sync, timestamp);
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0; uint8_t payload[24] = {0};
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) || playerId >= kMaxPlayers ||
        !HasBits(bsData, sizeof(payload) * 8u) || !bsData.ReadBits(payload, sizeof(payload) * 8u)) return;

    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptTimestamp(playerId, br::SyncChannel::Passenger, timestamp)) return;
    syncRegistry.RecordPassenger(playerId, payload, timestamp);

    CPlayerPool* pool = GetPlayerPool(); if (!pool) return;
    if (CRemotePlayer* remote = pool->GetAt(playerId)) remote->StorePassengerSyncData(payload, timestamp);
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0; uint8_t payload[40] = {0};
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) || playerId >= kMaxPlayers ||
        !HasBits(bsData, sizeof(payload) * 8u) || !bsData.ReadBits(payload, sizeof(payload) * 8u)) return;
    CPlayerPool* pool = GetPlayerPool(); if (!pool) return;
    CRemotePlayer* remote = pool->GetAt(playerId); CLocalPlayer* local = pool->GetLocalPlayer();
    if (!remote || !local || local->GetLocalPlayerID() == playerId) return;
    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptTimestamp(playerId, br::SyncChannel::Bullet, timestamp)) return;
    syncRegistry.RecordBullet(playerId, payload, timestamp);
    remote->StoreBulletSyncData(payload, timestamp);
}

void CNetGame::Packet_UnoccupiedSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0; uint8_t payload[67] = {};
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) || playerId >= kMaxPlayers ||
        !HasBits(bsData, sizeof(payload) * 8u) || !bsData.ReadBits(payload, sizeof(payload) * 8u)) return;
    uint16_t vehicleId = 0xFFFF;
    std::memcpy(&vehicleId, payload, sizeof(vehicleId));
    if (vehicleId >= br::kMaxSyncVehicles) { br::SyncRegistry::Instance().MarkRangeReject(); return; }
    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptVehicleTimestamp(vehicleId, br::SyncChannel::Unoccupied, timestamp)) return;
    syncRegistry.RecordUnoccupied(playerId, vehicleId, payload, timestamp);
}

void CNetGame::Packet_TrailerSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0; uint8_t payload[54] = {};
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) || playerId >= kMaxPlayers ||
        !HasBits(bsData, sizeof(payload) * 8u) || !bsData.ReadBits(payload, sizeof(payload) * 8u)) return;
    uint16_t trailerId = 0xFFFF;
    std::memcpy(&trailerId, payload, sizeof(trailerId));
    if (trailerId >= br::kMaxSyncVehicles) { br::SyncRegistry::Instance().MarkRangeReject(); return; }
    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptVehicleTimestamp(trailerId, br::SyncChannel::Trailer, timestamp)) return;
    syncRegistry.RecordTrailer(playerId, trailerId, payload, timestamp);
}

void CNetGame::Packet_SpectatorSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0; uint8_t payload[18] = {};
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) || playerId >= kMaxPlayers ||
        !HasBits(bsData, sizeof(payload) * 8u) || !bsData.ReadBits(payload, sizeof(payload) * 8u)) return;
    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptTimestamp(playerId, br::SyncChannel::Spectator, timestamp)) return;
    syncRegistry.RecordSpectator(playerId, payload, timestamp);
}

void CNetGame::Packet_MarkersSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length == 0 || GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0;
    uint32_t timestamp = 0;
    if (pkt->data[0] == ID_TIMESTAMP) { if (!ReadTimestampedHeader(bsData, pktId, timestamp)) return; }
    else if (!ReadPacketHeader(bsData, pktId)) return;

    int32_t playerCount = 0;
    if (!bsData.Read(playerCount) || playerCount < 0 || playerCount > static_cast<int32_t>(kMaxPlayers)) {
        br::SyncRegistry::Instance().MarkRangeReject();
        return;
    }

    br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
    if (!syncRegistry.AcceptMarkersTimestamp(timestamp)) return;

    std::array<br::MarkerSnapshot, br::kMaxSyncPlayers> markers{};
    for (int32_t i = 0; i < playerCount; ++i)
    {
        uint16_t playerId = 0;
        bool active = false;
        if (!bsData.Read(playerId) || playerId >= br::kMaxSyncPlayers || !bsData.Read(active)) {
            br::SyncRegistry::Instance().MarkInvalid();
            return;
        }
        if (!active) continue;

        int16_t x = 0, y = 0, z = 0;
        if (!bsData.Read(x) || !bsData.Read(y) || !bsData.Read(z)) {
            br::SyncRegistry::Instance().MarkInvalid();
            return;
        }
        markers[playerId].active = true;
        markers[playerId].position = CVector(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
        markers[playerId].lastTimestamp = timestamp;
    }
    syncRegistry.SetMarkers(markers);
}

void CNetGame::SendJsonData(int guiId, const char* data, uint32_t length)
{
    constexpr uint32_t kMaxGuiJsonSize = 1024u * 1024u;
    if (!pRakClient || (length > 0 && !data) || length > kMaxGuiJsonSize) return;
    RakNet::BitStream bsSend;
    bsSend.Write(static_cast<uint8_t>(252));
    bsSend.Write(static_cast<uint16_t>(guiId));
    bsSend.Write(length);
    if (length > 0) bsSend.Write(data, length);
    pRakClient->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C" JNIEXPORT void JNICALL Java_com_blackhub_bronline_game_core_JNIJSONTransport_sendJsonData(
    JNIEnv* env, jclass clazz, jint guiId, jbyteArray data)
{
    if (!data)
        return;

    if (!env) return;
    const jsize length = env->GetArrayLength(data);
    if (length < 0 || static_cast<uint32_t>(length) > 1024u * 1024u) return;

    jbyte* byteArray = env->GetByteArrayElements(data, nullptr);
    if (!byteArray && length > 0) return;

    if (guiId != 10)
        CNetGame::SendJsonData(guiId, reinterpret_cast<const char*>(byteArray), static_cast<uint32_t>(length));

    if (byteArray) env->ReleaseByteArrayElements(data, byteArray, JNI_ABORT);
}


void CNetGame::Packet_GUI(Packet* pkt)
{
    constexpr uint32_t kMaxGuiJsonSize = 1024u * 1024u;
    if (!pkt || !pkt->data || pkt->length < 7u) return;

    RakNet::BitStream bs(pkt->data, pkt->length, false);
    uint8_t packetId = 0;
    uint16_t guiId = 0;
    uint32_t jsonLen = 0;
    if (!bs.Read(packetId) || packetId != 252 || !bs.Read(guiId) || !bs.Read(jsonLen)) return;
    if (jsonLen > kMaxGuiJsonSize || bs.GetNumberOfUnreadBits() < static_cast<int>(jsonLen) * 8) return;

    const uintptr_t fn = CGameAPI::GetBase(xorstr("CNetGame::Packet_GUI"));
    if (fn) reinterpret_cast<void(*)(Packet*)>(fn)(pkt);
}

void CNetGame::Packet_Turnlights(Packet* pkt)
{
    if (pkt && pkt->data && pkt->length >= 5u)
    {
        RakNet::BitStream bs(pkt->data, pkt->length, false);
        uint8_t packetId = 0;
        uint16_t vehicleId = 0;
        uint8_t left = 0, right = 0, hazard = 0;
        if (bs.Read(packetId) && packetId == BR_ID_TURNLIGHTS_SYNC &&
            bs.Read(vehicleId) && bs.Read(left) && bs.Read(right) && bs.Read(hazard))
        {
            br::SyncRegistry::Instance().RecordTurnlights(vehicleId, left != 0, right != 0, hazard != 0);
        }
    }

    const uintptr_t fn = CGameAPI::GetBase(xorstr("CNetGame::Packet_Turnlights"));
    if (fn) reinterpret_cast<void(*)(Packet*)>(fn)(pkt);
}
