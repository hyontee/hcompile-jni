//
// Created by admin on 29.12.2023.
//

#include "RwHelper.h"
#include "RenderWare.h"
#include "../../main.h"

RpAtomic* GetFirstAtomicCallback(RpAtomic* atomic, void* data) {
    *(RpAtomic**)(data) = atomic;
    return nullptr;
}

RpAtomic* GetFirstAtomic(RpClump* clump) {
    RpAtomic* atomic{};
    RpClumpForAllAtomics(clump, GetFirstAtomicCallback, &atomic);
    return atomic;
}