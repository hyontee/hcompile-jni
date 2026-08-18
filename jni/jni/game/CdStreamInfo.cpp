//
// Created by plaka on 19.04.2023.
//

#include "CdStreamInfo.h"
#include "patch.h"


int32 CdStreamOpen(const char* lpFileName) {
    //return CHook::CallFunction<int32_t>(g_libGTASA + 0x00289BA0 + 1, lpFileName);
    return CHook::CallFunction<int32_t>("_Z12CdStreamOpenPKcb", lpFileName);
}