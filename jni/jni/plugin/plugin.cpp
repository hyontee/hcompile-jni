#include <iostream>
#include <fstream>
#include <string>

typedef int cell;
#define AMX_NATIVE_CALL
#define PLUGIN_EXPORT extern "C"

// Функция для безопасного чтения параметров из INI
std::string GetIniParam(const std::string& filename, const std::string& key) {
    std::ifstream file("scriptfiles/" + filename);
    if (!file.is_open()) file.open(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.find(key + "=") == 0 || line.find(key + " =") == 0) {
            size_t pos = line.find("=");
            std::string value = line.substr(pos + 1);
            value.erase(0, value.find_first_not_of(" \"\r\n"));
            value.erase(value.find_last_not_of(" \"\r\n") + 1);
            return value;
        }
    }
    return "";
}

// Нативная функция, которая выведет принт и проверит ключ лицензии
PLUGIN_EXPORT int AMX_NATIVE_CALL ReadSecureConfig(void* amx, cell* params)
{
    // Твой вечный защищенный принт в консоль Linux Maze Host
    std::cout << "\n" << std::endl;
    std::cout << "/* =================================================== */" << std::endl;
    std::cout << "/*  mod was created by westovsky69                     */" << std::endl;
    std::cout << "/*  this mod been private                              */" << std::endl;
    std::cout << "/*  Telegram channel of author is @biowest69           */" << std::endl;
    std::cout << "/* =================================================== */" << std::endl;
    std::cout << "\n" << std::endl;

    // 1. Проверяем настройки MySQL (чтобы убедиться, что мод запущен правильно)
    std::string host = GetIniParam("dimawest69_mysql_settings.ini", "host");
    if (host.empty()) return 0;

    // 2. Читаем ключ прямо из файла key.ini в папке scriptfiles
    std::string license = GetIniParam("key.ini", "key");

    // ТВОЙ СЕКРЕТНЫЙ КЛЮЧ
    if (license != "w69-secure-license-key-2026") {
        std::cout << "[SECURITY ERROR]: Invalid or missing license key in key.ini!" << std::endl;
        std::cout << "[SECURITY]: Initiating core memory dump..." << std::endl;
        
        // ЖЕСТКИЙ КРАШ ПЛАГИНА (Обращение к нулевому указателю)
        // Сервер моментально упадет без всяких мягких команд "exit"
        volatile int* crash_trigger = nullptr;
        *crash_trigger = 0xDEADBEEF; 
        
        return 0; 
    }

    return 1; 
}

PLUGIN_EXPORT unsigned int Supports() { return 0x0200 | 0x10000; }
PLUGIN_EXPORT bool Load(void **ppData) { return true; }
PLUGIN_EXPORT void Unload() {}
