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

    jmethodID s_GetClipboardText;
	jmethodID s_CallLauncherActivity;

	jmethodID s_showTuningstage;
	jmethodID s_showTuningmenu;

                  jmethodID s_RadarBR;

	jmethodID s_ShowInputLayout;
	jmethodID s_HideInputLayout;

	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;
	jmethodID s_MakeDialog;

	jmethodID s_showHud;
                  jmethodID s_hideHud;
	jmethodID s_updateHudInfo;

	jmethodID s_showradar;
                  jmethodID s_hideradar;

                  jmethodID s_parseTestServerJson;

	jmethodID s_showHudButtonG;
	jmethodID s_hideHudButtonG;

                  jmethodID s_hideHelper;

	jmethodID s_voiceOn;
	jmethodID s_voiceOff;

	jmethodID s_showGps;
                  jmethodID s_hideGps;

	jmethodID s_showTabWindow;
	jmethodID s_setTabStat;

	jmethodID s_showZona;
	jmethodID s_hideZona;

                  jmethodID s_showx2;
	jmethodID s_hidex2;

	jmethodID s_showSpeed;
                  jmethodID s_hideSpeed;
	jmethodID s_updateSpeedInfo;

	jmethodID s_showNotification;
	jmethodID s_showMenu;

	jmethodID s_setPauseState;

	jmethodID s_showSplash;
	jmethodID s_updateSplash;
	jmethodID s_showRadial;
	
	jmethodID s_showHudFeatures;
	jmethodID s_hideHudFeatures;

	jmethodID s_showDonate;
	jmethodID s_updateDonate;
	jmethodID s_show_sc;
	
	jmethodID s_showHudAndLogo;
	jmethodID s_hideHudAndLogo;
    jmethodID s_showboom;
    jmethodID s_setput;
    jmethodID s_setpause;
    jmethodID s_log;
    jmethodID j_toggleAutoShop;
    jmethodID j_updateAutoShop;
    jmethodID j_togglehud;
    jmethodID j_toggleperson;
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void ShowTuningstage(int money);
	void ShowTuningmenu(int money);

	void ShowRadar();
	void HideRadar();

	void SetRadar();

                  void AddTestServer(char* name);

	void ShowTabWindow();
	void SetTabStat(int id, char* name, int score, int ping);

	void ShowClientSettings();

	void SetUseFullScreen(int b);

	void UpdateHudInfo(int health, int armour, int hunger, int weaponidweik, int ammo, int ammoinclip, int money, int wanted);
	void ShowHud();
                  void HideHud();

	void ShowGPS();
                  void HideGPS();

	void ShowZona();
                  void HideZona();

	void ShowX2();
                  void HideX2();

                  void VoiceOn();
                  void VoiceOff();
                  
                  void ShowHudFeatures();
                  void HideHudFeatures();
   
                  void ShowHudAndLogo();
                  void HideHudAndLogo();

                  void ShowG();
                  void HideG();

	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock, int turnlight);
	void ShowSpeed();
                  void HideSpeed();

	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги
	void ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn);
	void ShowMenu();

	void SetPauseState(bool a1);

	void ShowSplash();

	void HideHelper();
	void UpdateSplash(int progress);	
	
	void ShowRadial(bool park, bool key, bool doors, bool lights, bool suspension, bool launch_control, bool engine, bool turbo);

	void ShowDonate(int money, int bc);
	void UpdateDonate(int money, int bc);
	void show_sc(int money, int bc);
    void ShowBoomBox(bool toggle);

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();

    jobject activity;

    void SetPut(bool toggle);

    void SetPause(bool toggle);

    void Log(char *log);

    void ToggleAutoShop(bool toggle);

    void UpdateAutoShop(const char *name, int price, int count, float maxspeed, float acceleration,
                        int gear);

    void ToggleHud(bool toggle);

	void TogglePerson(bool toggle);

    void toggleDrivingSchool(bool toggle);
};

extern CJavaWrapper* g_pJavaWrapper;