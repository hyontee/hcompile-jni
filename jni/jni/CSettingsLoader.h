//
// Created by Regerds on 14.12.2024.
//
#include "ini/config.h"

#ifndef BLACK_RUSSIA_CSETTINGSLOADER_H
#define BLACK_RUSSIA_CSETTINGSLOADER_H


class CSettingsLoader {
public:
    CSettingsLoader();
    ~CSettingsLoader();

    //char sky, aa, lowerdd, lowercars;
    const char* water;
    const char* sky;
    const char* aa;
    const char* lowerdd;
    const char* lowercars;
};


#endif //BLACK_RUSSIA_CSETTINGSLOADER_H
