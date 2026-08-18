#include <main.h>
#include <gui/gui.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <shadowhook.h>

#include "RQShader.h"
#include "patch.h"

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

char* pixel = 0; //pxl
char* byte_617258 = 0;

char* vertex = 0; //vertex

#define PXL_SOURCE(str) \
		strcat(byte_617290, str) \

#define VTX_SOURCE(str) \
		strcat(byte_619294, str) \


void (*BuildPixelSource)(unsigned int flags);
void BuildPixelSource_hook(unsigned int flags)
{
    char s[512]; // [sp+10h] [bp-220h] BYREF


    pixel[0] = 0;

    strcpy(s, "precision mediump float;");
    strcat(pixel, s);
    if ((flags & 0x20) != 0 )
    {
        strcpy(s, "uniform sampler2D Diffuse;");
        strcat(pixel, s);

        strcpy(s, "varying mediump vec2 Out_Tex0;");
        strcat(pixel, s);
    }
    if ((flags & 0x1000040) != 0 )
    {
        strcpy(s, "varying mediump vec3 Out_Refl;");
        if ((flags & 0x40) != 0 )
            strcpy(s, "varying mediump vec2 Out_Tex1;");

        strcat(pixel, s);

        strcpy(s, "uniform sampler2D EnvMap;");
        strcat(pixel, s);

        strcpy(s, "uniform lowp float EnvMapCoefficient;");
        //strcat(pixel, s);
    }
    else
    {
        if ((flags & 0x10000) == 0 )
            goto LABEL_10;
        strcpy(s, "uniform sampler2D EnvMap;");
        strcat(pixel, s);

        strcpy(s, "uniform float DetailTiling;");
    }
    strcat(pixel, s);
    LABEL_10:
    if ((flags & 0x400) != 0 )
    {
        strcpy(s, "varying mediump float Out_FogAmt;");
        strcat(pixel, s);

        strcpy(s, "uniform lowp vec3 FogColor;");
        strcat(pixel, s);
    }
    if ((flags & 0x12) != 0 ) {
        strcpy(s, "varying lowp vec4 Out_Color;");
        strcat(pixel, s);
    }
    unsigned int v26 = ((flags & 0x2000) >> 0xD) ^ 1 | ((flags & 0x1000040) == 0);
    if ( !v26 ) {
        strcpy(s, "varying lowp vec3 Out_Spec;");
        strcat(pixel, s);
    }

    if ((flags & 4) != 0 ) {
        strcpy(s, "uniform lowp float AlphaModulate;");
        strcat(pixel, s);
    }
    if ((flags & 0x80000) != 0 )
    {
        strcpy(s, "varying mediump vec2 Out_WaterDetail;");
        strcat(pixel, s);

        strcpy(s, "varying mediump vec2 Out_WaterDetail2;");
        strcat(pixel, s);

        strcpy(s, "varying mediump float Out_WaterAlphaBlend;");
        strcat(pixel, s);
    }
    strcpy(s, "void main()");
    strcat(pixel, s);

    strcpy(s, (const char *)"{");
    strcat(pixel, s);

    strcpy(s, "lowp vec4 fcolor;");
    strcat(pixel, s);

    if ((flags & 0x20) == 0 )
    {
        if ( (flags & 0x12) != 0 )
            strcpy(s, "fcolor = Out_Color;");
        else
            strcpy(s, "fcolor = 0.0;");
        goto LABEL_35;
    }

    strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -0.5);");
    if ((flags & 0x800) != 0 )
        strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -1.5);");

    strcat(pixel, s);

    strcpy(s, "fcolor = diffuseColor;");
    strcat(pixel, s);

    if ((flags & 0x12) != 0 )
    {
        if ((flags & 0x10000) != 0 )
        {
            if ((flags & 0x80000) == 0 )
            {
                snprintf(
                        s,
                        0x200u,
                        "fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * DetailTiling, -0.5).xyz * 2.0, Out_Color.w);");
                LABEL_35:
                strcat(pixel, s);
                goto LABEL_36;
            }
            snprintf(
                    s,
                    0x200u,
                    "float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;");
            strcat(pixel, s);

            strcpy(s, "fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);");
            strcat(pixel, s);

            LABEL_34:
            strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");

            goto LABEL_35;
        }
        strcpy(s, "fcolor *= Out_Color;");
        strcat(pixel, s);
    }
    if ((flags & 0x80000) != 0 )
        goto LABEL_34;
    LABEL_36:
    if ((flags & 0x40) != 0 ) {
        strcpy(s, "fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");
        strcat(pixel, s);
    }
    if ((flags & 0x1000000) != 0 )
    {
        strcpy(s, "vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);");
        strcat(pixel, s);

        strcpy(s, "ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);");
        strcat(pixel, s);

        strcpy(s, "lowp vec4 ReflTexture =  texture2D(EnvMap, ReflPos);");
        strcat(pixel, s);
        strcpy(s, "float newEnvMapCoef = EnvMapCoefficient + 0.17;");
        strcat(pixel, s);

        strcpy(s, "fcolor.xyz = mix(fcolor.xyz,ReflTexture.xyz, newEnvMapCoef);");
        strcat(pixel, s);

        strcpy(s, "fcolor.w += ReflTexture.b * 0.125;");
        strcat(pixel, s);
    }
    if ( !*(uint8_t *)(g_libGTASA + (VER_x32 ? 0x6B8BA4:0x896138)) )
    {
        if ( !v26 ) {
            strcpy(s, "fcolor.xyz += Out_Spec;");
            strcat(pixel, s);
        }
        if ((flags & 0x400) != 0 ) {
            strcpy(s, "fcolor.xyz = mix(fcolor.xyz, FogColor, Out_FogAmt);");
            strcat(pixel, s);
        }
    }
    if ((flags & 0x4000000) != 0 ) {
        strcpy(s, "fcolor.xyz += fcolor.xyz * 0.5;");
        strcat(pixel, s);
    }
    strcpy(s, "gl_FragColor = fcolor;");
    strcat(pixel, s);
    if ( flags << 0x1F )
    {
        strcpy(s, "/*ATBEGIN*/");
        strcat(pixel, s);
        if ((flags & 0x800) != 0 )
        {
            strcpy(s, "if (gl_FragColor.a < 0.8) { discard; }");
        }
        else
        {
            if ((flags & 0x200) != 0 )
            {

                strcpy(s, "if (gl_FragColor.a < 0.5) { discard; }");
                strcat(pixel, s);

                strcpy(s, "gl_FragColor.a = Out_Color.a;");
                goto LABEL_54;
            }
            strcpy(s, "if (gl_FragColor.a < 0.2) { discard; }");
        }

        LABEL_54:
        strcat(pixel, s);

        strcpy(s, "/*ATEND*/");
        strcat(pixel, s);
    }
    if ((flags & 4) != 0 ) {
        strcpy(s, "gl_FragColor.a *= AlphaModulate;");
        strcat(pixel, s);
    }


    strcpy(s, "}");
    strcat(pixel, s);

    //Log::print("build pixel shader");
    Log(pixel);
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

    vertex[0] = 0;


    snprintf(s, 0x200u, "#version 100\n");
    strcat(vertex, s);

    snprintf(s, 0x200u, "precision highp float;\n");
    strcat(vertex, s);

    snprintf(s, 0x200u, "uniform mat4 ProjMatrix;\n");
    strcat(vertex, s);

    snprintf(s, 0x200u, "uniform mat4 ViewMatrix;\n");
    strcat(vertex, s);

    snprintf(s, 0x200u, "uniform mat4 ObjMatrix;\n");
    strcat(vertex, s);

    if ((flags & 2) != 0 )
    {
        snprintf(s, 0x200u, "uniform lowp vec3 AmbientLightColor;\n");
        strcat(vertex, s);

        snprintf(s, 0x200u, "uniform lowp vec4 MaterialEmissive;\n");
        strcat(vertex, s);

        snprintf(s, 0x200u, "uniform lowp vec4 MaterialAmbient;\n");
        strcat(vertex, s);

        snprintf(s, 0x200u, "uniform lowp vec4 MaterialDiffuse;\n");
        strcat(vertex, s);
        if ((flags & 0x2000) != 0 )
        {
            snprintf(s, 0x200u, "uniform lowp vec3 DirLightDiffuseColor;\n");
            strcat(vertex, s);

            snprintf(s, 0x200u, "uniform vec3 DirLightDirection;\n");
            strcat(vertex, s);

            v10 = CHook::CallFunction<int>("_Z22GetMobileEffectSettingv");
            if ((flags & 0x1180) != 0 && v10 == 3 ) {
                snprintf(s, 0x200u, "uniform vec3 DirBackLightDirection;");
                strcat(vertex, s);
            }
        }
        if ((flags & 0x4000) != 0 )
        {
            snprintf(s, 0x200u, "uniform lowp vec3 DirLight2DiffuseColor;\n");
            strcat(vertex, s);

            snprintf(s, 0x200u, "uniform vec3 DirLight2Direction;\n");
            strcat(vertex, s);
        }
        if ((flags & 0x8000) != 0 )
        {
            snprintf(s, 0x200u, "uniform lowp vec3 DirLight3DiffuseColor;\n");
            strcat(vertex, s);

            snprintf(s, 0x200u, "uniform vec3 DirLight3Direction;\n");
            strcat(vertex, s);
        }
    }
    snprintf(s, 0x200u, "attribute vec3 Position;\n");
    strcat(vertex, s);

    snprintf(s, 0x200u, "attribute vec3 Normal;\n");
    strcat(vertex, s);

    if ((flags & 0x20) != 0 )
    {
        snprintf(s, 0x200u, "attribute vec2 TexCoord0;\n");
        if ((flags & 0x40000) != 0 )
            snprintf(s, 0x200u, "attribute vec4 TexCoord0;\n");
        strcat(vertex, s);
    }

    snprintf(s, 0x200u, "attribute vec4 GlobalColor;\n");
    strcat(vertex, s);
    if ((flags & 0x180) != 0 )
    {
        snprintf(s, 0x200u, "attribute vec4 BoneWeight;\n");
        strcat(vertex, s);

        snprintf(s, 0x200u, "attribute vec4 BoneIndices;\n");
        strcat(vertex, s);

        snprintf(s, 0x200u, "uniform highp vec4 Bones[%d];\n", 3 * *(int *)(g_libGTASA + (VER_x32 ? 0x6B8BAC:0x896140)));
        strcat(vertex, s);

    }
    if ((flags & 0x20) != 0 ) {
        snprintf(s, 0x200u, "varying mediump vec2 Out_Tex0;\n");
        strcat(vertex, s);
    }
    if ((flags & 0x2000000) != 0 ) {
        snprintf(s, 0x200u, "uniform mat3 NormalMatrix;\n");
        strcat(vertex, s);
    }
    unsigned int v19 = flags & 0x1000040;
    if ((flags & 0x1000040) != 0 ) {
        snprintf(s, 0x200u, "uniform lowp float EnvMapCoefficient;\n");
        strcat(vertex, s);
    }
    const char *v20;
    if ((flags & 0x40) != 0 )
    {
        v20 = "varying mediump vec2 Out_Tex1;\n";
    }
    else
    {
        if ((flags & 0x1000000) == 0 )
            goto LABEL_27;
        v20 = "varying mediump vec3 Out_Refl;\n";
    }
    strcat(vertex, v20);
    LABEL_27:
    if ((flags & 0x1880640) != 0 ) {
        snprintf(s, 0x200u, "uniform vec3 CameraPosition;\n");
        strcat(vertex, s);
    }
    if ((flags & 0x400) != 0 )
    {

        snprintf(s, 0x200u, "varying mediump float Out_FogAmt;");
        strcat(vertex, s);


        snprintf(s, 0x200u, "uniform vec3 FogDistances;");
        strcat(vertex, s);
    }
    if ((flags & 0x80000) != 0 )
    {
        snprintf(s, 0x200u, "uniform vec3 WaterSpecs;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "varying mediump vec2 Out_WaterDetail;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "varying mediump vec2 Out_WaterDetail2;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "varying mediump float Out_WaterAlphaBlend;");
        strcat(vertex, s);
    }
    if ((flags & 0x100000) != 0 )
    {
        snprintf(s, 0x200u, "attribute vec4 Color2;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "uniform lowp float ColorInterp;");
        strcat(vertex, s);
    }
    if ((flags & 0x12) != 0 ) {
        snprintf(s, 0x200u, "varying lowp vec4 Out_Color;");
        strcat(vertex, s);
    }
    if ((flags & 0x2000) != 0 && v19 ) {
        snprintf(s, 0x200u, "varying lowp vec3 Out_Spec;");
        strcat(vertex, s);
    }

    snprintf(s, 0x200u, "void main() {");
    strcat(vertex, s);
    if ((flags & 0x180) != 0 )
    {
        snprintf(s, 0x200u, "ivec4 BlendIndexArray = ivec4(BoneIndices);");
        strcat(vertex, s);

        snprintf(s, 0x200u, "mat4 BoneToLocal;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[0] = Bones[BlendIndexArray.x*3] * BoneWeight.x;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[1] = Bones[BlendIndexArray.x*3+1] * BoneWeight.x;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[2] = Bones[BlendIndexArray.x*3+2] * BoneWeight.x;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[3] = vec4(0.0,0.0,0.0,1.0);");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.y*3] * BoneWeight.y;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.y*3+1] * BoneWeight.y;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.y*3+2] * BoneWeight.y;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.z*3] * BoneWeight.z;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.z*3+1] * BoneWeight.z;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.z*3+2] * BoneWeight.z;");
        strcat(vertex, s);
        if ((flags & 0x100) != 0 )
        {
            snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.w*3] * BoneWeight.w;");
            strcat(vertex, s);

            snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.w*3+1] * BoneWeight.w;");
            strcat(vertex, s);

            snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.w*3+2] * BoneWeight.w;");
            strcat(vertex, s);
        }

        snprintf(s, 0x200u, "vec4 WorldPos = ObjMatrix * (vec4(Position,1.0) * BoneToLocal);");
    }
    else
    {
        snprintf(s, 0x200u, "vec4 WorldPos = ObjMatrix * vec4(Position,1.0);");
    }
    strcat(vertex, s);
    if ((flags & 0x800000) != 0 )
    {
        snprintf(s, 0x200u, "vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "vec3 ReflPos = normalize(ReflVector);");
        strcat(vertex, s);


        snprintf(s, 0x200u, "ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
        strcat(vertex, s);

        snprintf(s, 0x200u, "gl_Position = vec4(ReflPos.xy, length(ReflVector) * 0.002, 1.0);");
        strcat(vertex, s);
    }
    else
    {
        snprintf(s, 0x200u, "vec4 ViewPos = ViewMatrix * WorldPos;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "gl_Position = ProjMatrix * ViewPos;");
    }
    strcat(vertex, s);
    if ((flags & 2) != 0 )
    {
        if ((flags & 0x201) == 0x201 && (flags & 0xE000) != 0 )
        {
            strcpy(s, "vec3 WorldNormal = normalize(vec3(WorldPos.xy - CameraPosition.xy, 0.0001)) * 0.85;");

        }
        else
        {
            strcpy(s, "vec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;");

            if ((flags & 0x180) != 0 )
                strcpy(s, "vec3 WorldNormal = mat3(ObjMatrix) * (Normal * mat3(BoneToLocal));");

        }
    }
    else
    {
        if ( !v19 )
            goto LABEL_58;
        strcpy(s, "vec3 WorldNormal = vec3(0.0, 0.0, 0.0);");
    }
    strcat(vertex, s);
    LABEL_58:
    if ((flags & 0x400) != 0 ) {
        snprintf(s, 0x200u,
                 "Out_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 1.0);");
        strcat(vertex, s);
    }
    if ((flags & 0x20) != 0 )
    {
        const char *v47 = "TexCoord0";
        if ((flags & 0x20000) != 0 )
            v47 = "TexCoord0 / 512.0";
        if ((flags & 0x40000) != 0 )
            v47 = "TexCoord0.xy / TexCoord0.w";
        if ((flags & 0x2000000) != 0 )
        {
            snprintf(s, 0x200u, "Out_Tex0 = (NormalMatrix * vec3(%s, 1.0)).xy;", v47);
        }
        else
        {
            snprintf(s, 0x200u, "Out_Tex0 = %s;", v47);
        }
        strcat(vertex, s);
    }
    if ( v19 )
    {
        snprintf(s, 0x200u, "vec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);");
        strcat(vertex, s);

        snprintf(s, 0x200u, "reflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;");
        strcat(vertex, s);

        snprintf(s, 0x200u, "Out_Tex1 = vec2(length(reflVector.xy), (reflVector.z * 0.5) + 0.25);");
        if ((flags & 0x1000000) != 0 )
            snprintf(s, 0x200u, "Out_Refl = reflVector;");

        strcat(vertex, s);
    }
    const char *v58;
    if ((flags & 0x100000) != 0 )
    {
        snprintf(s, 0x200u, "lowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);");
        strcat(vertex, s);

        v58 = "InterpColor";
    }
    else
    {
        v58 = "GlobalColor";
    }
    if ((flags & 2) == 0 )
    {
        if ((flags & 0x12) == 0 )
            goto LABEL_104;

        snprintf(s, 0x200u, "Out_Color = %s;", v58);
        goto LABEL_103;
    }

    snprintf(s, 0x200u, "vec3 ambEmissLight = vec3(0.0, 0.0, 0.0);");
    strcat(vertex, s);

    snprintf(s, 0x200u, "vec3 diffColor = vec3(0.0, 0.0, 0.0);");
    strcat(vertex, s);

    if ((flags & 8) == 0 )
    {
        snprintf(s, 0x200u, "ambEmissLight = AmbientLightColor * MaterialAmbient.xyz + MaterialEmissive.xyz;");
        strcat(vertex, s);
        if ((flags & 0xE000) == 0 )
            goto LABEL_101;
        goto LABEL_85;
    }
    if ((flags & 0xE000) != 0 )
    {

        snprintf(s, 0x200u, "ambEmissLight = AmbientLightColor * MaterialAmbient.xyz + %s.xyz * MaterialDiffuse.xyz;", v58);
        strcat(vertex, s);
        LABEL_85:
        if ((flags & 0x2000) != 0 )
        {
            int v66 = CHook::CallFunction<int>("_Z22GetMobileEffectSettingv");

            snprintf(s, 0x200u, "diffColor += (max(dot(DirLightDirection, WorldNormal), 0.0) + max(dot(DirBackLightDirection, WorldNormal), 0.0)) * DirLightDiffuseColor;");
            bool v68 = v66 != 3 || (flags & 0x1180) == 0;
            if ( v68 )
                snprintf(s, 0x200u, "diffColor += max(dot(DirLightDirection, WorldNormal), 0.0) * DirLightDiffuseColor;");

            strcat(vertex, s);
        }
        if ((flags & 0x4000) != 0 ) {
            snprintf(s, 0x200u, "diffColor += max(dot(DirLight2Direction, WorldNormal), 0.0) * DirLight2DiffuseColor;");
            strcat(vertex, s);
        }
        if ((flags & 0x8000) != 0 )
        {
            snprintf(s, 0x200u, "diffColor += max(dot(DirLight3Direction, WorldNormal), 0.0) * DirLight3DiffuseColor;");
            strcat(vertex, s);
        }
        snprintf(s, 0x200u, "diffColor *= MaterialDiffuse.xyz;");

        strcat(vertex, s);
    }
    if ((flags & 0x200) != 0 )
    {
        snprintf(s, 0x200u, "vec3 vertClamped = clamp(%s.xyz, 0.0, 0.5);", v58);
        strcat(vertex, s);

        snprintf(s, 0x200u, "ambEmissLight = AmbientLightColor * MaterialAmbient.xyz + vertClamped;");
        strcat(vertex, s);
    }
    else
    {
        snprintf(s, 0x200u, "ambEmissLight = AmbientLightColor * MaterialAmbient.xyz + %s.xyz;", v58);
        strcat(vertex, s);
    }

    LABEL_101:
    if ((flags & 0x12) == 0 )
        goto LABEL_104;


    snprintf(s, 0x200u, "Out_Color = vec4(ambEmissLight + diffColor, MaterialAmbient.w * %s.w);", v58);
    strcat(vertex, s);

    snprintf(s, 0x200u, "Out_Color = clamp(Out_Color, 0.0, 1.0);");
    LABEL_103:
    strcat(vertex, s);
    bool v78;
    LABEL_104:
    v78 = v19 == 0;
    if ( v19 )
        v78 = (flags & 0x2000) == 0;
    if ( !v78 && !*(uint8_t *)(g_libGTASA + (VER_x32 ? 0x6B8BA4:0x896138)) )
    {

        const char *v81 = "10.0";
        if ( *(uint8_t *)(g_libGTASA + (VER_x32 ? 0x6B8BA9:0x89613D)) )
            v81 = "9.0";

        snprintf(s, 0x200u, "float specAmt = max(pow(dot(reflVector, DirLightDirection), %s), 0.0) * EnvMapCoefficient * 2.0;", v81);
        strcat(vertex, s);

        snprintf(s, 0x200u, "Out_Spec = specAmt * DirLightDiffuseColor;");
        strcat(vertex, s);
    }
    if ((flags & 0x80000) != 0 )
    {
        snprintf(s, 0x200u, "Out_WaterDetail = (Out_Tex0 * 4.0) + vec2(WaterSpecs.x * -0.3, WaterSpecs.x * 0.21);");
        strcat(vertex, s);

        snprintf(s, 0x200u, "Out_WaterDetail2 = (Out_Tex0 * -8.0) + vec2(WaterSpecs.x * 0.12, WaterSpecs.x * -0.05);");
        strcat(vertex, s);

        snprintf(s, 0x200u, "Out_WaterAlphaBlend = distance(WorldPos.xy, CameraPosition.xy) * WaterSpecs.y;");
        strcat(vertex, s);
    }

    snprintf(s, 0x200u, "}");
    strcat(vertex, s);
}

void (*glCompressedTexImage)(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
void glCompressedTexImage_hook(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
    if (level == 0 || (width >= 4 && height >= 4) || (format != 0x8C01 && format != 0x8C02)) {
        glCompressedTexImage(target, level, format, width, height, border, imageSize, data);
    }
}


bool bEnableSkyGFX = false;
void RQShader::InjectHooks()
{
    Log("RQShader::InjectHooks()");

    //CHook::NOP(g_libGTASA + (VER_x32 ? 0x5B6A1A : 0x6DAC88), 2);               // emu_SetAltRTContrast под бр кеш скорее всего эти 2 можно убрать
    //CHook::NOP(g_libGTASA + (VER_x32 ? 0x5B6A26 : 0x6DAC9C), 2);               // emu_SetAltRTGrading

    vertex = (char*)(g_libGTASA + (VER_x32 ? 0x6BABE9 : 0x89819C));
    vertex[0] = '\0';
    pixel = (char*)(g_libGTASA + (VER_x32 ? 0x6B8BE8 : 0x896198));


    // fix mip maps
    CHook::installPLTHook(g_libGTASA + (VER_x32 ? 0x6758F8 : 0x8494C0), (uintptr_t)glCompressedTexImage_hook, (uintptr_t*)&glCompressedTexImage);


    CHook::HookRedirect("_Z16BuildPixelSourcej", BuildPixelSource_hook);
    CHook::HookRedirect("_Z17BuildVertexSourcej", BuildVertexSource_hook);


}