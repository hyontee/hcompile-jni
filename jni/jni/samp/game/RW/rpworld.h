//
// Created by plaka on 21.02.2023.
//

#pragma once

#include "../common.h"
#include "rtanim.h"

#define rwMAXTEXTURECOORDS 8

#define RpAtomicGetGeometryMacro(_atomic)                               \
    ((_atomic)->geometry)

#define RpAtomicGetFrameMacro(_atomic)                                  \
    ((RwFrame *) rwObjectGetParent(_atomic))

struct RpLight
{
    RwObjectHasFrame    object; /**< object */
    RwReal              radius; /**< radius */
    RwRGBAReal          color; /**< color */  /* Light color */
    RwReal              minusCosAngle; /**< minusCosAngle */
    RwLinkList          WorldSectorsInLight; /**< WorldSectorsInLight */
    RwLLLink            inWorld; /**< inWorld */
    RwUInt16            lightFrame; /**< lightFrame */
    RwUInt16            pad;
};

struct RpSector
{
    RwInt32                 type;
};


/**
 * \ingroup rpmaterial
 * \ref RpMaterialCallBack
 * represents the function called from \ref RpGeometryForAllMaterials and
 * \ref RpWorldForAllMaterials for all materials referenced by polygons in a
 * given geometry. This function should return a pointer to the current
 * material to indicate success. The callback may return NULL to terminate
 * further callbacks on the materials.
 *
 * \param  material   Pointer to the current material
 * \param  data       Pointer to developer-defined data structure.
 *
 * \return Pointer to the current material.
 */
typedef RpMaterial *(*RpMaterialCallBack)(RpMaterial *material, void *data);

/**
 * \ingroup rpworldsub
 * \ref RpWorldRenderOrder
 * represents the options available for
 * the rendering order of world sectors in the camera's view frustum (see
 * API function \ref RpWorldSetRenderOrder).
 */
enum RpWorldRenderOrder
{
    rpWORLDRENDERNARENDERORDER = 0,
    rpWORLDRENDERFRONT2BACK,  /**<Renders nearest sectors first */
    rpWORLDRENDERBACK2FRONT, /**<Renders furthest sectors first */
    rpWORLDRENDERORDERFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldRenderOrder RpWorldRenderOrder;

typedef struct RpVertexNormal RpVertexNormal;

#if (!defined(DOXYGEN))
struct RpVertexNormal
{
    RwInt8          x;
    RwInt8          y;
    RwInt8          z;
    RwUInt8         pad; /* pad character to force alignment */
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpworldsector
 * \struct RpWorldSector
 * World Sector object. This should be
 * considered an opaque type. Use the RpWorldSector API functions to access.
 */
typedef struct RpWorldSector RpWorldSector;

#if (!defined(DOXYGEN))
struct RpWorldSector
{
    RwInt32                 type;

    RpTriangle              *triangles;             /* Triangles */

    RwV3d                   *vertices;              /* Vertex positions */
    RpVertexNormal          *normals;               /* Vertex normals */

    RwTexCoords             *texCoords[rwMAXTEXTURECOORDS]; /* Texture coordinates */

    RwRGBA                  *preLitLum;             /* Pre lit luminances */

    /* Pointer to memory allocated for vertex and poly pointers */
    RwResEntry              *repEntry;

    /* Atomics in this sectors */
    /* The pointers are frigged so they look like they are pointing to
       Atomics when they are pointing to here */
    RwLinkList              collAtomicsInWorldSector;       /* Coll priority */

    /* Lights in an atomic sector */
    RwLinkList              lightsInWorldSector;

    /* Outer bounding box of sector based on BSP planes */
    RwBBox                  boundingBox;

    /* Bounding box tightly enclosing geometry */
    RwBBox                  tightBoundingBox;

    /* The mesh which groups same material triangles together */
    RpMeshHeader            *mesh;

    /* The WorldSector object pipeline for this WorldSector */
    RxPipeline    *pipeline;

    /* Material list window base
     * (triangles in a given sector can "see"
     * the 256 materials from
     * MatList[matListWindowBase] -> MatList[matListWindowBase + 255])
     */
    RwUInt16                matListWindowBase;

