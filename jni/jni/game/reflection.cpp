//
// Created by Error on 26.05.2025.
//
#include "../main.h"
#include "game.h"
#include "../util/patch.h"

#include "Scene.h"
#include "CWorld.h"

uintptr_t dword_7BEE74;
uintptr_t dword_7BEE7C;
uintptr_t dword_7BEE78;
uintptr_t dword_7BEE80;
uintptr_t dword_7BEE84;
uintptr_t dword_7BEE88;
uintptr_t dword_7BEE8C;
uintptr_t dword_7BEE90;
uintptr_t dword_7BEE94;
uintptr_t dword_7BEE98;
uintptr_t dword_7BEE9C;
uintptr_t dword_7BEEA0;
uintptr_t dword_7BEEA4;
uintptr_t dword_7BEEA8;
uintptr_t dword_7BEEAC;
uintptr_t dword_7BEEB0;
uintptr_t dword_7BEEB4;
uintptr_t dword_7BEEB8;
uintptr_t dword_7BEEBC;
uintptr_t dword_7BEEC0;
uintptr_t dword_7BEEC4;
uintptr_t dword_7BEEC8;
uintptr_t dword_7BEECC;
uintptr_t dword_7BEED0;
uintptr_t dword_7BEED4;
uintptr_t dword_7BEED8;
uintptr_t dword_7BEEDC;
uintptr_t dword_7BEEE0;
uintptr_t dword_7BEEE4;
uintptr_t dword_7BEEE8;
uintptr_t dword_7BEEEC;
uintptr_t dword_7BEEF0;
uintptr_t dword_7BEEF8;
uintptr_t dword_7BEEF4;
uintptr_t dword_7BEEFC;
uintptr_t dword_7BEF00;
uintptr_t dword_7BEF04;
uintptr_t dword_7BEF08;
uintptr_t dword_7BEF0C;
uintptr_t dword_7BEF10;
uintptr_t dword_7BEF14;
uintptr_t dword_7BEF18;
uintptr_t dword_7BEF20;
uintptr_t dword_7BEF1C;
uintptr_t dword_7BEF24;
uintptr_t dword_7BEF28;
uintptr_t dword_7BEF2C;
uintptr_t dword_7BEF34;
uintptr_t dword_7BEF30;
uintptr_t dword_7BEF3C;
uintptr_t dword_7BEF40;
uintptr_t dword_7BEF48;
uintptr_t dword_7BEF44;
uintptr_t dword_7BEF54;
uintptr_t dword_7BEF50;
uintptr_t dword_7BEF58;
uintptr_t dword_7BEF4C;
uintptr_t dword_7BEF5C;
static uint32_t* dword_207F24 = nullptr;
uintptr_t dword_7BEF60;


extern void sub_55718(int *param1, uint8_t a2, uint8_t a3, uint16_t a4, int a5)
{
    param1[1] = a3;
    *param1 = a2;
    param1[2] = a4;
    param1[3] = a5;
}
extern void sub_5578E(int *a1){
    a1[2] | (a1[1] << 8) | (*a1 << 0x10) | (a1[3] << 0x18);
}

uintptr_t sub_57DE0(){
    return (uintptr_t)CWorld::Players;
}

void dword_7BEF38(int a1, int a2)
{
    return ((void ( *)(int, int))((char *)0x001E2894 + 1 + g_libGTASA))(a1, a2);
}

uintptr_t* dword_609F1C;

constexpr uintptr_t off_1F95D4 = 0x1F95D4;
constexpr uintptr_t unk_1D4EC2 = 0x1D4EC2;
constexpr uintptr_t unk_1D50BA = 0x1D50BA;

void sub_57F64(int a1, uintptr_t a2){}
bool GetPoolPed(uintptr_t a1)
{
    return (bool)((int (*)(void))(g_libGTASA + 0x00483DB8  + 1))();
}
void sub_55814(uintptr_t a1, uintptr_t a2, int a3){}

