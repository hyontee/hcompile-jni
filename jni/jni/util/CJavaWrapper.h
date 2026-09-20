#pragma once

#include <jni.h>

#include <string>

#define BrDialogVoice 0
#define BrDialogPlates 1
#define BrDialogFuelFill 2
#define BrDialogDiner 3
#define BrDialogHack 4
#define BrDialogRobbery 5
#define BrDialogRent 6
#define BrDialogWires 7
#define BrDialogPipes 8
#define BrAudioDialog 9
#define BrDialogWindow 10
#define BrChooseServerDialog 11
#define BrFingerPrintDialog 12
#define BrNotification 13
#define BrDialogMenu 14
#define BrDialogDance 15
#define BrDialogTaxi 16
#define BrDialogTaxiOrder 17
#define BrDialogTaxiRating 18
#define BrDialogGraphicsSetting 19
#define BrCaptcha 20
#define BrDialogMap 21
#define GUIDonate 22
#define BrDialogSawmill 23
#define GUISmiEditor 24
#define GUIPlayersList 25
#define BrNewCaptcha 26
#define GUIRadialMenuForCar 27
#define GUITuning 28
#define GUIHalloween 30
#define GUIHalloweenGame 31
#define HudManager 32
#define GUIUsersInventory 33
#define GUICarsTrunkOrCloset 34
#define GUIBlackPassBanner 35
#define GUISocialInteraction 36
#define GUIDrivingSchool 37
#define GUIRegistration 38
#define GUITutorial 39
#define GUIWoundSystem 40
#define GUIVipAccount 41
#define GUIEntertainmentSystem 42
#define GUIEntertainmentSystemFinalWindow 43
#define GUIFamilySystem 45
#define GUIFractionSystem 46
#define GUISpawnLocation 50
#define GUISocialNetworkLink 52
#define GUIInviteBanner 55

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
	jmethodID s_SetUseFullScreen;

	jmethodID s_setPauseState;

	jmethodID s_onJsonDataIncoming;
	jmethodID s_crashLog;
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void SetUseFullScreen(int b);

	void SetPauseState(bool a1);

	void UpdateHudInfo(int health, int armour, int hunger, int weaponidughur, int ammo, int ammoinclip, int money, int wanted);
	void ShowHud();
                  void HideHud();

	void ShowGPS();
                  void HideGPS();

	void ShowZona();
                  void HideZona();

	void ShowX2();
                  void HideX2();

                  void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги
	void ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn);
	void ShowSplash();
	void UpdateSplash(int progress);	

	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
	void ShowSpeed();
	void HideSpeed();

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();

    void CrashLog(const char *error);
};

extern CJavaWrapper* g_pJavaWrapper;