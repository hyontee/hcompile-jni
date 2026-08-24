//
// Created by admin on 06.01.2024.
//

#pragma once

#include "CompressedVector.h"
#include "CNodeAddress.h"

class CPathNode {
public:
    CPathNode        *m_next, *m_prev;
    CompressedVector m_vPos;
    int16            m_totalDistFromOrigin; /// Sum of linkLength's up to this node. Using this the current hash bucket (in `m_pathFindHashTable`) can be obtained by `% std::size(m_pathFindHashTable)`. Used in `DoPathSearch`. `SHRT_MAX - 1` by default.
    int16            m_wBaseLinkId;
    uint16           m_wAreaId; // TODO: Replace these 2 with `CNodeAddress`
    uint16           m_wNodeId;
    uint8            m_nPathWidth; // Fixed-point float, divide by 16
    uint8            m_nFloodFill;

    // byte 0
    uint32 m_nNumLinks : 4; // Mask: 0xF
    uint32 m_onDeadEnd : 1;
    uint32 m_isSwitchedOff : 1;
    uint32 m_bRoadBlocks : 1;
    uint32 m_bWaterNode : 1;

    // byte 1
    uint32 m_isOriginallyOnDeadEnd : 1;
    uint32 unk1 : 1; // not used in paths data files
    uint32 m_bDontWander : 1;
    uint32 unk2 : 1; // not used in paths data files
    uint32 m_bNotHighway : 1;
    uint32 m_bHighway : 1;
    uint32 unk3 : 1; // not used in paths data files
    uint32 unk4 : 1; // not used in paths data files

    // byte 2
    uint32 m_nSpawnProbability : 4;
    uint32 m_nBehaviourType : 4; // 1 - roadblock
    // 2 - parking node
    // byte 3
    char unused;
public:
    static void InjectHooks();

    /// Get uncompressed world position
    CVector GetNodeCoors() const {
        return UncompressLargeVector(m_vPos);
    }

    CNodeAddress GetAddress() const {
        return { m_wAreaId, m_wNodeId };
    }

    friend bool operator==(const CPathNode& lhs, const CPathNode& rhs) { return lhs.GetAddress() == rhs.GetAddress(); }
    friend bool operator!=(const CPathNode& lhs, const CPathNode& rhs) { return !(lhs == rhs); }

    /*!
    * @notsa
    * @brief Code based on 0x44D3E0
    */
    bool HasToBeSwitchedOff() const {
        switch (m_nBehaviourType) {
            case 1:
            case 2:
                return false;
            default:
                return true;
        }
    }
};
VALIDATE_SIZE(CPathNode, 0x1C);
