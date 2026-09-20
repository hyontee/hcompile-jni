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
    jmethodID s_tempHud;
	jmethodID s_updateHudInfo;
    jmethodID s_showNotification;

    // не нужно чинить все что работает
    // скоро поймешь это на своем опыте
	jmethodID s_updateLoading;
	jmethodID s_showTabWindow;
	jmethodID s_setTabStat;

	jmethodID s_showSpeed;
    jmethodID s_hideSpeed;
	jmethodID s_updateSpeedInfo;

	jmethodID s_showHudButtonG;
	jmethodID s_hideHudButtonG;

	jmethodID s_setPauseState;

    /// NEW
    jmethodID s_ShowAZS;
    jmethodID s_ShowSpawnSelector;
    jmethodID s_ShowCaptcha;
    jmethodID s_ShowNumbers;
    jmethodID s_GenerateNumber;
    jmethodID s_BuyNumber;
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
    void TempHud(bool state);

    void UpdateLoading(int status);

	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
	void ShowSpeed();
    void HideSpeed();

	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText);

	void ShowTabWindow();
	void SetTabStat(int id, char* name, int score, int ping);

    void ShowG();
    void HideG();

	void SetPauseState(bool a1);

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();

    /// NEW
    void ShowNotification(int type, char *text, int duration, char *actionforBtn, char *textBtn);
    void ShowAZS(int fuelId, int maxFuel, float currentFuel, int price, int balance);
    void ShowSpawnSelector(bool isHouse, bool isFraction);
    void ShowCaptcha();
    bool m_bCaptcha;
    void ShowNumbers(int balance);
    void GenerateNumber(bool status);
    void BuyNumber(bool status);
    bool m_bGPS = false, m_bGreenZone = false;
    ///
};

extern CJavaWrapper* pJava;