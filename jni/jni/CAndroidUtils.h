#include "main.h"
#include "CJavaWrapper.h"

#include <jni.h>
#include <string>
#include <sys/system_properties.h>

inline int GetAndroidVersion(char *value)
{
	return __system_property_get(obfuscate("ro.build.version.release"), value);
}

inline int GetBrand(char *value)
{
	return __system_property_get(obfuscate("ro.product.brand"), value);
}

inline int GetModel(char *value)
{
	return __system_property_get(obfuscate("ro.product.model"), value);
}

inline int GetArm(char *value)
{
    return __system_property_get(obfuscate("ro.product.cpu.abi"), value);
}

inline jstring GetPackageName(JNIEnv *env, jobject jActivity)
{
    jmethodID method = env->GetMethodID(env->GetObjectClass(jActivity), obfuscate("getPackageName"), obfuscate("()Ljava/lang/String;"));
    return (jstring)env->CallObjectMethod(jActivity, method);
}

inline jobject GetGlobalActivity(JNIEnv *env)
{
    jclass activityThread = env->FindClass(obfuscate("android/app/ActivityThread"));
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, obfuscate("currentActivityThread"), obfuscate("()Landroid/app/ActivityThread;"));
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    jmethodID getApplication = env->GetMethodID(activityThread, obfuscate("getApplication"), obfuscate("()Landroid/app/Application;"));
    jobject context = env->CallObjectMethod(at, getApplication);
    return context;
}

inline void toasty(JNIEnv *mEnv, const char* txt, int msDuration = 3500, ...)
{
    jclass ToastClass = mEnv->FindClass(obfuscate("android/widget/Toast"));
    jmethodID makeTextMethodID = mEnv->GetStaticMethodID(ToastClass, obfuscate("makeText"), obfuscate("(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;"));
    jmethodID showMethodID = mEnv->GetMethodID(ToastClass, obfuscate("show"), obfuscate("()V"));

    jstring message = mEnv->NewStringUTF(txt);
    jint duration = msDuration;
    jobject toast = mEnv->CallStaticObjectMethod(ToastClass, makeTextMethodID, GetGlobalActivity(mEnv), message, duration);

    mEnv->CallVoidMethod(toast, showMethodID);
    mEnv->DeleteLocalRef(message);
}