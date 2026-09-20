#pragma once

#include <array>
#include <cstdint>

#include "game/math/vector.h"
#include "plugin/common.h"

namespace br
{
constexpr uint16_t kMaxSyncPlayers = 1504;
constexpr uint16_t kMaxSyncVehicles = 2000;

enum class SyncChannel : uint8_t
{
    OnFoot = 0,
    Aim,
    InCar,
    Passenger,
    Bullet,
    Unoccupied,
    Trailer,
    Spectator,
};

struct PlayerSyncSnapshot
{
    bool active = false;
    uint32_t lastOnFootTimestamp = 0;
    uint32_t lastAimTimestamp = 0;
    uint32_t lastInCarTimestamp = 0;
    uint32_t lastPassengerTimestamp = 0;
    uint32_t lastBulletTimestamp = 0;
    uint32_t lastSpectatorTimestamp = 0;

    CVector position{};
    CVector moveSpeed{};
    float quatw = 1.0f;
    float quatz = 0.0f;
    float quatx = 0.0f;
    float quaty = 0.0f;
    uint8_t health = 0;
    uint8_t armour = 0;
    uint8_t weapon = 0;
    uint8_t specialAction = 0;
    uint16_t vehicleId = 0xFFFF;
    uint8_t seatId = 0xFF;
    bool driveBy = false;
    bool cuffed = false;
    bool activeSpectator = false;

    std::array<uint8_t, 31> aim{};
    std::array<uint8_t, 40> bullet{};
    std::array<uint8_t, 18> spectator{};
};

struct VehicleSyncSnapshot
{
    bool active = false;
    uint32_t lastInCarTimestamp = 0;
    uint32_t lastUnoccupiedTimestamp = 0;
    uint32_t lastTrailerTimestamp = 0;

    CVector position{};
    CVector moveSpeed{};
    float vehicleHealth = 0.0f;
    uint8_t playerHealth = 0;
    uint8_t playerArmour = 0;
    uint8_t currentWeapon = 0;
    bool siren = false;
    bool landingGear = false;
    bool leftSignal = false;
    bool rightSignal = false;
    bool hazard = false;
    uint16_t trailerId = 0xFFFF;

    std::array<uint8_t, 67> unoccupied{};
    std::array<uint8_t, 54> trailer{};
};

struct MarkerSnapshot
{
    bool active = false;
    CVector position{};
    uint32_t lastTimestamp = 0;
};

struct SyncCounters
{
    uint64_t accepted = 0;
    uint64_t rejectedStale = 0;
    uint64_t rejectedInvalid = 0;
    uint64_t rejectedRange = 0;
};

class SyncRegistry
{
public:
    static SyncRegistry& Instance();

    void Reset();

    bool AcceptTimestamp(uint16_t playerId, SyncChannel channel, uint32_t timestamp);
    bool AcceptVehicleTimestamp(uint16_t vehicleId, SyncChannel channel, uint32_t timestamp);
    bool AcceptMarkersTimestamp(uint32_t timestamp);
    void RecordOnFoot(uint16_t playerId, const BROnFootSyncData& data, uint32_t timestamp);
    void RecordInCar(uint16_t playerId, const BRInCarSyncData& data, uint32_t timestamp);
    void RecordPassenger(uint16_t playerId, const uint8_t payload[24], uint32_t timestamp);
    void RecordAim(uint16_t playerId, const uint8_t payload[31], uint32_t timestamp);
    void RecordBullet(uint16_t playerId, const uint8_t payload[40], uint32_t timestamp);
    void RecordTrailer(uint16_t playerId, uint16_t trailerId, const uint8_t payload[54], uint32_t timestamp);
    void RecordUnoccupied(uint16_t playerId, uint16_t vehicleId, const uint8_t payload[67], uint32_t timestamp);
    void RecordSpectator(uint16_t playerId, const uint8_t payload[18], uint32_t timestamp);
    void RecordTurnlights(uint16_t vehicleId, bool left, bool right, bool hazard);
    void SetMarkers(const std::array<MarkerSnapshot, kMaxSyncPlayers>& markers);

    bool NormalizeOnFoot(BROnFootSyncData& data);
    bool NormalizeInCar(BRInCarSyncData& data);

    void MarkInvalid();
    void MarkRangeReject();

    const PlayerSyncSnapshot* GetPlayer(uint16_t playerId) const;
    const VehicleSyncSnapshot* GetVehicle(uint16_t vehicleId) const;
    const MarkerSnapshot* GetMarker(uint16_t playerId) const;
    const SyncCounters& Counters() const { return m_counters; }

private:
    static bool IsNewerTimestamp(uint32_t timestamp, uint32_t previous);
    static bool IsValidVector(const CVector& vector, float maxAbs);
    static bool NormalizeQuaternion(float& w, float& x, float& y, float& z);
    static void ClampVector(CVector& vector, float limit);

    std::array<PlayerSyncSnapshot, kMaxSyncPlayers> m_players{};
    std::array<VehicleSyncSnapshot, kMaxSyncVehicles> m_vehicles{};
    std::array<MarkerSnapshot, kMaxSyncPlayers> m_markers{};
    SyncCounters m_counters{};
    uint32_t m_lastMarkersTimestamp = 0;
};
}
