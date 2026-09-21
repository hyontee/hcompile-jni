#include "main.h"
#include "../util/patch.h"

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
    /*char v2; // r6
    unsigned int v3; // r3
    bool v5; // r3
    const char *v6; // lr
    const char *v7; // lr
    int v8; // r0
    int v9; // r1
    int v10; // r2
    int v11; // r3
    int *v12; // lr
    int v13; // r1
    int v14; // r2
    int v15; // r3
    int v16; // r0
    int v17; // r1
    int v18; // r2
    int v19; // r3
    int *v20; // lr
    int v21; // r0
    int v22; // r1
    int v23; // r2
    int v24; // r3
    int v25; // r1
    int v26; // [sp+4h] [bp+4h]
    unsigned int v27; // [sp+8h] [bp+8h]
    unsigned int v28; // [sp+Ch] [bp+Ch]
    int v29[128]; // [sp+24h] [bp+24h] BYREF

      v26 = 13;
      PXL_SOURCE("precision mediump float;");

      if ( (flags & FLAG_TEX0) != 0 )
      {
          PXL_SOURCE("uniform sampler2D Diffuse;");
          PXL_SOURCE("varying mediump vec2 Out_Tex0;");
      }
      if ( (flags & (FLAG_SPHERE_ENVMAP | FLAG_ENVMAP)) != 0 )
      {
          PXL_SOURCE("uniform sampler2D EnvMap;");
          PXL_SOURCE("uniform lowp float EnvMapCoefficient;");
          v28 = flags & FLAG_ENVMAP;
          if ( (flags & FLAG_ENVMAP) != 0 )
          PXL_SOURCE("varying mediump vec2 Out_Tex1;");
          else
        PXL_SOURCE("varying mediump vec3 Out_Refl;");
      }
      else
      {
          v28 = flags & FLAG_ENVMAP;
          if ( (flags & FLAG_DETAILMAP) != 0 )
          {
              PXL_SOURCE("uniform sampler2D EnvMap;");
              PXL_SOURCE("uniform float DetailTiling;");
          }
      }

      if ( (flags & FLAG_FOG) != 0 )
      {
          PXL_SOURCE("varying mediump float Out_FogAmt;");
          PXL_SOURCE("uniform lowp vec3 FogColor;");
      }
      if ( (flags & (FLAG_LIGHTING | FLAG_COLOR)) != 0 )
      {
          PXL_SOURCE("varying lowp vec4 Out_Color;");
      }
      if ( (flags & FLAG_LIGHT1) != 0 && (flags & 0x10001C0) != 0 )
      {
          PXL_SOURCE("varying lowp vec3 Out_Spec;");
      }
      if ( (flags & 4) != 0 )
      {
          PXL_SOURCE("uniform lowp float AlphaModulate;");
      }
      v27 = flags & FLAG_WATER;
      if ( (flags & FLAG_WATER) != 0 )
      {
          PXL_SOURCE("varying mediump vec2 Out_WaterDetail;");
          PXL_SOURCE("varying mediump vec2 Out_WaterDetail2;");
          PXL_SOURCE("varying mediump float Out_WaterAlphaBlend;");

          PXL_SOURCE("uniform sampler2D reflectionTexture;");
          PXL_SOURCE("uniform sampler2D refractionTexture;");
      }

      PXL_SOURCE("void main()");
      PXL_SOURCE("{");
      PXL_SOURCE("lowp vec4 fcolor;");

      if ( (flags & FLAG_TEX0) == 0 )
      {
          if ( (flags & (FLAG_LIGHTING | FLAG_COLOR)) != 0 )
          {
              PXL_SOURCE("fcolor = Out_Color;");
              if ( !v28 )
                  goto LABEL_32;
          }
          else
          {
              PXL_SOURCE("fcolor = 0.0;");
              if ( !v28 )
                  goto LABEL_32;
          }
          PXL_SOURCE("fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");

  LABEL_32:
      if ( (flags & FLAG_SPHERE_ENVMAP) == 0 )
          goto LABEL_33;
      goto LABEL_29;
      }
      if ( (flags & FLAG_TEXBIAS) != 0 )
      {
          PXL_SOURCE("lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -1.5);");
      }
      else
      {
     // if ( byte_61725E )
        //PXL_SOURCE("lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);");
      //else
          PXL_SOURCE("lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -0.5);");

      }
      PXL_SOURCE("fcolor = diffuseColor;");

      if ( (flags & (FLAG_LIGHTING | FLAG_COLOR)) == 0 )
      {
  LABEL_67:
          if ( v27 )
              goto LABEL_26;
          goto LABEL_27;
      }
      if ( (flags & FLAG_DETAILMAP) == 0 )
      {
          PXL_SOURCE("fcolor *= Out_Color;");
          goto LABEL_67;
      }
      if ( v27 )
      {
          PXL_SOURCE("float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;");
          PXL_SOURCE("fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);");
          //PXL_SOURCE("vec4 reflectTexture = texture2D(reflectionTexture, Out_WaterDetail2);");
          //PXL_SOURCE("vec4 refractTexture = texture2D(refractionTexture, Out_WaterDetail2);");
          //PXL_SOURCE("fcolor = mix(reflectTexture, refractTexture, 0.5);");
  LABEL_26:
          PXL_SOURCE("fcolor.a += Out_WaterAlphaBlend;");
          goto LABEL_27;
      }
      PXL_SOURCE("fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * DetailTiling, -0.5).xyz * 2.0, Out_Color.w);");
  LABEL_27:
      if ( !v28 )
          goto LABEL_32;
      PXL_SOURCE("fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");

      if ( (flags & FLAG_SPHERE_ENVMAP) != 0 )
      {
  LABEL_29:
          PXL_SOURCE("vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);");
          PXL_SOURCE("ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);");
          PXL_SOURCE("lowp vec4 ReflTexture =  texture2D(EnvMap, ReflPos);");
          PXL_SOURCE("fcolor.xyz = mix(fcolor.xyz,ReflTexture.xyz, EnvMapCoefficient);");
          PXL_SOURCE("fcolor.w += ReflTexture.b * 0.125;");
      }
  LABEL_33:
      v2 = *byte_617258;
      if ( ((flags & FLAG_LIGHT1) != 0) > *byte_617258 && (flags & 0x10001C0) != 0 )
      {
          PXL_SOURCE("fcolor.xyz += Out_Spec;");
      }
      v3 = flags & FLAG_FOG;
      if ( (flags & FLAG_FOG) != 0 )
          v3 = 1;
      if ( v3 > v2 )
      {
      PXL_SOURCE("fcolor.xyz = mix(fcolor.xyz, FogColor, Out_FogAmt);");

      }
      if ( (flags & FLAG_GAMMA) != 0 )
      {
          PXL_SOURCE("fcolor.xyz += fcolor.xyz * 0.5;");
      }
      PXL_SOURCE("gl_FragColor = fcolor;");
      if ( (flags & FLAG_ALPHA_TEST) != 0 )
      {
          PXL_SOURCE("/*ATBEGIN*//*");

        v5 = v26 == 9;
        if ( (flags & FLAG_TEX0) == 0 )
        v5 = 0;
    if ( v5 )
    {
      if ( (flags & FLAG_TEXBIAS) != 0 )
      {
        v6 = "if (diffuseColor.a < 0.8) { discard; }";
      }
      else if ( (flags & FLAG_TEXBIAS) != 0 )
      {
        PXL_SOURCE("gl_FragColor.a = Out_Color.a;");

        v6 = "if (diffuseColor.a < 0.5) { discard; }";
      }
      else
      {
        v6 = "if (diffuseColor.a < 0.2) { discard; }";
      }
    }
    else if ( (flags & FLAG_TEXBIAS) != 0 )
    {
      v6 = "if (gl_FragColor.a < 0.8) { discard; }";
    }
    else
    {
      if ( (flags & FLAG_CAMERA_BASED_NORMALS) != 0 )
      {
        PXL_SOURCE("if (gl_FragColor.a < 0.5) { discard; }");
        PXL_SOURCE("gl_FragColor.a = Out_Color.a;");
LABEL_57:
        PXL_SOURCE("/*ATEND*//*");
        goto LABEL_43;
      }
      v6 = "if (gl_FragColor.a < 0.2) { discard; }";
    }

	PXL_SOURCE(v6);
    goto LABEL_57;
  }
LABEL_43:
  if ( (flags & FLAG_ALPHA_MODULATE) != 0 )
  {
    PXL_SOURCE("gl_FragColor.a *= AlphaModulate;");

  }
  PXL_SOURCE("}");
  return; */

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
    snprintf(
            s,
            0x200u,
            "float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;");
    strcat(byte_617290, s);

    strcpy(s, "fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);");
    strcat(byte_617290, s);

    strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
    strcat(byte_617290, s);

    strcpy(s, "vec4 reflectColour = texture2D(EnvMap, Out_WaterDetail2);");
    strcat(byte_617290, s);

    strcpy(s, "vec4 refractColour = texture2D(Diffuse, Out_WaterDetail2);");
    strcat(byte_617290, s);

    strcpy(s, "fcolor = mix(reflectColour, refractColour, 0.5);");
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

    Log("build pixel shader");
    Log(byte_617290);
    Log("end build pixel shader");
}

