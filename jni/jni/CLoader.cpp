//
// Created by Error on 25.04.2024.
//

#include "CLoader.h"
#include "main.h"
#include "util/CJavaWrapper.h"
//#include "br/CDonate.h"
#include "util/CSnapShotWrapper.h"


void CLoader::LoadClass(JavaVM* vm){
    JNIEnv* env = nullptr;
    vm->GetEnv((void**)& env, JNI_VERSION_1_6);

  //  CDonate::clazz = env->FindClass("com/rockstargames/gtacr/gui/donate/BrDonateManager");
  //  CDonate::clazz = (jclass) env->NewGlobalRef( CDonate::clazz );

    CSnapShotWrapper::clazz = env->FindClass("com/rkkdev/game/GameRender");
    CSnapShotWrapper::clazz = (jclass) env->NewGlobalRef( CSnapShotWrapper::clazz );
	
}