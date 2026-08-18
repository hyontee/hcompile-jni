//
// Created by plaka on 24.01.2023.
//

#include <jni.h>
#include "CLoader.h"
#include "game/bass.h"
#include "util/patch.h"
#include "crashlytics.h"
#include "CSettings.h"
#include "java_systems/CSpeedometr.h"
#include "java_systems/CDonate.h"
#include "java_systems/CEditobject.h"
#include "java_systems/CChooseSpawn.h"
#include "java_systems/CAuthrization.h"
#include "java_systems/CRegistration.h"
#include "java_systems/CHUD.h"
#include "java_systems/CGameActionMenu.h"
#include "java_systems/CDialog.h"
#include "CJavaGui.h"
#include "game/CSnapShotWrapper.h"

int CLoader::tick = 0;

void InitBASSFuncs();
void CLoader::loadBassLib()
{
    /*InitBASSFuncs();
    BASS_Init(-1, 44100, BASS_DEVICE_3D, nullptr, nullptr);
    BASS_Set3DFactors(1, 0.10, 1);
    BASS_Apply3D();*/
}

void CLoader::initCrashLytics()
{
    firebase::crashlytics::SetCustomKey("build data", __DATE__);
    firebase::crashlytics::SetCustomKey("build time", __TIME__);

    firebase::crashlytics::SetUserId(CSettings::m_Settings.szNickName);
   /// firebase::crashlytics::SetCustomKey("Nick", CSettings::m_Settings.szNickName);
}

void CLoader::loadSetting()
{
    CSettings::LoadSettings(nullptr);
}

void *CLoader::loadSettingThread(void *p)
{
    CSettings::LoadSettings(nullptr);

    pthread_exit(nullptr);
}

void CLoader::initJavaClasses(JavaVM* pjvm)
{
    JNIEnv* env = nullptr;
    pjvm->GetEnv((void**)& env, JNI_VERSION_1_6);

    CSpeedometr::clazz = env->FindClass("com/criminal/moscow/gui/SpeedometerNew");
    CSpeedometr::clazz = (jclass) env->NewGlobalRef( CSpeedometr::clazz );

    CDonate::clazz = env->FindClass("com/criminal/moscow/gui/donate/Donate");
    CDonate::clazz = (jclass) env->NewGlobalRef( CDonate::clazz );

    CEditobject::clazz = env->FindClass("com/criminal/moscow/gui/AttachEdit");
    CEditobject::clazz = (jclass) env->NewGlobalRef( CEditobject::clazz );

    CChooseSpawn::clazz = env->FindClass("com/criminal/moscow/gui/auth/SpawnSelect");
    CChooseSpawn::clazz = (jclass) env->NewGlobalRef(CChooseSpawn::clazz);


    CRegistration::clazz = env->FindClass("com/criminal/moscow/gui/auth/Registration");
    CRegistration::clazz = (jclass) env->NewGlobalRef(CRegistration::clazz);


    CAuthrization::clazz = env->FindClass("com/criminal/moscow/gui/auth/Autorization");
    CAuthrization::clazz = (jclass) env->NewGlobalRef(CAuthrization::clazz);


    CHUD::clazz = env->FindClass("com/criminal/moscow/gui/CriminalHud");
    CHUD::clazz = (jclass) env->NewGlobalRef(CHUD::clazz);

    CGameActionMenu::clazz = env->FindClass("com/criminal/moscow/gui/GameActionMenu");
    CGameActionMenu::clazz = (jclass) env->NewGlobalRef(CGameActionMenu::clazz);

    CDialog::clazz = env->FindClass("com/criminal/moscow/gui/dialogs/Dialog");
    CDialog::clazz = (jclass) env->NewGlobalRef( CDialog::clazz );

    CJavaGui::clazz = env->FindClass("com/criminal/moscow/NativeGuiManager");
    CJavaGui::clazz = (jclass) env->NewGlobalRef(CJavaGui::clazz );

    CSnapShotWrapper::clazz = env->FindClass("com/criminal/moscow/SnapShot");
    CSnapShotWrapper::clazz = (jclass) env->NewGlobalRef( CSnapShotWrapper::clazz );

}

