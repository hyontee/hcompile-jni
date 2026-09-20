#include "br_sync_state.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace br
{
SyncRegistry& SyncRegistry::Instance()
{
    static SyncRegistry registry;
    return registry;
}

void SyncRegistry::Reset()
{
    for (PlayerSyncSnapshot& player : m_players) player = PlayerSyncSnapshot{};
    for (VehicleSyncSnapshot& vehicle : m_vehicles) vehicle = VehicleSyncSnapshot{};
    for (MarkerSnapshot& marker : m_markers) marker = MarkerSnapshot{};
    m_counters = SyncCounters{};
    m_lastMarkersTimestamp = 0;
}

bool SyncRegistry::IsNewerTimestamp(uint32_t timestamp, uint32_t previous)
{
    if (timestamp == 0 || previous == 0) return true;
    if (timestamp == previous) return false;
    return static_cast<int32_t>(timestamp - previous) > 0;
}

bool SyncRegistry::AcceptTimestamp(uint16_t playerId, SyncChannel channel, uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers) {
        MarkInvalid();
        return false;
    }

    if (timestamp == 0) {
        ++m_counters.accepted;
        return true;
    }

    PlayerSyncSnapshot& player = m_players[playerId];
    uint32_t* previous = nullptr;
    switch (channel)
    {
        case SyncChannel::OnFoot: previous = &player.lastOnFootTimestamp; break;
        case SyncChannel::Aim: previous = &player.lastAimTimestamp; break;
        case SyncChannel::InCar: previous = &player.lastInCarTimestamp; break;
        case SyncChannel::Passenger: previous = &player.lastPassengerTimestamp; break;
        case SyncChannel::Bullet: previous = &player.lastBulletTimestamp; break;
        case SyncChannel::Spectator: previous = &player.lastSpectatorTimestamp; break;
        case SyncChannel::Unoccupied:
        case SyncChannel::Trailer:
            ++m_counters.accepted;
            return true;
    }

    if (!previous || !IsNewerTimestamp(timestamp, *previous)) {
        ++m_counters.rejectedStale;
        return false;
    }

    *previous = timestamp;
    ++m_counters.accepted;
    return true;
}


bool SyncRegistry::AcceptVehicleTimestamp(uint16_t vehicleId, SyncChannel channel, uint32_t timestamp)
{
    if (vehicleId >= kMaxSyncVehicles) {
        MarkRangeReject();
        return false;
    }
    if (timestamp == 0) {
        ++m_counters.accepted;
        return true;
    }

    VehicleSyncSnapshot& vehicle = m_vehicles[vehicleId];
    uint32_t* previous = nullptr;
    switch (channel)
    {
        case SyncChannel::Unoccupied: previous = &vehicle.lastUnoccupiedTimestamp; break;
        case SyncChannel::Trailer: previous = &vehicle.lastTrailerTimestamp; break;
        default:
            ++m_counters.accepted;
            return true;
    }

    if (!previous || !IsNewerTimestamp(timestamp, *previous)) {
        ++m_counters.rejectedStale;
        return false;
    }
    *previous = timestamp;
    ++m_counters.accepted;
    return true;
}

bool SyncRegistry::AcceptMarkersTimestamp(uint32_t timestamp)
{
    if (timestamp == 0) {
        ++m_counters.accepted;
        return true;
    }
    if (!IsNewerTimestamp(timestamp, m_lastMarkersTimestamp)) {
        ++m_counters.rejectedStale;
        return false;
    }
    m_lastMarkersTimestamp = timestamp;
    ++m_counters.accepted;
    return true;
}

bool SyncRegistry::IsValidVector(const CVector& vector, float maxAbs)
{
    return std::isfinite(vector.x) && std::isfinite(vector.y) && std::isfinite(vector.z) &&
           std::fabs(vector.x) <= maxAbs && std::fabs(vector.y) <= maxAbs && std::fabs(vector.z) <= maxAbs;
}

void SyncRegistry::ClampVector(CVector& vector, float limit)
{
    vector.x = std::max(-limit, std::min(limit, vector.x));
    vector.y = std::max(-limit, std::min(limit, vector.y));
    vector.z = std::max(-limit, std::min(limit, vector.z));
}

bool SyncRegistry::NormalizeQuaternion(float& w, float& x, float& y, float& z)
{
    if (!std::isfinite(w) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z))
        return false;

    const float lenSq = w * w + x * x + y * y + z * z;
    if (!std::isfinite(lenSq) || lenSq < 0.01f || lenSq > 100.0f)
        return false;

    const float invLen = 1.0f / std::sqrt(lenSq);
    w *= invLen;
    x *= invLen;
    y *= invLen;
    z *= invLen;
    return true;
}

