#pragma once

#include <jni.h>

#include <string>
#include <vector>

struct CSnapItem {
    std::uint32_t slot;
    std::int32_t type;
    std::uint32_t modelId;
	std::uint8_t color1;
	std::uint8_t color2;
    float rotX;
    float rotY;
    float rotZ;
    float zoom;
	std::int32_t scaleX;
	std::int32_t scaleY;
	bool isBig;
};

struct CSnapItemDelete {
    std::uint32_t modelId;
};

#define EXCEPTION_CHECK(env) \
	if ((env)->ExceptionCheck()) \ 
	{ \
		(env)->ExceptionDescribe(); \
		(env)->ExceptionClear(); \
		return; \
	}

class CJavaWrapper
{
	jobject activity;

	//jmethodID s_CallLauncherActivity;
	
public:
	JNIEnv* GetEnv();

    std::vector<CSnapItem> m_vecSnapItemsQueue;
    std::vector<CSnapItemDelete> m_vecSnapItemsDelete;
	
	std::string GetClipboardString();
	//void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void ShowClientSettings();

	void SetUseFullScreen(int b);
	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги
	
	void UpdateSlot(uint32_t slot, int32_t type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, int32_t scaleX, int32_t scaleY, bool isBig);
	void JavaSnapshotProcess(uint32_t CountId, int len, unsigned char * buffer, int pixelX, int pixelY);
	
	void ShowButton();
	void HideButton();
	
	void openSelectorServer();
	void showFingerPrintAuth(int id, char* text);
	
	void CheckAndroidID(int8_t id);
	
	void ShowRadialMenu(int8_t i, int8_t i2, int8_t i3, int8_t i4, int8_t i5, int8_t i6, int8_t i7, int8_t i8, int8_t i9, int8_t i10);
	void HideRadialMenu();
	
	void setCheckGPU();
	void setCheckFiles();

	void SetPauseState(bool status);
	void finishActivity();

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;