    RwUInt16                numVertices;            /* Num vertices */
    RwUInt16                numTriangles;           /* Num triangles */
    RwUInt16                pad;
};
#endif /* (!defined(DOXYGEN)) */

typedef RpWorldSector *(*RpWorldSectorCallBackRender) (RpWorldSector *worldSector);
/**
 * \ingroup rpworldsub
 * \struct RpWorld
 * World object. This should be considered an opaque type.
 * Use the RpWorld API functions to access.
 */

struct RpWorld
{
    RwObject            object;

    RwUInt32            flags;

    RpWorldRenderOrder  renderOrder;

    /* Materials */
    RpMaterialList      matList;

    /* The world stored as a BSP tree */
    RpSector           *rootSector;

    /* The number of texture coordinate sets in each sector */
    RwInt32             numTexCoordSets;

    /* Render frame used when last rendered */
    RwInt32             numClumpsInWorld;
    RwLLLink           *currentClumpLink;

    /* All the clumps in the world */
    RwLinkList          clumpList;

    /* All of the lights in the world */
    RwLinkList          lightList;

    /* Directional lights in the world */
    RwLinkList          directionalLightList;

    /* The worlds origin offset */
    RwV3d               worldOrigin;

    /* Bounding box around the whole world */
    RwBBox              boundingBox;

    /* The callbacks functions */
    RpWorldSectorCallBackRender renderCallBack;

    RxPipeline         *pipeline;
};

/* Type ID */
#define rpGEOMETRY 8

/**
 * \ingroup rpgeometry
 * RpGeometryFlag
 * Geometry type flags
 *
 * When creating a geometry, these flags can be OR'ed together to
 * specify the format along with the rpGEOMETRYTEXCOORDSETS(n) macro if more
 * than two sets of texture coordinates are required. See \ref RpGeometryCreate
 * for more details.
 *
 * \see RpGeometryCreate().
 */
enum RpGeometryFlag
{
    rpGEOMETRYTRISTRIP  = 0x00000001, /**<This geometry's meshes can be
                                          rendered as strips.
                                          \ref RpMeshSetTriStripMethod is
                                          used to change this method.*/
    rpGEOMETRYPOSITIONS = 0x00000002, /**<This geometry has positions */
    rpGEOMETRYTEXTURED  = 0x00000004, /**<This geometry has only one set of
                                          texture coordinates. Texture
                                          coordinates are specified on a per
                                          vertex basis */
    rpGEOMETRYPRELIT    = 0x00000008, /**<This geometry has pre-light colors */
    rpGEOMETRYNORMALS   = 0x00000010, /**<This geometry has vertex normals */
    rpGEOMETRYLIGHT     = 0x00000020, /**<This geometry will be lit */
    rpGEOMETRYMODULATEMATERIALCOLOR = 0x00000040, /**<Modulate material color
                                                      with vertex colors
                                                      (pre-lit + lit) */

    rpGEOMETRYTEXTURED2 = 0x00000080, /**<This geometry has at least 2 sets of
                                          texture coordinates. */

    /*
     * These above flags were stored in the flags field in an RwObject, they
     * are now stored in the flags file of the RpGeometry.
     */

    rpGEOMETRYNATIVE            = 0x01000000,
    rpGEOMETRYNATIVEINSTANCE    = 0x02000000,

    rpGEOMETRYFLAGSMASK         = 0x000000FF,
    rpGEOMETRYNATIVEFLAGSMASK   = 0x0F000000,

    rpGEOMETRYFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpGeometryFlag RpGeometryFlag;

RpLight* RpLightSetColor(RpLight* light, const RwRGBAReal* color);
RpLight* RpLightCreate(RwInt32 type);
RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light);
RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light);
RwBool RpLightDestroy(RpLight* light);
RwBool RpAtomicDestroy(RpAtomic* atomic);
RwBool RpClumpDestroy(RpClump* clump);
RpGeometry* RpGeometryForAllMaterials(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* data);

#define RpAtomicGetGeometry(_atomic) \
    RpAtomicGetGeometryMacro(_atomic)

#define RpAtomicGetFrame(_atomic) \
    RpAtomicGetFrameMacro(_atomic)
