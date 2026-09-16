//
// Created by x1y2z on 27.04.2024.
//

#pragma once

#include "../../common.h"
#include "../RxPipeline.h"
#include "../../Core/Pool.h"

constexpr auto ENV_MAP_PLUGIN_ID = 0x253F2FC;
constexpr auto ENV_MAP_ATM_PLUGIN_ID = 0x253F2F4;
constexpr auto SPECULAR_MAP_PLUGIN_ID = 0x253F2F6;
constexpr auto CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID = 0x53F2009A;

struct ReflectionMaterialStream {
    float scaleX;
    float scaleY;
    float transSclX;
    float transSclY;
    float shininess;
    RwTexture* texture;
};

struct CustomEnvMapPipeMaterialData {
    uint scaleX;
    uint scaleY;
    uint transSclX;
    uint transSclY;

    uint shininess;
    int renderFrameCounter;
    RwTexture* texture;

    void FromStream(const ReflectionMaterialStream& stream) {
        scaleX    = (uint)(stream.scaleX * 8.0f);
        scaleY    = (uint)(stream.scaleY * 8.0f);
        transSclX = (uint)(stream.transSclX * 8.0f);
        transSclY = (uint)(stream.transSclY * 8.0f);
        shininess = (uint)(stream.shininess * 255.0f);
        texture   = stream.texture;
        renderFrameCounter = 0;
    };
};

struct CustomEnvMapPipeAtomicData {
    float lastTrans;
    float posx;
    float posy;
};

struct SpecMatBuffer {
    float specularity;
    char name[24];
};

struct CustomSpecMapPipeMaterialData {
    float specularity;
    RwTexture* texture;
};

typedef CPool<CustomEnvMapPipeMaterialData, CustomEnvMapPipeMaterialData, true>     CustomEnvMapPipeMaterialDataPool;
typedef CPool<CustomEnvMapPipeAtomicData, CustomEnvMapPipeAtomicData, true>         CustomEnvMapPipeAtomicDataPool;
typedef CPool<CustomSpecMapPipeMaterialData, CustomSpecMapPipeMaterialData, true>   CustomSpecMapPipeMaterialDataPool;

class CCustomCarEnvMapPipeline {
    static inline RxPipeline* ObjPipeline = nullptr;

    static inline CustomEnvMapPipeMaterialData          fakeEnvMapPipeMatData;
    static inline CustomEnvMapPipeMaterialDataPool*     m_gEnvMapPipeMatDataPool;
    static inline CustomEnvMapPipeAtomicDataPool*       m_gEnvMapPipeAtmDataPool;
    static inline CustomSpecMapPipeMaterialDataPool*    m_gSpecMapPipeMatDataPool;

public:
    static void InjectHooks();

    static bool RegisterPlugin();

    static bool CreatePipe();
    static void DestroyPipe();
    static RxPipeline* CreateCustomObjPipe();

    static RwBool CustomPipeInstanceCB(void* object, RxOpenGLMeshInstanceData* instanceData, const RwBool instanceDLandVA, const RwBool reinstance);
    static void CustomPipeRenderCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags);
};