void (*BuildVertexSource)(unsigned int flags);
void BuildVertexSource_hook(unsigned int flags)
{
    /*const char *v2; // r12
    int v3; // r0
    int v4; // r1
    int v5; // r2
    int v6; // r3
    int v7; // r12
    int v8; // r1
    int v9; // r2
    unsigned int v10; // r11
    int v11; // lr
    int v12; // r0
    int v13; // r1
    int v14; // r2
    int v15; // r3
    int v16; // lr
    int v17; // r1
    int v18; // r2
    int v19; // r3
    const char *v20; // r2
    unsigned int v22; // [sp+8h] [bp+0h]
    unsigned int v23; // [sp+2Ch] [bp+24h]
    char v24[512]; // [sp+34h] [bp+2Ch] BYREF
    int v25[128]; // [sp+234h] [bp+22Ch] BYREF

    strcpy((char *)v25, "#version 100\n");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "precision highp float;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "uniform mat4 ProjMatrix;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "uniform mat4 ViewMatrix;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "uniform mat4 ObjMatrix;");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 2) != 0 )
    {
      strcpy((char *)v25, "uniform lowp vec3 AmbientLightColor;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "uniform lowp vec4 MaterialEmissit;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "uniform lowp vec4 MaterialAmbienve;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "uniform lowp vec4 MaterialDiffuse;");
      strcat(byte_619294, (const char *)v25);
      v23 = flags & 0x2000;
      if ( (flags & 0x2000) != 0 )
      {
        strcpy((char *)v25, "uniform lowp vec3 DirLightDiffuseColor;");
        strcat(byte_619294, (const char *)v25);
        strcpy((char *)v25, "uniform vec3 DirLightDirection;");
        strcat(byte_619294, (const char *)v25);
        //if ( GetMobileEffectSetting() == 3 && (flags & 0x1180) != 0 )
        if ((flags & 0x1180) != 0 )
        {
          strcpy((char *)v25, "uniform vec3 DirBackLightDirection;");
          strcat(byte_619294, (const char *)v25);
        }
      }
      if ( (flags & 0x4000) != 0 )
      {
        strcpy((char *)v25, "uniform lowp vec3 DirLight2DiffuseColor;");
        strcat(byte_619294, (const char *)v25);
        strcpy((char *)v25, "uniform vec3 DirLight2Direction;");
        strcat(byte_619294, (const char *)v25);
      }
      if ( (flags & 0x8000) != 0 )
      {
        strcpy((char *)v25, "uniform lowp vec3 DirLight3DiffuseColor;");
        strcat(byte_619294, (const char *)v25);
        strcpy((char *)v25, "uniform vec3 DirLight3Direction;");
        strcat(byte_619294, (const char *)v25);
      }
    }
    else
    {
      v23 = flags & 0x2000;
    }
    strcpy((char *)v25, "attribute vec3 Position;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "attribute vec3 Normal;");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 0x20) != 0 )
    {
      if ( (flags & 0x40000) != 0 )
        v2 = "attribute vec4 TexCoord0;";
      else
        v2 = "attribute vec2 TexCoord0;";

      strcat(byte_619294, (const char *)v2);
    }
    strcpy((char *)v25, "attribute vec4 GlobalColor;");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 0x180) != 0 )
    {
      strcpy((char *)v25, "attribute vec4 BoneWeight;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "attribute vec4 BoneIndices;");
      strcat(byte_619294, (const char *)v25);
      snprintf((char *)v25, 0x200u, "uniform highp vec4 Bones[%d];", 3 * *RQMaxBones);
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x20) != 0 )
    {
      strcpy((char *)v25, "varying mediump vec2 Out_Tex0;");
      strcat(byte_619294, (const char *)v25);
    }
    v22 = flags & 0x2000000;
    if ( (flags & 0x2000000) != 0 )
    {
      strcpy((char *)v25, "uniform mat3 NormalMatrix;");
      strcat(byte_619294, (const char *)v25);
    }
    v10 = flags & 0x1000040;
    if ( (flags & 0x1000040) != 0 )
    {
      if ( (flags & 0x40) != 0 )
        v11 = (int)"varying mediump vec2 Out_Tex1;";
      else
        v11 = (int)"varying mediump vec3 Out_Refl;";

      strcat(byte_619294, (const char *)v11);
      strcpy((char *)v25, "uniform lowp float EnvMapCoefficient;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x18807C0) != 0 )
    {
      strcpy((char *)v25, "uniform vec3 CameraPosition;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x400) != 0 )
    {
      strcpy((char *)v25, "varying mediump float Out_FogAmt;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "uniform vec3 FogDistances;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & FLAG_WATER) != 0 )
    {
      strcpy((char *)v25, "uniform vec3 WaterSpecs;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "varying mediump vec2 Out_WaterDetail;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "varying mediump vec2 Out_WaterDetail2;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "varying mediump float Out_WaterAlphaBlend;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x100000) != 0 )
    {
      strcpy((char *)v25, "attribute vec4 Color2;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "uniform lowp float ColorInterp;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x12) != 0 )
    {
      strcpy((char *)v25, "varying lowp vec4 Out_Color;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( v23 && (flags & 0x10001C0) != 0 )
    {
      strcpy((char *)v25, "varying lowp vec3 Out_Spec;");
      strcat(byte_619294, (const char *)v25);
    }
    strcpy((char *)v25, "void main() {");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 0x180) == 0 )
    {
      strcpy((char *)v25, "vec4 WorldPos = ObjMatrix * vec4(Position,1.0);");
      strcat(byte_619294, (const char *)v25);
      if ( (flags & 0x800000) != 0 )
        goto LABEL_34;
  LABEL_60:
      strcpy((char *)v25, "vec4 ViewPos = ViewMatrix * WorldPos;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "gl_Position = ProjMatrix * ViewPos;");
      strcat(byte_619294, (const char *)v25);
      if ( (flags & 2) != 0 )
        goto LABEL_35;
      goto LABEL_61;
    }
    strcpy((char *)v25, "ivec4 BlendIndexArray = ivec4(BoneIndices);");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "mat4 BoneToLocal;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[0] = Bones[BlendIndexArray.x*3] * BoneWeight.x;x;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[1] = Bones[BlendIndexArray.x*3+1] * BoneWeight.");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[2] = Bones[BlendIndexArray.x*3+2] * BoneWeight.x;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[3] = vec4(0.0,0.0,0.0,1.0);");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[0] += Bones[BlendIndexArray.y*3] * BoneWeight.y;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[1] += Bones[BlendIndexArray.y*3+1] * BoneWeight.y;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[2] += Bones[BlendIndexArray.y*3+2] * BoneWeight.y;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[0] += Bones[BlendIndexArray.z*3] * BoneWeight.z;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[1] += Bones[BlendIndexArray.z*3+1] * BoneWeight.z;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[2] += Bones[BlendIndexArray.z*3+2] * BoneWeight.z;");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 0x100) != 0 )
    {
      strcpy((char *)v25, "BoneToLocal[0] += Bones[BlendIndexArray.w*3] * BoneWeight.w;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "BoneToLocal[1] += Bones[BlendIndexArray.w*3+1] * BoneWeight.w;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "BoneToLocal[2] += Bones[BlendIndexArray.w*3+2] * BoneWeight.w;");
      strcat(byte_619294, (const char *)v25);
    }
    strcpy((char *)v25, "vec4 WorldPos = ObjMatrix * (vec4(Position,1.0) * BoneToLocal);");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 0x800000) == 0 )
      goto LABEL_60;
  LABEL_34:
    strcpy((char *)v25, "vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "vec3 ReflPos = normalize(ReflVector);");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "gl_Position = vec4(ReflPos.xy, length(ReflVector) * 0.002, 1.0);");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 2) != 0 )
    {
  LABEL_35:
      if ( (flags & 0x201) == 513 && (flags & 0xE000) != 0 )
      {
        strcpy((char *)v25, "vec3 WorldNormal = normalize(vec3(WorldPos.xy - CameraPosition.xy, 0.0001)) * 0.85;");
        strcat(byte_619294, (const char *)v25);
      }
      else
      {
        if ( (flags & 0x180) != 0 )
          strcpy((char *)v25, "vec3 WorldNormal = mat3(ObjMatrix) * (Normal * mat3(BoneToLocal));");
        else
          strcpy((char *)v25, "vec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;");
        strcat(byte_619294, (const char *)v25);
      }
      goto LABEL_39;
    }
  LABEL_61:
    if ( v10 )
    {
      strcpy((char *)v25, "vec3 WorldNormal = vec3(0.0, 0.0, 0.0);");
      strcat(byte_619294, (const char *)v25);
    }
  LABEL_39:
    if ( *byte_617258 < (unsigned int)((flags & 0x400) != 0) )
    {
      strcpy(
        (char *)v25,
        "Out_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 0.90);");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x20) != 0 )
    {
      if ( (flags & 0x40000) != 0 )
      {
        strcpy(v24, "TexCoord0.xy / TexCoord0.w");
      }
      else
      {
        if ( (flags & 0x20000) != 0 )
        {
          strcpy(v24, "TexCoord0 / 512.0");
          if ( v22 )
            goto LABEL_46;
  LABEL_90:
          snprintf((char *)v25, 0x200u, "Out_Tex0 = %s;", v24);
          strcat(byte_619294, (const char *)v25);
          goto LABEL_47;
        }
        strcpy(v24, "TexCoord0");
      }
      if ( v22 )
      {
  LABEL_46:
        snprintf((char *)v25, 0x200u, "Out_Tex0 = (NormalMatrix * vec3(%s, 1.0)).xy;", v24);
        strcat(byte_619294, (const char *)v25);
        goto LABEL_47;
      }
      goto LABEL_90;
    }
  LABEL_47:
    if ( v10 )
    {
      strcpy((char *)v25, "vec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "reflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;");
      strcat(byte_619294, (const char *)v25);
      if ( (flags & 0x1000000) != 0 )
        strcpy((char *)v25, "Out_Refl = reflVector;");
      else
        strcpy((char *)v25, "Out_Tex1 = vec2(length(reflVector.xy), (reflVector.z * 0.5) + 0.25);");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x100000) != 0 )
    {
      strcpy((char *)v25, "lowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);");
      strcat(byte_619294, (const char *)v25);
      strcpy(v24, "InterpColor");
      if ( (flags & 2) == 0 )
        goto LABEL_50;
    }
    else
    {
      strcpy(v24, "GlobalColor");
      if ( (flags & 2) == 0 )
      {
  LABEL_50:
        if ( (flags & 0x12) != 0 )
        {
          snprintf((char *)v25, 0x200u, "Out_Color = %s;", v24);
          strcat(byte_619294, (const char *)v25);
        }
        goto LABEL_52;
      }
    }
    strcpy((char *)v25, "vec3 Out_LightingColor;");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 8) != 0 )
    {
      if ( (flags & 0x200) != 0 )
        strcpy((char *)v25, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz * 1.5;");
      else
        snprintf((char *)v25, 0x200u, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + %s.xyz;", v24);
      strcat(byte_619294, (const char *)v25);
    }
    else
    {
      strcpy((char *)v25, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + MaterialEmissive.xyz;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0xE000) != 0 )
    {
      if ( v23 )
      {
        if ( (flags & 0x1180) != 0 )
        {
          strcpy(
            (char *)v25,
            "Out_LightingColor += (max(dot(DirLightDirection, WorldNormal), 0.0) + max(dot(DirBackLightDirection, WorldNorm"
            "al), 0.0)) * DirLightDiffuseColor;");
          strcat(byte_619294, (const char *)v25);
        }
        else
        {
          strcpy(
            (char *)v25,
            "Out_LightingColor += max(dot(DirLightDirection, WorldNormal), 0.0) * DirLightDiffuseColor;");
          strcat(byte_619294, (const char *)v25);
        }
      }
      if ( (flags & 0x4000) != 0 )
      {
        strcpy(
          (char *)v25,
          "Out_LightingColor += max(dot(DirLight2Direction, WorldNormal), 0.0) * DirLight2DiffuseColor;");
        strcat(byte_619294, (const char *)v25);
      }
      if ( (flags & 0x8000) != 0 )
      {
        strcpy(
          (char *)v25,
          "Out_LightingColor += max(dot(DirLight3Direction, WorldNormal), 0.0) * DirLight3DiffuseColor;");
        strcat(byte_619294, (const char *)v25);
      }
    }
    if ( (flags & 0x12) != 0 )
    {
      if ( (flags & 0x10) != 0 )
        snprintf(
          (char *)v25,
          0x200u,
          "Out_Color = vec4((Out_LightingColor.xyz + %s.xyz * 1.5) * MaterialDiffuse.xyz, (MaterialAmbient.w) * %s.w);",
          v24,
          v24);
      else
        snprintf(
          (char *)v25,
          0x200u,
          "Out_Color = vec4(Out_LightingColor * MaterialDiffuse.xyz, MaterialAmbient.w * %s.w);",
          v24);
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "Out_Color = clamp(Out_Color, 0.0, 1.0);");
      strcat(byte_619294, (const char *)v25);
    }
  LABEL_52:
    if ( *byte_617258 >= (unsigned int)(v23 != 0) )
      goto LABEL_56;
    if ( v10 )
    {
      v20 = "float specAmt = max(pow(dot(reflVector, DirLightDirection), %.1f), 0.0) * EnvMapCoefficient * 2.0;";
  LABEL_55:
      snprintf((char *)v25, 0x200u, "%s", v20);
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "Out_Spec = specAmt * DirLightDiffuseColor;");
      strcat(byte_619294, (const char *)v25);
      goto LABEL_56;
    }
    if ( (flags & 0x180) != 0 )
    {
      strcpy((char *)v25, "vec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "reflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;");
      strcat(byte_619294, (const char *)v25);
      v20 = "float specAmt = max(pow(dot(reflVector, DirLightDirection), %.1f), 0.0) * 0.125;";
      goto LABEL_55;
    }
  LABEL_56:
    if ( (flags & FLAG_WATER) != 0 )
    {
      strcpy((char *)v25, "Out_WaterDetail = (Out_Tex0 * 4.0) + vec2(WaterSpecs.x * -0.3, WaterSpecs.x * 0.21);");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "Out_WaterDetail2 = (Out_Tex0 * -8.0) + vec2(WaterSpecs.x * 0.12, WaterSpecs.x * -0.05);");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "Out_WaterAlphaBlend = distance(WorldPos.xy, CameraPosition.xy) * WaterSpecs.y;");
      strcat(byte_619294, (const char *)v25);
    }
    strcpy((char *)v25, "}");
    return strcat(byte_619294, (const char *)v25);*/

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

    //Log("build vertex shader");
    //Log(byte_619294);
    //Log("end build vertex shader");
}