bool SyncRegistry::NormalizeOnFoot(BROnFootSyncData& data)
{
    if (!IsValidVector(data.vecPos, 1000000.0f) || !IsValidVector(data.vecMoveSpeed, 1000.0f) ||
        !IsValidVector(data.vecSurfOffsets, 1000000.0f)) {
        MarkInvalid();
        return false;
    }

    if (!NormalizeQuaternion(data.quatw, data.quatx, data.quaty, data.quatz)) {
        MarkInvalid();
        return false;
    }

    ClampVector(data.vecMoveSpeed, 1000.0f);
    data.health = static_cast<uint16_t>(std::min<uint16_t>(data.health, 100u));
    data.armour = static_cast<uint16_t>(std::min<uint16_t>(data.armour, 100u));
    return true;
}

bool SyncRegistry::NormalizeInCar(BRInCarSyncData& data)
{
    if (data.VehicleID >= kMaxSyncVehicles || !IsValidVector(data.vecPos, 1000000.0f) ||
        !IsValidVector(data.vecMoveSpeed, 1000.0f) || !std::isfinite(data.fCarHealth)) {
        MarkInvalid();
        return false;
    }

    if (!NormalizeQuaternion(data.quatw, data.quatx, data.quaty, data.quatz)) {
        MarkInvalid();
        return false;
    }

    ClampVector(data.vecMoveSpeed, 1000.0f);
    data.fCarHealth = std::max(0.0f, std::min(10000.0f, data.fCarHealth));
    data.playerHealth = static_cast<uint16_t>(std::min<uint16_t>(data.playerHealth, 100u));
    data.playerArmour = static_cast<uint16_t>(std::min<uint16_t>(data.playerArmour, 100u));
    return true;
}

void SyncRegistry::RecordOnFoot(uint16_t playerId, const BROnFootSyncData& data, uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers) return;
    PlayerSyncSnapshot& player = m_players[playerId];
    player.active = true;
    player.position = data.vecPos;
    player.moveSpeed = data.vecMoveSpeed;
    player.quatw = data.quatw;
    player.quatx = data.quatx;
    player.quaty = data.quaty;
    player.quatz = data.quatz;
    player.health = static_cast<uint8_t>(std::min<uint16_t>(data.health, 100u));
    player.armour = static_cast<uint8_t>(std::min<uint16_t>(data.armour, 100u));
    player.weapon = data.byteCurrentWeapon;
    player.specialAction = data.byteSpecialAction;
    if (timestamp) player.lastOnFootTimestamp = timestamp;
}

void SyncRegistry::RecordInCar(uint16_t playerId, const BRInCarSyncData& data, uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers || data.VehicleID >= kMaxSyncVehicles) return;
    PlayerSyncSnapshot& player = m_players[playerId];
    VehicleSyncSnapshot& vehicle = m_vehicles[data.VehicleID];
    player.active = true;
    player.vehicleId = data.VehicleID;
    player.position = data.vecPos;
    player.moveSpeed = data.vecMoveSpeed;
    player.health = static_cast<uint8_t>(std::min<uint16_t>(data.playerHealth, 100u));
    player.armour = static_cast<uint8_t>(std::min<uint16_t>(data.playerArmour, 100u));
    player.weapon = data.byteCurrentWeapon;
    vehicle.active = true;
    vehicle.position = data.vecPos;
    vehicle.moveSpeed = data.vecMoveSpeed;
    vehicle.vehicleHealth = std::max(0.0f, std::min(10000.0f, data.fCarHealth));
    vehicle.playerHealth = static_cast<uint8_t>(std::min<uint16_t>(data.playerHealth, 100u));
    vehicle.playerArmour = static_cast<uint8_t>(std::min<uint16_t>(data.playerArmour, 100u));
    vehicle.currentWeapon = data.byteCurrentWeapon;
    vehicle.siren = data.byteSirenOn != 0;
    vehicle.landingGear = data.byteLandingGearState != 0;
    vehicle.trailerId = data.TrailerID;
    if (timestamp) {
        player.lastInCarTimestamp = timestamp;
        vehicle.lastInCarTimestamp = timestamp;
    }
}

