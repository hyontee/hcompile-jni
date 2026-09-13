//
// Created by Regerds on 14.12.2024.
//

#include "CSettingsLoader.h"
#include "ini/config.h"

CSettingsLoader::CSettingsLoader() {
    char filePath[0x7F] = "/storage/emulated/0/Android/data/blackrussia.online/files/SAMP/settings.ini";

    ini_table_s* config = ini_table_create();
    if (!ini_table_read_from_file(config, filePath))
    {
        return;
    }

    water = ini_table_get_entry_as_float(config, "graphics", "water", 0);
    sky = ini_table_get_entry_as_float(config, "graphics", "sky", 0);
    aa = ini_table_get_entry_as_float(config, "graphics", "aa", 0);
    lowerdd = ini_table_get_entry_as_float(config, "graphics", "lowerdd", 0);
    lowercars = ini_table_get_entry_as_float(config, "graphics", "lowercars", 0);
}

CSettingsLoader::~CSettingsLoader() {

}