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
        void ShowCalendar(const std::string& jsonString);
        void ShowMarketplace(const std::string& jsonString);
        void ShowCraft(const std::string& jsonString); //by kmonarch aka @werixtonstudio
        void ShowRadialMenu(const std::string& jsonString);
        void ShowElectric(const std::string& jsonString);
        void ShowGifts(const std::string& jsonString);
        void ShowBpRewards(const std::string& jsonString);
        void ShowCases(const std::string& jsonString);
        void ShowAdminTools(const std::string& jsonString);
        void ShowFuel(const std::string& jsonString);
	void ShowFraction(const std::string& jsonString);
	void ShowDonate(const std::string& jsonString);
	void ShowInventory(const std::string& jsonString);
	void ShowFamily(const std::string& jsonString);
	void ShowTuning(const std::string& jsonString);

    CJavaWrapper(JNIEnv* env);
    ~CJavaWrapper();

	jclass edgar;
jclass guiManagerClass;

    jobject activity;
};

extern CJavaWrapper* g_pJavaWrapper;