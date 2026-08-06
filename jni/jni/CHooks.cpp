//
// Created by weikton on 22.06.2025.
//
#include "main.h"

#include "customrender/ES2VertexBuffer.h"
#include "gtasa/IdleCam.h"

void InjectHooks()
{
    Log("InjectHooks");

    ES2VertexBuffer::InjectHooks();
    CIdleCam::InjectHooks();
}