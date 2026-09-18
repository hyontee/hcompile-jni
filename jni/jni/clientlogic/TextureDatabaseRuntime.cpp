#include "TextureDatabaseRuntime.h"

TextureDatabase* TextureDatabaseRuntime::Load(const char *withName, bool fullyLoad, int forcedFormat)
{
    return ((TextureDatabase* (*)(const char*, int, int))(g_libGTASA + 0x1BF244 + 1))(withName, fullyLoad, forcedFormat);
}

void TextureDatabaseRuntime::Register(TextureDatabase *toRegister)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x001BE898 + 1))(toRegister);
}