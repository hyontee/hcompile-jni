//
// Created by admin on 25.01.2024.
//

#include <main.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "util/armhook.h"

#define FLAG_ALPHA_TEST           0x01
#define FLAG_LIGHTING             0x02
#define FLAG_ALPHA_MODULATE       0x04
#define FLAG_COLOR_EMISSIVE       0x08
#define FLAG_COLOR                0x10
#define FLAG_TEX0                 0x20
#define FLAG_ENVMAP               0x40          // normal envmap
#define FLAG_BONE3                0x80
#define FLAG_BONE4                0x100
#define FLAG_CAMERA_BASED_NORMALS 0x200
#define FLAG_FOG                  0x400
#define FLAG_TEXBIAS              0x800
#define FLAG_BACKLIGHT            0x1000
#define FLAG_LIGHT1               0x2000
#define FLAG_LIGHT2               0x4000
#define FLAG_LIGHT3               0x8000
#define FLAG_DETAILMAP            0x10000
#define FLAG_COMPRESSED_TEXCOORD  0x20000
#define FLAG_PROJECT_TEXCOORD     0x40000
#define FLAG_WATER                0x80000
#define FLAG_COLOR2               0x100000
#define FLAG_SPHERE_XFORM         0x800000      // this renders the scene as a sphere map for vehicle reflections
#define FLAG_SPHERE_ENVMAP        0x1000000     // spherical real-time envmap
#define FLAG_TEXMATRIX            0x2000000
#define FLAG_GAMMA                0x4000000

int* _detailTexturesDataRasterParent;
int* RasterExtOffset;
int* textureDetail;

typedef struct {
    // Checks for GL_OES_depth24
    char has24BitDepthCap;                   // 0x00
    // Checks for GL_OES_packed_depth_stencil
    char hasPackedDepthStencilCap;           // 0x01
    // Checks for GL_NV_depth_nonlinear
    char hasDepthNonLinearCap;               // 0x02
    // Checks for GL_EXT_texture_compression_dxt1 or GL_EXT_texture_compression_s3tc
    char hasTextureCompressionDXT1OrS3TCCap; // 0x03
    // Checks for GL_AMD_compressed_ATC_texture
    char hasTextureCompressionATCCap;        // 0x04
    // Checks for GL_IMG_texture_compression_pvrtc
    char hasTextureCompressionPVRTCCap;      // 0x05
    // Checks for GL_OES_rgb8_rgba8
    char has32BitRenderTargetCap;            // 0x06
    // Checks for GL_EXT_texture_filter_anisotropic
    char hasAnisotropicFilteringCap;         // 0x07
    // Set when OS_SystemChip() <= 1
    char unk_08;                             // 0x08
    // Always set to 0
    char unk_09;                             // 0x09
    // Checks for GL_QCOM_binning_control
    char hasBinningControlCap;               // 0x0A
    // Checks for GL_QCOM_alpha_test
    char hasAlphaTestCap;                    // 0x0B
    // Checks for Adreno (TM) 320 or GL_AMD_compressed_ATC_texture
    char isAdreno;                           // 0x0C
    // Set when there is no compression support
    char isMaliChip;                         // 0x0D
    // Checks for 225 or 540
    char isSlowGPU;                          // 0x0E
    char unk_0f;                             // 0x0F
} RQCapabilities;

RQCapabilities *RQCaps;
int *RQMaxBones;

char pxlbuf[1024 * 10];
char vtxbuf[1024 * 10];

char* byte_617290 = 0; //pxl
char* byte_617258 = 0;

char* byte_619294 = 0; //vertex

#define PXL_SOURCE(str) \
		strcat(byte_617290, str) \

#define VTX_SOURCE(str) \
		strcat(byte_619294, str) \


