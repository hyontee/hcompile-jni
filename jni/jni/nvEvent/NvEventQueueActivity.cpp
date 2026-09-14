//
// Created by Regerds on 29.10.2024.
//

#include <jni.h>
#include "NvEventQueueActivity.h"
#include "json/json.hpp"
#include "CRegerdsRPC.h"

extern "C" JavaVM* javaVM;

jobject NvEventQueueActivity::nvEventContext = nullptr;

JNIEnv* NvEventQueueActivity::GetJNIEnv()
{
    JNIEnv* env = nullptr;
    int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED) return nullptr;
    if (getEnvStat == JNI_EVERSION) return nullptr;
    if (getEnvStat == JNI_ERR) return nullptr;

    return env;
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initNvEvent(JNIEnv* env, jobject thiz) {
        if (NvEventQueueActivity::nvEventContext == nullptr) {
            NvEventQueueActivity::nvEventContext = env->NewGlobalRef(thiz);
        }
    }
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendRadialOperation(JNIEnv *env, jobject thiz, jint i) {
        CRegerdsRPC::SendRadialOperation(i);
    }
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendNotificationResponse(JNIEnv *env, jobject thiz, jint sub_id, jint type) {
        CRegerdsRPC::SendNotificationResponse(sub_id, type);
    }
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendFuelFillRequest(JNIEnv *env, jobject thiz, jint v, jint f) {
        CRegerdsRPC::SendFuelFillRequest(v, f);
    }
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendPlayerMenuRequest(JNIEnv *env, jobject thiz, jint i) {
        CRegerdsRPC::SendPlayerMenuRequestToServer(i);
    }
}

void NvEventQueueActivity::CallCarRadialMenu(int params[]) {
    int length = sizeof(params);

    JNIEnv* env = NvEventQueueActivity::GetJNIEnv();
    if (env == nullptr) return;

    jclass clazz = env->GetObjectClass(NvEventQueueActivity::nvEventContext);
    jmethodID methodID = env->GetMethodID(clazz, "callCarRadialMenu", "([I)V");

    jintArray jParamArray = env->NewIntArray(length);

    if (methodID == nullptr) return;
    if (jParamArray == nullptr) return;

    env->SetIntArrayRegion(jParamArray, 0, length, params);

    env->CallVoidMethod(NvEventQueueActivity::nvEventContext, methodID, jParamArray);
    env->DeleteLocalRef(jParamArray);
}

void NvEventQueueActivity::OnPacketIncoming(int guiid, jbyteArray jbyteArray1) {
    JNIEnv* env = NvEventQueueActivity::GetJNIEnv();
    if (env == nullptr) return;

    jclass clazz = env->GetObjectClass(NvEventQueueActivity::nvEventContext);
    jmethodID methodID = env->GetMethodID(clazz, "onPacketIncoming", "(I[B)V");

    env->CallVoidMethod(NvEventQueueActivity::nvEventContext, methodID, guiid, jbyteArray1);
}

void NvEventQueueActivity::ShowNotification(int type,
                                            int duration,
                                            int id,
                                            int subId,
                                            char* caption,
                                            char* btnCaption) {
    JNIEnv* env = NvEventQueueActivity::GetJNIEnv();
    if (env == nullptr) return;

    jclass clazz = env->GetObjectClass(NvEventQueueActivity::nvEventContext);
    jmethodID methodID = env->GetMethodID(clazz, "showBrNotification", "(IIIILjava/lang/String;Ljava/lang/String;)V");

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray captionBytes = env->NewByteArray(strlen(caption));
    jbyteArray btnCaptionBytes = env->NewByteArray(strlen(btnCaption));

    env->SetByteArrayRegion(captionBytes, 0, strlen(caption), (jbyte*)caption);
    env->SetByteArrayRegion(btnCaptionBytes, 0, strlen(btnCaption), (jbyte*)btnCaption);

    jstring jcaption = (jstring) env->NewObject(strClass, ctorID, captionBytes, encoding);
    jstring jBtnCaption = (jstring) env->NewObject(strClass, ctorID, btnCaptionBytes, encoding);

    env->CallVoidMethod(NvEventQueueActivity::nvEventContext, methodID, type, duration, id, subId, jcaption, jBtnCaption);
}

void NvEventQueueActivity::ShowBrCarFuelFillDialog(int prices[], int maxFuel, char* mInitialHint) {
    JNIEnv* env = NvEventQueueActivity::GetJNIEnv();
    if (env == nullptr) return;

    jclass clazz = env->GetObjectClass(NvEventQueueActivity::nvEventContext);
    jmethodID methodID = env->GetMethodID(clazz, "showCarFuelFill", "(IIIIIILjava/lang/String;)V");

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray hintBytes = env->NewByteArray(strlen(mInitialHint));
    env->SetByteArrayRegion(hintBytes, 0, strlen(mInitialHint), (jbyte*)mInitialHint);

    jstring jHint = (jstring) env->NewObject(strClass, ctorID, hintBytes, encoding);

    env->CallVoidMethod(NvEventQueueActivity::nvEventContext, methodID, prices[0], prices[1], prices[2], prices[3], prices[4], maxFuel, jHint);
}

void NvEventQueueActivity::ShowNewDonate(int rub, int donate_coins) {
    JNIEnv* env = NvEventQueueActivity::GetJNIEnv();
    if (env == nullptr) return;

    jclass clazz = env->GetObjectClass(NvEventQueueActivity::nvEventContext);
    jmethodID methodID = env->GetMethodID(clazz, "showNewDonate", "(II)V");

    env->CallVoidMethod(NvEventQueueActivity::nvEventContext, methodID, rub, donate_coins);
}

void NvEventQueueActivity::ShoRegister(int i) {
    JNIEnv* env = NvEventQueueActivity::GetJNIEnv();
    if (env == nullptr) return;

    jclass clazz = env->GetObjectClass(NvEventQueueActivity::nvEventContext);
    jmethodID methodID = env->GetMethodID(clazz, "showRegister", "(I)V");

    env->CallVoidMethod(NvEventQueueActivity::nvEventContext, methodID, i);
}

void NvEventQueueActivity::ShowDinnerDialog() {
    JNIEnv* env = NvEventQueueActivity::GetJNIEnv();
    if (env == nullptr) return;

    jclass clazz = env->GetObjectClass(NvEventQueueActivity::nvEventContext);
    jmethodID methodID = env->GetMethodID(clazz, "showDinnerDialog", "()V");

    env->CallVoidMethod(NvEventQueueActivity::nvEventContext, methodID);
}