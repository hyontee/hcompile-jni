//
// Created by Error on 03.06.2025.
//

#ifndef CRIMINAL_MOSCOW_CGUIWRAPPER_H
#define CRIMINAL_MOSCOW_CGUIWRAPPER_H
#include <jni.h>
template<class T>
class CGuiWrapper {
public:
    using base_class = T;
    static jclass clazz;
    static jobject thiz;
    static bool bIsShow;

    static void hide();

    static void show();
};


#endif //CRIMINAL_MOSCOW_CGUIWRAPPER_H
