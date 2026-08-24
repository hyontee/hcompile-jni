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

struct INFO_DIALOG{
    int dialogid;
    int response;
    int listitem;
    char inputtext[256 + 1];
};

class CJavaWrapper
{
	jobject activity;

	jmethodID s_GetClipboardText;
	jmethodID s_CallLauncherActivity;
	
	
	jmethodID s_ShowInputLayout;
	jmethodID s_HideInputLayout;

	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;

	jmethodID s_ToastMakeText;


	jmethodID s_BuildDialog;

    jmethodID s_ToggleRender;
    jmethodID s_SetVisibleDialog;

    jmethodID s_SetVisibleKeyboardStandard;

	jmethodID s_ShowSpeedometer;
	jmethodID s_HideSpeedometer;

	jmethodID s_SetSpeedometerSpeed;
	jmethodID s_SetSpeedometerMileage;
	jmethodID s_SetSpeedometerFuel;
	jmethodID s_SetSpeedometerCarHP;
	jmethodID s_SetSpeedometerCarTurnlights;

	jmethodID s_SetSpeedometerEngine;
	jmethodID s_SetSpeedometerLight;
	jmethodID s_SetSpeedometerBelt;
	jmethodID s_SetSpeedometerLock;

	jmethodID s_ShowTab;
	jmethodID s_HideTab;
	jmethodID s_ClearTabStat;
	jmethodID s_SetTabStat;
	jmethodID s_ShowHUD;
	jmethodID s_ShowLogo;
	jmethodID s_HideHUD;
	jmethodID s_SetMoney;
	jmethodID s_SetHP;
	jmethodID s_SetArmour;
	jmethodID s_SetEat;
	jmethodID s_SetAmmo1;
	jmethodID s_SetAmmo2;
	jmethodID s_SetWantedLevel;
	jmethodID s_SetOnline;
	jmethodID s_SetX2;
	jmethodID s_UpdateTime;
	jmethodID s_UpdateHudIcon;
	jmethodID s_UpdateHud;
	jmethodID s_setCashMoney;
	jmethodID s_SetHudOnline;
	jmethodID s_setServerID;
	jmethodID s_setServerInfo;

	jmethodID s_ExitGame;

	jmethodID s_showNotification;
	jmethodID s_hideNotification;

	jmethodID s_showCaptcha;

    jmethodID s_SetInterface;
    jmethodID s_ShowWin;
    jmethodID s_SetFuel;
	jmethodID s_SetCar;
	jmethodID s_showMine;
	jmethodID s_showAutoSalon;
	jmethodID s_hideAutoSalon;
    jmethodID s_ShowAd;
    jmethodID s_ShowCases;
	jmethodID s_showSawmill;

	jmethodID s_showRubbish;
	
	jmethodID s_showCollectors;

	jmethodID s_setLoadingText;

	jmethodID s_showJobInfo;
	jmethodID s_hideJobInfo;

	jmethodID s_byteArray;

	jmethodID s_showBizWar;
	jmethodID s_hideBizWar;

	jmethodID s_showPotato;

	jmethodID s_nativeRendered;

	jmethodID s_showInventory;
	jmethodID s_hideInventory;
	jmethodID s_setInventorySkin;
	jmethodID s_updateSlots;
    jmethodID s_setNullSlot;
	jmethodID s_updateAcsSlots;
	jmethodID s_setInventoryInfo;
	jmethodID s_clearInventory;

	jmethodID s_showInventoryUniversal;
	jmethodID s_hideInventoryUniversal;
	jmethodID s_updateSlotsLeft;
	jmethodID s_updateSlotsRight;
	jmethodID s_setNullSlotLeft;
	jmethodID s_setNullSlotRight;
	jmethodID s_clearInventoryLeft;
	jmethodID s_clearInventoryRight;
	jmethodID s_setUniversalInventoryInfo;

	jmethodID s_showBuyAuto;
	jmethodID s_hideBuyAuto;
	jmethodID s_addCarToRecycler;

	jmethodID s_showTuning;
	jmethodID s_hideTuning;

	jmethodID s_showTrade;
	jmethodID s_hideTrade;
	jmethodID s_updateTradeRightSlotsInfo;
	jmethodID s_updateTradeLeftSlotsInfo;
	jmethodID s_setNullTradeRightSlotsInfo;
	jmethodID s_clearTradeRightSlotsInfo;
	jmethodID s_setTradeMaxPages;
	jmethodID s_clearTradeLeftSlotsInfo;
    jmethodID s_setTradeReadiness;
    jmethodID s_setTradePlayerName;
    jmethodID s_setTradeMoney;

	jmethodID s_showVehicleSpawner;
	jmethodID s_hideVehicleSpawner;