uintptr_t* sub_5212C(uintptr_t* a1)
{
    *reinterpret_cast<uintptr_t*>(a1 + 8) = 1;
    *reinterpret_cast<uintptr_t*>(a1) = off_1F95D4;
    *reinterpret_cast<uintptr_t*>(a1 + 0xC) = sub_57DE0();
    *reinterpret_cast<uintptr_t*>(a1 + 4) = sub_57DE0();

    uintptr_t v2 = *reinterpret_cast<uintptr_t*>(a1 + 0xC);
    *reinterpret_cast<char*>(a1 + 0x10) = 0;

    sub_57F64(0, v2);
    sub_55814(unk_1D4EC2, *reinterpret_cast<uintptr_t*>(a1 + 8), 0);

    if (*reinterpret_cast<uintptr_t*>(a1 + 0xC) && GetPoolPed(*reinterpret_cast<uintptr_t*>(a1 + 8)))
        sub_55814(unk_1D50BA, *reinterpret_cast<uintptr_t*>(a1 + 8), 1);

    *reinterpret_cast<uintptr_t*>(a1 + 0x11) = 0;
    *reinterpret_cast<uint16_t*>(a1 + 0x1D) = 1;
    *reinterpret_cast<uintptr_t*>(a1 + 0x15) = 0;
    *reinterpret_cast<uintptr_t*>(a1 + 0x19) = 0;
    *reinterpret_cast<uintptr_t*>(a1 + 0x1F) = 0x3C03126F;

    memset(reinterpret_cast<void*>(a1 + 0x50), 0, 0x23Au);

    reinterpret_cast<void(*)(uintptr_t)>(g_libGTASA + 0x004AC974 + 1)(*reinterpret_cast<uintptr_t*>(a1 + 0xC));
    reinterpret_cast<void(*)(uintptr_t)>(g_libGTASA + 0x004AC984 + 1)(*reinterpret_cast<uintptr_t*>(a1 + 0xC));

    return a1;
}

extern void sub_3F614(uintptr_t* a1, int a2)
{
    uint32_t* v4; // r5
    int v5; // r6
    uint32_t* v6; // r8
    int v7; // r6
    uint32_t* v8; // r1
    uint32_t* v9; // r0
    bool v10; // zf
    int v11; // r2
    bool v12; // zf
    int v13; // r4
    int v14; // r5
    int v15; // r0
    char v16; // r0
    uint32_t* v17; // r0
    int v18; // r0
    int v19; // r6
    float v20; // s0
    int v21; // r0
    float v22; // s4
    void (__fastcall ***v23)(uint32_t, uint32_t); // r6
    void (__fastcall **v24)(uint32_t, uint32_t); // r0
    void (__fastcall ***v25)(uint32_t, uint32_t); // r6
    void (__fastcall **v26)(uint32_t, uint32_t); // r0
    int* v27; // r1
    int v28; // r1
    int v29; // r0
    int v30; // r0
    uint32_t* v31; // r0
    int v33; // [sp+10h] [bp-48h] BYREF
    int v34[3]; // [sp+14h] [bp-44h] BYREF
    int64_t v35; // [sp+20h] [bp-38h] BYREF
    int v36; // [sp+28h] [bp-30h]
    float v37; // [sp+2Ch] [bp-2Ch] BYREF
    float v38; // [sp+30h] [bp-28h]
    float v39; // [sp+34h] [bp-24h]
    int v40; // [sp+38h] [bp-20h]

    v4 = dword_207F24;
    if ( dword_207F24 )
    {
        if ( !*dword_207F24 )
        {
            sub_5212C(a1);
            *v4 = v5;
        }
        dword_7BEF38(1, 0);
        dword_7BEF38(6, 0);
        dword_7BEF38(8, 0);
        dword_7BEF38(0xC, 0);
        dword_7BEF38(0xA, 5);
        dword_7BEF38(0xB, 6);
        dword_7BEF38(0xE, 0);
        dword_7BEF38(0x14, 1);
        v6 = (uint32_t*)(0x001D5CF0 + 1);
        v7 = *(uint32_t*)((char*)g_libGTASA + (int)Scene.m_pRwCamera);
        v33 = 0xFFFFFFFF;
        if ( a2 )
            v7 = a2;
        ((void (__fastcall *)(int, int*, int))(0x001D5CF0 + 1 + g_libGTASA))(v7, &v33, 2);
        ((void (__fastcall *)(int, int*, int))(0x001D5CF0 + 1 + g_libGTASA))(v7, &v33, 3);
        v10 = v9 == 0;
        if ( v9 )
        {
            v10 = v6 == 0;
        }
        if ( !v10 )
        {
            v11 = v8[8];
            v12 = v11 == 0;
            if ( v11 )
                v12 = v8[0xC] == 0;
            if ( !v12 )
            {
                if ( !a2 )
                    v6 = v9;
                v13 = g_libGTASA;
                v14 = *(uint32_t*)(0x6B7094 + g_libGTASA);
                v15 = g_libGTASA;
                if ( (v14 & 0x800000) != 0 )
                {
                    *(uint32_t*)(0x6B7094 + g_libGTASA) = v14 & 0xF77FFFFF;
                    if ( (v16 & 1) == 0 )
                    {
                        v17 = (uint32_t*)(::operator new(4u));
                        *v17 = 0;
                    }
                    *(uint32_t*)((char*)0x6B7094 + v13) |= v14 & 0x8000000 | 0x800000;
                    v15 = g_libGTASA;
                }

                v37 = v38;
            }
        }
    }
}

