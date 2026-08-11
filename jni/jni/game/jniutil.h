#pragma once

#include <vector>
#include <string>
#include <jni.h>

#define EXCEPTION_CHECK(env) \
	if ((env)->ExceptionCheck()) \
	{ \
		(env)->ExceptionDescribe(); \
		(env)->ExceptionClear(); \
		return; \
	}

class CJavaWrapper
{

    jmethodID s_showradar;
   
public:
    JNIEnv* GetEnv();

    void ShowReg(const std::string& jsonString);
	void ShowReg2(const std::string& jsonString);
    void ShowReward(const std::string& jsonString);
	void ShowCase(const std::string& jsonString);
		void ShowCase2(const std::string& jsonString);
void ShowTuning(const std::string& jsonString);
    void ShowDonate(const std::string& jsonString);
void ShowInv(const std::string& jsonString);

    CJavaWrapper(JNIEnv* env);
    ~CJavaWrapper();

	jclass edgar;
jclass guiManagerClass;

    jobject activity;
};

extern CJavaWrapper* g_pJavaWrapper;