	jmethodID s_showSkinSpawner;
	jmethodID s_hideSkinSpawner;
public:
	uintptr_t oldTarget;
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);
	void ShowInputLayout();
	void HideInputLayout();
	void ShowToastText(char* text);
	void SetSpeedometerFuel(int percfuel, int fuel);
	void ShowClientSettings();

	void SetUseFullScreen(int b);

	void BuildDialog(uint16_t wDialogID, char* title, char* content, char* button1, char* button2, uint8_t typeDialog);

	void SetVisibleKeyboard(bool active, int type);

    void ToggleRender(bool active);
    void SetVisibleDialog(bool active);

	void ExitGame();

	void ShowVehicleSpawner();
	void HideVehicleSpawner();

	void ShowSkinSpawner();
	void HideSkinSpawner();

	void ShowSpeedometer(bool isAnimation);
	void HideSpeedometer(bool isAnimation);

	void SetSpeedometerSpeed(int speed);
	void SetSpeedometerMileage(int mileage);
	void SetSpeedometerCarHP(int hp);

	void SetEngineState(int state);
	void SetLightState(int state);
	void SetBeltState(int state);
	void SetLockState(int state);

	void ShowTab(bool isAnim);
	void HideTab();
	void ClearTabStat();
	void SetTabStat(int id, char* names, int score, int pings);

	void ShowHUD(bool isAnimation);
	void ShowLogo(bool enable);
	void HideHUD(bool isAnimation);
	void SetMoney(int money);
	void SetHP(int value);
	void SetArmour(int value);
	void SetEat(int value);
	void SetAmmo(int ammo1, int ammo2);
	void SetWantedLevel(int level);
	void SetOnline(int online);
	void SetX2(bool state);
	void UpdateTime();
	void UpdateHudIcon(int gunId);
	void UpdateHud();
	void SetHudServerID(int id);
	void SetHudLogo(int type);
	void SetHudOnline(int type);
	void SetHudServerInfo(int id, char name[]);

	void UpdateSpeedometerTurnlights(int turnlights);

	void showNotification(int type, char* text, int duration, int actionId, char* text2, char* text3);
	void hideNotification();

	void ShowCaptcha(char* str);

	void ShowMine();

    void SetInterface(int type);
    void ShowWin(int number);
    void SetFuel(int fuel);
    void SetCar(int price, char* namecar);
	void showAutoSalon();
    void ShowCases();
    void ShowAd();
	void hideAutoSalon();
	void ShowSawmill();

	void ShowRubbish();

	void ShowCollectors();

	void SetLoadingText(int id);

	void ShowJobInfo(int progress, int money, char* string);
	void HideJobInfo();

	void showImageFromByte(long len, uint8_t* array);

	void showBizWar(int time, uint32_t colorLeft, uint32_t colorRight, int kills, int deaths, float damage, float take, char* player,  int points1, int points2);
	void hideBizWar();

	void showPotato();

	void onNativeRendered(int id, jbyteArray array, int sizeX, int sizeY);

	void ShowInventory(int pagecount, int slotcount);
	void HideInventory();
	void SetInventorySkin(int modelId, float x, float y, float z, float zoom);
	void UpdateSlotsInfo(int id, int type, int amount, int modelId, float x, float y, float z, float zoom, char* caption, char* info);
	void UpdateSlotsAcsInfo(int acc_slot, int modelId, float x, float y, float z, float zoom, char* caption, char* info);
	void SetInventoryInfo(int health, int thirst, int hunger, int money, int donate);
	void ClearInventory();
	void SetNullSlot(int id);

	void ShowTradeInventory(int pagecount, int slotcount);
	void HideTradeInventory();
	void UpdateTradeRightSlotsInfo(int id, int type, int amount, int modelId, float x, float y, float z, float zoom, char* caption, char* info, char* amountText);
	void UpdateTradeLeftSlotsInfo(int id, int type, int amount, int modelId, float x, float y, float z, float zoom, char* caption, char* info, char* amountText);
	void ClearTradeRightInventory();
	void SetTradeNullRightSlot(int id);
	void SetTradeMaxPages(int pages);
	void ClearTradeLeftInventory();
	void SetTradePlayerName(int player, char* name);
	void SetTradePlayerReadiness(int player, int status);
    void SetTradeMoney(int money1, int money2, char* inventoryName);

	void ShowInventoryUniversal(int rightpagecount, int rightslotcount, int leftpagecount, int leftslotcount);
	void HideInventoryUniversal();
	void UpdateSlotsRightInfo(int id, int type, int amount, int modelId, float x, float y, float z,
							  float zoom, char *caption, char *info);
	void UpdateSlotsLeftInfo(int typeInventory, int id, int type, int amount, int modelId, float x, float y, float z,
							 float zoom, char *caption, char *info, int price);
	void SetNullSlotLeft(int id);
	void SetNullSlotRight(int id);
	void ClearInventoryLeft();
	void ClearInventoryRight();
	void SetUniversalInventoryInfo(int money, int donate, char* inventory1, char* inventory2);

	void ShowBuyAuto();
	void HideBuyAuto();
	void AddCarToRecycler(int modelId, float x, float y, float z, float zoom, int price, int maxSpeed, int maxFuel, float timeTo100, int availabilityInStock, char* name);

	void ShowTuning();
	void HideTuning();

    void Process();
    bool m_bLastSentActiveDialog = false;
    bool m_bWaitResponseDialog;
    INFO_DIALOG m_infoDialogWaitReponse;
    bool m_bWaitActiveNewDialog;

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();

	bool isGlobalShowTab;

	bool isLocalShowHUD;
};

extern CJavaWrapper* g_pJavaWrapper;