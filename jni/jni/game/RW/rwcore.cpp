//
// Created by admin on 29.12.2023.
//

#include "../../main.h"
#include "rwcore.h"

RwFrame* RwFrameUpdateObjects(RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(RwFrame*))(g_libGTASA + 0x1AEB1C + 1))(frame);
}