void (*BuildPixelSource)(unsigned int flags);
void BuildPixelSource_hook(unsigned int flags)
{
    int v2; // r8
    bool v3; // zf
    size_t v4; // r0
    const char *v5; // r2
    char s[512]; // [sp+10h] [bp-220h] BYREF
    int v8; // [sp+210h] [bp-20h]

    byte_617290[0] = 0;
    strcpy(s, "#version 100\n");
    strcat(byte_617290, s);

    strcpy(s, "precision mediump float;");
    strcat(byte_617290, s);

    if ( (flags & 0x20) != 0 )
    {
        strcpy(s, "uniform sampler2D Diffuse;");
        strcat(byte_617290, s);

        strcpy(s, "varying mediump vec2 Out_Tex0;");
        strcat(byte_617290, s);

    }
    if ( (flags & 0x1000040) != 0 )
    {
        strcpy(s, "uniform sampler2D EnvMap;");
        strcat(byte_617290, s);

        strcpy(s, "uniform lowp float EnvMapCoefficient;");
        strcat(byte_617290, s);

        if ( (flags & 0x40) != 0 )
            strcpy(s, "varying mediump vec2 Out_Tex1;");
        else
            strcpy(s, "varying mediump vec3 Out_Refl;");
    }
    else
    {
        if ( (flags & 0x10000) == 0 )
            goto LABEL_10;
        strcpy(s, "uniform sampler2D EnvMap;");
        strcat(byte_617290, s);

        strcpy(s, "uniform float DetailTiling;");
    }
    strcat(byte_617290, s);

    LABEL_10:
    if ( (flags & 0x400) != 0 )
    {
        strcpy(s, "varying mediump float Out_FogAmt;");
        strcat(byte_617290, s);

        strcpy(s, "uniform lowp vec3 FogColor;");
        strcat(byte_617290, s);

        strcpy(s, "uniform sampler2D FogMap;");
        strcat(byte_617290, s);

        strcpy(s, "varying lowp vec4 Out_ClipSpace;");
        strcat(byte_617290, s);

    }
    if ( (flags & 0x12) != 0 )
    {
        strcpy(s, "varying lowp vec4 Out_Color;");
        strcat(byte_617290, s);

    }
    if ( (flags & 4) != 0 )
    {
        strcpy(s, "uniform lowp float AlphaModulate;");
        strcat(byte_617290, s);

    }
    v2 = flags & 0x80000;
    if ( (flags & FLAG_WATER) != 0 )
    {
        strcpy(s, "varying mediump vec2 Out_WaterDetail;");
        strcat(byte_617290, s);

        strcpy(s, "varying mediump vec2 Out_WaterDetail2;");
        strcat(byte_617290, s);

        strcpy(s, "varying mediump float Out_WaterAlphaBlend;");
        strcat(byte_617290, s);

        //strcpy(s, "uniform sampler2D reflectionTexture;");
        //strcat(byte_617290, s);

        //strcpy(s, "uniform sampler2D refractionTexture;");
        //strcat(byte_617290, s);

    }
    if ( (flags & 0x2000) != 0 )
    {
        strcpy(s, "varying lowp vec3 Out_Spec;");
        strcat(byte_617290, s);

    }
    strcpy(s, "void main()");
    strcat(byte_617290, s);

    strcpy(s, (const char *)"{");
    strcat(byte_617290, s);

    strcpy(s, "lowp vec4 fcolor;");
    strcat(byte_617290, s);

    if ( (flags & 0x400) != 0 )
    {
        strcpy(s, "lowp vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;");
        strcat(byte_617290, s);

        strcpy(s, "lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);");
        strcat(byte_617290, s);

        strcpy(s, "lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);");
        strcat(byte_617290, s);

    }
    if ( (flags & 0x20) == 0 )
    {
        if ( (flags & 0x12) != 0 )
            strcpy(s, "fcolor = Out_Color;");
        else
            strcpy(s, "fcolor = 0.0;");
        goto LABEL_42;
    }
    if ( (flags & 0x800) != 0 )
    {
        strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -1.5);");
    }
    else if ( *(char *)(RQCaps + 14) )
    {
        strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);");
    }
    else
    {
        strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -0.5);");
    }
    strcat(byte_617290, s);

    strcpy(s, "fcolor = diffuseColor;");
    strcat(byte_617290, s);

    if ( (flags & 0x12) == 0 )
    {
        if ( !v2 )
            goto LABEL_43;
        strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
        goto LABEL_42;
    }
    if ( (flags & 0x10000) == 0 )
    {
        strcpy(s, "fcolor *= Out_Color;");
        strcat(byte_617290, s);

        if ( v2 )
        {
            strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
            strcat(byte_617290, s);


            if ( (flags & FLAG_ENVMAP) == 0 )
                goto LABEL_45;
            LABEL_44:
            strcpy(s, "fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");
            strcat(byte_617290, s);

            goto LABEL_45;
        }
        LABEL_43:
        if ( (flags & FLAG_ENVMAP) == 0 )
            goto LABEL_45;
        goto LABEL_44;
    }
    if ( !v2 )
    {
        snprintf(
                s,
                0x200u,
                "fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * DetailTiling, -0.5).xyz * 2.0, Out_Color.w);");
        LABEL_42:
        strcat(byte_617290, s);

        goto LABEL_43;
    }

    strcpy(s, "vec2 waterDetailMod = vec2(0.2, 0.6);");
    strcat(byte_617290, s);

    strcpy(s, "vec2 waterDetailMod2 = vec2(0.3, 0.5);");
    strcat(byte_617290, s);

    snprintf(
            s,
            0x200u,
            "float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;");
    strcat(byte_617290, s);

    strcpy(s, "vec4 lightBlueColor = vec4(0.5, 0.5, 1.0, 0.5);");
    strcat(byte_617290, s);

    strcpy(s, "vec4 lightBlueColor2 = vec4(0.7, 0.7, 1.0, 0.9);");
    strcat(byte_617290, s);

    // strcpy(s, "fcolor *= vec4(lightBlueColor.xyz * Out_Color.xyz * waterDetail * 1.1, lightBlueColor.w * Out_Color.w);");
    // strcat(byte_617290, s);

    // strcpy(s, "Out_WaterAlphaBlend -= 0.5;");
    // strcat(byte_617290, s);



    strcpy(s, "vec4 reflectColour = lightBlueColor * texture2D(EnvMap, Out_WaterDetail2);");
    strcat(byte_617290, s);

    strcpy(s, "vec4 refractColour = lightBlueColor2 * texture2D(Diffuse, Out_WaterDetail2);");
    strcat(byte_617290, s);

    strcpy(s, "fcolor = mix(reflectColour, refractColour, 0.7);");
    strcat(byte_617290, s);

    strcpy(s, "fcolor.a = 0.7;");
    strcat(byte_617290, s);

    if ( (flags & 0x40) != 0 )
        goto LABEL_44;
    LABEL_45:
    if ( (flags & 0x1000000) != 0 )
    {
        strcpy(s, "vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);");
        strcat(byte_617290, s);

        strcpy(s, "ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);");
        strcat(byte_617290, s);

        strcpy(s, "lowp vec4 ReflTexture =  texture2D(EnvMap, ReflPos);");
        strcat(byte_617290, s);

        strcpy(s, "float newEnvMapCoef = EnvMapCoefficient + 0.14;");
        strcat(byte_617290, s);

        strcpy(s, "fcolor.xyz = mix(fcolor.xyz,ReflTexture.xyz, newEnvMapCoef);");
        strcat(byte_617290, s);

        strcpy(s, "fcolor.w += ReflTexture.b * 0.125;");
        strcat(byte_617290, s);

    }
    if ( !*(char*)(RQCaps + 8) )
    {
        v3 = (flags & 0x2000) == 0;
        if ( (flags & 0x2000) != 0 )
            v3 = (flags & 0x1000040) == 0;
        if ( !v3 )
        {
            strcpy(s, "fcolor.xyz += Out_Spec;");
            strcat(byte_617290, s);

        }
        if ( (flags & 0x400) != 0 )
        {
            strcpy(s, "fcolor.xyz = mix(fcolor.xyz, fogColorNew.xyz, Out_FogAmt);");
            strcat(byte_617290, s);

        }
    }
    if ( (flags & 0x4000000) != 0 )
    {
        strcpy(s, "fcolor.xyz += fcolor.xyz * 0.5;");
        strcat(byte_617290, s);

    }
    strcpy(s, "gl_FragColor = fcolor;");
    strcat(byte_617290, s);

    if ( flags << 31 )
    {
        strcpy(s, "/*ATBEGIN*/");
        strcat(byte_617290, s);

        if ( *(int *)(g_libGTASA + 6151472) == 9 && (flags & 0x20) != 0 )
        {
            if ( (flags & 0x800) != 0 )
            {
                strcpy(s, "if (diffuseColor.a < 0.8) { discard; }");
                goto LABEL_70;
            }
            if ( (flags & 0x200) == 0 )
            {
                strcpy(s, "if (diffuseColor.a < 0.2) { discard; }");
                LABEL_70:
                strcat(byte_617290, s);

                strcpy(s, "/*ATEND*/");
                strcat(byte_617290, s);

                goto LABEL_71;
            }
            strcpy(s, "gl_FragColor.a = Out_Color.a;");
            strcat(byte_617290, s);
            v4 = strlen(byte_617290);
            v5 = "if (diffuseColor.a < 0.5) { discard; }";
        }
        else
        {
            if ( (flags & 0x800) != 0 )
            {
                strcpy(s, "if (gl_FragColor.a < 0.8) { discard; }");
                goto LABEL_70;
            }
            if ( (flags & 0x200) == 0 )
            {
                strcpy(s, "if (gl_FragColor.a < 0.2) { discard; }");
                goto LABEL_70;
            }
            strcpy(s, "if (gl_FragColor.a < 0.5) { discard; }");
            strcat(byte_617290, s);
            v4 = strlen(byte_617290);
            v5 = "gl_FragColor.a = Out_Color.a;";
        }
        strcpy(s, v5);
        goto LABEL_70;
    }
    LABEL_71:
    if ( (flags & 4) != 0 )
    {
        strcpy(s, "gl_FragColor.a *= AlphaModulate;");
        strcat(byte_617290, s);

    }
    strcpy(s, "}");
    strcat(byte_617290, s);

    //Log::print("build pixel shader");
    Log(byte_617290);
    //Log::print("end build pixel shader");
}

