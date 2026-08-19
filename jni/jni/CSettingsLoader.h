#include "ini/config.h"

#ifndef BLACK_RUSSIA_CSETTINGSLOADER_H
#define BLACK_RUSSIA_CSETTINGSLOADER_H

class CSettingsLoader {
public:
    CSettingsLoader();
    ~CSettingsLoader();

    const char* water;
    const char* sky;
    const char* aa;
    const char* lowerdd;
    const char* lowercars;
    const char* lastserver;
};
#endif 
