#pragma once

#include <jni.h>

#include <string>

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

	jmethodID s_GetClipboardText;
	jmethodID s_CallLauncherActivity;

	jmethodID s_ShowInputLayout;
	jmethodID s_HideInputLayout;

	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;
	jmethodID s_MakeDialog;

	jmethodID s_showHud;
    jmethodID s_hideHud;
	
	jmethodID s_Showonline;
    jmethodID s_Hideonlone;
	jmethodID s_ShowTimeData;
	jmethodID s_HideTimeData;
	
        jmethodID s_showMenu;
 
	jmethodID s_upDatesaint;
	
	

	jmethodID s_setPauseState;
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();
	void upDatesaint(int health, int armour, int hunger, int weaponid, int ammo, int ammoinclip, int money, int wanted);
	void ShowHud();
    void HideHud();
	
	void Showonline();
    void Hideonlone();
	void ShowTimeData();
	void HideTimeData();

        void ShowMenu();

	void ShowClientSettings();

	void SetUseFullScreen(int b);
	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги

	void SetPauseState(bool a1);

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;

// edited source code by x 1 y 2 z
// if you delete this code i fuck ur mom