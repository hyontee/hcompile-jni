//
// Created by weikton on 22.06.2025.
//

#include "ES2VertexBuffer.h"
#include "util/patch.h"

void ES2VertexBuffer::InjectHooks() {
    CHook::Write(g_libGTASA + ES2_ADDR, &ES2VertexBuffer::curCPUBuffer);
}