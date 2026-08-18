//
// Created by August on 28.09.2025.
//

#ifndef CRIMINAL_MOSCOW_CMVDIDOVERLAY_H
#define CRIMINAL_MOSCOW_CMVDIDOVERLAY_H

#endif //CRIMINAL_MOSCOW_CMVDIDOVERLAY_H
// app/src/main/cpp/samp/java_systems/CMvdIdOverlay.h
#pragma once
#include <jni.h>

class CMvdIdOverlay {
public:
    static void show();
    static void hide();
    static void updateId(const char* nicknameOneLine, const char* rankText);
    static void renderSkin(int skinId);

    // глобальная ссылка на Java-объект (MvdIdOverlay)
    static jobject j_obj;
};
