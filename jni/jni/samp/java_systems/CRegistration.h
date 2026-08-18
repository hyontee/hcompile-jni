//
// Created by Error on 21.05.2025.
//

#ifndef CRIMINAL_MOSCOW_CREGISTRATION_H
#define CRIMINAL_MOSCOW_CREGISTRATION_H

#include <jni.h>

class CRegistration {
    static jobject thiz;

public:
    static bool bIsShow;
    static int RegisterSexMale;
    static int RegisterSkinValue;
    static int RegisterSkinId;
    static jclass clazz;


    static void hide();

    static void show();

    static uint32_t ChangeRegisterSkin(int skin);
};


#endif //CRIMINAL_MOSCOW_CREGISTRATION_H
