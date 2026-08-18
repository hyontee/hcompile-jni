//
// Created by Error on 25.05.2025.
//

#ifndef CRIMINAL_MOSCOW_CGAMEACTIONMENU_H
#define CRIMINAL_MOSCOW_CGAMEACTIONMENU_H
#include <jni.h>


class CGameActionMenu {
    static jobject thiz;
public:
    static jclass clazz;
    static bool bIsShow;

    static void hide();

    static void show();
};


#endif //CRIMINAL_MOSCOW_CGAMEACTIONMENU_H
