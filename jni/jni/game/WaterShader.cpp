#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../chatwindow.h"
#include "WaterShader.h"
#include <cmath>

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;



uintptr_t* BuildShaderSource(uintptr_t* a1, const char *a2, const char *a3, int a4){
    char *v7; // r6    
	char *v8; // r0
    *(uintptr_t *)(a1 + 17) = 0;
    *(uintptr_t *)(a1 + 5) = a4; 
	*(uintptr_t *)(a1 + 9) = ((int (__fastcall *)(const char *))(g_libGTASA + 0x1BC269))(a2);
    *(uintptr_t *)(a1 + 13) = ((int (__fastcall *)(const char *))(g_libGTASA + 0x1BC269))(a3);  
	v7 = strdup(a2);
    v8 = strdup(a3);  
    *(char *)(a1 + 4) = 1;
    *(uintptr_t* *)a1 = ((uintptr_t *(__fastcall *)(char *, char *, uintptr_t))(g_libGTASA + 0x1A3A69))(v7, v8, *(uintptr_t *)(a1 + 5));  
	return a1;
}
int  WaterShader::EmuShader__Select3(uintptr_t* a1)
{
    return ((int ( *)(uintptr_t*, int))(g_libGTASA + 0x1984A5))(a1, 1);
}

int WaterShader::BuildShadersSource1(uintptr_t* a1)
{
    uintptr_t *v2; // r5
    int result; // r0
    v2 = a1;
    v2 = BuildShaderSource(
            (uintptr_t*)v2,
					("#version 100\n"
					  "precision highp float;\n"
					  "uniform sampler2D Diffuse;\n"
					  "uniform sampler2D EnvMap;\n"
					  "uniform sampler2D RefractionMap;\n"
					  "uniform sampler2D DuDvMap;\n"
					  "uniform sampler2D FogMap;\n"
					  "uniform highp float Waveness;\n"
					  "uniform highp float UnderWaterness;\n"
					  "varying vec2 Out_Tex0;\n"
					  "varying vec4 Out_ClipSpace;\n"
					  "varying vec3 Out_CameraVector;\n"
					  "varying vec2 Out_DuDvTexCoord;\n"
					  "varying vec3 Out_Refl;\n"
					  "varying mediump float Out_FogAmt;\n"
					  "vec3 sampleEquirectangularMap(vec3 dir) {\n"
					  "    vec3 normalizedDir = normalize(dir);\n"
					  "\n"
					  "    float phi = atan(normalizedDir.z, normalizedDir.x);\n"
					  "    float theta = asin(normalizedDir.y);\n"
					  "\n"
					  "    float u = phi / (2.0 * 3.14159265359) + 0.5;\n"
					  "    float v = theta / 3.14159265359 + 0.5;\n"
					  "\n"
					  "    vec3 color = texture2D(EnvMap, vec2(u, v)).rgb;\n"
					  "    return color;\n"
					  "}\n"
					  "void main()\n"
					  "{\n"
					  "\tvec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;\n"
					  "\n"
					  "\tvec2 distortedTexCoords = texture2D(DuDvMap, vec2(Out_DuDvTexCoord.x + Waveness, Out_DuDvTexCoord.y)).rg * 0.1;\n"
					  "\tdistortedTexCoords = Out_DuDvTexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + Waveness);\n"
					  "\tvec2 distSum = (texture2D(DuDvMap, distortedTexCoords).rg * 2.0 - 1.0) * 0.01;\n"
					  "\n"
					  "\tvec2 refractTexCoords = vec2(ndc.x, ndc.y);\n"
					  "\trefractTexCoords += distSum;\n"
					  "\trefractTexCoords = clamp(refractTexCoords, 0.001, 0.999);\n"
					  "\n"
					  "\tvec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5); \n"
					  "\tReflPos = (ReflPos * vec2(0.5, 0.5)) + vec2(0.5, 0.5); \n"
					  "\tReflPos += distSum;\n"
					  "\n"
					  "\tReflPos = clamp(ReflPos, 0.001, 0.999); \n"
					  "\n"
					  "\tvec4 reflectTex = texture2D(EnvMap, ReflPos); \n"
					  "\n"
					  "\tvec4 refract = texture2D(RefractionMap, refractTexCoords);\n"
					  "\tvec3 ViewVector = normalize(Out_CameraVector);\n"
					  "\tfloat RefractionCoef = dot(ViewVector, vec3(0.0, 0.0, 1.0));\n"
					  "\tRefractionCoef = pow(RefractionCoef, 1.3);\n"
					  "\t\n"
					  "\tgl_FragColor = mix(reflectTex, refract, RefractionCoef);\n"
					  "\tgl_FragColor = mix(gl_FragColor, vec4(60.0/255.0, 144.0/255.0, 164.0/255.0, 0.5), 0.2);\n"
					  "\tgl_FragColor.a = 0.5;\n"
					  "\tlowp vec4 fogColorNew = texture2D(FogMap, ndc);\n"
					  "\tgl_FragColor.xyz = mix(gl_FragColor.xyz, fogColorNew.xyz, Out_FogAmt);\n"
					  "}\n"),
           ("#version 100\n"
					   "precision highp float;\n"
					   "uniform mat4 ProjMatrix;\n"
					   "uniform mat4 ViewMatrix;\n"
					   "uniform mat4 ObjMatrix;\n"
					   "uniform vec3 CameraPosition;\n"
					   "varying mediump float Out_FogAmt;\n"
					   "uniform vec3 FogDistances;\n"
					   "attribute vec3 Position;\n"
					   "attribute vec2 TexCoord0;\n"
					   "varying vec2 Out_Tex0;\n"
					   "varying vec4 Out_ClipSpace;\n"
					   "varying vec3 Out_CameraVector;\n"
					   "varying vec2 Out_DuDvTexCoord;\n"
					   "varying vec3 Out_Refl;\n"
					   "varying vec3 Out_FromLightVector;\n"
					   "void main()\n"
					   "{\n"
					   "\tOut_Tex0 = TexCoord0;\n"
					   "\tvec4 WorldPos = ObjMatrix * vec4(Position, 1.0);\n"
					   "\tvec4 ViewPos = ViewMatrix * WorldPos;\n"
					   "\tOut_CameraVector = CameraPosition - WorldPos.xyz;\n"
					   "\tOut_DuDvTexCoord = vec2(Position.x/2.0 + 0.5, Position.y/2.0 + 0.5) * 0.2;\n"
					   "\t\n"
					   "\tvec3 WorldNormal = vec3(0.0, 0.0, 1.0);\n"
					   "\tvec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);\n"
					   "\treflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;\n"
					   "\tOut_Refl = reflVector;\n"
					   "\tOut_ClipSpace = vec4(normalize(reflVector.xy) * (reflVector.z * 0.5 + 0.5), length(reflVector) * 0.002, 1.0);\n"
					   "\tOut_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 1.0);\n"
					   "\t\n"
					   "\tgl_Position = ProjMatrix * ViewPos;\n"
					   "}\n"),
            0x1002422);
    result = 1;
    a1 = v2;
    return result;
}

