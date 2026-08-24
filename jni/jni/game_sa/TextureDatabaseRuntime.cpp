//
// Created by x1y2z on 29.12.2023.
//

#include "TextureDatabaseRuntime.h"
#include "../util/armhook.h"
#include "main.h"

//unsigned int *TextureDatabaseRuntime::storedTexels = (g_libGTASA + 0x61B8C0);

TextureDatabase* TextureDatabaseRuntime::Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat)
{
    return ((TextureDatabase* (*)(const char*, int, int))(g_libGTASA + 0x1BF244 + 1))(withName, fullyLoad, forcedFormat);
   // CHook::CallFunction<void>(g_libGTASA + 0x1BF244 + 1, a1, a2, a3);
}

void TextureDatabaseRuntime::Register(TextureDatabase *thiz)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x1BE898 + 1))(thiz);
}

void TextureDatabaseRuntime::UnRegister(TextureDatabase *thiz)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x1BE938 + 1))(thiz);
}

RwTexture* TextureDatabaseRuntime::GetTexture(const char *name) {
    return ((RwTexture*(*)(const char*))(g_libGTASA + 0x001BE990 + 1))(name);
}