void SyncRegistry::RecordPassenger(uint16_t playerId, const uint8_t payload[24], uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers || !payload) return;
    PlayerSyncSnapshot& player = m_players[playerId];
    player.active = true;
    std::memcpy(&player.vehicleId, payload, sizeof(player.vehicleId));
    player.seatId = static_cast<uint8_t>(payload[2] & 0x3F);
    player.driveBy = (payload[2] & 0x40) != 0;
    player.cuffed = (payload[2] & 0x80) != 0;
    player.weapon = static_cast<uint8_t>(payload[3] & 0x3F);
    player.health = payload[4];
    player.armour = payload[5];
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    std::memcpy(&posX, payload + 12, sizeof(posX));
    std::memcpy(&posY, payload + 16, sizeof(posY));
    std::memcpy(&posZ, payload + 20, sizeof(posZ));
    player.position = CVector(posX, posY, posZ);
    if (!IsValidVector(player.position, 1000000.0f)) {
        MarkInvalid();
        return;
    }
    if (timestamp) player.lastPassengerTimestamp = timestamp;
}

void SyncRegistry::RecordAim(uint16_t playerId, const uint8_t payload[31], uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers || !payload) return;
    std::memcpy(m_players[playerId].aim.data(), payload, 31);
    m_players[playerId].active = true;
    if (timestamp) m_players[playerId].lastAimTimestamp = timestamp;
}

void SyncRegistry::RecordBullet(uint16_t playerId, const uint8_t payload[40], uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers || !payload) return;
    std::memcpy(m_players[playerId].bullet.data(), payload, 40);
    m_players[playerId].active = true;
    if (timestamp) m_players[playerId].lastBulletTimestamp = timestamp;
}

void SyncRegistry::RecordTrailer(uint16_t playerId, uint16_t trailerId, const uint8_t payload[54], uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers || !payload) return;
    PlayerSyncSnapshot& player = m_players[playerId];
    player.active = true;
    player.vehicleId = trailerId;
    if (trailerId < kMaxSyncVehicles) {
        VehicleSyncSnapshot& vehicle = m_vehicles[trailerId];
        vehicle.active = true;
        vehicle.trailerId = trailerId;
        if (timestamp) vehicle.lastTrailerTimestamp = timestamp;
        // Keep the complete trailer payload for later render/vehicle backends.
        std::memcpy(vehicle.trailer.data(), payload, 54u);
    }
}

void SyncRegistry::RecordUnoccupied(uint16_t playerId, uint16_t vehicleId, const uint8_t payload[67], uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers || vehicleId >= kMaxSyncVehicles || !payload) return;
    VehicleSyncSnapshot& vehicle = m_vehicles[vehicleId];
    vehicle.active = true;
    vehicle.lastUnoccupiedTimestamp = timestamp;
    std::memcpy(vehicle.unoccupied.data(), payload, 67);
}

void SyncRegistry::RecordTurnlights(uint16_t vehicleId, bool left, bool right, bool hazard)
{
    if (vehicleId >= kMaxSyncVehicles) {
        MarkRangeReject();
        return;
    }
    VehicleSyncSnapshot& vehicle = m_vehicles[vehicleId];
    vehicle.active = true;
    vehicle.leftSignal = left;
    vehicle.rightSignal = right;
    vehicle.hazard = hazard;
}

void SyncRegistry::RecordSpectator(uint16_t playerId, const uint8_t payload[18], uint32_t timestamp)
{
    if (playerId >= kMaxSyncPlayers || !payload) return;
    PlayerSyncSnapshot& player = m_players[playerId];
    player.active = true;
    player.activeSpectator = true;
    std::memcpy(player.spectator.data(), payload, 18);
    if (timestamp) player.lastSpectatorTimestamp = timestamp;
}

void SyncRegistry::SetMarkers(const std::array<MarkerSnapshot, kMaxSyncPlayers>& markers)
{
    m_markers = markers;
}

void SyncRegistry::MarkInvalid()
{
    ++m_counters.rejectedInvalid;
}

void SyncRegistry::MarkRangeReject()
{
    ++m_counters.rejectedRange;
}

const PlayerSyncSnapshot* SyncRegistry::GetPlayer(uint16_t playerId) const
{
    return playerId < kMaxSyncPlayers && m_players[playerId].active ? &m_players[playerId] : nullptr;
}

const VehicleSyncSnapshot* SyncRegistry::GetVehicle(uint16_t vehicleId) const
{
    return vehicleId < kMaxSyncVehicles && m_vehicles[vehicleId].active ? &m_vehicles[vehicleId] : nullptr;
}

const MarkerSnapshot* SyncRegistry::GetMarker(uint16_t playerId) const
{
    return playerId < kMaxSyncPlayers && m_markers[playerId].active ? &m_markers[playerId] : nullptr;
}
}
