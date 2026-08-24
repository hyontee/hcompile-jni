//
// Created by admin on 29.12.2023.
//

#pragma once
#include "main.h"

#pragma pack(push, 1)
class CQueuedMode {
public:
    uint16 m_nMode;
    uint8 _undefined_0;
    uint8 _undefined_1;
    float  m_fDuration;
    uint16 m_nMinZoom;
    uint16 m_nMaxZoom;
};
#pragma pack(pop)

VALIDATE_SIZE(CQueuedMode, 0xC);
