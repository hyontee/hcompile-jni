//
// Created by Weikton
//
#ifndef STREAMING_H
#define STREAMING_H

#include <cstdint>
#include <cassert>
#include "../constants.h"

enum class eChannelState
{
    IDLE = 0,
    READING = 1,
    STARTED = 2,
    ERR = 3,
};

enum eResourceFirstID : int32_t {
    // First ID of the resource
    RESOURCE_ID_DFF                = 0,                                     // default: 0
    RESOURCE_ID_TXD                = RESOURCE_ID_DFF + TOTAL_DFF_MODEL_IDS, // default: 20000
    RESOURCE_ID_COL                = RESOURCE_ID_TXD + TOTAL_TXD_MODEL_IDS, // default: 25000
    RESOURCE_ID_IPL                = RESOURCE_ID_COL + TOTAL_COL_MODEL_IDS, // default: 25255
    RESOURCE_ID_DAT                = RESOURCE_ID_IPL + TOTAL_IPL_MODEL_IDS, // default: 25511
    RESOURCE_ID_IFP                = RESOURCE_ID_DAT + TOTAL_DAT_MODEL_IDS, // default: 25575
    RESOURCE_ID_RRR                = RESOURCE_ID_IFP + TOTAL_IFP_MODEL_IDS, // default: 25755   (vehicle recordings)
    RESOURCE_ID_SCM                = RESOURCE_ID_RRR + TOTAL_RRR_MODEL_IDS, // default: 26230   (streamed scripts)

    // Used for CStreaming lists, just search for xrefs (VS: shift f12)
    RESOURCE_ID_LOADED_LIST_START  = RESOURCE_ID_SCM + TOTAL_SCM_MODEL_IDS, // default: 26312
    RESOURCE_ID_LOADED_LIST_END    = RESOURCE_ID_LOADED_LIST_START + 1,     // default: 26313

    RESOURCE_ID_REQUEST_LIST_START = RESOURCE_ID_LOADED_LIST_END + 1,       // default: 26314
    RESOURCE_ID_REQUEST_LIST_END   = RESOURCE_ID_REQUEST_LIST_START + 1,    // default: 26315
    RESOURCE_ID_TOTAL                                               // default: 26316
};

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

// Helper functions to deal with modelID's


inline bool IsModelDFF(int32_t model) { return RESOURCE_ID_DFF <= model && model < RESOURCE_ID_TXD; }
inline bool IsModelTXD(int32_t model) { return RESOURCE_ID_TXD <= model && model < RESOURCE_ID_COL; }
inline bool IsModelCOL(int32_t model) { return RESOURCE_ID_COL <= model && model < RESOURCE_ID_IPL; }
inline bool IsModelIPL(int32_t model) { return RESOURCE_ID_IPL <= model && model < RESOURCE_ID_DAT; }
inline bool IsModelDAT(int32_t model) { return RESOURCE_ID_DAT <= model && model < RESOURCE_ID_IFP; }
inline bool IsModelIFP(int32_t model) { return RESOURCE_ID_IFP <= model && model < RESOURCE_ID_RRR; }
inline bool IsModelRRR(int32_t model) { return RESOURCE_ID_RRR <= model && model < RESOURCE_ID_SCM; }
inline bool IsModelSCM(int32_t model) { return RESOURCE_ID_SCM <= model && model < RESOURCE_ID_LOADED_LIST_START; }

inline eModelType GetModelType(int32_t model) {
    if (IsModelDFF(model))
        return eModelType::DFF;

    else if (IsModelTXD(model))
        return eModelType::TXD;

    else if (IsModelCOL(model))
        return eModelType::COL;

    else if (IsModelIPL(model))
        return eModelType::IPL;

    else if (IsModelDAT(model))
        return eModelType::DAT;

    else if (IsModelIFP(model))
        return eModelType::IFP;

    else if (IsModelRRR(model))
        return eModelType::RRR;

    else if (IsModelSCM(model))
        return eModelType::SCM;

    else {
        assert(0); // NOTSA
        return (eModelType)-1;
    }
}

// Turn relative IDs into absolute ones.
inline int32_t DFFToModelId(int32_t relativeId) { return RESOURCE_ID_DFF + relativeId; }
inline int32_t TXDToModelId(int32_t relativeId) { return RESOURCE_ID_TXD + relativeId; }
inline int32_t COLToModelId(int32_t relativeId) { return RESOURCE_ID_COL + relativeId; }
inline int32_t IPLToModelId(int32_t relativeId) { return RESOURCE_ID_IPL + relativeId; }
inline int32_t DATToModelId(size_t relativeId) { return (size_t)RESOURCE_ID_DAT + relativeId; }
inline int32_t IFPToModelId(int32_t relativeId) { return RESOURCE_ID_IFP + relativeId; }
inline int32_t RRRToModelId(int32_t relativeId) { return RESOURCE_ID_RRR + relativeId; }
inline int32_t SCMToModelId(int32_t relativeId) { return RESOURCE_ID_SCM + relativeId; }

// Turn absolute IDs into relative ones
inline int32_t ModelIdToDFF(int32_t absId) { return absId - RESOURCE_ID_DFF; }
inline int32_t ModelIdToTXD(int32_t absId) { return absId - RESOURCE_ID_TXD; }
inline int32_t ModelIdToCOL(int32_t absId) { return absId - RESOURCE_ID_COL; }
inline int32_t ModelIdToIPL(int32_t absId) { return absId - RESOURCE_ID_IPL; }
inline int32_t ModelIdToDAT(int32_t absId) { return absId - RESOURCE_ID_DAT; }
inline int32_t ModelIdToIFP(int32_t absId) { return absId - RESOURCE_ID_IFP; }
inline int32_t ModelIdToRRR(int32_t absId) { return absId - RESOURCE_ID_RRR; }
inline int32_t ModelIdToSCM(int32_t absId) { return absId - RESOURCE_ID_SCM; }

struct tRwStreamInitializeData {
    uint8_t* m_pBuffer;
    uint32_t m_uiBufferSize;
};

class CStreaming {

public:
    static int AddImageToList(const char *name, bool b);

    static void RemoveAllUnusedModels();

    static void RequestModel(int index, int flags);

    static void LoadAllRequestedModels(bool bPriorityRequestsOnly);

    static void RemoveModel(int index);
};


#endif //STREAMING_H
