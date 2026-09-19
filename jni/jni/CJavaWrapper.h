#pragma once

#include "main.h"
#include "../CAndroidUtils.h"

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
	jmethodID s_updateHudInfo;

	jmethodID s_showLogoLayout;
	jmethodID s_hideLogoLayout;

    jmethodID s_showNotification;

	jmethodID s_updateLoading;

    jmethodID s_showTab;
    jmethodID s_setTab;

    jmethodID s_showTabWindow;
    jmethodID s_setTabStat;

	jmethodID s_showSpeed;
    jmethodID s_hideSpeed;
	
	jmethodID s_updateSpeedInfo;

	jmethodID s_showHudButtonG;
	jmethodID s_hideHudButtonG;

	jmethodID s_setPauseState;

    /// NEW

    ///
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void ShowClientSettings();

	void SetUseFullScreen(int b);

	void UpdateHudInfo(int health, int armour, int hunger, int weaponid, int ammo, int ammoinclip, int playerid, int money, int wanted);
	void ShowHud();
    void HideHud();

	void ShowLogoLayout();
	void HideLogoLayout();

    void UpdateLoading(int status);

	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
	void ShowSpeed();
    void HideSpeed();

	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText);

    void ShowG();
    void HideG();
	
	void ShowTab();
	
    void ShowTabWindow();
    void SetTabStat(int id, char* name, int score, int ping);	

	void SetPauseState(bool a1);

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();

    /// NEW
    void ShowNotification(int type, char *text, int duration, char *actionforBtn, char *textBtn);
    bool m_bGPS = false, m_bGreenZone = false;
    ///
};

extern CJavaWrapper* pJava;