void sub_6ED20(int a1, uintptr_t* a2, uintptr_t* a3, int a4, int a5, int a6)
{
    int v8 = g_libGTASA;
    int v9;
    float *v10;
    float v11;
    int v12;
    int v13;
    int *v144;
    bool v14;
    int v16;
    int v17;
    int v18;
    int v19;
    float v166;
    float v155;
    CVector v77;
    int v222; // r0
    int v233; // [sp+8h] [bp+0h]
    float v288; // [sp+24h] [bp+1Ch]

    int v20;
    float v21;
    int v22[9];
    if (a2)
    {
        v9 = (int)Scene.m_pRwCamera;
        v17 = *(int*)(v9 + 0x60);
        v16 = *(int*)(v9 + 0x64);
        *(int*)(g_libGTASA + 0xA5A154) = 3;
        *(uintptr_t*)(v9 + 0x64) = *a3;
        *(uintptr_t*)(v9 + 0x60) = *a2;
        *(int *)((char *)0x6B7094 + v8) |= a1;
        v22[0] = 0xFFFFFFFF;
        ((void (__fastcall *)(int, int *, int))((char *)0x1D5CF0 + 1 + g_libGTASA))(v9, v22, 3);

        sub_55718(
                &v19,
                *(uint8_t *)(g_libGTASA + 0x966570),
                *(uint8_t *)(g_libGTASA + 0x966574),
                *(uint16_t *)(g_libGTASA + 0x966576),
                0xFF);
        sub_5578E(&v19);

        ((void ( *)(int, int *, int))((char *)0x1D5CF0 + 1 + g_libGTASA))(v9, (int*)&v19, 3);

        if (((int (__fastcall *)(int))(g_libGTASA + 0x004D42E0 + 1))(v9) == 1)
        {
            *(int *)(g_libGTASA + Scene.m_pRwCamera + 0x84) = *(int *)(g_libGTASA + 0xA5A160);
            *(float *)(g_libGTASA + Scene.m_pRwCamera + 0x88) = *(float *)(g_libGTASA + 0xA5A160) * 100.0; //in gtasa

            *(uint8_t *)(v8 + 0xA5A15C) = 1;
            v21 = 0.0;
            v19 = 0;
            v20 = 0;

            v77.z = 0.0f;
                if ( g_libGTASA + 0x9421C5 )
                {
                    v77 = *(CVector*)(0x960B50 + g_libGTASA);
                }

            if ((a1 & 0x8000000) == 0)
                v21 = v21 + 2.0;
            float cameraPosition[3] = {v77.x, v77.y, v77.z};
            ((void ( *)(float [3]))(0x001C0588 + g_libGTASA + 1))(cameraPosition);

            if (*(int *)(g_libGTASA + 0x95957C))
            {
                v18 = 0;
                ((void (__fastcall *)(int, int *, int))((char *)0x001D5CF0 + 1 + g_libGTASA))(v9, &v18, 2);
                ((void (__fastcall *)(int, int *, int))((char *)0x001D5CF0 + 1 + g_libGTASA))(v9, &v18, 3);
                a4 = 1;
                v13 = a6;
            }
            else
            {
                v13 = a6;

            }
            ((void (*)(void))(g_libGTASA + 0x005D0C10 + 1))();
            v14 = (a4 & (v13 ^ 1)) == 0;
            if ((a4 & (v13 ^ 1)) == 0)
                v14 = a5 == 0;

            if (v14)

            {
                if (dword_609F1C)
                    sub_3F614(dword_609F1C, 0);
                ((void (*)(void))(g_libGTASA + 0x0040EDF8 + 1 ))();
                ((void (*)(void))(g_libGTASA + 0x0040FBC4 + 1))();
                ((void (*)(void))(g_libGTASA + 0x0040FD64 + 1))();
            }

            ((void ( *)(int))((char *)0x001D5A14 + 1 + g_libGTASA))(v9);

            *(uint8_t *)(v8 + 0xA5A15C) = 0;
        }
        *(int *)((char *)0x6B7094 + v8) &= ~a1;
        *(int *)(v9 + 0x64) = v16;
        *(int *)(v9 + 0x60) = v17;
        *(int *)(g_libGTASA + 0xA5A154) = 0;
    }
}
RwRaster* dword_748F80;
RwRaster* dword_748F84;
RwRaster* dword_748F88;
RwRaster* dword_748F8C;
RwRaster* dword_748F90;
RwRaster* dword_748F94;
int sub_6F04C()
{
    unsigned int v1;
    int v2;
    int v3;
    int result;
    int settings_WaterReflLvl = 3;

    //  if ( *(uintptr_t *)(g_libGTASA + 0x63E464) )
    //{
    //Log("1");
    ((void (*)())(g_libGTASA + 0x5C4974))();// CMirrors::CreateBuffer(CMirrors *this)
    // }
    v1 = *(uintptr_t*)(g_libGTASA + 0x6E04E4);
    if ( v1 >= 1 )
        sub_6ED20(0x800000, *(uintptr_t**)(g_libGTASA + 0xA5A13C), *(uintptr_t**)(g_libGTASA + 0xA5A140), v1 == 2, 0, v1 == 3);
    v3 = settings_WaterReflLvl;
    if ( v3 >= 1 )                                // water
    {
        if ( v3 != 1 )
            v3 = 0;
        sub_6ED20(0x8800000, *(uintptr_t**)dword_748F90, *(uintptr_t**)dword_748F94, v3, 0, 0);
        if ( settings_WaterReflLvl >= 3 )
        {
            sub_6ED20(0x8000000, *(uintptr_t**)dword_748F88, *(uintptr_t**)dword_748F8C, 0, 0, 0);
        }
    }
    result = 1;
    if ( result ){
        sub_6ED20(0, *(uintptr_t**)dword_748F80, *(uintptr_t**)dword_748F84, 0, 1, 0);
        return 0;
    }

    return result;
}

