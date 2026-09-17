#ifndef IMAGELOADER_H
#define IMAGELOADER_H

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
#include "..//util/patch.h"
struct tStreamingFileDesc {
    tStreamingFileDesc() = default;

    tStreamingFileDesc(const char* name, bool bNotPlayerImg) :
            m_bNotPlayerImg(bNotPlayerImg),
            m_StreamHandle(CHook::CallFunction<int32_t>(g_libGTASA + 0x00289BA0 + 1, name))
    {
        strncpy(m_szName, name, size_t(m_szName));
    }

    [[nodiscard]] bool IsInUse() const noexcept { return m_szName[0]; }

    char  m_szName[40]{}; // If this string is empty (eg.: first elem in array is NULL) the entry isnt in use
    bool  m_bNotPlayerImg{};
    uint8_t pad[3];
    int32_t m_StreamHandle{-1};
};
struct tRwStreamInitializeData {
    uint8_t* m_pBuffer;
    uint32_t m_uiBufferSize;
};

class ImageLoader {

public:
    static int AddImageToList(const char *name, bool b);

    static void RemoveAllUnusedModels();

    // static void LoadStreamingCfg();

    static inline tStreamingFileDesc ms_files[12];

    static void RequestModel(int32_t modelId, int32_t flags = 0x2);

    static void LoadAllRequestedModels(bool bPriorityRequestsOnly);

    static void RemoveModel(int index);

    static void InitImageList();

    static void InjectHooks();

    static void RemoveBuildingsNotInArea(int areaCode);

    static void EncryptImage();

    static const char* key;
};


#endif //IMAGELOADER_H