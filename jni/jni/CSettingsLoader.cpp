#include <stdio.h>
#include "CSettingsLoader.h"
#include "ini/config.h"

CSettingsLoader::CSettingsLoader() {
    char filePath[0x7F] = "/storage/emulated/0/Android/data/blackrussia.online/files/SAMP/settings.ini";

    ini_table_s* config = ini_table_create();
    if (!ini_table_read_from_file(config, filePath))
    {
        return;
    }

     water = ini_table_get_entry(config, "graphics", "water");
     sky = ini_table_get_entry(config, "graphics", "sky");
     aa = ini_table_get_entry(config, "graphics", "aa");
    lowerdd = ini_table_get_entry(config, "graphics", "lowerdd");
    lowercars = ini_table_get_entry(config, "graphics", "lowercars");
    lastserver = ini_table_get_entry(config, "gui", "lastserver");
}
CSettingsLoader::~CSettingsLoader() {
}