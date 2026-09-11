//
// Created by admin on 28.12.2023.
//

#pragma once
#include "../main.h"

#pragma pack(push, 1)
class CNodeAddress {
public:
    uint16 m_wAreaId{ (uint16)-1 };
    uint16 m_wNodeId{ (uint16)-1 };

    constexpr CNodeAddress() = default;
    constexpr CNodeAddress(uint16 areaId, uint16 nodeId) : m_wAreaId(areaId), m_wNodeId(nodeId) {}
    constexpr CNodeAddress(uint16 areaId) : m_wAreaId(areaId), m_wNodeId(-1) {}

    bool operator==(CNodeAddress const&) const = default;
    bool operator!=(CNodeAddress const&) const = default;

    void ResetAreaId() { m_wAreaId = UINT16_MAX; }
    void ResetNodeId() { m_wNodeId = UINT16_MAX; }

    [[nodiscard]] bool IsAreaValid() const { return m_wAreaId != (uint16)-1; }
    [[nodiscard]] bool IsValid() const { return IsAreaValid() && m_wNodeId != UINT16_MAX; }

    operator bool() const { return IsValid(); }
};

#pragma pop
VALIDATE_SIZE(CNodeAddress, 0x4);
