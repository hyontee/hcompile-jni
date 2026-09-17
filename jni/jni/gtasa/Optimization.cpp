#include "Optimization.h"
#include <cstdint>
#include "../util/armhook.h"
#include "../main.h"
#include "../util/patch.h"
#include "GLES.h"

void Optimization::ApplyNewPatches()
{
    // path patches
    WriteMemory(g_libGTASA + 0x5AC280, (uintptr_t)"utils\\effects.fxp\0", 19);
    WriteMemory(g_libGTASA + 0x5BAD10, (uintptr_t)"utils\\coll\\peds.col\0", 22);
    WriteMemory(g_libGTASA + 0x596954, (uintptr_t)"config.psf\0", 12);
    WriteMemory(g_libGTASA + 0x594F4C, (uintptr_t)"multiplayer\\adjustable.cfg\0", 32);
    NOP(g_libGTASA + 0x26D9CC, 2);
    NOP(g_libGTASA + 0x33FDB6, 2);

    // weather patches
    WriteMemory(g_libGTASA + 0x52DD38, (uintptr_t)"\x00\x20\x70\x47", 4);
    NOP(g_libGTASA + 0x39Ad14, 1);

    // matrix patches
    *(float*)(g_libGTASA + 0x608558) = 200.0f;

    // crash when folding
    NOP(g_libGTASA + 0x0056CD4A, 2); // 0x56CD4A

    // Prevent cheats processing
    NOP(g_libGTASA + 0x3987BA, 2);

    // Draw Distance Fix
    UnFuck(g_libGTASA + 0x3BDDF4);
    *(uint32_t*)(g_libGTASA + 0x3BDDF4) = 0x41C80000;

    UnFuck(g_libGTASA + 0x3BE2D0);
    *(uint32_t*)(g_libGTASA + 0x3BE2D0) = 0x40A00000;

    // Huawei (Y7) crash fix
    if (!*(uintptr_t*)(g_libGTASA + 0x61B298)) {
        NOP(g_libGTASA + 0x1A61, 4);
        *(uintptr_t*)(g_libGTASA + 0x61B298) = ((uintptr_t(*)(const char*))(g_libGTASA + 0x179A20))("glAlphaFuncQCOM");
    }

    if (!*(uintptr_t*)(g_libGTASA + 0x61B298)) {
        *(uintptr_t*)(g_libGTASA + 0x61B298) = ((uintptr_t(*)(const char*))(g_libGTASA + 0x179A20))("glAlphaFunc");
    }

    if (!*(uintptr_t*)(g_libGTASA + 0x61B298)) {
        Log("crash 0x61B298!!!");
    }

    // fix black roads
    WriteMemory(g_libGTASA + 0x1A69F8, (uintptr_t)"\x42\xF2\x05\x72", 4); // movw r2, #0x2705
    WriteMemory(g_libGTASA + 0x1A6A1E, (uintptr_t)"\x42\xF2\x05\x72", 4); // movw r2, #0x2705
    WriteMemory(g_libGTASA + 0x1A6A10, (uintptr_t)"\x42\xF2\x05\x72", 4); // movw r2, #0x2705
    WriteMemory(g_libGTASA + 0x1A69E8, (uintptr_t)"\x42\xF2\x05\x72", 4); // movw r2, #0x2705

    // br
    NOP(g_libGTASA + 0x56C342, 2);
    NOP(g_libGTASA + 0x4BE6FE, 4);
    NOP(g_libGTASA + 0x339240, 7);
    NOP(g_libGTASA + 0x39B1E4, 2);

    NOP(g_libGTASA + 0x1BDA64, 3);
    NOP(g_libGTASA + 0x3D373A, 2);
    NOP(g_libGTASA + 0x4E55E2, 2); // dangerous
    NOP(g_libGTASA + 0x519AC8, 2);

    NOP(g_libGTASA + 0x51013E, 1);
    NOP(g_libGTASA + 0x510166, 1);
    NOP(g_libGTASA + 0x4BEBE6, 2);
    NOP(g_libGTASA + 0x39A844 + 0x518, 2);
    NOP(g_libGTASA + 0x39A844 + 0x207E14, 2);

    NOP(g_libGTASA + 0x39A844 + 0x540, 2);
    NOP(g_libGTASA + 0x53E4EE, 2);
    NOP(g_libGTASA + 0x53E35E, 2);
    NOP(g_libGTASA + 0x2467AA, 1);
    NOP(g_libGTASA + 0x4874E0, 5);

    if ( *(uint8_t *)(g_libGTASA + 0x207E58 + 0x130) )
    {
        UnFuck(g_libGTASA + 0x63E3A4);
        *(uint8_t *)((char *)g_libGTASA + 0x63E3A4) = 0x64;
        NOP(g_libGTASA + 0x39B1E4, 2);
    }

    UnFuck(g_libGTASA + 0x5D097C);
    *(uint8_t *)((char *)g_libGTASA + 0x5D097C) = 0x72F31C;

    UnFuck(g_libGTASA + 0x63D4F0);
    *(uint8_t *)((char *)g_libGTASA + 0x63D4F0) = 0x739F00;

    UnFuck(g_libGTASA + 0x5D1E8C);
    *(uint8_t *)((char *)g_libGTASA + 0x5D1E8C) = 0x738F60;

    NOP(g_libGTASA + 0x45477E, 2);
    NOP(g_libGTASA + 0x405398, 0xE);
    NOP(g_libGTASA + 0x3B1AF0, 2);
    NOP(g_libGTASA + 0x556914, 2);

    UnFuck(g_libGTASA + 0x67E1A8);
    *(uint8_t *)(g_libGTASA + 0x67E1A8) = 0;

    UnFuck(g_libGTASA + 0x67E1AC);
    *(uint8_t *)(g_libGTASA + 0x67E1AC) = 0;

    UnFuck(g_libGTASA + 0x67E1B0);
    *(uint8_t *)(g_libGTASA + 0x67E1B0) = 0;

    UnFuck(g_libGTASA + 0x67E1B4);
    *(uint8_t *)(g_libGTASA + 0x67E1B4) = 0;

    UnFuck(g_libGTASA + 0x67E1B8);
    *(uint8_t *)(g_libGTASA + 0x67E1B8) = 0;

    UnFuck(g_libGTASA + 0x67E1BC);
    *(uint8_t *)(g_libGTASA + 0x67E1BC) = 0;
    // end br

    // mordor
    NOP(g_libGTASA + 0x51013E, 1u);
    NOP(g_libGTASA + 0x510166, 1u);
    NOP(g_libGTASA + 0x508F36, 2u);
    NOP(g_libGTASA + 0x508F54, 2u);
    NOP(g_libGTASA + 0x39840A, 2u);

    NOP(g_libGTASA + 0x39844E, 2u);
    NOP(g_libGTASA + 0x39845E, 2u);
    NOP(g_libGTASA + 0x3983C6, 2u);
    NOP(g_libGTASA + 0x4E55E2, 2u);
    NOP(g_libGTASA + 0x39A1D8, 2u);

    NOP(g_libGTASA + 0x4E31A6, 2u);
    NOP(g_libGTASA + 0x4EE7D2, 2u);
    NOP(g_libGTASA + 0x4F741E, 2u);
    NOP(g_libGTASA + 0x50AB4A, 2u);
    NOP(g_libGTASA + 0x3688DA, 2u);

    NOP(g_libGTASA + 0x368DF0, 2u);
    NOP(g_libGTASA + 0x369072, 2u);
    NOP(g_libGTASA + 0x369168, 2u);
    NOP(g_libGTASA + 0x519198 - 0x82, 2u);
    NOP(g_libGTASA + 0x519198 - 0x7C, 4u);

    NOP(g_libGTASA + 0x519198, 4u);
    NOP(g_libGTASA + 0x519198, 2u);
    NOP(g_libGTASA + 0x519198 + 0x38, 4u);
    NOP(g_libGTASA + 0x519198 + 0x44, 2u);
    NOP(g_libGTASA + 0x405398, 0xEu);
    // end mordor

    // lit
    NOP(g_libGTASA + 0x039894E, 2); // SUN AND MOON NOP

    NOP(g_libGTASA + 0x039D43C, 4); // look behind player
    NOP(g_libGTASA + 0x039D436, 4); // look behind car

    // nop fog for skybox
    //NOP(g_libGTASA + 0x0570E5C, 4); // надо только если у нас нету опенгла
}