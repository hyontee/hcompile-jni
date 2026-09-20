#include "br_features.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace
{
    float ClampFloat(float value, float minValue, float maxValue)
    {
        if (!std::isfinite(value)) return minValue;
        return std::max(minValue, std::min(maxValue, value));
    }

    std::string EscapeJsonString(const char* value)
    {
        std::string out;
        if (!value) return out;
        for (const unsigned char* p = reinterpret_cast<const unsigned char*>(value); *p; ++p)
        {
            switch (*p)
            {
                case '"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default: out.push_back(static_cast<char>(*p)); break;
            }
        }
        return out;
    }
}

namespace br
{
FeatureState& FeatureState::Instance()
{
    static FeatureState state;
    return state;
}

void FeatureState::Reset()
{
    for (MapIconState& icon : m_mapIcons) icon = MapIconState{};
    for (PickupState& pickup : m_pickups) pickup = PickupState{};
    m_waypoint = WaypointState{};
    m_checkpoint = CheckpointState{};
    m_fuel = FuelState{};
}

void FeatureState::SetMapIcon(uint8_t id, const CVector& position, uint8_t type, uint32_t color, uint8_t style)
{
    if (id >= m_mapIcons.size()) return;
    MapIconState& icon = m_mapIcons[id];
    icon.active = true;
    icon.iconId = id;
    icon.position = position;
    icon.type = type;
    icon.color = color;
    icon.style = style;
}

void FeatureState::RemoveMapIcon(uint8_t id)
{
    if (id < m_mapIcons.size()) m_mapIcons[id] = MapIconState{};
}

void FeatureState::SetPickup(uint16_t id, int32_t model, const CVector& position, int32_t pickupType)
{
    if (id >= m_pickups.size()) return;
    PickupState& pickup = m_pickups[id];
    pickup.active = true;
    pickup.id = id;
    pickup.model = model;
    pickup.position = position;
    pickup.pickupType = pickupType;
}

void FeatureState::RemovePickup(uint16_t id)
{
    if (id < m_pickups.size()) m_pickups[id] = PickupState{};
}

void FeatureState::SetWaypoint(const CVector& position)
{
    m_waypoint.active = true;
    m_waypoint.position.x = position.x;
    m_waypoint.position.y = position.y;
    m_waypoint.position.z = position.z;
}

void FeatureState::ClearWaypoint()
{
    m_waypoint = WaypointState{};
}

void FeatureState::SetCheckpoint(const CVector& position, float size)
{
    m_checkpoint.active = true;
    m_checkpoint.position = position;
    m_checkpoint.size = ClampFloat(size, 0.0f, 1000000.0f);
}

void FeatureState::ClearCheckpoint()
{
    m_checkpoint = CheckpointState{};
}

void FeatureState::SetFuelState(uint16_t vehicleId, float fuel, float health)
{
    m_fuel.known = true;
    m_fuel.inVehicle = true;
    m_fuel.vehicleId = vehicleId;
    m_fuel.fuel = ClampFloat(fuel, 0.0f, 1000000.0f);
    m_fuel.vehicleHealth = ClampFloat(health, 0.0f, 1000000.0f);
}

void FeatureState::SetVehicleHealth(uint16_t vehicleId, float health)
{
    m_fuel.vehicleId = vehicleId;
    m_fuel.inVehicle = true;
    m_fuel.vehicleHealth = ClampFloat(health, 0.0f, 1000000.0f);
}

void FeatureState::ClearVehicle()
{
    m_fuel.inVehicle = false;
    m_fuel.vehicleId = 0xFFFF;
}

const MapIconState* FeatureState::GetMapIcon(uint8_t id) const
{
    return id < m_mapIcons.size() && m_mapIcons[id].active ? &m_mapIcons[id] : nullptr;
}

const PickupState* FeatureState::GetPickup(uint16_t id) const
{
    return id < m_pickups.size() && m_pickups[id].active ? &m_pickups[id] : nullptr;
}

std::string FeatureState::MakeFuelMenuEvent(uint16_t vehicleId, float fuel, float pricePerLiter)
{
    char buf[256];
    std::snprintf(buf, sizeof(buf),
        "{\"type\":\"vehicle_fuel\",\"action\":\"open\",\"vehicleId\":%u,\"fuel\":%.2f,\"pricePerLiter\":%.2f}",
        static_cast<unsigned>(vehicleId),
        static_cast<double>(ClampFloat(fuel, 0.0f, 1000000.0f)),
        static_cast<double>(ClampFloat(pricePerLiter, 0.0f, 1000000.0f)));
    return std::string(buf);
}

std::string FeatureState::MakeFuelActionEvent(const char* action, float liters)
{
    const std::string escaped = EscapeJsonString(action ? action : "cancel");
    char buf[192];
    std::snprintf(buf, sizeof(buf),
        "{\"type\":\"vehicle_fuel\",\"action\":\"%s\",\"liters\":%.2f}",
        escaped.c_str(),
        static_cast<double>(ClampFloat(liters, 0.0f, 1000000.0f)));
    return std::string(buf);
}
}
