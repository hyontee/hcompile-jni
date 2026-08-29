#include <iostream>

typedef int cell;
#define AMX_NATIVE_CALL
#define PLUGIN_EXPORT extern "C"

// Плагин просто выводит твой защищенный копирайт при старте
PLUGIN_EXPORT int AMX_NATIVE_CALL ReadSecureConfig(void* amx, cell* params)
{
    std::cout << "\n" << std::endl;
    std::cout << "/* =================================================== */" << std::endl;
    std::cout << "/*  mod was created by westovsky69                     */" << std::endl;
    std::cout << "/*  this mod been private                              */" << std::endl;
    std::cout << "/*  Telegram channel of author is @biowest69           */" << std::endl;
    std::cout << "/* =================================================== */" << std::endl;
    std::cout << "\n" << std::endl;
    return 1; 
}

PLUGIN_EXPORT unsigned int Supports() { return 0x0200 | 0x10000; }
PLUGIN_EXPORT bool Load(void **ppData) { return true; }
PLUGIN_EXPORT void Unload() {}
