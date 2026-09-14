#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include "dobby.h"

#define TAG "PLUGIN_JNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void*){
    LOGI("JNI_OnLoad");
    return JNI_VERSION_1_6;
}
