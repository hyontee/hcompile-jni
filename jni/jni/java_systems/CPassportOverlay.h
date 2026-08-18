//
// Created by August on 18.09.2025.
//

#ifndef CRIMINAL_MOSCOW_CPASSPORTOVERLAY_H
#define CRIMINAL_MOSCOW_CPASSPORTOVERLAY_H

#endif //CRIMINAL_MOSCOW_CPASSPORTOVERLAY_H
// app/src/main/cpp/samp/java_systems/CPassportOverlay.h
#pragma once
#include <jni.h>

class CPassportOverlay {
public:
    static void show();
    static void hide();

    // обновление текстов
    static void updatePassport(const char* surname,
                               const char* name,
                               int   lawAbidance,
                               const char* militaryTicket,
                               const char* fraction,
                               int   yearsInRegion,
                               const char* sex,
                               const char* issueDate,
                               const char* serialNumber);

    // отрисовать «фото» (скин) игрока слева
    static void renderSkin(int skinId);

    // хранение Java-объекта
    static jobject j_passport;
};
