//
// Created by Error on 21.05.2025.
//

#include "CRegistration.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"

jobject CRegistration::thiz = nullptr;
jclass CRegistration::clazz = nullptr;

int CRegistration::RegisterSkinValue = 0;
int CRegistration::RegisterSexMale = 0;
int CRegistration::RegisterSkinId = 0;
bool CRegistration::bIsShow = false;

// ======== РЕГИСТРАЦИЯ (камера левее, смотрит прямо в лицо персонажу) ========

struct Vec3 { float x, y, z; };

// Позиция персонажа (без изменений)
static constexpr Vec3 kCommonPos = { -90.0f, 794.7f, 12.15f };

static constexpr int  kCommonInterior = 0;
static constexpr int  kCommonVW       = 0;
static constexpr float kCommonHeading = 0.0f; // персонаж смотрит на камеру

// КАМЕРА: левее на 1.5м, дистанция та же (Y/Z не трогаем)
static constexpr Vec3 kCommonCamPos  = { -89.5f, 799.0f, 13.0f };

// Смотрим точно в лицо персонажу
static constexpr Vec3 kCommonCamLook = { -88.5f, 794.7f, 12.15f };




extern CGame* pGame;
extern CNetGame* pNetGame;

static inline void ApplyCommonPreview() {
    if (!pGame) return;
    CPlayerPed* pPlayer = pGame->FindPlayerPed();
    CCamera*    pCamera = pGame->GetCamera();
    if (!pPlayer || !pCamera) return;

    if (pPlayer->IsInVehicle())
        pPlayer->RemoveFromVehicleAndPutAt(kCommonPos.x, kCommonPos.y, kCommonPos.z);
    else
        pPlayer->TeleportTo(kCommonPos.x, kCommonPos.y, kCommonPos.z);

    pPlayer->ForceTargetRotation(kCommonHeading);
    pPlayer->SetInterior(kCommonInterior, true);

    pCamera->SetPosition(kCommonCamPos.x, kCommonCamPos.y, kCommonCamPos.z, 0.0f, 0.0f, 0.0f);
    pCamera->LookAtPoint(kCommonCamLook.x, kCommonCamLook.y, kCommonCamLook.z, 2);
}

// ======================================

const uint32_t cRegisterSkin[2][10] = {
        {9, 195, 231, 232, 1, 1, 1, 1, 1, 1},  // female
        {16, 79, 134, 135, 200, 234, 235, 236, 239}  // male
};

void CRegistration::hide() {
    if (CRegistration::thiz != nullptr) {
        JNIEnv* env = g_pJavaWrapper->GetEnv();
        jmethodID method = env->GetMethodID(CRegistration::clazz, "destroy", "()V");
        env->CallVoidMethod(CRegistration::thiz, method);
        env->DeleteGlobalRef(CRegistration::thiz);
        CRegistration::thiz = nullptr;
        CRegistration::bIsShow = false;
    }
}

void CRegistration::show() {
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if (CRegistration::thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(CRegistration::clazz, "<init>", "()V");
        CRegistration::thiz = env->NewObject(CRegistration::clazz, constructor);
        CRegistration::thiz = env->NewGlobalRef(CRegistration::thiz);
        CRegistration::bIsShow = true;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Registration_onRegisterClick(JNIEnv* pEnv, jobject thiz,
                                                               jstring pass, jstring mail,
                                                               jint sex, jint age) {
    const char* inputPassword = pEnv->GetStringUTFChars(pass, nullptr);
    const char* inputMail = pEnv->GetStringUTFChars(mail, nullptr);

    if (pNetGame) {
        pNetGame->SendRegisterPacket(
                (char*)inputPassword,
                (char*)inputMail,
                sex,
                age,
                CRegistration::ChangeRegisterSkin(CRegistration::RegisterSkinValue)
        );
    }

    pGame->ToggleHUDElement(0, false);

    pEnv->ReleaseStringUTFChars(pass, inputPassword);
    pEnv->ReleaseStringUTFChars(mail, inputMail);
}

uint32_t CRegistration::ChangeRegisterSkin(int skin) {
    uint32_t uiSkin = 16;
    bool bIsMan = (CRegistration::RegisterSexMale == 1);
    uint32_t uiMaxSkins = bIsMan ? 9 : 4;

    if (!(0 < skin && skin <= uiMaxSkins)) {
        CRegistration::RegisterSkinId = uiSkin;
        return uiSkin;
    }

    uiSkin = cRegisterSkin[(int)bIsMan][skin - 1];
    CRegistration::RegisterSkinId = uiSkin;
    return uiSkin;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Registration_onRegisterSkinBackClick(JNIEnv* env, jobject thiz) {
    CRegistration::RegisterSkinValue--;
    if (CRegistration::RegisterSexMale == 1) {
        if (CRegistration::RegisterSkinValue < 1)
            CRegistration::RegisterSkinValue = 9;
    } else if (CRegistration::RegisterSexMale == 2) {
        if (CRegistration::RegisterSkinValue < 1)
            CRegistration::RegisterSkinValue = 4;
    }
    if (pNetGame)
        pNetGame->SendRegisterSkinPacket(
                CRegistration::ChangeRegisterSkin(CRegistration::RegisterSkinValue)
        );

    ApplyCommonPreview();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Registration_onRegisterSkinNextClick(JNIEnv* env, jobject thiz) {
    CRegistration::RegisterSkinValue++;
    if (CRegistration::RegisterSexMale == 1) {
        if (CRegistration::RegisterSkinValue > 9)
            CRegistration::RegisterSkinValue = 1;
    } else if (CRegistration::RegisterSexMale == 2) {
        if (CRegistration::RegisterSkinValue > 4)
            CRegistration::RegisterSkinValue = 1;
    }
    if (pNetGame)
        pNetGame->SendRegisterSkinPacket(
                CRegistration::ChangeRegisterSkin(CRegistration::RegisterSkinValue)
        );

    ApplyCommonPreview();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Registration_onRegisterMaleSend(JNIEnv* env, jobject thiz,
                                                                  jint sex) {
    CRegistration::RegisterSexMale = sex;

    CPlayerPed* pPlayer = pGame->FindPlayerPed();
    CCamera* pCamera = pGame->GetCamera();

    if (pNetGame)
        pNetGame->SendRegisterSkinPacket(
                CRegistration::ChangeRegisterSkin(CRegistration::RegisterSkinValue)
        );

    if (pPlayer->IsInVehicle())
        pPlayer->RemoveFromVehicleAndPutAt(kCommonPos.x, kCommonPos.y, kCommonPos.z);
    else
        pPlayer->TeleportTo(kCommonPos.x, kCommonPos.y, kCommonPos.z);

    pPlayer->ForceTargetRotation(kCommonHeading);

    if (pPlayer && pCamera) {
        pCamera->SetPosition(kCommonCamPos.x, kCommonCamPos.y, kCommonCamPos.z, 0.0f, 0.0f, 0.0f);
        pCamera->LookAtPoint(kCommonCamLook.x, kCommonCamLook.y, kCommonCamLook.z, 2);
        pPlayer->SetInterior(kCommonInterior, true);
    }

    ApplyCommonPreview();
}
