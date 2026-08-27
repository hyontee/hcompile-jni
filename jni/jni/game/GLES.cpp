#include "../main.h"
#include "../util/armhook.h"
#include "../util/patch.h"
#include <GLES2/gl2.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>   // Для malloc и free
#include <cstdint>   // Для uint16_t

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

RQCapabilities* RQCaps;
int* RQMaxBones;

char* DAT_00370344 = 0; //pxl
char* byte_617258 = 0;
char* DAT_00370348 = 0; //vertex

void (*BuildPixelSource)(unsigned int param_1);
void BuildPixelSource_hook(unsigned int param_1)
{
    uint uVar1;
    char *pcVar2;
    uint uVar3;
    uint uVar4;
    char acStack_102c [4096];
    int iStack_2c;

    DAT_00370344[0] = 0;
    snprintf(acStack_102c,0x200,"#version 100\n");
    strcat(DAT_00370344,acStack_102c);
    snprintf(acStack_102c,0x200,"precision mediump float;");
    strcat(DAT_00370344,acStack_102c);
    uVar4 = param_1 & 0x20;
    if (uVar4 != 0) {
        snprintf(acStack_102c,0x200,"uniform sampler2D Diffuse;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"varying mediump vec2 Out_Tex0;");
        strcat(DAT_00370344,acStack_102c);
    }
    uVar1 = param_1 & 0x1000040;
    if (uVar1 == 0) {
        if ((int)(param_1 << 0xf) < 0) {
            snprintf(acStack_102c,0x200,"uniform sampler2D EnvMap;");
            strcat(DAT_00370344,acStack_102c);
            pcVar2 = "uniform float DetailTiling;";
            goto LAB_00072780;
        }
    }
    else {
        snprintf(acStack_102c,0x200,"uniform sampler2D EnvMap;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"uniform lowp float EnvMapCoefficient;");
        strcat(DAT_00370344,acStack_102c);
        if ((int)(param_1 << 0x19) < 0) {
            pcVar2 = "varying mediump vec2 Out_Tex1;";
        }
        else {
            pcVar2 = "varying mediump vec3 Out_Refl;";
        }
        LAB_00072780:
        snprintf(acStack_102c,0x200,"%s", pcVar2);
        strcat(DAT_00370344,acStack_102c);
    }
    if ((param_1 & 0x400) != 0) {
        snprintf(acStack_102c,0x200,"varying mediump float Out_FogAmt;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"uniform lowp vec3 FogColor;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"uniform sampler2D FogMap;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"varying lowp vec4 Out_ClipSpace;");
        strcat(DAT_00370344,acStack_102c);
    }
    if ((param_1 & 0x12) != 0) {
        snprintf(acStack_102c,0x200,"varying lowp vec4 Out_Color;");
        strcat(DAT_00370344,acStack_102c);
    }
    if ((param_1 & 4) != 0) {
        snprintf(acStack_102c,0x200,"uniform lowp float AlphaModulate;");
        strcat(DAT_00370344,acStack_102c);
    }
    uVar3 = param_1 & 0x80000;
    if (uVar3 != 0) {
        snprintf(acStack_102c,0x200,"varying mediump vec2 Out_WaterDetail;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"varying mediump vec2 Out_WaterDetail2;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"varying mediump float Out_WaterAlphaBlend;");
        strcat(DAT_00370344,acStack_102c);
    }
    if ((param_1 & 0x2000) != 0) {
        snprintf(acStack_102c,0x200,"varying lowp vec3 Out_Spec;");
        strcat(DAT_00370344,acStack_102c);
    }
    if (uVar4 != 0) {
        snprintf(acStack_102c,0x200,
                 "bool isSky(vec4 targetColor){vec2 textureCoord = vec2(0.0, 1.0);vec4 colorAtPosition = texture2D(Diffuse, textureCoord);const float colorThreshold = 0.02;return abs(colorAtPosition.r-targetColor.r)<=colorThreshold&&abs(colorAtPosition.g-targetColor.g)<=colorThreshold&&abs(colorAtPosition.b-targetColor.b)<=colorThreshold&&colorAtPosition.a==1.0;} "
        );
        strcat(DAT_00370344,acStack_102c);
    }
    snprintf(acStack_102c,0x200,"void main() {");
    strcat(DAT_00370344,acStack_102c);
    snprintf(acStack_102c,0x200,"lowp vec4 fcolor;");
    strcat(DAT_00370344,acStack_102c);
    if ((param_1 & 0x400) != 0) {
        snprintf(acStack_102c,0x200,"lowp vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);");
        strcat(DAT_00370344,acStack_102c);
    }
    if (uVar4 == 0) {
        if ((param_1 & 0x12) == 0) {
            pcVar2 = "fcolor = 0.0;";
        }
        else {
            pcVar2 = "fcolor = Out_Color;";
        }
        LAB_00072a7e:
        snprintf(acStack_102c,0x200,"%s", pcVar2);
        LAB_00072a90:
        strcat(DAT_00370344,acStack_102c);
    }
    else {
        if ((int)(param_1 << 0x14) < 0) {
            snprintf(acStack_102c,0x200,"lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -2.5);");
        }
        else if (g_libGTASA == -0x617250) {
            snprintf(acStack_102c,0x200,"lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -0.5);");
        }
        else if (*(char *)(g_libGTASA + 0x61725e) == '\0') {
            snprintf(acStack_102c,0x200,"lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -0.5);");
        }
        else {
            snprintf(acStack_102c,0x200,"lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);");
        }
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"lowp vec4 newDiffuseColor = diffuseColor;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"fcolor = newDiffuseColor;");
        strcat(DAT_00370344,acStack_102c);
        if ((param_1 & 0x12) == 0) {
            if (uVar3 == 0) goto joined_r0x00072a98;
            pcVar2 = "fcolor.a += Out_WaterAlphaBlend;";
            goto LAB_00072a7e;
        }
        if ((int)(param_1 << 0xf) < 0) {
            if (uVar3 != 0) {
                snprintf(acStack_102c,0x200,
                         "float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;"
                );
                strcat(DAT_00370344,acStack_102c);
                snprintf(acStack_102c,0x200,
                         "fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);");
                strcat(DAT_00370344,acStack_102c);
                snprintf(acStack_102c,0x200,"fcolor.a += Out_WaterAlphaBlend;");
                strcat(DAT_00370344,acStack_102c);
                goto joined_r0x00072a98;
            }
            snprintf(acStack_102c,0x200,
                     "fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * DetailTiling, -0.5).xyz * 2.0, Out_Color.w);"
            );
            goto LAB_00072a90;
        }
        snprintf(acStack_102c,0x200,"fcolor *= Out_Color;");
        strcat(DAT_00370344,acStack_102c);
        if (uVar3 != 0) {
            snprintf(acStack_102c,0x200,"fcolor.a += Out_WaterAlphaBlend;");
            goto LAB_00072a90;
        }
    }
    joined_r0x00072a98:
    if ((int)(param_1 << 0x19) < 0) {
        snprintf(acStack_102c,0x200,
                 "fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");
        strcat(DAT_00370344,acStack_102c);
    }
    if ((int)(param_1 << 7) < 0) {
        snprintf(acStack_102c,0x200,"vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);")
                ;
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"lowp vec4 ReflTexture = texture2D(EnvMap, ReflPos);");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"float newEnvMapCoef = EnvMapCoefficient + 0.1;");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"fcolor.xyz = mix(fcolor.xyz, ReflTexture.xyz, newEnvMapCoef);");
        strcat(DAT_00370344,acStack_102c);
        snprintf(acStack_102c,0x200,"fcolor.w += ReflTexture.b * 0.125;");
        strcat(DAT_00370344,acStack_102c);
    }
    if ((g_libGTASA != -0x617250) && (*(int *)(g_libGTASA + 0x617258) == 0)) {
        uVar3 = uVar1;
        if (uVar1 != 0) {
            uVar3 = param_1 & 0x2000;
        }
        if (uVar1 != 0 && uVar3 != 0) {
            snprintf(acStack_102c,0x200,"fcolor.xyz += Out_Spec;");
            strcat(DAT_00370344,acStack_102c);
        }
        if ((param_1 & 0x400) != 0) {
            snprintf(acStack_102c,0x200,"fcolor.xyz = mix(fcolor.xyz, FogColor.xyz, Out_FogAmt);");
            strcat(DAT_00370344,acStack_102c);
        }
    }
    if ((int)(param_1 << 5) < 0) {
        snprintf(acStack_102c,0x200,"fcolor.xyz += fcolor.xyz * 0.5;");
        strcat(DAT_00370344,acStack_102c);
    }
    snprintf(acStack_102c,0x200,"gl_FragColor = fcolor;");
    strcat(DAT_00370344,acStack_102c);
    if ((param_1 & 1) == 0) goto LAB_00072d64;
    snprintf(acStack_102c,0x200,"/*ATBEGIN*/");
    strcat(DAT_00370344,acStack_102c);
    if ((*(int *)(g_libGTASA + 0x5ddd30) == 9) && (uVar4 != 0)) {
        if ((param_1 & 0x800) != 0) {
            pcVar2 = "if (diffuseColor.a < 0.8) { discard; }";
            goto LAB_00072cce;
        }
        if (-1 < (int)(param_1 << 0x16)) {
            pcVar2 = "if (diffuseColor.a < 0.2) { discard; }";
            goto LAB_00072cce;
        }
        snprintf(acStack_102c,0x200,"gl_FragColor.a = Out_Color.a;");
        strcat(DAT_00370344,acStack_102c);
        pcVar2 = "if (diffuseColor.a < 0.5) { discard; }";
        LAB_00072d34:
        snprintf(acStack_102c,0x200,"%s",pcVar2);
    }
    else {
        if ((param_1 & 0x800) == 0) {
            if ((int)(param_1 << 0x16) < 0) {
                snprintf(acStack_102c,0x200,"if (gl_FragColor.a < 0.5) { discard; }");
                strcat(DAT_00370344,acStack_102c);
                pcVar2 = "gl_FragColor.a = Out_Color.a;";
                goto LAB_00072d34;
            }
            pcVar2 = "if (gl_FragColor.a < 0.2) { discard; }";
        }
        else {
            pcVar2 = "if (gl_FragColor.a < 0.8) { discard; }";
        }
        LAB_00072cce:
        snprintf(acStack_102c,0x200,"%s", pcVar2);
    }
    strcat(DAT_00370344,acStack_102c);
    snprintf(acStack_102c,0x200,"/*ATEND*/");
    strcat(DAT_00370344,acStack_102c);
    LAB_00072d64:
    if ((param_1 & 4) != 0) {
        snprintf(acStack_102c,0x200,"gl_FragColor.a *= AlphaModulate;");
        strcat(DAT_00370344,acStack_102c);
    }
    if (uVar4 != 0) {
        snprintf(acStack_102c, 0x200,
                 "if(isSky(vec4(95.0/255.0 * 1.2, 144.0/255.0 * 1.2, 188.0/255.0 * 1.2, 1.0)) || "
                 "isSky(vec4(125.0/255.0 * 1.2, 152.0/255.0 * 1.2, 212.0/255.0 * 1.2, 1.0)) || "
                 "isSky(vec4(164.0/255.0 * 1.2, 110.0/255.0 * 1.2, 91.0/255.0 * 1.2, 1.0)) || "
                 "isSky(vec4(20.0/255.0 * 1.2, 21.0/255.0 * 1.2, 28.0/255.0 * 1.2, 1.0))) { "
                 "gl_FragColor.rgb = diffuseColor.rgb; }"
        );
        strcat(DAT_00370344,acStack_102c);
    }
    snprintf(acStack_102c,0x200,"}");
    strcat(DAT_00370344,acStack_102c);
    //Log("test gtasa pix: %s",g_libGTASA + 0x617290);
}