void (*BuildVertexSource)(unsigned int flags);
void BuildVertexSource_hook(unsigned int flags)
{
    int v2; // r8
    int v3; // r9
    int v4; // r0
    int v5; // r11
    size_t v6; // r0
    const char *v7; // r2
    const char *v8; // r3
    const char *v9; // r8
    int v10; // r0
    int v12; // [sp+18h] [bp-238h]
    char s[512]; // [sp+30h] [bp-220h] BYREF
    int v14; // [sp+230h] [bp-20h]

    byte_619294[0] = 0;
    snprintf(s, 0x200u, "#version 100\n");
    strcat(byte_619294, s);

    snprintf(s, 0x200u, "precision highp float;");
    strcat(byte_619294, s);

    snprintf(s, 0x200u, "uniform mat4 ProjMatrix;");
    strcat(byte_619294, s);

    snprintf(s, 0x200u, "uniform mat4 ViewMatrix;");
    strcat(byte_619294, s);

    snprintf(s, 0x200u, "uniform mat4 ObjMatrix;");
    strcat(byte_619294, s);

    if ( (flags & 2) != 0 )
    {
        snprintf(s, 0x200u, "uniform lowp vec3 AmbientLightColor;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "uniform lowp vec4 MaterialEmissive;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "uniform lowp vec4 MaterialAmbient;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "uniform lowp vec4 MaterialDiffuse;");
        strcat(byte_619294, s);

        if ( (flags & 0x2000) != 0 )
        {
            snprintf(s, 0x200u, "uniform lowp vec3 DirLightDiffuseColor;");
            strcat(byte_619294, s);

            snprintf(s, 0x200u, "uniform vec3 DirLightDirection;");
            strcat(byte_619294, s);

            if ( (flags & 0x1180) != 0 && *(int *)(g_libGTASA + 9361520) == 3 )
            {
                snprintf(s, 0x200u, "uniform vec3 DirBackLightDirection;");
                strcat(byte_619294, s);

            }
        }
        if ( (flags & 0x4000) != 0 )
        {
            snprintf(s, 0x200u, "uniform lowp vec3 DirLight2DiffuseColor;");
            strcat(byte_619294, s);

            snprintf(s, 0x200u, "uniform vec3 DirLight2Direction;");
            strcat(byte_619294, s);

        }
        if ( (flags & 0x8000) != 0 )
        {
            snprintf(s, 0x200u, "uniform lowp vec3 DirLight3DiffuseColor;");
            strcat(byte_619294, s);

            snprintf(s, 0x200u, "uniform vec3 DirLight3Direction;");
            strcat(byte_619294, s);

        }
    }
    snprintf(s, 0x200u, "attribute vec3 Position;");
    strcat(byte_619294, s);

    snprintf(s, 0x200u, "attribute vec3 Normal;");
    strcat(byte_619294, s);

    if ( (flags & 0x20) != 0 )
    {
        if ( (flags & 0x40000) != 0 )
            snprintf(s, 0x200u, "attribute vec4 TexCoord0;");
        else
            snprintf(s, 0x200u, "attribute vec2 TexCoord0;");
        strcat(byte_619294, s);

    }
    snprintf(s, 0x200u, "attribute vec4 GlobalColor;");
    strcat(byte_619294, s);

    if ( (flags & 0x180) != 0 )
    {
        snprintf(s, 0x200u, "attribute vec4 BoneWeight;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "attribute vec4 BoneIndices;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "uniform highp vec4 Bones[%d];", 3 * *(int *)(g_libGTASA + 6386248));
        strcat(byte_619294, s);

    }
    if ( (flags & 0x20) != 0 )
    {
        snprintf(s, 0x200u, "varying mediump vec2 Out_Tex0;");
        strcat(byte_619294, s);

    }
    if ( (flags & 0x2000000) != 0 )
    {
        snprintf(s, 0x200u, "uniform mat3 NormalMatrix;");
        strcat(byte_619294, s);

    }
    v2 = flags & 0x1000040;
    if ( (flags & 0x1000040) != 0 )
    {
        snprintf(s, 0x200u, "uniform lowp float EnvMapCoefficient;");
        strcat(byte_619294, s);

        if ( (flags & 0x40) != 0 )
            snprintf(s, 0x200u, "varying mediump vec2 Out_Tex1;");
        else
            snprintf(s, 0x200u, "varying mediump vec3 Out_Refl;");
        strcat(byte_619294, s);

    }
    if ( (flags & 0x9880640) != 0 )
    {
        snprintf(s, 0x200u, "uniform vec3 CameraPosition;");
        strcat(byte_619294, s);

    }
    if ( (flags & 0x400) != 0 )
    {
        snprintf(s, 0x200u, "varying mediump float Out_FogAmt;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "uniform vec3 FogDistances;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "varying mediump vec4 Out_ClipSpace;");
        strcat(byte_619294, s);

    }
    if ( (flags & FLAG_WATER) != 0 )
    {
        snprintf(s, 0x200u, "uniform vec3 WaterSpecs;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "varying mediump vec2 Out_WaterDetail;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "varying mediump vec2 Out_WaterDetail2;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "varying mediump float Out_WaterAlphaBlend;");
        strcat(byte_619294, s);

    }
    if ( (flags & 0x100000) != 0 )
    {
        snprintf(s, 0x200u, "attribute vec4 Color2;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "uniform lowp float ColorInterp;");
        strcat(byte_619294, s);

    }
    if ( (flags & 0x12) != 0 )
    {
        snprintf(s, 0x200u, "varying lowp vec4 Out_Color;");
        strcat(byte_619294, s);

    }
    v3 = v2 == 0;
    v4 = 0;
    v5 = flags & 0x1000040;
    if ( (flags & 0x2000) == 0 )
        v4 = 1;
    v12 = v4;
    if ( (flags & 0x2000) != 0 && v2 )
    {
        snprintf(s, 0x200u, "varying lowp vec3 Out_Spec;");
        strcat(byte_619294, s);

    }
    snprintf(s, 0x200u, "void main() {");
    strcat(byte_619294, s);

    if ( (flags & 0x180) != 0 )
    {
        snprintf(s, 0x200u, "ivec4 BlendIndexArray = ivec4(BoneIndices);");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "mat4 BoneToLocal;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[0] = Bones[BlendIndexArray.x*3] * BoneWeight.x;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[1] = Bones[BlendIndexArray.x*3+1] * BoneWeight.x;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[2] = Bones[BlendIndexArray.x*3+2] * BoneWeight.x;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[3] = vec4(0.0,0.0,0.0,1.0);");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.y*3] * BoneWeight.y;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.y*3+1] * BoneWeight.y;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.y*3+2] * BoneWeight.y;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.z*3] * BoneWeight.z;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.z*3+1] * BoneWeight.z;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.z*3+2] * BoneWeight.z;");
        strcat(byte_619294, s);

        if ( (flags & 0x100) != 0 )
        {
            snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.w*3] * BoneWeight.w;");
            strcat(byte_619294, s);

            snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.w*3+1] * BoneWeight.w;");
            strcat(byte_619294, s);

            snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.w*3+2] * BoneWeight.w;");
            strcat(byte_619294, s);

        }
        snprintf(s, 0x200u, "vec4 WorldPos = ObjMatrix * (vec4(Position,1.0) * BoneToLocal);");
    }
    else
    {
        snprintf(s, 0x200u, "vec4 WorldPos = ObjMatrix * vec4(Position,1.0);");
    }
    strcat(byte_619294, s);

    if ( (flags & 0x8000000) != 0 )
    {
        snprintf(s, 0x200u, "vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "float coefOffset = 1.0 - clamp(length(ReflVector.xy) / 45.0, 0.0, 1.0);");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "coefOffset = -1000.0 * coefOffset;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "vec3 ReflPos = normalize(ReflVector);");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
        strcat(byte_619294, s);
        v6 = strlen(byte_619294);
        v7 = "gl_Position = vec4(ReflPos.xy, coefOffset + length(ReflVector) * 0.002, 1.0);";
        LABEL_52:
        snprintf(s, 0x200u, "%s", v7);
        goto LABEL_53;
    }
    if ( (flags & 0x800000) != 0 )
    {
        snprintf(s, 0x200u, "vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "vec3 ReflPos = normalize(ReflVector);");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
        strcat(byte_619294, s);
        v6 = strlen(byte_619294);
        v7 = "gl_Position = vec4(ReflPos.xy, length(ReflVector) * 0.002, 1.0);";
        goto LABEL_52;
    }
    snprintf(s, 0x200u, "vec4 ViewPos = ViewMatrix * WorldPos;");
    strcat(byte_619294, s);

    snprintf(s, 0x200u, "gl_Position = ProjMatrix * ViewPos;");
    strcat(byte_619294, s);

    if ( (flags & 0x400) == 0 )
        goto LABEL_54;
    snprintf(s, 0x200u, "Out_ClipSpace = gl_Position;");
    LABEL_53:
    strcat(byte_619294, s);

    LABEL_54:
    if ( (flags & 2) != 0 )
    {
        if ( (flags & 0x201) == 513 && (flags & 0xE000) != 0 )
        {
            snprintf(s, 0x200u, "vec3 WorldNormal = normalize(vec3(WorldPos.xy - CameraPosition.xy, 0.0001)) * 0.85;");
        }
        else if ( (flags & 0x180) != 0 )
        {
            snprintf(s, 0x200u, "vec3 WorldNormal = mat3(ObjMatrix) * (Normal * mat3(BoneToLocal));");
        }
        else
        {
            snprintf(s, 0x200u, "vec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;");
        }
        goto LABEL_63;
    }
    if ( v5 )
    {
        snprintf(s, 0x200u, "vec3 WorldNormal = vec3(0.0, 0.0, 0.0);");
        LABEL_63:
        strcat(byte_619294, s);

    }
    if ( !*(char *)(RQCaps + 8) && (flags & 0x400) != 0 )
    {
        snprintf(
                s,
                0x200u,
                "Out_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 1.0);");
        strcat(byte_619294, s);

    }
    if ( (flags & 0x20) != 0 )
    {
        if ( (flags & 0x40000) != 0 )
        {
            v8 = "TexCoord0.xy / TexCoord0.w";
        }
        else
        {
            v8 = "TexCoord0";
            if ( (flags & 0x20000) != 0 )
                v8 = "TexCoord0 / 512.0";
        }
        if ( (flags & 0x2000000) != 0 )
            snprintf(s, 0x200u, "Out_Tex0 = (NormalMatrix * vec3(%s, 1.0)).xy;", v8);
        else
            snprintf(s, 0x200u, "Out_Tex0 = %s;", v8);
        strcat(byte_619294, s);

    }
    if ( v5 )
    {
        snprintf(s, 0x200u, "vec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "reflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;");
        strcat(byte_619294, s);

        if ( (flags & 0x1000000) != 0 )
            snprintf(s, 0x200u, "Out_Refl = reflVector;");
        else
            snprintf(s, 0x200u, "Out_Tex1 = vec2(length(reflVector.xy), (reflVector.z * 0.5) + 0.25);");
        strcat(byte_619294, s);

    }
    if ( (flags & 0x100000) != 0 )
    {
        snprintf(s, 0x200u, "lowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);");
        strcat(byte_619294, s);

        v9 = "InterpColor";
    }
    else
    {
        v9 = "GlobalColor";
    }
    if ( (flags & 2) != 0 )
    {
        snprintf(s, 0x200u, "vec3 Out_LightingColor;");
        strcat(byte_619294, s);

        if ( (flags & 8) != 0 )
        {
            if ( (flags & 0x200) != 0 )
                snprintf(s, 0x200u, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz * 1.5;");
            else
                snprintf(s, 0x200u, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + %s.xyz;", v9);
        }
        else
        {
            snprintf(s, 0x200u, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + MaterialEmissive.xyz;");
        }
        strcat(byte_619294, s);

        if ( (flags & 0xE000) != 0 )
        {
            if ( (flags & 0x2000) != 0 )
            {
                if ( (flags & 0x1180) != 0 && *(int*)(g_libGTASA + 9361520) == 3 )
                    snprintf(
                            s,
                            0x200u,
                            "Out_LightingColor += (max(dot(DirLightDirection, WorldNormal), 0.0) + max(dot(DirBackLightDirection, WorldNo"
                            "rmal), 0.0)) * DirLightDiffuseColor;");
                else
                    snprintf(
                            s,
                            0x200u,
                            "Out_LightingColor += max(dot(DirLightDirection, WorldNormal), 0.0) * DirLightDiffuseColor;");
                strcat(byte_619294, s);

                if ( (flags & 0x4000) == 0 )
                {
                    LABEL_96:
                    if ( (flags & 0x8000) == 0 )
                        goto LABEL_98;
                    goto LABEL_97;
                }
            }
            else if ( (flags & 0x4000) == 0 )
            {
                goto LABEL_96;
            }
            snprintf(
                    s,
                    0x200u,
                    "Out_LightingColor += max(dot(DirLight2Direction, WorldNormal), 0.0) * DirLight2DiffuseColor;");
            strcat(byte_619294, s);

            if ( (flags & 0x8000) != 0 )
            {
                LABEL_97:
                snprintf(
                        s,
                        0x200u,
                        "Out_LightingColor += max(dot(DirLight3Direction, WorldNormal), 0.0) * DirLight3DiffuseColor;");
                strcat(byte_619294, s);

                goto LABEL_98;
            }
        }
        LABEL_98:
        if ( (flags & 0x12) != 0 )
        {
            if ( (flags & 0x10) != 0 )
                snprintf(
                        s,
                        0x200u,
                        "Out_Color = vec4((Out_LightingColor.xyz + %s.xyz * 1.5) * MaterialDiffuse.xyz, (MaterialAmbient.w) * %s.w);",
                        v9,
                        v9);
            else
                snprintf(s, 0x200u, "Out_Color = vec4(Out_LightingColor * MaterialDiffuse.xyz, MaterialAmbient.w * %s.w);", v9);
            strcat(byte_619294, s);

            snprintf(s, 0x200u, "Out_Color = clamp(Out_Color, 0.0, 1.0);");
            strcat(byte_619294, s);

        }
        goto LABEL_106;
    }
    if ( (flags & 0x12) != 0 )
    {
        snprintf(s, 0x200u, "Out_Color = %s;", v9);
        strcat(byte_619294, s);

    }
    LABEL_106:
    v10 = *(uint8_t *)(g_libGTASA + 6386264);
    if ( *(uint8_t *)(g_libGTASA + 6386264) )
        v10 = 1;
    if ( !(v10 | v12 | v3) )
    {
        snprintf(
                s,
                0x200u,
                "float specAmt = max(pow(dot(reflVector, DirLightDirection), %.1f), 0.0) * EnvMapCoefficient * 1.3;");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "Out_Spec = specAmt * DirLightDiffuseColor;");
        strcat(byte_619294, s);

    }
    if ( (flags & FLAG_WATER) != 0 )
    {
        snprintf(s, 0x200u, "Out_WaterDetail = (Out_Tex0 * 4.0) + vec2(WaterSpecs.x * -0.3, WaterSpecs.x * 0.21);");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "Out_WaterDetail2 = (Out_Tex0 * -8.0) + vec2(WaterSpecs.x * 0.12, WaterSpecs.x * -0.05);");
        strcat(byte_619294, s);

        snprintf(s, 0x200u, "Out_WaterAlphaBlend = distance(WorldPos.xy, CameraPosition.xy) * WaterSpecs.y;");
        strcat(byte_619294, s);

    }
    snprintf(s, 0x200u, (const char *)"}");
    strcat(byte_619294, s);
}

void (*glCompressedTexImage)(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
void glCompressedTexImage_hook(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
    if (level == 0 || (width >= 4 && height >= 4) || (format != 0x8C01 && format != 0x8C02)) {
        glCompressedTexImage(target, level, format, width, height, border, imageSize, data);
    }
}

#define GREEN_TEXTURE_ID 14
void (*emu_TextureSetDetailTexture)(void* texture, unsigned int tilingScale);
void emu_TextureSetDetailTexture_hook(void* texture, unsigned int tilingScale)
{
    if(!texture)
    {
        noDetail:
        emu_TextureSetDetailTexture(NULL, 0);
        return;
    }

    if(texture == *(void**)(**(int**)(*_detailTexturesDataRasterParent + 4 * (GREEN_TEXTURE_ID-1)) + *RasterExtOffset))
    {
        *textureDetail = 0;
        goto noDetail;
    }

    emu_TextureSetDetailTexture(texture, tilingScale);
    *textureDetail = 1;
}

bool bEnableSkyGFX = true;
void InitialiseShaders()
{
    if(!bEnableSkyGFX) {
        return;
    }

    makeNOP(g_libGTASA + 0x53E4EE, 2);               // emu_SetAltRTContrast
    makeNOP(g_libGTASA + 0x53E35E, 2);               // emu_SetAltRTGrading

    byte_617258 = (char*)(g_libGTASA + 0x617258);
    byte_619294 = (char*)(g_libGTASA + 0x619294);
    byte_619294[0] = '\0';
    byte_617290 = (char*)(g_libGTASA + 0x617290);
    byte_617258 = (char*)(g_libGTASA + 0x617258);

    RQMaxBones = (int*)(g_libGTASA + 0x617248);

    RQCaps = (RQCapabilities *)g_libGTASA + 0x617250;

    _detailTexturesDataRasterParent = (int*)(g_libGTASA + 0x61B878); // 61B878 ?? ?? ?? ??    _detailTexturesDataRasterParent
    RasterExtOffset = (int*)(g_libGTASA + 0x611844); // 611844 ?? ?? ?? ??    RasterExtOffset
    textureDetail = (int*)(g_libGTASA + 0x6118D0); // 6118D0 ?? ?? ?? ??    textureDetail

    // Fix green details texture
    // Memory::makeNOP(g_libGTASA + 0x1878A4, 4);
    // Memory::makeNOP(g_libGTASA + 0x1876E6, 4);
    // pGTALibHook->install(g_libGTASA + 0x194400, (uintptr_t)emu_TextureSetDetailTexture_hook, (uintptr_t*)&emu_TextureSetDetailTexture);

    // disable detail texture
    // Memory::makeRET(g_libGTASA + 0x00194400);

    // fix mip maps
    installPLTHook(g_libGTASA + 0x5CDB70, (uintptr_t)glCompressedTexImage_hook, (uintptr_t*)&glCompressedTexImage);


    // todo
    /*
    pGTALibHook->install(g_libGTASA + 0x1A7A58, (uintptr_t)ES2Texture__UploadMip_hook, (uintptr_t*)&ES2Texture__UploadMip);
installHook((unsigned int *)(g_libGTASA + 0x1A921C), (int)RQIndexBuffer::Set, RQIndexBuffer::Set);
  installHook((unsigned int *)(g_libGTASA + 0x1A21B4), (int)RQ_Command_rqIndexBufferCPU, RQ_Command_rqIndexBufferCPU);
  installHook((unsigned int *)(g_libGTASA + 0x1A8E20), (int)RQVertexBuffer::Set, RQVertexBuffer::Set);
  return installHook(
           (unsigned int *)(g_libGTASA + 0x1A1F40),
           (int)RQ_Command_rqVertexBufferCPU,
           RQ_Command_rqVertexBufferCPU);
    */

    // graphic shaders
    //InlineHook(g_libGTASA, 0x1A40B4, &BuildPixelSource_hook, &BuildPixelSource);
    //InlineHook(g_libGTASA, 0x1A4AB4, &BuildVertexSource_hook, &BuildVertexSource);
}