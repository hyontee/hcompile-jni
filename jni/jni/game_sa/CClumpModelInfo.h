#pragma once
#include "CBaseModelInfo.h"

struct CClumpModelInfo : public CBaseModelInfo {
    union {
        char *m_animFileName;
        unsigned int m_dwAnimFileIndex;
    };
};
// 0x3C