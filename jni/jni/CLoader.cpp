//
// Created by Error on 25.04.2024.
//

#include "CLoader.h"
#include "main.h"
#include "util/CJavaWrapper.h"
#include "br/CFamily.h"

void CLoader::LoadClass(JavaVM* vm){
    JNIEnv* env = nullptr;
    vm->GetEnv((void**)& env, JNI_VERSION_1_6);

    CFamily::clazz = env->FindClass("com/rockstargames/gtacr/gui/family/BrFamilyManager");
    CFamily::clazz = (jclass) env->NewGlobalRef( CFamily::clazz );
}