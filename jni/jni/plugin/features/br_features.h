#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "game/math/vector.h"

namespace br
{
struct MapIconState
{
    bool active = false;
    uint8_t iconId = 0;
    uint8_t type = 0;
    uint8_t style = 0;
    uint32_t color = 0;
    CVector position{};
};

struct PickupState
{
    bool active = false;
    uint16_t id = 0;
    int32_t model = 0;
    int32_t pickupType = 0;
    CVector position{};
};

struct WaypointState
{
    bool active = false;
    CVector position{};
};

struct CheckpointState
{
    bool active = false;
    CVector position{};
    float size = 0.0f;
};

struct FuelState
{
    bool known = false;
    bool inVehicle = false;
    uint16_t vehicleId = 0xFFFF;
    float fuel = 0.0f;
    float vehicleHealth = 0.0f;
};

class FeatureState
{
public:
    static FeatureState& Instance();

    void Reset();

    void SetMapIcon(uint8_t id, const CVector& position, uint8_t type, uint32_t color, uint8_t style);
    void RemoveMapIcon(uint8_t id);
    void SetPickup(uint16_t id, int32_t model, const CVector& position, int32_t pickupType);
    void RemovePickup(uint16_t id);
    void SetWaypoint(const CVector& position);
    void ClearWaypoint();
    void SetCheckpoint(const CVector& position, float size);
    void ClearCheckpoint();

    void SetFuelState(uint16_t vehicleId, float fuel, float health);
    void SetVehicleHealth(uint16_t vehicleId, float health);
    void ClearVehicle();

    const MapIconState* GetMapIcon(uint8_t id) const;
    const PickupState* GetPickup(uint16_t id) const;
    const WaypointState& GetWaypoint() const { return m_waypoint; }
    const CheckpointState& GetCheckpoint() const { return m_checkpoint; }
    const FuelState& GetFuel() const { return m_fuel; }

    // JSON envelope used by the existing BR GUI bridge. It does not invent a
    // new renderer; the stock client's GUI layer remains responsible for drawing.
    static std::string MakeFuelMenuEvent(uint16_t vehicleId, float fuel, float pricePerLiter);
    static std::string MakeFuelActionEvent(const char* action, float liters);

private:
    FeatureState() = default;

    std::vector<MapIconState> m_mapIcons = std::vector<MapIconState>(100);
    std::vector<PickupState> m_pickups = std::vector<PickupState>(4096);
    WaypointState m_waypoint{};
    CheckpointState m_checkpoint{};
    FuelState m_fuel{};
};
}