RwRaster* dword_748C98;
RwRaster* dword_748C94;
RwRaster* dword_748C9C;
RwRaster* dword_748C90;
RwRaster* dword_748C8C;
RwRaster* dword_748C88;
uintptr_t* get_CMirrors_reflBuffer()
{
    return (uintptr_t*)dword_748C98;
}
void sub_6F160(){
    int *v2;
    RwRaster* result;
    dword_748F80 = RwRasterCreate(256, 256, 32, 1285);
    v2 = (int *)operator new(0x48u);

    dword_609F1C = (uintptr_t*)v2;
    dword_748F84 = RwRasterCreate(256, 256, 0, 1);
    dword_748F88 = RwRasterCreate(256, 256, 32, 1285);
    dword_748F8C = RwRasterCreate(256, 100, 0, 1);
    dword_748F90 = RwRasterCreate(256, 256, 32, 1285);
//	dword_748C88 = *(RwRaster**)LoadTextureFromDB("mobile", "BR_Nebo_Day");
    //  dword_748C90 = *(RwRaster**)LoadTextureFromDB("mobile", "waterDUDV");
    dword_748C98 = RwRasterCreate(256, 256, 32, 1285);
    dword_748C94 = RwRasterCreate(256, 256, 0, 1);
    dword_748C9C = RwRasterCreate(256, 256, 0, 1);
    //dword_748C8C = /*RwRasterCreate(256, 256, 0, 1);*/*(RwRaster**)LoadTextureFromDB("mobile", "BR_Nebo_Day");
    result = RwRasterCreate(100, 100, 0, 1);
    dword_748F94 = result;
    // dword_7BEE74 = g_libGTASA + 0x95B068;
    dword_207F24 = (uint32_t*)(g_libGTASA+0x9528A4);
}

uint8_t jj = 0;
void (*CMirrors__RenderReflections)(uintptr_t* thiz);
void CMirrors__RenderReflections_hook(uintptr_t* thiz) {
    if (jj != 0) {
        sub_6F04C();
        return ;
    }

    sub_6F160();
    jj = 1;

    sub_6F04C();

    return ;
}

void SetupReflection() {
    CHook::HookRedirect("_ZN8CMirrors17RenderReflectionsEv", CMirrors__RenderReflections_hook);
}