int WaterShader::BuildShadersSource2(uintptr_t* a1)
{
	uintptr_t* v2; // r5
	int result; // r0
	v2 = a1;
	v2 = BuildShaderSource(
		(uintptr_t*)v2,
		("#version 100\n"
			"precision highp float;\n"
			"uniform sampler2D Diffuse;\n"
			"uniform sampler2D EnvMap;\n"
			"uniform sampler2D RefractionMap;\n"
			"uniform sampler2D DuDvMap;\n"
			"uniform sampler2D FogMap;\n"
			"uniform highp float Waveness;\n"
			"uniform highp float UnderWaterness;\n"
			"varying vec2 Out_Tex0;\n"
			"varying vec4 Out_ClipSpace;\n"
			"varying vec3 Out_CameraVector;\n"
			"varying vec2 Out_DuDvTexCoord;\n"
			"varying vec3 Out_Refl;\n"
			"varying mediump float Out_FogAmt;\n"
			"vec3 sampleEquirectangularMap(vec3 dir) {\n"
			"    vec3 normalizedDir = normalize(dir);\n"
			"\n"
			"    float phi = atan(normalizedDir.z, normalizedDir.x);\n"
			"    float theta = asin(normalizedDir.y);\n"
			"\n"
			"    float u = phi / (2.0 * 3.14159265359) + 0.5;\n"
			"    float v = theta / 3.14159265359 + 0.5;\n"
			"\n"
			"    vec3 color = texture2D(EnvMap, vec2(u, v)).rgb;\n"
			"    return color;\n"
			"}\n"
			"void main()\n"
			"{\n"
			"\tvec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;\n"
			"\n"
			"\tvec2 distortedTexCoords = texture2D(DuDvMap, vec2(Out_DuDvTexCoord.x + Waveness, Out_DuDvTexCoord.y)).rg * 0.1;\n"
			"\tdistortedTexCoords = Out_DuDvTexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + Waveness);\n"
			"\tvec2 distSum = (texture2D(DuDvMap, distortedTexCoords).rg * 2.0 - 1.0) * 0.01;\n"
			"\n"
			"\tvec2 refractTexCoords = vec2(ndc.x, ndc.y);\n"
			"\trefractTexCoords += distSum;\n"
			"\trefractTexCoords = clamp(refractTexCoords, 0.001, 0.999);\n"
			"\n"
			"\tvec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5); \n"
			"\tReflPos = (ReflPos * vec2(0.5, 0.5)) + vec2(0.5, 0.5); \n"
			"\tReflPos += distSum;\n"
			"\n"
			"\tReflPos = clamp(ReflPos, 0.001, 0.999); \n"
			"\n"
			"\tvec4 reflectTex = texture2D(EnvMap, ReflPos); \n"
			"\n"
			"\tvec4 refract = texture2D(RefractionMap, refractTexCoords);\n"
			"\tvec3 ViewVector = normalize(Out_CameraVector);\n"
			"\tfloat RefractionCoef = dot(ViewVector, vec3(0.0, 0.0, 1.0));\n"
			"\tRefractionCoef = pow(RefractionCoef, 1.3);\n"
			"\t\n"
			"\tgl_FragColor = mix(reflectTex, refract, RefractionCoef);\n"
			"\tgl_FragColor = mix(gl_FragColor, vec4(60.0/255.0, 144.0/255.0, 164.0/255.0, 0.5), 0.2);\n"
			"\tgl_FragColor.a = 0.5;\n"
			"\tlowp vec4 fogColorNew = texture2D(FogMap, ndc);\n"
			"\tgl_FragColor.xyz = mix(gl_FragColor.xyz, fogColorNew.xyz, Out_FogAmt);\n"
			"}\n"),
		("#version 100\n"
			"precision highp float;\n"
			"uniform mat4 ProjMatrix;\n"
			"uniform mat4 ViewMatrix;\n"
			"uniform mat4 ObjMatrix;\n"
			"uniform vec3 CameraPosition;\n"
			"varying mediump float Out_FogAmt;\n"
			"uniform vec3 FogDistances;\n"
			"attribute vec3 Position;\n"
			"attribute vec2 TexCoord0;\n"
			"varying vec2 Out_Tex0;\n"
			"varying vec4 Out_ClipSpace;\n"
			"varying vec3 Out_CameraVector;\n"
			"varying vec2 Out_DuDvTexCoord;\n"
			"varying vec3 Out_Refl;\n"
			"varying vec3 Out_FromLightVector;\n"
			"void main()\n"
			"{\n"
			"\tOut_Tex0 = TexCoord0;\n"
			"\tvec4 WorldPos = ObjMatrix * vec4(Position, 1.0);\n"
			"\tvec4 ViewPos = ViewMatrix * WorldPos;\n"
			"\tOut_CameraVector = CameraPosition - WorldPos.xyz;\n"
			"\tOut_DuDvTexCoord = vec2(Position.x/2.0 + 0.5, Position.y/2.0 + 0.5) * 0.2;\n"
			"\t\n"
			"\tvec3 WorldNormal = vec3(0.0, 0.0, 1.0);\n"
			"\tvec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);\n"
			"\treflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;\n"
			"\tOut_Refl = reflVector;\n"
			"\tOut_ClipSpace = vec4(normalize(reflVector.xy) * (reflVector.z * 0.5 + 0.5), length(reflVector) * 0.002, 1.0);\n"
			"\tOut_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 1.0);\n"
			"\t\n"
			"\tgl_Position = ProjMatrix * ViewPos;\n"
			"}\n"),
		0x1002422);
	result = 1;
	a1 = v2;
	return result;
}
