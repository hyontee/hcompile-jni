//
// Created by weikton on 22.06.2025.
//

#pragma once
#define ES2_ADDR 0x5CF5B8 // 1.08

#include <GLES3/gl32.h>
#include "RQVertexBuffer.h"

struct ES2VertexBuffer : RQVertexBuffer
{
    unsigned int    size;
    GLuint          bufferId;
public:
    static inline const void *curCPUBuffer;

public:
    static void InjectHooks();
};
static_assert(sizeof(ES2VertexBuffer) == 0xc);




