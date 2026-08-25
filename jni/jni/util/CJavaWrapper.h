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
	jmethodID s_updateHudInfo;
	jmethodID s_updateLvlInfo;
	jmethodID s_showJavaGreenZone;
	jmethodID s_hideLogoForChat;
	jmethodID s_showNotification;
	jmethodID s_showStylingCenter;
	jmethodID s_hideStylingCenter;
	jmethodID s_showSpeed;
    jmethodID s_hideSpeed;
	jmethodID s_updateSpeedInfo;
	jmethodID s_showAdminRecon;
	jmethodID s_showAchievements;
	jmethodID s_showMagicStore;
	jmethodID s_showChooseSpawn;


	jmethodID s_setPauseState;
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void ShowClientSettings();

	void ShowNotification(int type, char* text, int duration, char* positive_command, char* negative_command);
	void ShowAdminRecon(char* name, int id);

	void SetUseFullScreen(int b);
	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги

	void SetPauseState(bool a1);

	void UpdateHudInfo(int health, int armour, int hunger, int weaponid, int ammo, int ammoinclip, int money, int wanted);
	void ShowHud();
    void HideHud();
	void UpdateLvl(int lvl, int exp, int max_exp);
	void JavaGreenZone(int isShow);
	void HideLogoForChat(int isShow);
	void ShowStylingCenter(int money);
	void HideStylingCenter();
	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
	void ShowSpeed();
    void HideSpeed();
	void ShowMagicStore(int bronze_coins, int silver_coins, int gold_coins);
	void ShowAchievements(int progress_1, int progress_2, int progress_3, int progress_4, int progress_5);
	void ShowChooseSpawn(int organizationstate, int stationstate, int exitstate, int garagestate, int housestate);

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;

// edited source code by x 1 y 2 z
// if you delete this code i fuck ur mom