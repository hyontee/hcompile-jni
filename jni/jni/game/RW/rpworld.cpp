//
// Created by admin on 29.12.2023.
//
#include "../../main.h"
#include "rpworld.h"

RpClump* RpClumpStreamRead(RwStream* stream) {
    return ((RpClump*(__cdecl *)(RwStream*))(g_libGTASA + 0x1E1E2C + 1))(stream);
}

RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* data) {
    return ((RpClump*(__cdecl *)(RpClump*, RpAtomicCallBack, void*))(g_libGTASA + 0x1E0EA0 + 1))(clump, callback, data);
}

RpAtomic* RpAtomicSetFrame(RpAtomic* atomic, RwFrame* frame) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*, RwFrame*))(g_libGTASA + 0x1E1A2C + 1))(atomic, frame);
}