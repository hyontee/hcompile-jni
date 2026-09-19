#pragma once

#include <vector>
#include <string>
#include <jni.h>
#include "game/jniutil.h"

#define EXCEPTION_CHECK(env) \
	if ((env)->ExceptionCheck()) \
	{ \
		(env)->ExceptionDescribe(); \
		(env)->ExceptionClear(); \
		return; \
	}

class CJavaWrapper
{
public:
    JNIEnv* GetEnv();

    void CallGUI(int guiId, const std::string& jsonString, const char* methodName);

    void SendJsonData(int guiId, const std::string& jsonString);

    CJavaWrapper(JNIEnv* env);
    ~CJavaWrapper();

    jclass guiManagerClass;
    jclass s_JSONTransportClass;
    jclass death;
    jobject activity;
};

extern CJavaWrapper* g_pJavaWrapper;
