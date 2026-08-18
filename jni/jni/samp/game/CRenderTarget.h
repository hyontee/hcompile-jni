#pragma once

#include <vector>
#include <stdint.h>
#include "game/RW/rwlpcore.h"

class CRenderTarget
{
    static struct RwTexture* m_pResultTexture;

    static inline struct RwCamera* m_pCamera;
    static inline struct RpLight* m_pLight;
    static inline struct RwFrame* m_pFrame;
    static inline struct RwRaster* m_zBuffer;

//	uint32_t m_dwResultSizeX;
//	uint32_t m_dwResultSizeY;

    static bool m_bReady;

//	static inline bool m_b2D;

    static void PreProcessCamera(RwInt32 sizeX, RwInt32 sizeY);
    static void ProcessCamera(RwRGBA* bgcolor, bool b2D);
    static void PostProcessCamera();

    static inline bool m_bSucessfull;
public:
    //CRenderTarget(uint32_t dwResultSizeX, uint32_t dwResultSizeY, bool b2D);
    //~CRenderTarget();

    static void Begin(RwInt32 sizeX, RwInt32 sizeY, RwRGBA* bgColor, bool b2D);
    static struct RwTexture* End();

    void shutDown();

    static bool InitialiseScene();

    static void Initialise();
};