int (*RQShader__BuildSource)(int flags, char **pxlsrc, char **vtxsrc);
int RQShader__BuildSource_hook(int flags, char **pxlsrc, char **vtxsrc) {

    //int v2;
    //bool v3;
    //size_t v4;
    //const char *v5;
    //char s[2096];
    //int v8;

    /*pxlbuf[0] = 0;
    *(uintptr_t *)(g_libGTASA + 0x61572C) |= FLAG_SPHERE_ENVMAP;
    flags |= FLAG_SPHERE_ENVMAP;


    strcpy(s, "#version 100\n");
    strcat(pxlbuf, s);

    strcpy(s, "precision highp float;");
    strcat(pxlbuf, s);

    if ( (flags & 0x20) != 0 )
    {
      strcpy(s, "uniform sampler2D Diffuse;");
      strcat(pxlbuf, s);

      strcpy(s, "varying mediump vec2 Out_Tex0;");
      strcat(pxlbuf, s);

    }
    if ( (flags & 0x1000040) != 0 )
    {
      strcpy(s, "uniform sampler2D EnvMap;");
      strcat(pxlbuf, s);

      strcpy(s, "uniform lowp float EnvMapCoefficient;");
      strcat(pxlbuf, s);

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
      strcat(pxlbuf, s);

      strcpy(s, "uniform float DetailTiling;");
    }
    strcat(pxlbuf, s);

  LABEL_10:
    if ( (flags & 0x400) != 0 )
    {
      strcpy(s, "varying mediump float Out_FogAmt;");
      strcat(pxlbuf, s);

      strcpy(s, "uniform sampler2D FogMap;");
      strcat(pxlbuf, s);

      strcpy(s, "varying lowp vec4 Out_ClipSpace;");
      strcat(pxlbuf, s);

    }
    if ( (flags & 0x12) != 0 )
    {
      strcpy(s, "varying lowp vec4 Out_Color;");
      strcat(pxlbuf, s);

    }
    if ( (flags & 4) != 0 )
    {
      strcpy(s, "uniform lowp float AlphaModulate;");
      strcat(pxlbuf, s);

    }
    v2 = flags & 0x80000;
    if ( (flags & FLAG_WATER) != 0 )
    {
      strcpy(s, "varying mediump vec2 Out_WaterDetail;");
      strcat(pxlbuf, s);

      strcpy(s, "varying mediump vec2 Out_WaterDetail2;");
      strcat(pxlbuf, s);

      strcpy(s, "varying mediump float Out_WaterAlphaBlend;");
      strcat(pxlbuf, s);

      //strcpy(s, "uniform sampler2D reflectionTexture;");
      //strcat(pxlbuf, s);

      //strcpy(s, "uniform sampler2D refractionTexture;");
      //strcat(pxlbuf, s);

    }
    if ( (flags & 0x2000) != 0 )
    {
      strcpy(s, "varying lowp vec3 Out_Spec;");
      strcat(pxlbuf, s);

    }
    strcpy(s, "void main()");
    strcat(pxlbuf, s);

    strcpy(s, (const char *)"{");
    strcat(pxlbuf, s);

    strcpy(s, "lowp vec4 fcolor;");
    strcat(pxlbuf, s);

    if ( (flags & 0x400) != 0 )
    {
      strcpy(s, "lowp vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;");
      strcat(pxlbuf, s);

      strcpy(s, "lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);");
      strcat(pxlbuf, s);

      strcpy(s, "lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);");
      strcat(pxlbuf, s);

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
    strcat(pxlbuf, s);

    strcpy(s, "fcolor = diffuseColor;");
    strcat(pxlbuf, s);

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
      strcat(pxlbuf, s);

      if ( v2 )
      {
        strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
        strcat(pxlbuf, s);


        if ( (flags & FLAG_ENVMAP) == 0 )
          goto LABEL_45;
  LABEL_44:
        strcpy(s, "fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");
        strcat(pxlbuf, s);

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
        "fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * 1.0, -0.5).xyz * 2.0, Out_Color.w);");
  LABEL_42:
      strcat(pxlbuf, s);

      goto LABEL_43;
    }
    snprintf(
      s,
      0x200u,
      "float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;");
    strcat(pxlbuf, s);

    strcpy(s, "fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);");
    strcat(pxlbuf, s);

    strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
    strcat(pxlbuf, s);

    strcpy(s, "vec4 reflectColour = texture2D(EnvMap, Out_WaterDetail2);");
    strcat(pxlbuf, s);

    strcpy(s, "vec4 refractColour = texture2D(Diffuse, Out_WaterDetail2);");
    strcat(pxlbuf, s);

    strcpy(s, "fcolor = mix(reflectColour, refractColour, 0.5);");
    strcat(pxlbuf, s);

    if ( (flags & 0x40) != 0 )
      goto LABEL_44;
  LABEL_45:
    if ( (flags & 0x1000000) != 0 )
    {
      strcpy(s, "vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);");
      strcat(pxlbuf, s);

      strcpy(s, "ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);");
      strcat(pxlbuf, s);

      strcpy(s, "lowp vec4 ReflTexture =  texture2D(EnvMap, ReflPos);");
      strcat(pxlbuf, s);

      strcpy(s, "float newEnvMapCoef = EnvMapCoefficient + 0.14;");
      strcat(pxlbuf, s);

      strcpy(s, "fcolor.xyz = mix(fcolor.xyz,ReflTexture.xyz, newEnvMapCoef);");
      strcat(pxlbuf, s);

      strcpy(s, "fcolor.w += ReflTexture.b * 0.125;");
      strcat(pxlbuf, s);

    }
    if ( !*(char*)(RQCaps + 8) )
    {
      v3 = (flags & 0x2000) == 0;
      if ( (flags & 0x2000) != 0 )
        v3 = (flags & 0x1000040) == 0;
      if ( !v3 )
      {
        strcpy(s, "fcolor.xyz += Out_Spec;");
        strcat(pxlbuf, s);

      }
      if ( (flags & 0x400) != 0 )
      {
        strcpy(s, "fcolor.xyz = mix(fcolor.xyz, fogColorNew.xyz, Out_FogAmt);");
        strcat(pxlbuf, s);

      }
    }
    if ( (flags & 0x4000000) != 0 )
    {
      strcpy(s, "fcolor.xyz += fcolor.xyz * 0.5;");
      strcat(pxlbuf, s);

    }
    strcpy(s, "gl_FragColor = fcolor;");
    strcat(pxlbuf, s);
  LABEL_71:
    if ( (flags & 4) != 0 )
    {
      strcpy(s, "gl_FragColor.a *= AlphaModulate;");
      strcat(pxlbuf, s);

    }
    strcpy(s, "}");
    strcat(pxlbuf, s);*/

    //BuildPixelSource_hook(flags/* | FLAG_SPHERE_ENVMAP*/);


    /*strcpy(s, "#version 100\n");
    strcat(byte_617290, s);
    strcpy(s, "precision highp float;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D Diffuse;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D EnvMap;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D RefractionMap;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D DuDvMap;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D FogMap;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform highp float SkyMixCoef;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec2 Out_Tex0;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec4 Out_ClipSpace;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec3 Out_CameraVector;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec2 Out_DuDvTexCoord;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec3 Out_Refl;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying mediump float Out_FogAmt;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform lowp float EnvMapCoefficient;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying mediump vec2 Out_Tex1;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying lowp vec4 Out_Color;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform lowp float AlphaModulate;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying lowp vec3 Out_Spec;\n");
    strcat(byte_617290, s);
    strcpy(s, "void main()");
    strcat(byte_617290, s);

    strcpy(s, (const char *)"{");
    strcat(byte_617290, s);


    //water
    strcpy(s, "\tmediump vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;\n");
  strcat(byte_617290, s);
      strcpy(s, "vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);");
  strcat(byte_617290, s);
      strcpy(s, "ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);");
  strcat(byte_617290, s);
    strcpy(s, "\n");
  strcat(byte_617290, s);
    strcpy(s, "\thighp float Waveness = 6.0;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tlowp vec2 distortedTexCoords = texture2D(DuDvMap, vec2(Out_Tex0.x + Waveness, Out_Tex0.y)).rg * 0.");
  strcat(byte_617290, s);
    strcpy(s, "1;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tdistortedTexCoords = Out_Tex0 + vec2(distortedTexCoords.x, distortedTexCoords.y + Waveness);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tmediump vec2 distSum = (texture2D(DuDvMap, distortedTexCoords).rg * 2.0 - 1.0) * 0.01;\n");
  strcat(byte_617290, s);
    strcpy(s, "\n");
  strcat(byte_617290, s);
    strcpy(s, "\tlowp vec2 startCoordOnTex = vec2(0.0, 0.0);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tmediump vec4 waterMask = texture2D(Diffuse, startCoordOnTex);\n");
  strcat(byte_617290, s);


    strcpy(s, "lowp vec4 fcolor;");
    strcat(byte_617290, s);

    if ( (flags & 0x400) != 0 )
    {

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
        "fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * 1.0, -0.5).xyz * 2.0, Out_Color.w);");
  LABEL_42:
      strcat(byte_617290, s);

      goto LABEL_43;
    }
    snprintf(
      s,
      0x200u,
      "float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;");
    strcat(byte_617290, s);

    strcpy(s, "fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);");
    strcat(byte_617290, s);

    strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
    strcat(byte_617290, s);

    strcpy(s, "vec4 reflectColour = texture2D(EnvMap, Out_WaterDetail2);");
    strcat(byte_617290, s);

    strcpy(s, "vec4 refractColour = texture2D(Diffuse, Out_WaterDetail2);");
    strcat(byte_617290, s);

    strcpy(s, "fcolor = mix(reflectColour, refractColour, 0.5);");
    strcat(byte_617290, s);

    if ( (flags & 0x40) != 0 )
      goto LABEL_44;
  LABEL_45:
    if ( (flags & 0x1000000) != 0 )
    {

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

  LABEL_71:
    if ( (flags & 4) != 0 )
    {
      strcpy(s, "gl_FragColor.a *= AlphaModulate;");
      strcat(byte_617290, s);

    }

    //water
    strcpy(s, "\tif((waterMask.r == 0.0) && (waterMask.g > 0.25) && (waterMask.b > 0.25)){\n");
  strcat(byte_617290, s);
    strcpy(s, "\tlowp vec4 reflectTex = texture2D(EnvMap, ReflPos); \n");
  strcat(byte_617290, s);
    strcpy(s, "\n");
  strcat(byte_617290, s);
    strcpy(s, "\tvec3 ViewVector = normalize(Out_CameraVector);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tfloat RefractionCoef = dot(ViewVector, vec3(0.0, 0.0, 1.0));\n");
  strcat(byte_617290, s);
    strcpy(s, "\tRefractionCoef = pow(RefractionCoef, 1.3);\n");
  strcat(byte_617290, s);
    strcpy(s, "\t\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor = reflectTex;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor.a = 1.0 - RefractionCoef;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor = mix(gl_FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor = mix(gl_FragColor, texture2D(DuDvMap, Out_DuDvTexCoord), 0.2);\n");
  strcat(byte_617290, s);
    strcpy(s, "\t}\n");
  strcat(byte_617290, s);

    strcpy(s, "}");
    strcat(byte_617290, s);*/


    /*strcpy(s, "#version 100\n");
    strcat(byte_617290, s);
    strcpy(s, "precision highp float;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D Diffuse;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D EnvMap;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D RefractionMap;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D DuDvMap;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform sampler2D FogMap;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform highp float SkyMixCoef;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec2 Out_Tex0;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec4 Out_ClipSpace;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec3 Out_CameraVector;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec2 Out_DuDvTexCoord;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying vec3 Out_Refl;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying mediump float Out_FogAmt;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform lowp float EnvMapCoefficient;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying mediump vec2 Out_Tex1;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying lowp vec4 Out_Color;\n");
    strcat(byte_617290, s);
    strcpy(s, "uniform lowp float AlphaModulate;\n");
    strcat(byte_617290, s);
    strcpy(s, "varying lowp vec3 Out_Spec;\n");
    strcat(byte_617290, s);
    strcpy(s, "void main()");
    strcat(byte_617290, s);

    strcpy(s, (const char *)"{");
    strcat(byte_617290, s);

  //  strcpy(s, "\thighp float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;\n");
  //strcat(byte_617290, s);
    strcpy(s, "\tmediump vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;\n");
  strcat(byte_617290, s);
    strcpy(s, "vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);\n");
  strcat(byte_617290, s);
    strcpy(s, "\n");
  strcat(byte_617290, s);
    strcpy(s, "\thighp float Waveness = 6.0;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tlowp vec2 distortedTexCoords = texture2D(DuDvMap, vec2(Out_Tex0.x + Waveness, Out_Tex0.y)).rg * 0.");
  strcat(byte_617290, s);
    strcpy(s, "1;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tdistortedTexCoords = Out_Tex0 + vec2(distortedTexCoords.x, distortedTexCoords.y + Waveness);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tmediump vec2 distSum = (texture2D(DuDvMap, distortedTexCoords).rg * 2.0 - 1.0) * 0.01;\n");
  strcat(byte_617290, s);
    strcpy(s, "\n");
  strcat(byte_617290, s);
    strcpy(s, "\tReflPos += distSum;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tReflPos = clamp(ReflPos, 0.001, 0.999); \n");
  strcat(byte_617290, s);

    strcpy(s, "\tlowp vec2 startCoordOnTex = vec2(0.0, 0.0);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tmediump vec4 waterMask = texture2D(Diffuse, startCoordOnTex);\n");
  strcat(byte_617290, s);

    strcpy(s, "lowp vec4 fcolor;\n");
    strcat(byte_617290, s);

    if ( (flags & 0x400) != 0 )
    {

      strcpy(s, "lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);\n");
      strcat(byte_617290, s);

      strcpy(s, "lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);\n");
      strcat(byte_617290, s);

    }
    if ( (flags & 0x20) == 0 )
    {
      if ( (flags & 0x12) != 0 )
        strcpy(s, "fcolor = Out_Color;\n");
      else
        strcpy(s, "fcolor = 0.0;\n");
      goto LABEL_42;
    }
    if ( (flags & 0x800) != 0 )
    {
      strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -1.5);\n");
    }
    else if ( *(char *)(RQCaps + 14) )
    {
      strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);\n");
    }
    else
    {
      strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -0.5);\n");
    }
    strcat(byte_617290, s);

    strcpy(s, "fcolor = diffuseColor;\n");
    strcat(byte_617290, s);

    if ( (flags & 0x12) == 0 )
    {
      if ( !v2 )
        goto LABEL_43;
      strcpy(s, "fcolor.a += Out_WaterAlphaBlend;\n");
      goto LABEL_42;
    }
    if ( (flags & 0x10000) == 0 )
    {
      strcpy(s, "fcolor *= Out_Color;\n");
      strcat(byte_617290, s);

      if ( v2 )
      {
        strcpy(s, "fcolor.a += Out_WaterAlphaBlend;\n");
        strcat(byte_617290, s);


        if ( (flags & FLAG_ENVMAP) == 0 )
          goto LABEL_45;
  LABEL_44:
        strcpy(s, "fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);\n");
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
        "fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * 1.0, -0.5).xyz * 2.0, Out_Color.w);\n");
  LABEL_42:
      strcat(byte_617290, s);

      goto LABEL_43;
    }
    snprintf(
      s,
      0x200u,
      "float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;\n");
    strcat(byte_617290, s);

    strcpy(s, "fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);\n");
    strcat(byte_617290, s);

    strcpy(s, "fcolor.a += Out_WaterAlphaBlend;\n");
    strcat(byte_617290, s);

    strcpy(s, "vec4 reflectColour = texture2D(EnvMap, Out_WaterDetail2);\n");
    strcat(byte_617290, s);

    strcpy(s, "vec4 refractColour = texture2D(Diffuse, Out_WaterDetail2);\n");
    strcat(byte_617290, s);

    strcpy(s, "fcolor = mix(reflectColour, refractColour, 0.5);\n");
    strcat(byte_617290, s);

    if ( (flags & 0x40) != 0 )
      goto LABEL_44;
  LABEL_45:
    if ( (flags & 0x1000000) != 0 )
    {

      strcpy(s, "lowp vec4 ReflTexture =  texture2D(EnvMap, ReflPos);\n");
      strcat(byte_617290, s);

      strcpy(s, "float newEnvMapCoef = EnvMapCoefficient + 0.14;\n");
      strcat(byte_617290, s);

      strcpy(s, "fcolor.xyz = mix(fcolor.xyz,ReflTexture.xyz, newEnvMapCoef);\n");
      strcat(byte_617290, s);

      strcpy(s, "fcolor.w += ReflTexture.b * 0.125;\n");
      strcat(byte_617290, s);

    }
    if ( !*(char*)(RQCaps + 8) )
    {
      v3 = (flags & 0x2000) == 0;
      if ( (flags & 0x2000) != 0 )
        v3 = (flags & 0x1000040) == 0;
      if ( !v3 )
      {
        strcpy(s, "fcolor.xyz += Out_Spec;\n");
        strcat(byte_617290, s);

      }
      if ( (flags & 0x400) != 0 )
      {
        strcpy(s, "fcolor.xyz = mix(fcolor.xyz, fogColorNew.xyz, Out_FogAmt);\n");
        strcat(byte_617290, s);

      }
    }
    if ( (flags & 0x4000000) != 0 )
    {
      strcpy(s, "fcolor.xyz += fcolor.xyz * 0.5;\n");
      strcat(byte_617290, s);

    }
    strcpy(s, "gl_FragColor = fcolor;\n");
    strcat(byte_617290, s);
    goto LABEL_71;

  LABEL_71:
    if ( (flags & 4) != 0 )
    {
      strcpy(s, "gl_FragColor.a *= AlphaModulate;\n");
      strcat(byte_617290, s);

    }

    strcpy(s, "\tif((waterMask.r == 0.0) && (waterMask.g > 0.25) && (waterMask.b > 0.25)){\n");
  strcat(byte_617290, s);
    strcpy(s, "\tlowp vec4 reflectTex = texture2D(EnvMap, ReflPos); \n");
  strcat(byte_617290, s);
    strcpy(s, "\n");
  strcat(byte_617290, s);
    strcpy(s, "\tvec3 ViewVector = normalize(Out_CameraVector);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tfloat RefractionCoef = dot(ViewVector, vec3(0.0, 0.0, 1.0));\n");
  strcat(byte_617290, s);
    strcpy(s, "\tRefractionCoef = pow(RefractionCoef, 1.3);\n");
  strcat(byte_617290, s);
    strcpy(s, "\t\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor = reflectTex;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor.a = 1.0 - RefractionCoef;\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor = mix(gl_FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor = mix(gl_FragColor, texture2D(DuDvMap, Out_DuDvTexCoord), 0.2);\n");
  strcat(byte_617290, s);
    strcpy(s, "\t}\n");
  strcat(byte_617290, s);
    strcpy(s, "\tbool condition = (waterMask == vec4(1.0, 1.0, 1.0, 1.0));\n");
  strcat(byte_617290, s);
    strcpy(s, "\tif(condition){\n");
  strcat(byte_617290, s);
    strcpy(s, "\tgl_FragColor = mix(Out_Color, (texture2D(EnvMap, Out_Tex0) * 0.25), 0.6);\n");
  strcat(byte_617290, s);
    strcpy(s, "\t}\n");
  strcat(byte_617290, s);

    strcpy(s, "}");
    strcat(byte_617290, s);*/

    /*
    precision mediump float;
    uniform sampler2D Diffuse;
    varying mediump vec2 Out_Tex0;
    uniform mediump vec4 RedGrade;
    void main(){
    vec4 color = texture2D(Diffuse, Out_Tex0);
    gl_FragColor = vec4(0, 0, 0, (1.0 - color.x) * RedGrade.a);
    }
    */

    if(!(flags & FLAG_WATER)){
        return RQShader__BuildSource(flags, pxlsrc, vtxsrc);
    }

    *(uintptr_t *)(g_libGTASA + 0x61572C) |= FLAG_SPHERE_ENVMAP;
    flags |= FLAG_SPHERE_ENVMAP;

    /*

struct VS_OUTPUT_HS_INPUT
{
  vec3 vWorldPos : WORLDPOS;
  vec3 vNormal : NORMAL;

  vec2 vTexCoord : TEXCOORD0;
  vec4 vColor : COLOR;
  float fVertexDistanceFactor : VERTEXDISTANCEFACTOR;
};

VS_OUTPUT_HS_INPUT VS(VS_INPUT i)
{
  VS_OUTPUT_HS_INPUT Out;
// Compute clip-space position.
float4  outPos = float4(i.inPosition.xyz, 1.0);
  //outPos.z += sin(i.inPosition.x) * cos(i.inPosition.y);
//outPos = mul(outPos, World);
  Out.vWorldPos = outPos.xyz;

//Out.vPosition = mul(outPos, Projection);

Out.vNormal = i.vInNormal;//float4(mul(i.vInNormal, (mat3)World), outPos.z);
  Out.vTexCoord = i.inTexCoord;
Out.vColor = i.vInColor;
  // Min and max distance should be chosen according to scene quality requirements
  const float fMinDistance = 2.0f;
  const float fMaxDistance = 550.0f;

  // Calculate distance between vertex and camera, and a vertex distance factor issued from it
  float fDistance = distance(outPos.xyz, mViewInv[3].xyz);
  Out.fVertexDistanceFactor = 1.0 - clamp(((fDistance - fMinDistance) / (fMaxDistance - fMinDistance)),
                                           0.0, 1.0 - (1.0 / 16.0f));

return Out;
}
    */

    /*


    precision mediump float;
    uniform sampler2D Diffuse;
    varying mediump vec2 Out_Tex0;
    varying mediump float Out_FogAmt;
    uniform lowp vec3 FogColor;
    uniform sampler2D FogMap;
    varying lowp vec4 Out_ClipSpace;
    varying lowp vec4 Out_Color;
    void main(){
    lowp vec4 fcolor;
    lowp vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;
    lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);
    lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);
    lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);
    fcolor = diffuseColor;
    fcolor *= Out_Color;
    gl_FragColor = fcolor;
    if (gl_FragColor.a < 0.2) { discard; }
    }


    precision mediump float;
    uniform sampler2D Diffuse;
    varying mediump vec2 Out_Tex0;
    varying mediump float Out_FogAmt;
    uniform lowp vec3 FogColor;
    uniform sampler2D FogMap;
    varying lowp vec4 Out_ClipSpace;
    varying lowp vec4 Out_Color;
    void main(){
    lowp vec4 fcolor;
    lowp vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;
    lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);
    lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);
    lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);
    fcolor = diffuseColor;
    fcolor *= Out_Color;
    gl_FragColor = fcolor;
    }


    precision mediump float;
    uniform sampler2D Diffuse;
    varying mediump vec2 Out_Tex0;
    varying lowp vec4 Out_Color;
    void main(){
    lowp vec4 fcolor;
    lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);
    fcolor = diffuseColor;
    fcolor *= Out_Color;
    fcolor.xyz += fcolor.xyz * 0.5;
    gl_FragColor = fcolor;
    }
    */

    /*precision mediump float;
    uniform sampler2D Diffuse;
    varying mediump vec2 Out_Tex0;
    uniform sampler2D EnvMap;
    uniform lowp float EnvMapCoefficient;
    varying mediump vec3 Out_Refl;
    varying mediump float Out_FogAmt;
    uniform lowp vec3 FogColor;
    uniform sampler2D FogMap;
    varying lowp vec4 Out_ClipSpace;
    varying lowp vec4 Out_Color;
    varying lowp vec3 Out_Spec;
    void main(){
      lowp vec4 fcolor;
      lowp vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;
      lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);
      lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);
      lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);
      fcolor = diffuseColor;
      fcolor *= Out_Color;
      vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);
      ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);
      lowp vec4 ReflTexture =  texture2D(EnvMap, ReflPos);
      float newEnvMapCoef = EnvMapCoefficient + 0.14;
      fcolor.xyz = mix(fcolor.xyz,ReflTexture.xyz, newEnvMapCoef);
      fcolor.w += ReflTexture.b * 0.125;
      gl_FragColor = fcolor;
      }

    */


    /*
    precision highp float;
    uniform mat4 ProjMatrix;
    uniform mat4 ViewMatrix;
    uniform mat4 ObjMatrix;
    uniform lowp vec3 AmbientLightColor;
    uniform lowp vec4 MaterialEmissive;
    uniform lowp vec4 MaterialAmbient;
    uniform lowp vec4 MaterialDiffuse;
    uniform lowp vec3 DirLightDiffuseColor;
    uniform vec3 DirLightDirection;
    uniform lowp vec3 DirLight2DiffuseColor;
    uniform vec3 DirLight2Direction;
    attribute vec3 Position;
    attribute vec3 Normal;
    attribute vec2 TexCoord0;
    attribute vec4 GlobalColor;
    varying mediump vec2 Out_Tex0;
    uniform vec3 CameraPosition;
    varying mediump float Out_FogAmt;
    uniform vec3 FogDistances;
    varying mediump vec4 Out_ClipSpace;
    varying lowp vec4 Out_Color;
    void main() {
    vec4 WorldPos = ObjMatrix * vec4(Position,1.0);
    vec4 ViewPos = ViewMatrix * WorldPos;
    gl_Position = ProjMatrix * ViewPos;
    Out_ClipSpace = gl_Position;
    vec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;
    Out_Tex0 = TexCoord0;
    vec3 Out_LightingColor;
    Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + MaterialEmissive.xyz;
    Out_LightingColor += max(dot(DirLightDirection, WorldNormal), 0.0) * DirLightDiffuseColor;
    Out_LightingColor += max(dot(DirLight2Direction, WorldNormal), 0.0) * DirLight2DiffuseColor;
    Out_Color = vec4((Out_LightingColor.xyz + GlobalColor.xyz * 1.5) * MaterialDiffuse.xyz, (MaterialAmbient.w) * GlobalColor.w);
    Out_Color = clamp(Out_Color, 0.0, 1.0);
    }



    precision highp float;
    uniform mat4 ProjMatrix;
    uniform mat4 ViewMatrix;
    uniform mat4 ObjMatrix;
    uniform lowp vec3 AmbientLightColor;
    uniform lowp vec4 MaterialEmissive;
    uniform lowp vec4 MaterialAmbient;
    uniform lowp vec4 MaterialDiffuse;
    attribute vec3 Position;
    attribute vec3 Normal;
    attribute vec2 TexCoord0;
    attribute vec4 GlobalColor;
    varying mediump vec2 Out_Tex0;
    uniform vec3 CameraPosition;
    varying mediump float Out_FogAmt;
    uniform vec3 FogDistances;
    varying mediump vec4 Out_ClipSpace;
    attribute vec4 Color2;
    uniform lowp float ColorInterp;
    varying lowp vec4 Out_Color;
    void main() {
    vec4 WorldPos = ObjMatrix * vec4(Position,1.0);
    vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;
    vec3 ReflPos = normalize(ReflVector);
    ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);
    gl_Position = vec4(ReflPos.xy, length(ReflVector) * 0.002, 1.0);
    vec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;
    Out_Tex0 = TexCoord0;
    lowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);
    vec3 Out_LightingColor;
    Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + InterpColor.xyz;
    Out_Color = vec4(Out_LightingColor * MaterialDiffuse.xyz, MaterialAmbient.w * InterpColor.w);
    Out_Color = clamp(Out_Color, 0.0, 1.0);
    }
    */


    if((flags & FLAG_SPHERE_ENVMAP) == 0){
        return RQShader__BuildSource(flags, pxlsrc, vtxsrc);
    }


    //in black russia used of sub_6FF3C

    //pixel shader source (obfuscated)
    char* pxl = OBFUSCATE("#version 100\n"
                          "precision highp float;\n"
                          "uniform sampler2D Diffuse;\n"
                          "uniform sampler2D EnvMap;\n"
                          "uniform sampler2D RefractionMap;\n"
                          "uniform sampler2D DuDvMap;\n"
                          "uniform sampler2D FogMap;\n"
                          "uniform highp float SkyMixCoef;\n"
                          "varying vec2 Out_Tex0;\n"
                          "varying vec4 Out_ClipSpace;\n"
                          "varying vec3 Out_CameraVector;\n"
                          "varying vec2 Out_DuDvTexCoord;\n"
                          "varying vec3 Out_Refl;\n"
                          "varying mediump float Out_FogAmt;\n"
                          "uniform lowp float EnvMapCoefficient;\n"
                          "varying lowp vec3 Out_Spec;\n"
                          "varying mediump vec2 Out_Tex1;\n"
                          "varying lowp vec4 Out_Color;\n"
                          "varying mediump vec2 Out_WaterDetail;\n"
                          "varying mediump vec2 Out_WaterDetail2;\n"
                          "varying mediump float Out_WaterAlphaBlend;\n"
                          "varying mediump vec4 Out_Position;\n"
                          "varying vec3 Out_Vertex;\n"
                          "varying vec3 Out_CameraPos;\n"
                          "varying vec3 Out_Normal;\n"
                          "varying vec3 Out_DirLightDirection;\n"
                          "vec2 ellipticalDiscToSquare(float u, float v)"
                          "{\n"
                          "    float u2 = u * u;\n"
                          "    float v2 = v * v;\n"
                          "    float twosqrt2 = 2.0 * sqrt(2.0);\n"
                          "    float subtermx = 2.0 + u2 - v2;\n"
                          "    float subtermy = 2.0 - u2 + v2;\n"
                          "    float termx1 = subtermx + u * twosqrt2;\n"
                          "    float termx2 = subtermx - u * twosqrt2;\n"
                          "    float termy1 = subtermy + v * twosqrt2;\n"
                          "    float termy2 = subtermy - v * twosqrt2;\n"
                          "    float x = 0.5 * sqrt(termx1) - 0.5 * sqrt(termx2);\n"
                          "    float y = 0.5 * sqrt(termy1) - 0.5 * sqrt(termy2);\n"
                          "    \n"
                          "    vec2 xy = vec2(x, y);\n"
                          "    return xy;\n"
                          "}\n"
                          "void main()\n"
                          "{\n"
                          "\tlowp vec2 startCoordOnTex = vec2(0.0, 0.0);\n"
                          "\thighp vec4 waterMask = texture2D(Diffuse, startCoordOnTex);\n"
                          "\tvec2 ReflPosCar = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);\n"
                          "\tReflPosCar = (ReflPosCar * vec2(0.5,0.5)) + vec2(0.5,0.5);\n"
                          "\tgl_FragColor = vec4(Out_Color.xyz * texture2D(Diffuse, Out_Tex0.xy * 1.0, -0.5).xyz * 2.0, Out_Color.w) * texture2D(Diffuse, Out_Tex0);\n"
                          "\tif((texture2D(Diffuse, startCoordOnTex).xyz == vec3(1.0, 1.0, 1.0))){\n"
                          "\tlowp vec4 fcolor;\n"
                          "\tlowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);\n"
                          "\tfcolor = diffuseColor;\n"
                          "\tfcolor *= Out_Color;\n"
                          "\tfcolor.xyz += fcolor.xyz * 0.5;\n"
                          "\tgl_FragColor = fcolor;\n"
                          "\t}\n"
                          "\tif((waterMask.rgb == vec3(1.0, 1.0, 1.0)) && (texture2D(Diffuse, vec2(0.5, 0.5)).rgb == vec3(1.0, 1.0, 1.0))){\n"
                          "\thighp vec3 ViewVectorCar = normalize(Out_Vertex - Out_CameraPos);\n"
                          "\thighp vec3 ReflCar = reflect(normalize(Out_CameraVector), Out_Normal);\n"
                          //"\tRefl *= cos(length(normalize(Out_CameraVector)));\n"
                          "highp float angle1 = acos(dot(ViewVectorCar, normalize(ReflCar)));\n" // угол в радианах
                          "highp float newLength1 = length(ViewVectorCar) / cos(angle1);\n"// новая длина второго вектора
                          "ReflCar = normalize(ReflCar) * newLength1;" // установка новой длины вектора
                          "ReflCar = -ReflCar;"
                          "\thighp vec2 ReflPosCar1 = normalize(ReflCar.xy) * (ReflCar.z + 0.5); \n"
                          "\tReflPosCar1 = (ReflPosCar1 * vec2(0.5, 0.5)) + vec2(0.5, 0.5); \n"
                          //"\tReflPosCar1 += distSum;\n"
                          "\n"
                          //"\tReflPos = rotate(ReflPos, radians(90.0)); \n"
                          "\tReflPosCar1 = clamp(ReflPosCar1, 0.0, 1.0); \n"
                          "\tReflPosCar1 = vec2(1.0 - ReflPosCar1.x, 1.0 - ReflPosCar1.y); \n"
                          "\tvec3 normal_ = normalize(Out_Normal);\n"
                          "\tfloat diff_ = max(dot(normal_, Out_DirLightDirection), 0.0);\n"
                          "\tvec3 distColor_ = vec3(0.85, 0.85, 1.0);\n"
                          "\tvec3 finalColor_ = mix(gl_FragColor.rgb, distColor_, diff_ *0.35);\n"
                          "\tgl_FragColor.rgb = finalColor_;\n"
                          //"\tgl_FragColor = texture2D(EnvMap, ReflPosCar1);\n"
                          "\tgl_FragColor = mix(gl_FragColor, vec4(Out_Color.xyz * texture2D(Diffuse, Out_Tex0.xy * 1.0, -0.5).xyz * 2.0, Out_Color.w) * texture2D(Diffuse, Out_Tex0), 0.8);\n"
                          "\tgl_FragColor = mix(gl_FragColor, vec4(1.0, 1.0, 1.0, 1.0), 0.4);\n"
                          "\t}\n"
                          "\tif((waterMask.r == 0.0) && (waterMask.g > 0.25) && (waterMask.b > 0.25))\n"
                          "\t{\n"
                          "\n"
                          "\thighp float Waveness = 1.0;\n"
                          "\tvec3 ViewVector = normalize(Out_Vertex - Out_CameraPos);\n"
                          "\tvec3 Refl = reflect(normalize(Out_CameraVector), Out_Normal);\n"
                          //"\tRefl *= cos(length(normalize(Out_CameraVector)));\n"
                          "float angle = acos(dot(normalize(Out_CameraVector), normalize(Refl)));\n" // угол в радианах
                          "float newLength = length(normalize(Out_CameraVector)) / cos(angle);\n"// новая длина второго вектора
                          "Refl = normalize(Refl) * newLength;" // установка новой длины вектора
                          //"Refl = -Refl;"
                          //"\tRefl = clamp(Refl, 0.0, 1.0); \n"
                          "\tfloat diff1 = max(dot(Refl, Out_DirLightDirection), 0.0);\n"
                          "\tvec3 distColor1 = vec3(1.0, 1.0, 1.0);\n"
                          "\thighp vec2 distortedTexCoords = texture2D(DuDvMap, vec2(Out_DuDvTexCoord.x + Waveness, Out_DuDvTexCoord.y)).rg * 0.1;\n"
                          "\tdistortedTexCoords = Out_DuDvTexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + Waveness);\n"
                          "\thighp vec2 distSum = (texture2D(DuDvMap, distortedTexCoords).rg * 2.0 - 1.0) * 0.01;\n"
                          "\n"
                          "\tvec2 ReflPos = normalize(Refl.xy) * (Refl.z + 0.5); \n"
                          "\tReflPos = (ReflPos * vec2(0.5, 0.5)) + vec2(0.5, 0.5); \n"
                          "\tReflPos += distSum;\n"
                          "\n"
                          //"\tReflPos = rotate(ReflPos, radians(90.0)); \n"
                          "\tReflPos = clamp(ReflPos, 0.0, 1.0); \n"
                          "\tReflPos = vec2(1.0 - ReflPos.x, 1.0 - ReflPos.y); \n"
                          "\n"
                          /*"\tmediump float sphereRadius = 0.5;\n"
                          "\thighp float theta = 2.0 * 3.14159 * ReflPos.y;\n"
                          "\thighp float phi = (2.0 * 3.14159 * ReflPos.x) - 3.14159;\n"
                          "\t\n"
                          "\tvec2 centeredCoords;\n"
                          "\tcenteredCoords.x = cos(phi) * cos(theta);\n"
                          "\tcenteredCoords.y = cos(phi) * sin(theta);\n"
                          "\n"
                          "\tvec2 textureReflCoords = centeredCoords * sphereRadius + vec2(0.5, 0.5);\n"
                          "\n"
                          //"\ttextureReflCoords += distSum;\n"
                          "\ttextureReflCoords = clamp(textureReflCoords, 0.0, 1.0); \n"
                          "\n"
                          "\tvec2 normalizedTexCoord = ReflPos * 2.0 - vec2(1.0);\n"
                          "\n"
                          "\tfloat radius = length(normalizedTexCoord);\n"
                          "\tfloat angle = atan(normalizedTexCoord.y, normalizedTexCoord.x);\n"
                          "\n"
                          "\tvec2 flatTexCoord = vec2(angle / (2.0 * 3.1415926535), radius);"*/
                          /*"\n"
                          "vec3 normalizedReflection = normalize(Out_Refl);\n"
                          "\n"
                          "\thighp float azimuthal = atan(normalizedReflection.y, normalizedReflection.x);\n"
                          "\thighp float inclination = acos(normalizedReflection.z);\n"
                          "\n"
                          "\thighp float u = (azimuthal + 3.1415926535) / (2.0 * 3.1415926535);\n"
                          "\thighp float v = inclination / 3.1415926535;"
                          "\thighp vec2 ReflCoord = vec2(u, v);"
                          "\n"*/
                          "\thighp vec4 reflectTex = texture2D(EnvMap, ReflPos); \n"//ellipticalDiscToSquare(ReflPos.x, ReflPos.y)
                          "\n"
                          "\tfloat samples = 4.0;\n"
                          "\t\n"
                          "\tfloat offsetX[4];\n"
                          "\tfloat offsetY[4];\n"
                          "\t\n"
                          "\toffsetX[0] = 0.25;\n"
                          "\toffsetY[0] = 0.25;\n"
                          "\t\n"
                          "\toffsetX[1] = 0.75;\n"
                          "\toffsetY[1] = 0.25;\n"
                          "\t\n"
                          "\toffsetX[2] = 0.25;\n"
                          "\toffsetY[2] = 0.75;\n"
                          "\t\n"
                          "\toffsetX[3] = 0.75;\n"
                          "\toffsetY[3] = 0.75;\n"
                          "\t\n"
                          "\tvec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
                          "\t\n"
                          "\tvec2 subpixelCoord = Out_Tex0 + vec2(offsetX[0], offsetY[0]);\n"
                          "\tcolor += texture2D(Diffuse, subpixelCoord);\n"
                          "\tsubpixelCoord = Out_Tex0 + vec2(offsetX[1], offsetY[1]);\n"
                          "\tcolor += texture2D(Diffuse, subpixelCoord);\n"
                          "\tsubpixelCoord = Out_Tex0 + vec2(offsetX[2], offsetY[2]);\n"
                          "\tcolor += texture2D(Diffuse, subpixelCoord);\n"
                          "\tsubpixelCoord = Out_Tex0 + vec2(offsetX[3], offsetY[3]);\n"
                          "\tcolor += texture2D(Diffuse, subpixelCoord);\n"
                          "\t\n"
                          "\tcolor /= samples;\n"
                          //"\tfloat dist = distance(ReflPos * 512.0, vec2(256.0, 256.0));\n"
                          //"\tif (dist > 128.0) {\n"
                          //"\t\tfloat angle = atan(ReflPos.y - 0.5, ReflPos.x - 0.5);\n"
                          //"\t\tvec2 newTexCoord = ReflPos - vec2(cos(angle), sin(angle)) * 128.0 / 512.0;\n"
                          //"\t\treflectTex = texture2D(EnvMap, newTexCoord);"
                          //"\t}"
                          "\tvec3 brightnessVec = vec3(1.0) - normalize(Out_CameraVector);\n"
                          "\tbrightnessVec = vec3(0.5 - brightnessVec.x, 0.5 - brightnessVec.y, 0.5 - brightnessVec.z);\n"
                          "\tfloat brightness = brightnessVec.x * brightnessVec.y * brightnessVec.z;\n"
                          "\tgl_FragColor = reflectTex;\n"
                          "\tgl_FragColor = mix(gl_FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.15);\n"
                          "\tcolor = mix(color, vec4(0.0, 0.3, 0.5, 1.0), 0.5);\n"
                          "\tgl_FragColor = mix(gl_FragColor, color, 0.35);\n"
                          //"\tgl_FragColor.rgb = mix(gl_FragColor.rgb, distColor1.rgb, brightness*0.3);\n"
                          //"\tgl_FragColor.a = 0.75;\n"
                          //"\tgl_FragColor.a = 1.0;\n"
                          /*"  vec3 N = normalize(Out_Normal + texture2D(DuDvMap, Out_Tex0).rgb * 2.0 - 1.0);\n"
                          "  vec3 R = -Out_CameraVector;\n"
                          "  vec2 envMapCoord = vec3(-R.x, R.y);\n"
                          "  float fresnel = pow(1.0 - max(dot(N, Out_CameraVector), 0.0), 0.5);\n"
                          "  vec4 reflectColor = texture2D(EnvMap, envMapCoord);\n"
                          "  vec4 refractColor = texture2D(Diffuse, Out_Tex0);\n"
                          "  vec4 waterColor = mix(refractColor, reflectColor, fresnel * 0.6);\n"
                          "  vec3 L = normalize(Out_DirLightDirection);\n"
                          "  vec3 V = normalize(Out_CameraVector);\n"
                          "  vec3 H = normalize(L + V);\n"
                          "  float diffuse = max(dot(N, L), 0.0);\n"
                          "  float specular = pow(max(dot(N, H), 0.0), 32.0);\n"
                          "  vec3 lightColor = Out_AmbientLightColor + Out_DirLightDiffuseColor * (diffuse + specular);\n"
                          "  waterColor.rgb *= lightColor;\n"
                          "  float height = texture2D(DuDvMap, Out_Tex0 + vec2(1.0 * 0.75, 0.0)).r;\n"
                          "  float foam = height;\n"
                          "  waterColor = mix(waterColor, vec4(1.0, 1.0, 1.0, 1.0), foam);"*/
                          "\t}\n"
                          "}\n");


    /*
    "void main()\n"
    "{\n"
    "\tlowp vec4 fcolor;\n"//color for setting after calculations
    "\thighp float Waveness = 6.0;\n"//waveness coefficient
    "\tlowp vec2 startCoordOnTex = vec2(0.0, 0.0);\n"//start coords
    "\tmediump vec4 waterMask = texture2D(Diffuse, startCoordOnTex);\n"//the color is to check if it`s a car, water or another`s
    "\tlowp vec4 standartColor = vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * 1.0, -0.5).xyz * 2.0, Out_Color.w);\n"//standart color(it usually fits everything)
    "\n"
    //for ped`s
    "\tif((standartColor.rgb == vec3(0.0, 0.0, 0.0)) && (waterMask.rgb != vec3(1.0, 1.0, 1.0))){\n"
    //"\tfcolor = vec4(0.0, 0.0, 0.0, 1.0);\n"//set color to 0
    "\tfcolor = texture2D(Diffuse, Out_Tex0);\n"//set color from standart texture
    "\t}\n"
    "\n"
    //for text`s
    "\tif((Out_ClipSpace.z < 0.1)){\n"
    //"\tfcolor = vec4(0.0, 0.0, 0.0, 1.0);\n"//set color to 0
    "\tfcolor = texture2D(Diffuse, Out_Tex0);\n"//set color from standart texture
    "\t}\n"
    "\n"
    //for vehicle`s(car`s)
    "\tif((waterMask.rgb == vec3(1.0, 1.0, 1.0))){\n"
    //"\tfcolor = vec4(0.0, 0.0, 0.0, 1.0);\n"//set color to 0
    "\tfcolor = standartColor;\n"//set standart color
    //"\thighp vec3 reflectTexCar = texture2D(EnvMap, ((Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5)).rgb;\n"
    //"\tfcolor.rgb = mix(fcolor.rgb, reflectTex, 0.25);\n"
    //"\tfcolor.a = reflectTexCar.b * 0.125;\n"
    "\t}\n"
    "\n"
    //for water
    "\tif((waterMask.r == 0.0) && (waterMask.g > 0.25) && (waterMask.b > 0.25))\n"
    "\t{\n"
    "\tmediump vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;\n"//normalized device coordinates
    "\n"
    "\tlowp vec2 distortedTexCoords = texture2D(DuDvMap, vec2(Out_DuDvTexCoord.x + Waveness, Out_DuDvTexCoord.y)).rg * 0."//distored coords for texture
    "1;\n"
    "\tdistortedTexCoords = Out_DuDvTexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + Waveness);\n"
    "\tmediump vec2 distSum = (texture2D(DuDvMap, distortedTexCoords).rg * 2.0 - 1.0) * 0.01;\n"
    "\n"
    "\tmediump vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5); \n"//coords for refl`s
    "\tReflPos = (ReflPos * vec2(0.5, 0.5)) + vec2(0.5, 0.5); \n"
    "\tReflPos += distSum;\n"
    "\n"
    "\tReflPos = clamp(ReflPos, 0.001, 0.999); \n"//guarantee that the number is in the range from 0.001 to 0.999, that is from 0.001 to 0.999
    "\n"
    "\thighp vec4 reflectTexWater = texture2D(EnvMap, ReflPos); \n"//getting distored and reflected color
    "\n"
    //"\tfcolor = vec4(0.0, 0.0, 0.0, 1.0);\n"//set color to 0
    "\tfcolor = reflectTexWater;\n"
    "\tfcolor = mix(fcolor, vec4(0.0, 0.4, 0.5, 1.0), 0.2);\n"
    "\t}\n"
    //assign the previously obtained color to gl_FragColor
    "\tgl_FragColor = fcolor;\n"//setting color to gl_FragColor
    "}\n"
    */


    /*
"#version 100\n"
"precision mediump float;\n"
"uniform sampler2D EnvMap;\n"
"uniform lowp float EnvMapCoefficient;\n"
"uniform sampler2D Diffuse;\n"
"varying mediump vec2 Out_Tex0;\n"
"varying mediump vec3 Out_Refl;\n"
"uniform float DetailTiling;\n"
"varying mediump float Out_WaterAlphaBlend;\n"
"varying mediump vec2 Out_WaterDetail;\n"
"varying mediump vec2 Out_WaterDetail2;\n"
"varying lowp vec3 Out_Spec;\n"
"void main()"
"{"
"lowp vec4 fcolor;\n"
"lowp vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;\n"
"lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);\n"
"lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);\n"
"lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);\n"
"fcolor = diffuseColor;\n"
"fcolor *= Out_Color;\n"
"fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);\n"
"gl_FragColor = fcolor;\n"
"if (gl_FragColor.a < 0.5) { discard; }"
"gl_FragColor.a = Out_Color.a;\n"
"if (flags & 4)"
"gl_FragColor.a *= AlphaModulate;\n"
"}"
    */


    //vertex shader source (obfuscated)
    char* vtx = OBFUSCATE("#version 100\n"
                          "precision highp float;\n"
                          "uniform mat4 ProjMatrix;\n"
                          "uniform mat4 ViewMatrix;\n"
                          "uniform mat4 ObjMatrix;\n"
                          "uniform vec3 CameraPosition;\n"
                          "uniform vec3 SunPosition;\n"
                          "uniform lowp vec3 AmbientLightColor;\n"
                          "uniform lowp vec4 MaterialEmissive;\n"
                          "uniform lowp vec4 MaterialAmbient;\n"
                          "uniform lowp vec4 MaterialDiffuse;\n"
                          "uniform lowp vec3 DirLightDiffuseColor;\n"
                          "uniform vec3 DirLightDirection;\n"
                          "uniform lowp vec3 DirLight2DiffuseColor;\n"
                          "uniform vec3 DirLight2Direction;\n"
                          "uniform lowp vec3 DirLight3DiffuseColor;\n"
                          "uniform vec3 DirLight3Direction;\n"
                          "uniform vec3 DirBackLightDirection;\n"
                          "varying mediump float Out_FogAmt;\n"
                          "attribute vec3 Position;\n"
                          "attribute vec3 Normal;\n"
                          "attribute vec2 TexCoord0;\n"
                          "varying mediump vec2 Out_Tex0;\n"
                          "varying mediump vec4 Out_ClipSpace;\n"
                          "varying vec3 Out_CameraVector;\n"
                          "varying mediump vec2 Out_DuDvTexCoord;\n"
                          "varying mediump vec3 Out_Refl;\n"
                          "attribute vec4 BoneWeight;\n"
                          "attribute vec4 BoneIndices;\n"
                          "uniform highp vec4 Bones[96];\n"
                          "uniform lowp float EnvMapCoefficient;\n"
                          "varying mediump vec2 Out_Tex1;\n"
                          "varying lowp vec4 Out_Color;\n"
                          "attribute vec4 GlobalColor;\n"
                          "uniform lowp float ColorInterp;\n"
                          "attribute vec4 Color2;\n"
                          "varying lowp vec3 Out_Spec;\n"
                          "varying highp vec4 Out_Position;\n"
                          "uniform vec3 WaterSpecs;\n"
                          "varying mediump vec2 Out_WaterDetail;\n"
                          "varying mediump vec2 Out_WaterDetail2;\n"
                          "varying mediump float Out_WaterAlphaBlend;\n"
                          "varying mediump vec3 Out_LightingColor;\n"
                          "uniform sampler2D EnvMap;\n"
                          "uniform sampler2D Diffuse;\n"
                          "varying highp vec3 Out_Vertex;\n"
                          "varying highp vec3 Out_CameraPos;\n"
                          "varying highp vec3 Out_Normal;\n"
                          "varying highp vec3 Out_DirLightDirection;\n"
                          "float CalculateShadowFactor(vec4 fragPosLightSpace)\n"
                          "{\n"
                          "   float bias = 0.005;"
                          "    vec3 fragPos = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
                          "    float shadowFactor = 1.0;\n"
                          "    if (fragPos.z > 0.0)\n"
                          "    {\n"
                          "        vec3 projCoords = 0.5 * fragPosLightSpace.xyz / fragPosLightSpace.w + 0.5;\n"
                          "        projCoords = clamp(projCoords, 0.0, 1.0);\n"
                          "        float depth = texture2D(Diffuse, projCoords.xy).r;\n"
                          "        shadowFactor = (projCoords.z - bias > depth) ? 1.0 : 0.0;\n"
                          "    }\n"
                          "    return shadowFactor;\n"
                          "}\n"
                          "void main()\n"
                          "{\n"
                          "\tvec3 FogDistances = vec3(1.0, 1.0, 1.0);\n"
                          "\tOut_Tex0 = TexCoord0;\n"
                          "\tivec4 BlendIndexArray = ivec4(BoneIndices);\n"
                          "\tmat4 BoneToLocal;\n"
                          "\tBoneToLocal[0] = Bones[BlendIndexArray.x*3] * BoneWeight.x;\n"
                          "\tBoneToLocal[1] = Bones[BlendIndexArray.x*3+1] * BoneWeight.x;\n"
                          "\tBoneToLocal[2] = Bones[BlendIndexArray.x*3+2] * BoneWeight.x;\n"
                          "\tBoneToLocal[3] = vec4(0.0,0.0,0.0,1.0);\n"
                          "\tBoneToLocal[0] += Bones[BlendIndexArray.y*3] * BoneWeight.y;\n"
                          "\tBoneToLocal[1] += Bones[BlendIndexArray.y*3+1] * BoneWeight.y;\n"
                          "\tBoneToLocal[2] += Bones[BlendIndexArray.y*3+2] * BoneWeight.y;\n"
                          "\tBoneToLocal[0] += Bones[BlendIndexArray.z*3] * BoneWeight.z;\n"
                          "\tBoneToLocal[1] += Bones[BlendIndexArray.z*3+1] * BoneWeight.z;\n"
                          "\tBoneToLocal[2] += Bones[BlendIndexArray.z*3+2] * BoneWeight.z;\n"
                          "\tvec4 PlayerPos;\n"
                          "\tif(BoneWeight.x > 0.0){\n"
                          "\tPlayerPos = ObjMatrix * (vec4(Position,1.0) * BoneToLocal);\n"
                          "\t}else{\n"
                          "\tPlayerPos = ObjMatrix * vec4(Position,1.0);\n"
                          "\t}\n"
                          "\tvec4 ViewWithPlayerPos = ViewMatrix * PlayerPos;\n"
                          "\tOut_ClipSpace = ProjMatrix * ViewWithPlayerPos;\n"
                          "\tOut_Position = ProjMatrix * ObjMatrix * vec4(Position,1.0);\n"
                          "\tOut_CameraVector = CameraPosition - PlayerPos.xyz;\n"
                          "\tOut_DuDvTexCoord = vec2(Position.x/2.0 + 0.5, Position.y/2.0 + 0.5) * 0.2;\n"
                          "\t\n"
                          "\tvec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;\n"
                          "\tvec3 reflVector = normalize(PlayerPos.xyz - CameraPosition.xyz);\n"
                          "\treflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;\n"
                          "\tOut_Tex1 = vec2(length(reflVector.xy), (reflVector.z * 0.5) + 0.25);\n"
                          "\tOut_Refl = reflVector;\n"
                          "\tOut_FogAmt = clamp((length(PlayerPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 1.0);\n"
                          //"\tvec4 fragPosLightSpace = ProjMatrix * ViewMatrix * ObjMatrix * vec4(Position, 1.0);\n"
                          //"\tfloat shadowFactor = CalculateShadowFactor(fragPosLightSpace);\n"
                          "\tlowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);\n"
                          "\tOut_LightingColor = AmbientLightColor * MaterialAmbient.xyz + InterpColor.xyz;\n"
                          "\tOut_LightingColor += max(dot(DirLightDirection, WorldNormal), 0.0) * DirLightDiffuseColor;\n"
                          "\tOut_LightingColor += max(dot(DirLight2Direction, WorldNormal), 0.0) * DirLight2DiffuseColor;\n"
                          "\tOut_Color = vec4((Out_LightingColor.xyz + InterpColor.xyz * 1.5) * MaterialDiffuse.xyz, (MaterialAmbient.w) * InterpColor.w);\n"
                          "\tOut_Color = clamp(Out_Color, 0.0, 1.0);\n"
                          //"\tOut_Color = vec4(Out_LightingColor * MaterialDiffuse.xyz, MaterialAmbient.w * InterpColor.w);\n"
                          "\tfloat specAmt = max(pow(dot(reflVector, DirLightDirection), 1.0), 0.0) * EnvMapCoefficient * 2.0;\n"
                          "\tOut_Spec = specAmt * DirLightDiffuseColor;\n"
                          "\tOut_WaterDetail = (Out_Tex0 * 4.0) + vec2(WaterSpecs.x * -0.3, WaterSpecs.x * 0.21);\n"
                          "\tOut_WaterDetail2 = (Out_Tex0 * -8.0) + vec2(WaterSpecs.x * 0.12, WaterSpecs.x * -0.05);\n"
                          "\tOut_WaterAlphaBlend = distance(PlayerPos.xy, CameraPosition.xy) * WaterSpecs.y;\n"
                          //"\tOut_Position = Position;\n"
                          "\tgl_Position = Out_ClipSpace;\n"
                          "Out_DirLightDirection = DirLightDirection;"
                          "Out_Normal = (ObjMatrix * vec4(Normal,0.0)).xyz;\n"
                          "Out_CameraVector = CameraPosition - PlayerPos.xyz;\n"
                          "Out_CameraPos = CameraPosition;\n"
                          "Out_Vertex = Position;\n"
                          "\t\n"
                          "}\n");




    //in testing
    /*vtxbuf = OBFUSCATE("#version 100\n"
      "precision highp float;\n"
      "uniform mat4 ProjMatrix;\n"
      "uniform mat4 ViewMatrix;\n"
      "uniform mat4 ObjMatrix;\n"
      "uniform lowp vec3 AmbientLightColor;\n"
      "uniform lowp vec4 MaterialEmissive;\n"
      "uniform lowp vec4 MaterialAmbient;\n"
      "uniform lowp vec4 MaterialDiffuse;\n"
      "uniform lowp vec3 DirLightDiffuseColor;\n"
      "uniform vec3 DirLightDirection;\n"
      "uniform lowp vec3 DirLight2DiffuseColor;\n"
      "uniform vec3 DirLight2Direction;\n"
      "uniform lowp vec3 DirLight3DiffuseColor;\n"
      "uniform vec3 DirLight3Direction;\n"
      "uniform vec3 DirBackLightDirection;\n"
      "attribute vec3 Position;\n"
      "attribute vec3 Normal;\n"
      "attribute vec2 TexCoord0;\n"
      "attribute vec4 GlobalColor;\n"
      "attribute vec4 BoneWeight;\n"
      "attribute vec4 BoneIndices;\n"
      "uniform highp vec4 Bones[128*3];\n"
      "uniform mat3 NormalMatrix;\n"
      "uniform vec3 CameraPosition;\n"
      "uniform vec3 FogDistances;\n"
      "uniform vec3 WaterSpecs;\n"
      "varying mediump vec2 Out_WaterDetail;\n"
      "varying mediump vec2 Out_WaterDetail2;\n"
      "varying mediump float Out_WaterAlphaBlend;\n"
      "attribute vec4 Color2;\n"
      "varying vec3 Out_CameraVector;\n"
      "varying mediump vec2 Out_DuDvTexCoord;\n"
      "uniform lowp float ColorInterp;\n"
      "uniform lowp float EnvMapCoefficient;\n"
      "varying mediump vec2 Out_Tex1;\n"
      "varying mediump vec2 Out_Tex0;\n"
      "varying mediump vec3 Out_Refl;\n"
      "varying lowp vec4 Out_Color;\n"
      "varying lowp vec3 Out_Spec;\n"
      "varying mediump float Out_FogAmt;\n"
      "varying mediump vec4 Out_ClipSpace;\n"
      "void main() {\n"
      "\tivec4 BlendIndexArray = ivec4(BoneIndices);\n"
      "\tmat4 BoneToLocal;\n"
      "\tBoneToLocal[0] = Bones[BlendIndexArray.x*3] * BoneWeight.x;\n"
      "\tBoneToLocal[1] = Bones[BlendIndexArray.x*3+1] * BoneWeight.x;\n"
      "\tBoneToLocal[2] = Bones[BlendIndexArray.x*3+2] * BoneWeight.x;\n"
      "\tBoneToLocal[3] = vec4(0.0,0.0,0.0,1.0);\n"
      "\tBoneToLocal[0] += Bones[BlendIndexArray.y*3] * BoneWeight.y;\n"
      "\tBoneToLocal[1] += Bones[BlendIndexArray.y*3+1] * BoneWeight.y;\n"
      "\tBoneToLocal[2] += Bones[BlendIndexArray.y*3+2] * BoneWeight.y;\n"
      "\tBoneToLocal[0] += Bones[BlendIndexArray.z*3] * BoneWeight.z;\n"
      "\tBoneToLocal[1] += Bones[BlendIndexArray.z*3+1] * BoneWeight.z;\n"
      "\tBoneToLocal[2] += Bones[BlendIndexArray.z*3+2] * BoneWeight.z;\n"
      "\tBoneToLocal[0] += Bones[BlendIndexArray.w*3] * BoneWeight.w;\n"
      "\tBoneToLocal[1] += Bones[BlendIndexArray.w*3+1] * BoneWeight.w;\n"
      "\tBoneToLocal[2] += Bones[BlendIndexArray.w*3+2] * BoneWeight.w;\n"
      "\tvec4 WorldPos = ObjMatrix * (vec4(Position,1.0) * BoneToLocal);\n"
      "\tvec3 WorldNormal = mat3(ObjMatrix) * (Normal * mat3(BoneToLocal));\n"
      "\tOut_CameraVector = CameraPosition - WorldPos.xyz;\n"
      "\tOut_DuDvTexCoord = vec2(Position.x/2.0 + 0.5, Position.y/2.0 + 0.5) * 0.2;\n"
      "\tvec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);\n"
      "\treflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;\n"
      "\tvec3 ReflPos = normalize(reflVector);\n"
      "\tgl_Position = vec4(ReflPos.xy, length(reflVector) * 0.002, 1.0);\n"
      "\tvec4 ViewPos = ViewMatrix * WorldPos;\n"
      "\tOut_ClipSpace = ProjMatrix * ViewPos\n"
      //"\tvec3 WorldNormal = normalize(vec3(WorldPos.xy - CameraPosition.xy, 0.0001)) * 0.85;\n"
      "\tOut_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 0.90);\n"
      "\tOut_Tex0 = (NormalMatrix * vec3(TexCoord0, 1.0)).xy;\n"
      "\tOut_Refl = reflVector;\n"
      "\tOut_Tex1 = vec2(length(reflVector.xy), (reflVector.z * 0.5) + 0.25);\n"
      "\tlowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);\n"
      //"\tOut_Color = InterpColor;\n"
      "\tvec3 Out_LightingColor;\n"
      "\tOut_LightingColor += (max(dot(DirLightDirection, WorldNormal), 0.0) + max(dot(DirBackLightDirection, WorldNormal), 0.0)) * DirLightDiffuseColor;\n"
      "\tOut_Color = vec4((Out_LightingColor.xyz + InterpColor.xyz * 1.5) * MaterialDiffuse.xyz, (MaterialAmbient.w) * InterpColor.w);\n"
      "\tfloat specAmt = max(pow(dot(reflVector, DirLightDirection), 1.0), 0.0) * EnvMapCoefficient * 2.0;\n"
      "\tOut_Spec = specAmt * DirLightDiffuseColor;\n"
      "\tOut_WaterDetail = (Out_Tex0 * 4.0) + vec2(WaterSpecs.x * -0.3, WaterSpecs.x * 0.21);\n"
      "\tOut_WaterDetail2 = (Out_Tex0 * -8.0) + vec2(WaterSpecs.x * 0.12, WaterSpecs.x * -0.05);\n"
      "\tOut_WaterAlphaBlend = distance(WorldPos.xy, CameraPosition.xy) * WaterSpecs.y;\n"
      "}\n");*/

    //for debug
    //Log(pxlbuf);
    //Log(vtxbuf);

    *pxlsrc = pxl;
    *vtxsrc = vtx;

    *(char* *)(g_libGTASA+0x617290) = *pxlsrc;
    *(char* *)(g_libGTASA+0x619294) = *vtxsrc;

    return 1;
}



void SetUpGLHooks()
{
    byte_617258 = (char*)(g_libGTASA + 0x617258);
    byte_619294 = (char*)(g_libGTASA + 0x619294);
    byte_619294[0] = '\0';
    CHook::InlineHook(g_libGTASA, 0x1A5EB0, &RQShader__BuildSource_hook, &RQShader__BuildSource);
    byte_617290 = (char*)(g_libGTASA + 0x617290);
    byte_617258 = (char*)(g_libGTASA + 0x617258);

    RQMaxBones = (int*)(g_libGTASA + 0x617248);

    RQCaps = (RQCapabilities *)g_libGTASA + 0x617250;

    CHook::InlineHook(g_libGTASA, 0x1A40B4, &BuildPixelSource_hook, &BuildPixelSource);
    CHook::InlineHook(g_libGTASA, 0x1A4AB4, &BuildVertexSource_hook, &BuildVertexSource);
}