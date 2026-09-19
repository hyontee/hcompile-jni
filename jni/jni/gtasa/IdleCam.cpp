//
// Created by weikton on 22.06.2025.
//

#include "IdleCam.h"
#include "util/patch.h"

CIdleCam gIdleCam;
void CIdleCam::InjectHooks() {
    CHook::Write(g_libGTASA + IDLE_ADDR, &gIdleCam);
}
