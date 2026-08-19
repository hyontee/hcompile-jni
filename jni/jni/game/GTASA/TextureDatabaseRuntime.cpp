//
// Created by Weikton
//
// Загрузка любых текстур.
// Включая .TXD
#include "TextureDatabaseRuntime.h"

// Reversed from IDA PRO 7
// TextureDatabaseRuntime::Load(char const*,bool,TextureDatabaseFormat)	0x1BF244
// TextureDatabaseRuntime::GetDatabase(char const*)	                                    0x1BF530
// TextureDatabaseRuntime::Register(TextureDatabaseRuntime*)	                  0x1BE898
// TextureDatabaseRuntime::Unregister(TextureDatabaseRuntime*)	                  0x1BE938
// TextureDatabaseRuntime::GetTexture(char const*)	                                                      0x1BE990
		
TextureDatabase* TextureDatabaseRuntime::Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat)
{
    Log("Покдлючаем текстуру..");
    return ((TextureDatabase* (*)(const char*, int, int))(g_libGTASA + 0x001BF244 + 1))(withName, fullyLoad, forcedFormat);
}

TextureDatabase* TextureDatabaseRuntime::GetDatabase(const char *a1)
{
    return ((TextureDatabase* (*)(const char*))(g_libGTASA + 0x001BF530 + 1))(a1);
}
	
void TextureDatabaseRuntime::Register(TextureDatabase *toRegister)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x001BE898 + 1))(toRegister);
}
	
void TextureDatabaseRuntime::Unregister(TextureDatabase *toUnregister)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x001BE938 + 1))(toUnregister);
}
	
RwTexture* TextureDatabaseRuntime::GetTexture(const char *name)
{
    return ((RwTexture* (*)(const char*))(g_libGTASA + 0x1BE990 + 1))(name);
}
