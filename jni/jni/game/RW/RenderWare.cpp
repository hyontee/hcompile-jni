#include "../../main.h"
#include "RenderWare.h"
#include "patch.h"


RsGlobalType* RsGlobal;

/* rwcore.h */
RwCamera* (*RwCameraBeginUpdate)(RwCamera* camera);
RwCamera* (*RwCameraEndUpdate)(RwCamera* camera);
RwCamera* (*RwCameraShowRaster)(RwCamera * camera, void *pDev, RwUInt32 flags);

RwRaster* 	(*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwBool 		(*RwRasterDestroy)(RwRaster * raster);
RwRaster* 	(*RwRasterGetOffset)(RwRaster *raster, RwInt16 *xOffset, RwInt16 *yOffset);
RwInt32 	(*RwRasterGetNumLevels)(RwRaster * raster);
RwRaster* 	(*RwRasterSubRaster)(RwRaster * subRaster, RwRaster * raster, RwRect * rect);
RwRaster* 	(*RwRasterRenderFast)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster* 	(*RwRasterRender)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster* 	(*RwRasterRenderScaled)(RwRaster * raster, RwRect * rect);
RwRaster* 	(*RwRasterPushContext)(RwRaster * raster);
RwRaster* 	(*RwRasterPopContext)(void);
RwRaster* 	(*RwRasterGetCurrentContext)(void);
RwBool 		(*RwRasterClear)(RwInt32 pixelValue);
RwBool 		(*RwRasterClearRect)(RwRect * rpRect, RwInt32 pixelValue);
RwRaster* 	(*RwRasterShowRaster)(RwRaster * raster, void *dev, RwUInt32 flags);
RwUInt8* 	(*RwRasterLock)(RwRaster * raster, RwUInt8 level, RwInt32 lockMode);
RwRaster* 	(*RwRasterUnlock)(RwRaster * raster);
RwUInt8* 	(*RwRasterLockPalette)(RwRaster * raster, RwInt32 lockMode);
RwRaster* 	(*RwRasterUnlockPalette)(RwRaster * raster);
RwImage* 	(*RwImageCreate)(RwInt32 width, RwInt32 height, RwInt32 depth);
RwBool 		(*RwImageDestroy)(RwImage * image);
RwImage* 	(*RwImageAllocatePixels)(RwImage * image);
RwImage* 	(*RwImageFreePixels)(RwImage * image);
RwImage* 	(*RwImageCopy)(RwImage * destImage, const RwImage * sourceImage);
RwImage* 	(*RwImageResize)(RwImage * image, RwInt32 width, RwInt32 height);
RwImage* 	(*RwImageApplyMask)(RwImage * image, const RwImage * mask);
RwImage* 	(*RwImageMakeMask)(RwImage * image);
RwImage* 	(*RwImageReadMaskedImage)(const RwChar * imageName, const RwChar * maskname);
RwImage* 	(*RwImageRead)(const RwChar * imageName);
RwImage* 	(*RwImageWrite)(RwImage * image, const RwChar * imageName);
RwImage* 	(*RwImageSetFromRaster)(RwImage *image, RwRaster *raster);
RwRaster* 	(*RwRasterSetFromImage)(RwRaster *raster, RwImage *image);
RwRaster* 	(*RwRasterRead)(const RwChar *filename);
RwRaster* 	(*RwRasterReadMaskedRaster)(const RwChar *filename, const RwChar *maskname);
RwImage* 	(*RwImageFindRasterFormat)(RwImage *ipImage, RwInt32 nRasterType, RwInt32 *npWidth, RwInt32 *npHeight, RwInt32 *npDepth,RwInt32 *npFormat);

/* rwlpcore.h */
RwReal 		(*RwIm2DGetNearScreenZ)(void);
RwReal 		(*RwIm2DGetFarScreenZ)(void);
RwBool 		(*RwRenderStateGet)(RwRenderState state, void *value);
RwBool 		(*RwRenderStateSet)(RwRenderState state, void *value);
RwBool 		(*RwIm2DRenderLine)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwBool 		(*RwIm2DRenderTriangle)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
RwBool 		(*RwIm2DRenderPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
RwBool 		(*RwIm2DRenderIndexedPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices, RwImVertexIndex *indices, RwInt32 numIndices);

/* rtpng.h */
RwImage*	(*RtPNGImageWrite)(RwImage* image, const RwChar* imageName);
RwImage* 	(*RtPNGImageRead)(const RwChar* imageName);

RwBool (*RwTextureDestroy)(RwTexture* texture);
RwTexture* (*RwTextureSetName)(RwTexture* texture, const char* name);
RwTexture* (*RwTextureCreate)(RwRaster* raster);



void WriteMemory(uintptr_t dest, uintptr_t src, size_t size);
template <typename Func>
void SetFuncSym(Func func, const char* sym)
{
    void *funcorig = dlsym((void *)CHook::Lib, sym);
    if ( !funcorig )
    {
        Log("Error find %s", sym);
        exit(0);
    }
    *(void **)(func) = (void*)(funcorig);
}
void InitRenderWareFunctions()
{
    Log("Initializing RenderWare..");

    /* skeleton.h */
    RsGlobal = (RsGlobalType*)(g_libGTASA + (VER_x32 ? 0x009FC8FC : 0xC9B320));

    /* rwCore.h */
    /**(void**)(&RwCameraBeginUpdate) 			= (void*)(g_libGTASA+0x001D5A18+1);
    *(void**)(&RwCameraEndUpdate) 				= (void*)(g_libGTASA+0x001D5A14+1);
    *(void**)(&RwCameraShowRaster)				= (void*)(g_libGTASA+0x001D5D14+1);

    *(void **)(&RwRasterCreate) 				= (void*)(g_libGTASA+0x001DA9D0+1);
    *(void **)(&RwRasterDestroy) 				= (void*)(g_libGTASA+0x001DA7D0+1);
    *(void **)(&RwRasterGetOffset) 				= (void*)(g_libGTASA+0x001DA6AC+1);
    *(void **)(&RwRasterGetNumLevels) 			= (void*)(g_libGTASA+0x001DA900+1);
    *(void **)(&RwRasterSubRaster) 				= (void*)(g_libGTASA+0x001DA974+1);
    *(void **)(&RwRasterRenderFast)				= (void*)(g_libGTASA+0x001DA734+1);
    *(void **)(&RwRasterRender)					= (void*)(g_libGTASA+0x001DA780+1);
    *(void **)(&RwRasterRenderScaled)			= (void*)(g_libGTASA+0x001DA68C+1);
    *(void **)(&RwRasterPushContext)			= (void*)(g_libGTASA+0x001DA818+1);
    *(void **)(&RwRasterPopContext)				= (void*)(g_libGTASA+0x001DA8B8+1);
    *(void **)(&RwRasterGetCurrentContext)		= (void*)(g_libGTASA+0x001DA66C+1);
    *(void **)(&RwRasterClear)					= (void*)(g_libGTASA+0x001DA6DC+1);
    *(void **)(&RwRasterClearRect)				= (void*)(g_libGTASA+0x001DA760+1);
    *(void **)(&RwRasterShowRaster)				= (void*)(g_libGTASA+0x001DA93C+1);
    *(void **)(&RwRasterLock)					= (void*)(g_libGTASA+0x001DAA74+1);
    *(void **)(&RwRasterUnlock)					= (void*)(g_libGTASA+0x001DA6B8+1);
    *(void **)(&RwRasterLockPalette)			= (void*)(g_libGTASA+0x001DA88C+1);
    *(void **)(&RwRasterUnlockPalette)			= (void*)(g_libGTASA+0x001DA7AC+1);
    //*(void **)(&RwImageCreate)					= (void*)(g_libGTASA+0x001D8E20+1);


    *(void **)(&RwImageDestroy)					= (void*)(g_libGTASA+0x001D8E78+1);
    *(void **)(&RwImageAllocatePixels)			= (void*)(g_libGTASA+0x001D8F04+1);
    *(void **)(&RwImageFreePixels)				= (void*)(g_libGTASA+0x001D8ED8+1);
    *(void **)(&RwImageCopy)					= (void*)(g_libGTASA+0x001D9560+1);
    *(void **)(&RwImageResize)					= (void*)(g_libGTASA+0x001D8FA0+1);
    *(void **)(&RwImageApplyMask)				= (void*)(g_libGTASA+0x001D9280+1);
    *(void **)(&RwImageMakeMask)				= (void*)(g_libGTASA+0x001D90C8+1);
    *(void **)(&RwImageReadMaskedImage)			= (void*)(g_libGTASA+0x001D9DDC+1);
    *(void **)(&RwImageRead)					= (void*)(g_libGTASA+0x001D97AC+1);
    *(void **)(&RwImageWrite)					= (void*)(g_libGTASA+0x001D9D40+1);
    *(void **)(&RwImageSetFromRaster)			= (void*)(g_libGTASA+0x001DA454+1);
    *(void **)(&RwRasterSetFromImage)			= (void*)(g_libGTASA+0x001DA478+1);
    *(void **)(&RwRasterRead)					= (void*)(g_libGTASA+0x001DA574+1);
    *(void **)(&RwRasterReadMaskedRaster)		= (void*)(g_libGTASA+0x001DA614+1);
    *(void **)(&RwImageFindRasterFormat)		= (void*)(g_libGTASA+0x001DA49C+1);*/

    /* rwlpcore.h */
    /**(void **)(&RwIm2DGetNearScreenZ)			= (void*)(g_libGTASA+0x001E2874+1);
    *(void **)(&RwIm2DGetFarScreenZ)			= (void*)(g_libGTASA+0x001E2884+1);
    *(void **)(&RwRenderStateGet)				= (void*)(g_libGTASA+0x001E28C8+1);
    *(void **)(&RwRenderStateSet)				= (void*)(g_libGTASA+0x001E2894+1);
    *(void **)(&RwIm2DRenderLine)				= (void*)(g_libGTASA+0x001E28D8+1);
    *(void **)(&RwIm2DRenderTriangle)			= (void*)(g_libGTASA+0x001E28F0+1);
    *(void **)(&RwIm2DRenderPrimitive)			= (void*)(g_libGTASA+0x001E2908+1);
    *(void **)(&RwIm2DRenderIndexedPrimitive)	= (void*)(g_libGTASA+0x001E2918+1);

    /* rtpng.h */
    /**(void **)(&RtPNGImageWrite)				= (void*)(g_libGTASA+0x0020A144+1);
    *(void **)(&RtPNGImageRead)					= (void*)(g_libGTASA+0x0020A3F4+1);

	*(void**)(&RwTextureDestroy) = (void*)(g_libGTASA + 0x001DB6E4 + 1);*/

    SetFuncSym(&RwCameraBeginUpdate, "_Z19RwCameraBeginUpdateP8RwCamera");
    SetFuncSym(&RwCameraShowRaster, "_Z18RwCameraShowRasterP8RwCameraPvj");

    SetFuncSym(&RwRasterCreate, "_Z14RwRasterCreateiiii");

    SetFuncSym(&RwRasterDestroy, "_Z15RwRasterDestroyP8RwRaster");

    SetFuncSym(&RwRasterGetOffset, "_Z17RwRasterGetOffsetP8RwRasterPsS1_");

    SetFuncSym(&RwRasterGetNumLevels, "_Z20RwRasterGetNumLevelsP8RwRaster");

    SetFuncSym(&RwRasterSubRaster, "_Z17RwRasterSubRasterP8RwRasterS0_P6RwRect");

    SetFuncSym(&RwRasterRenderFast, "_Z18RwRasterRenderFastP8RwRasterii");

    SetFuncSym(&RwRasterRender, "_Z14RwRasterRenderP8RwRasterii");

    SetFuncSym(&RwRasterRenderScaled, "_Z20RwRasterRenderScaledP8RwRasterP6RwRect");

    SetFuncSym(&RwRasterPushContext, "_Z19RwRasterPushContextP8RwRaster");

    SetFuncSym(&RwRasterPopContext, "_Z18RwRasterPopContextv");

    SetFuncSym(&RwRasterGetCurrentContext, "_Z25RwRasterGetCurrentContextv");

    SetFuncSym(&RwRasterClear, "_Z13RwRasterCleari");

    SetFuncSym(&RwRasterClearRect, "_Z17RwRasterClearRectP6RwRecti");

    SetFuncSym(&RwRasterShowRaster, "_Z18RwRasterShowRasterP8RwRasterPvj");

    SetFuncSym(&RwRasterLock, "_Z12RwRasterLockP8RwRasterhi");

    SetFuncSym(&RwRasterUnlock, "_Z14RwRasterUnlockP8RwRaster");

    SetFuncSym(&RwRasterLockPalette, "_Z19RwRasterLockPaletteP8RwRasteri");

    SetFuncSym(&RwImageCreate, "_Z13RwImageCreateiii");

    SetFuncSym(&RwImageDestroy, "_Z14RwImageDestroyP7RwImage");

    SetFuncSym(&RwImageAllocatePixels, "_Z21RwImageAllocatePixelsP7RwImage");

    SetFuncSym(&RwImageFreePixels, "_Z17RwImageFreePixelsP7RwImage");

    SetFuncSym(&RwImageCopy, "_Z11RwImageCopyP7RwImagePKS_");

    SetFuncSym(&RwImageResize, "_Z13RwImageResizeP7RwImageii");

    SetFuncSym(&RwImageApplyMask, "_Z16RwImageApplyMaskP7RwImagePKS_");

    SetFuncSym(&RwImageMakeMask, "_Z15RwImageMakeMaskP7RwImage");

    SetFuncSym(&RwImageReadMaskedImage, "_Z22RwImageReadMaskedImagePKcS0_");

    SetFuncSym(&RwImageRead, "_Z11RwImageReadPKc");

    SetFuncSym(&RwImageWrite, "_Z12RwImageWriteP7RwImagePKc");

    SetFuncSym(&RwImageSetFromRaster, "_Z20RwImageSetFromRasterP7RwImageP8RwRaster");

    SetFuncSym(&RwRasterSetFromImage, "_Z20RwRasterSetFromImageP8RwRasterP7RwImage");

    SetFuncSym(&RwRasterRead, "_Z12RwRasterReadPKc");

    SetFuncSym(&RwRasterReadMaskedRaster, "_Z24RwRasterReadMaskedRasterPKcS0_");

    SetFuncSym(&RwImageFindRasterFormat, "_Z23RwImageFindRasterFormatP7RwImageiPiS1_S1_S1_");

    SetFuncSym(&RwIm2DGetNearScreenZ, "_Z20RwIm2DGetNearScreenZv");

    SetFuncSym(&RwIm2DGetFarScreenZ, "_Z19RwIm2DGetFarScreenZv");

    SetFuncSym(&RwRenderStateGet, "_Z16RwRenderStateGet13RwRenderStatePv");

    SetFuncSym(&RwRenderStateSet, "_Z16RwRenderStateSet13RwRenderStatePv");

    SetFuncSym(&RwIm2DRenderLine, "_Z23RwIm2DRenderLine_BUGFIXP14RwOpenGLVertexiii");

    SetFuncSym(&RwIm2DRenderTriangle, "_Z27RwIm2DRenderTriangle_BUGFIXP14RwOpenGLVertexiiii");

    SetFuncSym(&RwIm2DRenderPrimitive, "_Z28RwIm2DRenderPrimitive_BUGFIX15RwPrimitiveTypeP14RwOpenGLVertexi");

    SetFuncSym(&RwIm2DRenderIndexedPrimitive, "_Z35RwIm2DRenderIndexedPrimitive_BUGFIX15RwPrimitiveTypeP14RwOpenGLVertexiPti");

    SetFuncSym(&RtPNGImageWrite, "_Z15RtPNGImageWriteP7RwImagePKc");

    SetFuncSym(&RtPNGImageRead, "_Z14RtPNGImageReadPKc");


    SetFuncSym(&RwTextureDestroy, "_Z16RwTextureDestroyP9RwTexture");

    SetFuncSym(&RwTextureSetName, "_Z16RwTextureSetNameP9RwTexturePKc");

    SetFuncSym(&RwTextureCreate, "_Z15RwTextureCreateP8RwRaster");

    SetFuncSym(&RwCameraEndUpdate, "_Z17RwCameraEndUpdateP8RwCamera");





//	CHook::WriteMemory(g_libGTASA + 0x001A7EF2, "\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
//	CHook::WriteMemory(g_libGTASA + 0x001A7F34, "\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
}