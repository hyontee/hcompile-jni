//
// Created by plaka on 11.01.2023.
//

#include "TextureDatabaseRuntime.h"


//unsigned int *TextureDatabaseRuntime::storedTexels = (g_libGTASA + 0x61B8C0);

TextureDatabase* TextureDatabaseRuntime::Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat)
{
    return CHook::CallFunction<TextureDatabase *>(
            "_ZN22TextureDatabaseRuntime4LoadEPKcb21TextureDatabaseFormat", withName, fullyLoad,
            forcedFormat);
}

TextureDatabase* TextureDatabaseRuntime::GetDatabase(const char *a1)
{
    return CHook::CallFunction<TextureDatabase *>("_ZN22TextureDatabaseRuntime11GetDatabaseEPKc",
                                                  a1);
}

void TextureDatabaseRuntime::Register(TextureDatabase *toRegister)
{
    CHook::CallFunction<void>("_ZN22TextureDatabaseRuntime8RegisterEPS_", toRegister);
}

void TextureDatabaseRuntime::Unregister(TextureDatabase *toUnregister)
{
    CHook::CallFunction<void>("_ZN22TextureDatabaseRuntime10UnregisterEPS_", toUnregister);
}

RwTexture* TextureDatabaseRuntime::GetTexture(const char *name)
{
    return CHook::CallFunction<RwTexture *>("_ZN22TextureDatabaseRuntime10GetTextureEPKc", name);
}
