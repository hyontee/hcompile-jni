#include "TextureLoader.h"

TextureDatabase* TextureLoader::Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat)
{
    Log("Loading: %s...", withName);
    return ((TextureDatabase* (*)(const char*, int, int))(g_libGTASA + 0x001BF244 + 1))(withName, fullyLoad, forcedFormat);
}

TextureDatabase* TextureLoader::GetDatabase(const char *a1)
{
    return ((TextureDatabase* (*)(const char*))(g_libGTASA + 0x001BF530 + 1))(a1);
}
	
void TextureLoader::Register(TextureDatabase *toRegister)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x001BE898 + 1))(toRegister);
}
	
void TextureLoader::Unregister(TextureDatabase *toUnregister)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x001BE938 + 1))(toUnregister);
}
	
RwTexture* TextureLoader::GetTexture(const char *name)
{
    return ((RwTexture* (*)(const char*))(g_libGTASA + 0x1BE990 + 1))(name);
}
