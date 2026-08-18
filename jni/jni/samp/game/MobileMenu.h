//
// Created by nikk on 23.10.2025.
//

#ifndef CRIMINAL_MOSCOW_MOBILEMENU_H
#define CRIMINAL_MOSCOW_MOBILEMENU_H


#include <jni.h>

static class MobileMenu {
public:
    static jobject thiz;

    static void InjectMobileMenuHooks();
};


#endif //CRIMINAL_MOSCOW_MOBILEMENU_H