void (*BuildVertexSource)(unsigned int param_1);
void BuildVertexSource_hook(unsigned int param_1) {
    int uVar1;
    int uVar2;
    int uVar3;
    int uVar4;
    char *pcVar5;
    char *pcVar6;
    int uVar7;
    int uVar8;
    int uVar9;
    char acStack_102c[4096];
    int iStack_2c;

    DAT_00370348[0] = 0;
    snprintf(acStack_102c, 0x200, "#version 100\n");
    strcat(DAT_00370348, acStack_102c);
    snprintf(acStack_102c, 0x200, "precision highp float;");
    strcat(DAT_00370348, acStack_102c);
    snprintf(acStack_102c, 0x200, "uniform mat4 ProjMatrix;");
    strcat(DAT_00370348, acStack_102c);
    snprintf(acStack_102c, 0x200, "uniform mat4 ViewMatrix;");
    strcat(DAT_00370348, acStack_102c);
    snprintf(acStack_102c, 0x200, "uniform mat4 ObjMatrix;");
    strcat(DAT_00370348, acStack_102c);
    uVar1 = param_1 & 2;
    if (uVar1 != 0) {
        snprintf(acStack_102c, 0x200, "uniform lowp vec3 AmbientLightColor;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "uniform lowp vec4 MaterialEmissive;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "uniform lowp vec4 MaterialAmbient;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "uniform lowp vec4 MaterialDiffuse;");
        strcat(DAT_00370348, acStack_102c);
        if ((int) (param_1 << 0x12) < 0) {
            snprintf(acStack_102c, 0x200, "uniform lowp vec3 DirLightDiffuseColor;");
            strcat(DAT_00370348, acStack_102c);
            snprintf(acStack_102c, 0x200, "uniform vec3 DirLightDirection;");
            strcat(DAT_00370348, acStack_102c);
            if (((param_1 & 0x1180) != 0) && (*(int *) (g_libGTASA + 0x8ed870) != 0)) {
                snprintf(acStack_102c, 0x200, "uniform vec3 DirBackLightDirection;");
                strcat(DAT_00370348, acStack_102c);
            }
        }
        if ((int) (param_1 << 0x11) < 0) {
            snprintf(acStack_102c, 0x200, "uniform lowp vec3 DirLight2DiffuseColor;");
            strcat(DAT_00370348, acStack_102c);
            snprintf(acStack_102c, 0x200, "uniform vec3 DirLight2Direction;");
            strcat(DAT_00370348, acStack_102c);
        }
        if ((int) (param_1 << 0x10) < 0) {
            snprintf(acStack_102c, 0x200, "uniform lowp vec3 DirLight3DiffuseColor;");
            strcat(DAT_00370348, acStack_102c);
            snprintf(acStack_102c, 0x200, "uniform vec3 DirLight3Direction;");
            strcat(DAT_00370348, acStack_102c);
        }
    }
    snprintf(acStack_102c, 0x200, "attribute vec3 Position;");
    strcat(DAT_00370348, acStack_102c);
    snprintf(acStack_102c, 0x200, "attribute vec3 Normal;");
    strcat(DAT_00370348, acStack_102c);
    uVar7 = param_1 & 0x20;
    if (uVar7 != 0) {
        if ((int) (param_1 << 0xd) < 0) {
            pcVar5 = "attribute vec4 TexCoord0;";
        } else {
            pcVar5 = "attribute vec2 TexCoord0;";
        }
        snprintf(acStack_102c, 0x200, "%s", pcVar5);
        strcat(DAT_00370348, acStack_102c);
    }
    snprintf(acStack_102c, 0x200, "attribute vec4 GlobalColor;");
    strcat(DAT_00370348, acStack_102c);
    uVar2 = param_1 & 0x180;
    if (uVar2 != 0) {
        snprintf(acStack_102c, 0x200, "attribute vec4 BoneWeight;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "attribute vec4 BoneIndices;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "uniform highp vec4 Bones[%d];",
                 *(int *) (g_libGTASA + 0x617248) * 3);
        strcat(DAT_00370348, acStack_102c);
    }
    if (uVar7 != 0) {
        snprintf(acStack_102c, 0x200, "varying mediump vec2 Out_Tex0;");
        strcat(DAT_00370348, acStack_102c);
    }
    if ((param_1 & 0x2000000) != 0) {
        snprintf(acStack_102c, 0x200, "uniform mat3 NormalMatrix;");
        strcat(DAT_00370348, acStack_102c);
    }
    uVar9 = param_1 & 0x1000040;
    if (uVar9 != 0) {
        snprintf(acStack_102c, 0x200, "uniform lowp float EnvMapCoefficient;");
        strcat(DAT_00370348, acStack_102c);
        if ((int) (param_1 << 0x19) < 0) {
            pcVar5 = "varying mediump vec2 Out_Tex1;";
        } else {
            pcVar5 = "varying mediump vec3 Out_Refl;";
        }
        snprintf(acStack_102c, 0x200, "%s", pcVar5);
        strcat(DAT_00370348, acStack_102c);
    }
    if ((param_1 & 0x9880640) != 0) {
        snprintf(acStack_102c, 0x200, "uniform vec3 CameraPosition;");
        strcat(DAT_00370348, acStack_102c);
    }
    uVar3 = param_1 & 0x400;
    if (uVar3 != 0) {
        snprintf(acStack_102c, 0x200, "varying mediump float Out_FogAmt;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "uniform vec3 FogDistances;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "varying mediump vec4 Out_ClipSpace;");
        strcat(DAT_00370348, acStack_102c);
    }
    if ((param_1 & 0x80000) != 0) {
        snprintf(acStack_102c, 0x200, "uniform vec3 WaterSpecs;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "varying mediump vec2 Out_WaterDetail;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "varying mediump vec2 Out_WaterDetail2;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "varying mediump float Out_WaterAlphaBlend;");
        strcat(DAT_00370348, acStack_102c);
    }
    if ((param_1 & 0x100000) != 0) {
        snprintf(acStack_102c, 0x200, "attribute vec4 Color2;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "uniform lowp float ColorInterp;");
        strcat(DAT_00370348, acStack_102c);
    }
    uVar4 = param_1 & 0x12;
    if (uVar4 != 0) {
        snprintf(acStack_102c, 0x200, "varying lowp vec4 Out_Color;");
        strcat(DAT_00370348, acStack_102c);
    }
    uVar8 = param_1 & 0x2000;
    if (uVar8 != 0 && uVar9 != 0) {
        snprintf(acStack_102c, 0x200, "varying lowp vec3 Out_Spec;");
        strcat(DAT_00370348, acStack_102c);
    }
    snprintf(acStack_102c, 0x200, "void main() {");
    strcat(DAT_00370348, acStack_102c);
    if (uVar2 == 0) {
        pcVar5 = "\tvec4 WorldPos = ObjMatrix * vec4(Position,1.0);";
    } else {
        snprintf(acStack_102c, 0x200, "\tivec4 BlendIndexArray = ivec4(BoneIndices);");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "\tmat4 BoneToLocal;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[0] = Bones[BlendIndexArray.x*3] * BoneWeight.x;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[1] = Bones[BlendIndexArray.x*3+1] * BoneWeight.x;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[2] = Bones[BlendIndexArray.x*3+2] * BoneWeight.x;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "\tBoneToLocal[3] = vec4(0.0,0.0,0.0,1.0);");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[0] += Bones[BlendIndexArray.y*3] * BoneWeight.y;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[1] += Bones[BlendIndexArray.y*3+1] * BoneWeight.y;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[2] += Bones[BlendIndexArray.y*3+2] * BoneWeight.y;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[0] += Bones[BlendIndexArray.z*3] * BoneWeight.z;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[1] += Bones[BlendIndexArray.z*3+1] * BoneWeight.z;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "\tBoneToLocal[2] += Bones[BlendIndexArray.z*3+2] * BoneWeight.z;");
        strcat(DAT_00370348, acStack_102c);
        if ((int) (param_1 << 0x17) < 0) {
            snprintf(acStack_102c, 0x200,
                     "\tBoneToLocal[0] += Bones[BlendIndexArray.w*3] * BoneWeight.w;");
            strcat(DAT_00370348, acStack_102c);
            snprintf(acStack_102c, 0x200,
                     "\tBoneToLocal[1] += Bones[BlendIndexArray.w*3+1] * BoneWeight.w;"
            );
            strcat(DAT_00370348, acStack_102c);
            snprintf(acStack_102c, 0x200,
                     "\tBoneToLocal[2] += Bones[BlendIndexArray.w*3+2] * BoneWeight.w;"
            );
            strcat(DAT_00370348, acStack_102c);
        }
        pcVar5 = "\tvec4 WorldPos = ObjMatrix * (vec4(Position,1.0) * BoneToLocal);";
    }
    snprintf(acStack_102c, 0x200, "%s", pcVar5);
    strcat(DAT_00370348, acStack_102c);
    if ((int) (param_1 << 4) < 0) {
        Log("skybox yes");
        snprintf(acStack_102c, 0x200, "    vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "    float coefOffset = 1.0 - clamp(length(ReflVector.xy) / 45.0, 0.0, 1.0);");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "    coefOffset = -1000.0 * coefOffset;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "\tvec3 ReflPos = normalize(ReflVector);");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "    ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
        strcat(DAT_00370348, acStack_102c);
        pcVar5 = "\tgl_Position = vec4(ReflPos.xy, coefOffset + length(ReflVector) * 0.002, 1.0);";
        LAB_000738d6:
        snprintf(acStack_102c, 0x200, "%s", pcVar5);
        LAB_000738e4:
        strcat(DAT_00370348, acStack_102c);
    } else {
        if ((int) (param_1 << 8) < 0) {
            snprintf(acStack_102c, 0x200,
                     "    vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
            strcat(DAT_00370348, acStack_102c);
            snprintf(acStack_102c, 0x200, "\tvec3 ReflPos = normalize(ReflVector);");
            strcat(DAT_00370348, acStack_102c);
            snprintf(acStack_102c, 0x200,
                     "    ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
            strcat(DAT_00370348, acStack_102c);
            pcVar5 = "\tgl_Position = vec4(ReflPos.xy, length(ReflVector) * 0.002, 1.0);";
            goto LAB_000738d6;
        }
        snprintf(acStack_102c, 0x200, "\tvec4 ViewPos = ViewMatrix * WorldPos;");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "\tgl_Position = ProjMatrix * ViewPos;");
        strcat(DAT_00370348, acStack_102c);
        if (uVar3 != 0) {
            snprintf(acStack_102c, 0x200, "\tOut_ClipSpace = gl_Position;");
            goto LAB_000738e4;
        }
    }
    if (uVar1 == 0) {
        if (uVar9 != 0) {
            pcVar5 = "vec3 WorldNormal = vec3(0.0, 0.0, 0.0);";
            goto LAB_00073930;
        }
    } else {
        if (((param_1 & 0x201) == 0x201) && ((param_1 & 0xe000) != 0)) {
            pcVar5 = "vec3 WorldNormal = normalize(vec3(WorldPos.xy - CameraPosition.xy, 0.0001)) * 0.85;";
        } else if (uVar2 == 0) {
            pcVar5 = "vec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;";
        } else {
            pcVar5 = "vec3 WorldNormal = mat3(ObjMatrix) * (Normal * mat3(BoneToLocal));";
        }
        LAB_00073930:
        snprintf(acStack_102c, 0x200, "%s", pcVar5);
        strcat(DAT_00370348, acStack_102c);
    }
    if (((g_libGTASA != -0x617250) && (*(int *) (g_libGTASA + 0x617258) == 0)) &&
        (uVar3 != 0)) {
        snprintf(acStack_102c, 0x200,
                 "Out_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 1.0);"
        );
        strcat(DAT_00370348, acStack_102c);
    }
    if (uVar7 != 0) {
        if ((int) (param_1 << 0xd) < 0) {
            pcVar5 = "TexCoord0.xy / TexCoord0.w";
        } else {
            pcVar5 = "TexCoord0 / 512.0";
            if (-1 < (int) (param_1 << 0xe)) {
                pcVar5 = "TexCoord0";
            }
        }
        if ((param_1 & 0x2000000) == 0) {
            pcVar6 = "Out_Tex0 = %s;";
        } else {
            pcVar6 = "Out_Tex0 = (NormalMatrix * vec3(%s, 1.0)).xy;";
        }
        snprintf(acStack_102c, 0x200, pcVar6, pcVar5);
        strcat(DAT_00370348, acStack_102c);
    }
    if (uVar9 != 0) {
        snprintf(acStack_102c, 0x200,
                 "vec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "reflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;");
        strcat(DAT_00370348, acStack_102c);
        if ((int) (param_1 << 7) < 0) {
            pcVar5 = "Out_Refl = reflVector;";
        } else {
            pcVar5 = "Out_Tex1 = vec2(length(reflVector.xy), (reflVector.z * 0.5) + 0.25);";
        }
        snprintf(acStack_102c, 0x200, "%s", pcVar5);
        strcat(DAT_00370348, acStack_102c);
    }
    if ((param_1 & 0x100000) == 0) {
        pcVar5 = "GlobalColor";
    } else {
        snprintf(acStack_102c, 0x200,
                 "lowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);");
        strcat(DAT_00370348, acStack_102c);
        pcVar5 = "InterpColor";
    }
    uVar7 = uVar8 >> 0xd ^ 1;
    if (uVar1 == 0) {
        if (uVar4 == 0) goto LAB_00073bca;
        snprintf(acStack_102c, 0x200, "Out_Color = %s;", pcVar5);
    } else {
        snprintf(acStack_102c, 0x200, "vec3 Out_LightingColor;");
        strcat(DAT_00370348, acStack_102c);
        if ((int) (param_1 << 0x1c) < 0) {
            if ((int) (param_1 << 0x16) < 0) {
                pcVar6 = "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz * 1.5;";
                goto LAB_00073af8;
            }
            snprintf(acStack_102c, 0x200,
                     "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz * 1.5 + %s.xyz;",
                     pcVar5);
        } else {
            pcVar6 =
                    "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz * 1.5 + MaterialEmissive.xyz;";
            LAB_00073af8:
            snprintf(acStack_102c, 0x200, "%s", pcVar6);
        }
        strcat(DAT_00370348, acStack_102c);
        if ((param_1 & 0xe000) != 0) {
            if (uVar8 != 0) {
                if (((param_1 & 0x1180) == 0) || (*(uint *) (g_libGTASA + 0x8ed870) < 2)) {
                    pcVar6 =
                            "Out_LightingColor += max(dot(DirLightDirection, WorldNormal), 0.0) * DirLightDiffuseColor;";
                } else {
                    pcVar6 =
                            "Out_LightingColor += (max(dot(DirLightDirection, WorldNormal), 0.0) + max(dot(DirBackLightDirection, WorldNormal), 0.0)) * DirLightDiffuseColor;";
                }
                snprintf(acStack_102c, 0x200, "%s", pcVar6);
                strcat(DAT_00370348, acStack_102c);
            }
            if ((int) (param_1 << 0x11) < 0) {
                snprintf(acStack_102c, 0x200,
                         "Out_LightingColor += max(dot(DirLight2Direction, WorldNormal), 0.0) * DirLight2DiffuseColor;"
                );
                strcat(DAT_00370348, acStack_102c);
            }
            if ((int) (param_1 << 0x10) < 0) {
                snprintf(acStack_102c, 0x200,
                         "Out_LightingColor += max(dot(DirLight3Direction, WorldNormal), 0.0) * DirLight3DiffuseColor;"
                );
                strcat(DAT_00370348, acStack_102c);
            }
        }
        if (uVar4 == 0) goto LAB_00073bca;
        if ((int)(param_1 << 0x1b) < 0) {
            snprintf(acStack_102c,0x200,
                     "Out_Color = vec4((Out_LightingColor.xyz + %s.xyz * 1.5) * MaterialDiffuse.xyz, (MaterialAmbient.w) * %s.w);"
                    ,pcVar5,pcVar5);
        }
        else {
            snprintf(acStack_102c,0x200,
                     "Out_Color = vec4(Out_LightingColor * MaterialDiffuse.xyz, MaterialAmbient.w * %s.w );"
                    ,pcVar5);
        }
        strcat(DAT_00370348,acStack_102c);
        snprintf(acStack_102c,0x200,"Out_Color = clamp(Out_Color, 0.0, 1.0);");
        strcat(DAT_00370348,acStack_102c);
    }
    strcat(DAT_00370348, acStack_102c);
    LAB_00073bca:
    uVar1 = *(uint8_t * )(g_libGTASA + 0x617258);
    if (uVar9 == 0) {
        uVar7 = 1;
    }
    if (uVar1 != 0) {
        uVar1 = 1;
    }
    if ((uVar1 | uVar7) == 0) {
        snprintf(acStack_102c, 0x200,
                 "float specAmt = max(pow(dot(reflVector, DirLightDirection), %.1f), 0.0) * EnvMapCoefficient * 0.5;"
        );
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200, "Out_Spec = specAmt * DirLightDiffuseColor;");
        strcat(DAT_00370348, acStack_102c);
    }
    if ((param_1 & 0x80000) != 0) {
        snprintf(acStack_102c, 0x200,
                 "Out_WaterDetail = (Out_Tex0 * 4.0) + vec2(WaterSpecs.x * -0.3, WaterSpecs.x * 0.21);");
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "Out_WaterDetail2 = (Out_Tex0 * -8.0) + vec2(WaterSpecs.x * 0.12, WaterSpecs.x * -0.05);"
        );
        strcat(DAT_00370348, acStack_102c);
        snprintf(acStack_102c, 0x200,
                 "Out_WaterAlphaBlend = distance(WorldPos.xy, CameraPosition.xy) * WaterSpecs.y;");
        strcat(DAT_00370348, acStack_102c);
    }
    snprintf(acStack_102c, 0x200, "}");
    strcat(DAT_00370348, acStack_102c);
    //Log("test gtasa vert: %s", g_libGTASA + 0x619294);
}

void (*RQ_Command_rqTextureMipMode)(int** a1);
void RQ_Command_rqTextureMipMode_hook(int** a1)
{
    int v1; // r3
    int v2; // r6
    GLuint v3; // r4

    v1 = **a1;
    *a1 += 2;
    v2 = g_libGTASA;
    v3 = *(uintptr_t*)(v1 + 40);
    if (*(uintptr_t*)(g_libGTASA + 0x5D2468) != 5)
    {
        glActiveTexture(0x84C5);
        *(uintptr_t*)(v2 + 0x5D2468) = 5;
    }
    glBindTexture(0xDE1, v3);
    *(uintptr_t*)(g_libGTASA + 0x5D245C) = v3;
    glTexParameteri(0xDE1, 0x2801, 9729);
    glTexParameteri(0xDE1, 0x2800, 9729);
}

void SetUpGLHooks()
{
    int v0;
    byte_617258 = (char*)(g_libGTASA + 0x617258);
    DAT_00370348 = (char*)(g_libGTASA + 0x619294);
    DAT_00370348[0] = '\0';
    DAT_00370344 = (char*)(g_libGTASA + 0x617290);
    byte_617258 = (char*)(g_libGTASA + 0x617258);

    RQMaxBones = (int*)(g_libGTASA + 0x617248);

    RQCaps = (RQCapabilities*)g_libGTASA + 0x617250;

    //CHook::InlineHook(g_libGTASA, 0x1A6994, &RQ_Command_rqTextureMipMode_hook, &RQ_Command_rqTextureMipMode);
    CHook::InlineHook(g_libGTASA, 0x1A40B4, (uintptr_t)&BuildPixelSource_hook, (uintptr_t *)&BuildPixelSource);
    CHook::InlineHook(g_libGTASA, 0x1A4AB4, (uintptr_t)&BuildVertexSource_hook, (uintptr_t *)&BuildVertexSource);
    //SetUpHook(g_libGTASA + 0x18DAAC, (uintptr_t)emu_glEndInternal_hook, (uintptr_t*)&emu_glEndInternal);
}