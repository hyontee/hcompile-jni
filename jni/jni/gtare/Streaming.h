//
// Created by @obfuscati0n
//
#ifndef STREAMING_H
#define STREAMING_H

#include <cstdint>
#include <cassert>
#include "constants.h"

enum class eModelType {
    DFF,
    TXD,
    COL,
    IPL,
    DAT,
    IFP,
    RRR,
    SCM,

    INTERNAL_1,
    INTERNAL_2,
    INTERNAL_3,
    INTERNAL_4
};

struct tRwStreamInitializeData {
    uint8_t* m_pBuffer;
    uint32_t m_uiBufferSize;
};

class CStreaming {

public:
    static int AddImageToList(const char *name, bool b);

    static void RemoveAllUnusedModels();

   // static void LoadStreamingCfg();

    static void RequestModel(int index, int flags);

    static void LoadAllRequestedModels(bool bPriorityRequestsOnly);

    static void RemoveModel(int index);
};


#endif //STREAMING_H
