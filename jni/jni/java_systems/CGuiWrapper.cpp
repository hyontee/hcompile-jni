//
// Created by Error on 03.06.2025.
//

#include "CGuiWrapper.h"
#include "util/CJavaWrapper.h"
template<> jclass  CGuiWrapper<class T>::clazz = nullptr;
template<> jobject CGuiWrapper<class T>::thiz = nullptr;
template<> bool    CGuiWrapper<class T>::bIsShow = false;

template<> void CGuiWrapper<class T>::hide() {
    if(CGuiWrapper::thiz != nullptr)
    {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        jmethodID method = env->GetMethodID(CGuiWrapper::clazz, "destroy", "()V");
        env->CallVoidMethod(CGuiWrapper::thiz, method);

        env->DeleteGlobalRef(CGuiWrapper::thiz);
        CGuiWrapper::thiz = nullptr;
        CGuiWrapper::bIsShow = false;
    }
}

template<> void CGuiWrapper<class T>::show() {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(CGuiWrapper::thiz == nullptr) {

        jmethodID constructor = env->GetMethodID(CGuiWrapper::clazz, "<init>",
                                                 "()V");
        CGuiWrapper::thiz = env->NewObject(CGuiWrapper::clazz, constructor);
        CGuiWrapper::thiz = env->NewGlobalRef(CGuiWrapper::thiz);
        CGuiWrapper::bIsShow = true;

    }
}