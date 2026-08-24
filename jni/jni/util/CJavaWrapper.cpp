#include "CJavaWrapper.h"
#include <thread>
#include "../main.h"
#include "../game/game.h"

extern "C" JavaVM* javaVM;
extern int showHud;

#include <stddef.h>
#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../voice/CVoiceChatClient.h"
#include "CServerManager.h"
#include "../vendor/curl/include/curl/curl.h";
#include "../util/util.h"
#include "../vendor/json/json/json.h"
#include "../dialog.h"
#include "../checkfilehash.h"
#include "../game/CTurnLights.h"
//#include "..//JNIStringUtil.h"
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <KillList.h>

extern CVoiceChatClient* pVoice;
extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;
extern CDialogWindow* pDialogWindow;
extern CGame *pGame;
extern KillList *pKillList;
extern CSnapShotHelper* pSnapShotHelper;

Json::Reader reader;
extern int greenzone;
bool isActiveSelectedServer = false;

JNIEnv* CJavaWrapper::GetEnv()
{
	JNIEnv* env = nullptr;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_4);

	if (getEnvStat == JNI_EDETACHED)
	{
		Log(OBFUSCATE("GetEnv: not attached"));
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		{
			Log(OBFUSCATE("Failed to attach"));
			return nullptr;
		}
	}
	if (getEnvStat == JNI_EVERSION)
	{
		Log(OBFUSCATE("GetEnv: version not supported"));
		return nullptr;
	}

	if (getEnvStat == JNI_ERR)
	{
		Log(OBFUSCATE("GetEnv: JNI_ERR"));
		return nullptr;
	}

	return env;
}

	
std::string CJavaWrapper::GetClipboardString()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return std::string("");
	}

	auto retn = (jbyteArray)env->CallObjectMethod(activity, s_GetClipboardText);

	if ((env)->ExceptionCheck())
	{
		(env)->ExceptionDescribe();
		(env)->ExceptionClear();
		return std::string("");
	}

	if (!retn)
	{
		return std::string("");
	}

	jboolean isCopy = true;

	jbyte* pText = env->GetByteArrayElements(retn, &isCopy);
	jsize length = env->GetArrayLength(retn);

	std::string str((char*)pText, length);

	env->ReleaseByteArrayElements(retn, pText, JNI_ABORT);
	
	return str;
}

void CJavaWrapper::CallLauncherActivity(int type)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_CallLauncherActivity, type);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetVisibleKeyboard(bool active, int type){
    JNIEnv* env = GetEnv();

    if (!env)
    {
        Log(OBFUSCATE("No env"));
        return;
    }

    env->CallVoidMethod(activity, s_SetVisibleKeyboardStandard, active ? 1 : 0, type);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::Process(){
    /*uint32_t isActiveCamera_36_4 = *(uint32_t*)(SA_ADDR(0x63E048+36));
    if(isActiveCamera_36_4 == 0){
        if(m_bLastSentActiveDialog != true){
            m_bLastSentActiveDialog = true;

            g_pJavaWrapper->ToggleRender(true);
        }
    }
    if(isActiveCamera_36_4 == 1){
        if(m_bLastSentActiveDialog != false){
            m_bLastSentActiveDialog = false;

            g_pJavaWrapper->ToggleRender(false);
        }
    }*/

    /*if(m_bWaitResponseDialog){
        m_bWaitResponseDialog = false;
        if (pNetGame && pGame && pGame->FindPlayerPed()){
            //pGame->FindPlayerPed()->TogglePlayerControllable(true);
            //pGame->FindPlayerPed()->SetTogglePlayerControllable(true, false);
        }
        pDialogWindow->m_bActiveDialog = false;
        if (pNetGame){
            pNetGame->SendDialogResponse(m_infoDialogWaitReponse.dialogid, m_infoDialogWaitReponse.response, m_infoDialogWaitReponse.listitem, m_infoDialogWaitReponse.inputtext);
        }
    }*/
}

void CJavaWrapper::ToggleRender(bool active) {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_ToggleRender, active ? 1 : 0);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetVisibleDialog(bool active) {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_SetVisibleDialog, active ? 1 : 0);

	EXCEPTION_CHECK(env);
}



void CJavaWrapper::ShowInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}


	env->CallVoidMethod(activity, s_ShowInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_HideInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowClientSettings()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_ShowClientSettings);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::BuildDialog(uint16_t dialogId, char* title, char* content, char* button1, char* button2, uint8_t typeDialog){
	//s_BuildDialog
    JNIEnv* env = GetEnv();

    if (!env)
    {
        Log(OBFUSCATE("No env"));
        return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(title));
    env->SetByteArrayRegion(bytes, 0, strlen(title), (jbyte*)title);

	jbyteArray bytes2 = env->NewByteArray(strlen(content));
    env->SetByteArrayRegion(bytes2, 0, strlen(content), (jbyte*)content);

	jbyteArray bytes3 = env->NewByteArray(strlen(button1));
    env->SetByteArrayRegion(bytes3, 0, strlen(button1), (jbyte*)button1);

	jbyteArray bytes4 = env->NewByteArray(strlen(button2));
    env->SetByteArrayRegion(bytes4, 0, strlen(button2), (jbyte*)button2);

    env->CallVoidMethod(activity, s_BuildDialog, dialogId, bytes, bytes2, bytes3, bytes4,(int) typeDialog);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
    
	EXCEPTION_CHECK(env);
}




void CJavaWrapper::ShowToastText(char* text){


}

void CJavaWrapper::SetUseFullScreen(int b)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_SetUseFullScreen, b);

	EXCEPTION_CHECK(env);
}


void CJavaWrapper::ShowSpeedometer(bool isAnimation)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ShowSpeedometer, isAnimation);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideSpeedometer(bool isAnimation)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_HideSpeedometer, isAnimation);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetSpeedometerSpeed(int speed)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetSpeedometerSpeed, speed);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::UpdateSpeedometerTurnlights(int turnlights)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetSpeedometerCarTurnlights, turnlights);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetSpeedometerMileage(int mileage)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetSpeedometerMileage, mileage);

	EXCEPTION_CHECK(env);
}



void CJavaWrapper::SetSpeedometerCarHP(int hp)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetSpeedometerCarHP, hp / 10);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetEngineState(int state)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetSpeedometerEngine, state);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetLightState(int state)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	

	env->CallVoidMethod(activity, s_SetSpeedometerLight, state);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetBeltState(int state)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetSpeedometerBelt, state);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetLockState(int state)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetSpeedometerLock, state);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetTabStat(int id, char* names, int score, int pings) {
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jbyteArray bytes = env->NewByteArray(strlen(names));
    env->SetByteArrayRegion(bytes, 0, strlen(names), (jbyte*)names);
	
	env->CallVoidMethod(activity, s_SetTabStat, id, bytes, score, pings);
	env->DeleteLocalRef(bytes);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowTab(bool isAnim) {
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	g_pJavaWrapper->isGlobalShowTab = true;

	pGame->ToggleHUDElement(HUD_ELEMENT_CHAT, false);
	pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, false);
	pGame->ToggleHUDElement(HUD_ELEMENT_MAP, false);

	env->CallVoidMethod(activity, s_ShowTab, isAnim);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideTab() {
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_HideTab);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ClearTabStat() {
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ClearTabStat);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowHUD(bool isAnimation)
{
	if (!pSettings->GetReadOnly().iNewHud)
		return;

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ShowHUD, isAnimation);
	isLocalShowHUD = true;

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowLogo(bool enable)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ShowLogo, enable);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideHUD(bool isAnimation)
{
	

	if (!isLocalShowHUD)
		return;

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_HideHUD, isAnimation);
	isLocalShowHUD = false;

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetMoney(int money)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetMoney, money);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetHP(int value)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetHP, value);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetArmour(int value)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetArmour, value);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetEat(int value)
{

}

void CJavaWrapper::SetAmmo(int ammo1, int ammo2)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetAmmo1, ammo1);
	env->CallVoidMethod(activity, s_SetAmmo2, ammo2);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetWantedLevel(int level)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetWantedLevel, level);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetOnline(int online)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetOnline, online);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::UpdateTime()
{
	CallLauncherActivity(17);
}

void CJavaWrapper::UpdateHudIcon(int gunId)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_UpdateHudIcon, gunId);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::UpdateHud()
{

}

void CJavaWrapper::SetHudServerInfo(int id, char name[])
{

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}


	jstring jmsg = env->NewStringUTF(name);

	env->CallVoidMethod(activity, s_setServerInfo, id, jmsg);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetHudServerID(int id)
{

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_setServerID, id);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetHudOnline(int type)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetHudOnline, type);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ExitGame() 
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_ExitGame);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowMine() 
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_showMine);

	EXCEPTION_CHECK(env);
}
void CJavaWrapper::SetFuel(int fuel)
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    env->CallVoidMethod(this->activity, this->s_SetFuel, fuel);

    EXCEPTION_CHECK(env);
}
void CJavaWrapper::ShowWin(int number)
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    env->CallVoidMethod(this->activity, this->s_ShowWin, number);

    EXCEPTION_CHECK(env);
}
void CJavaWrapper::SetCar(int price, char* namecar) 
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
	char utf81[256];
    cp1251_to_utf8(utf81, namecar);

    jstring jtext = env->NewStringUTF(utf81);

    env->CallVoidMethod(this->activity, this->s_SetCar, price, jtext);

	EXCEPTION_CHECK(env);
}
void CJavaWrapper::SetInterface(int type)
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_SetInterface, type);

    EXCEPTION_CHECK(env);
}
void CJavaWrapper::hideAutoSalon() 
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_hideAutoSalon);

	EXCEPTION_CHECK(env);
}
void CJavaWrapper::ShowAd()
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_ShowAd);

    EXCEPTION_CHECK(env);
}
void CJavaWrapper::ShowCases()
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_ShowCases);

    EXCEPTION_CHECK(env);
}
void CJavaWrapper::showAutoSalon() 
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_showAutoSalon);

	EXCEPTION_CHECK(env);
}
void CJavaWrapper::ShowSawmill() 
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_showSawmill);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowRubbish() 
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_showRubbish);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowCollectors() 
{
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_showCollectors);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetLoadingText(int id)
{
	JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_setLoadingText, id);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::showNotification(int type, char* text, int duration, int actionId, char* text2, char* text3) {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

    char utf81[255], utf82[255], utf83[255];
    cp1251_to_utf8(utf81, text);
	cp1251_to_utf8(utf82, text2);
	cp1251_to_utf8(utf83, text3);

    jstring jtext = env->NewStringUTF(utf81);
	jstring jtext2 = env->NewStringUTF(utf82);
	jstring jtext3 = env->NewStringUTF(utf83);

    env->CallVoidMethod(activity, s_showNotification, type, jtext, duration, actionId, jtext2, jtext3);
    env->DeleteLocalRef(jtext);
	env->DeleteLocalRef(jtext2);
	env->DeleteLocalRef(jtext3);
}

void CJavaWrapper::hideNotification() {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_hideNotification);
}

void CJavaWrapper::ShowCaptcha(char* str) {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

	char utf81[256];
    cp1251_to_utf8(utf81, str);

    jstring jtext = env->NewStringUTF(utf81);

    env->CallVoidMethod(activity, s_showCaptcha, jtext);
	env->DeleteLocalRef(jtext);
}

void CJavaWrapper::ShowJobInfo(int progress, int money, char* str) {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

	char utf81[256];
    cp1251_to_utf8(utf81, str);

    jstring jtext = env->NewStringUTF(utf81);

    env->CallVoidMethod(activity, s_showJobInfo, progress, money, jtext);
	env->DeleteLocalRef(jtext);
}

void CJavaWrapper::HideJobInfo() {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_hideJobInfo);
}

void CJavaWrapper::showImageFromByte(long len, uint8_t* array) {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

	jbyteArray jbArray = env->NewByteArray((long)len);
	env->SetByteArrayRegion(jbArray, 0, (long)len, reinterpret_cast<jbyte*>(array));

    env->CallVoidMethod(activity, s_byteArray, jbArray);
	env->DeleteLocalRef(jbArray);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::showBizWar(int time, uint32_t colorLeft, uint32_t colorRight, int kills, int deaths, float damage, float take, char* player, int points1, int points2) {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

	char utf81[256];
    cp1251_to_utf8(utf81, player);

    jstring jtext = env->NewStringUTF(utf81);
    env->CallVoidMethod(activity, s_showBizWar, time, (int)colorLeft, (int)colorRight, kills, deaths, damage, take, jtext, points1, points2);
	env->DeleteLocalRef(jtext);
}

void CJavaWrapper::hideBizWar() {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_hideBizWar);
}

void CJavaWrapper::showPotato() {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_showPotato);
}

void CJavaWrapper::onNativeRendered(int id, jbyteArray array, int sizeX, int sizeY) {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_nativeRendered, id, array, sizeX, sizeY);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowInventory(int pagecount, int slotcount) {
    JNIEnv *env = GetEnv();

     if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_showInventory, pagecount, slotcount);
}

void CJavaWrapper::HideInventory() {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_hideInventory);
}

void CJavaWrapper::SetInventorySkin(int modelId, float x, float y, float z, float zoom) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_setInventorySkin, modelId, x,y,z,zoom);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::UpdateSlotsInfo(int id, int type, int amount, int modelId, float x, float y, float z, float zoom, char* caption, char* info) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	Log("UpdateSlotsInfo 1 %s .:.:. %s", caption, info);

	jstring jtext = env->NewStringUTF(caption);
	jstring jtext1 = env->NewStringUTF(info);

    Log("UpdateSlotsInfo 2 %s .:.:. %s", caption, info);

	env->CallVoidMethod(activity, s_updateSlots, id, type, amount, modelId,x,y,z,zoom, jtext, jtext1);
	env->DeleteLocalRef(jtext);
	env->DeleteLocalRef(jtext1);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetNullSlot(int id) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_setNullSlot, id);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::UpdateSlotsAcsInfo(int acc_slot, int modelId, float x, float y, float z, float zoom, char* caption, char* info) {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

	jstring jtext = env->NewStringUTF(caption);
	jstring jtext1 = env->NewStringUTF(info);

	Log("updateAcsSlotsInfo %s %s", caption, info);

    env->CallVoidMethod(activity, s_updateAcsSlots, acc_slot, modelId,x,y,z,zoom, jtext, jtext1);
	env->DeleteLocalRef(jtext);
	env->DeleteLocalRef(jtext1);
    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetInventoryInfo(int health, int thirst, int hunger, int money, int donate) {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_setInventoryInfo, health, thirst, hunger, money, donate);
    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetUniversalInventoryInfo(int money, int donate, char* inventory1, char* inventory2) {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    char utf8[256], utf81[256];
    cp1251_to_utf8(utf8, inventory1);
    cp1251_to_utf8(utf81, inventory2);

    jstring jtext = env->NewStringUTF(utf8);
    jstring jtext1 = env->NewStringUTF(utf81);

    env->CallVoidMethod(activity, s_setUniversalInventoryInfo, money, donate, jtext, jtext1);
    env->DeleteLocalRef(jtext);
    env->DeleteLocalRef(jtext1);
    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ClearInventory()
{
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_clearInventory);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowInventoryUniversal(int rightpagecount, int rightslotcount, int leftpagecount, int leftslotcount) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_showInventoryUniversal, rightpagecount, rightslotcount, leftpagecount, leftslotcount);
}

void CJavaWrapper::HideInventoryUniversal() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_hideInventoryUniversal);
}

void CJavaWrapper::UpdateSlotsLeftInfo(int typeInventory, int id, int type, int amount, int modelId, float x, float y, float z, float zoom, char* caption, char* info, int price) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	jstring jtext = env->NewStringUTF(caption);
	jstring jtext1 = env->NewStringUTF(info);

	env->CallVoidMethod(activity, s_updateSlotsLeft, typeInventory, id, type, amount, modelId,x,y,z,zoom, jtext, jtext1, price);
	env->DeleteLocalRef(jtext);
	env->DeleteLocalRef(jtext1);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::UpdateSlotsRightInfo(int id, int type, int amount, int modelId, float x, float y, float z, float zoom, char* caption, char* info) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	jstring jtext = env->NewStringUTF(caption);
	jstring jtext1 = env->NewStringUTF(info);

	env->CallVoidMethod(activity, s_updateSlotsRight, id, type, amount, modelId,x,y,z,zoom, jtext, jtext1);
	env->DeleteLocalRef(jtext);
	env->DeleteLocalRef(jtext1);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetNullSlotLeft(int id) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_setNullSlotLeft, id);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetNullSlotRight(int id) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_setNullSlotRight, id);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ClearInventoryLeft()
{
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_clearInventoryLeft);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ClearInventoryRight()
{
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_clearInventoryRight);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowBuyAuto() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_showBuyAuto);
}

void CJavaWrapper::HideBuyAuto() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_hideBuyAuto);
}

void CJavaWrapper::ShowTuning() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_showTuning);
}

void CJavaWrapper::HideTuning() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_hideTuning);
}

void CJavaWrapper::AddCarToRecycler(int modelId, float x, float y, float z, float zoom, int price, int maxSpeed, int maxFuel, float timeTo100, int availabilityInStock, char* name) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	char utf8[256];
	cp1251_to_utf8(utf8, name);

	jstring jtext = env->NewStringUTF(utf8);

	env->CallVoidMethod(activity, s_addCarToRecycler, modelId,x,y,z,zoom, price, maxSpeed, maxFuel, timeTo100, availabilityInStock, jtext);
	env->DeleteLocalRef(jtext);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowVehicleSpawner() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_showVehicleSpawner);
}

void CJavaWrapper::HideVehicleSpawner() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_hideVehicleSpawner);
}

void CJavaWrapper::ShowSkinSpawner() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_showSkinSpawner);
}


void CJavaWrapper::HideSkinSpawner() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_hideSkinSpawner);
}

void CJavaWrapper::ShowTradeInventory(int pagecount, int slotcount) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_showTrade, pagecount, slotcount);
}

void CJavaWrapper::HideTradeInventory() {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_hideTrade);
}
void CJavaWrapper::SetSpeedometerFuel(int percfuel, int fuel)
{
	

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetSpeedometerFuel, percfuel, fuel);

	EXCEPTION_CHECK(env);
}
void CJavaWrapper::UpdateTradeRightSlotsInfo(int id, int type, int amount, int modelId, float x, float y, float z, float zoom, char* caption, char* info, char* amountText) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	Log("UpdateSlotsInfo 1 %s .:.:. %s", caption, info);

	jstring jtext = env->NewStringUTF(caption);
	jstring jtext1 = env->NewStringUTF(info);
	jstring jtext2 = env->NewStringUTF(amountText);

	Log("UpdateSlotsInfo 2 %s .:.:. %s", caption, info);

	env->CallVoidMethod(activity, s_updateTradeRightSlotsInfo, id, type, amount, modelId,x,y,z,zoom, jtext, jtext1, jtext2);
	env->DeleteLocalRef(jtext);
	env->DeleteLocalRef(jtext1);
	env->DeleteLocalRef(jtext2);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::UpdateTradeLeftSlotsInfo(int id, int type, int amount, int modelId, float x, float y, float z, float zoom, char* caption, char* info, char* amountText) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	Log("UpdateSlotsInfo 1 %s .:.:. %s", caption, info);

	jstring jtext = env->NewStringUTF(caption);
	jstring jtext1 = env->NewStringUTF(info);
	jstring jtext2 = env->NewStringUTF(amountText);

	Log("UpdateSlotsInfo 2 %s .:.:. %s", caption, info);

	env->CallVoidMethod(activity, s_updateTradeLeftSlotsInfo, id, type, amount, modelId,x,y,z,zoom, jtext, jtext1, jtext2);
	env->DeleteLocalRef(jtext);
	env->DeleteLocalRef(jtext1);
	env->DeleteLocalRef(jtext2);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ClearTradeRightInventory()
{
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_clearTradeRightSlotsInfo);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetTradeNullRightSlot(int id) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_setNullTradeRightSlotsInfo, id);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetTradeMaxPages(int pages) {
	JNIEnv *env = GetEnv();

	if (!env) {
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_setTradeMaxPages, pages);
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ClearTradeLeftInventory() {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_clearTradeLeftSlotsInfo);
    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetTradePlayerReadiness(int player, int status) {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

	Log("SetTradePlayerReadiness %d %d", player, status);

    env->CallVoidMethod(activity, s_setTradeReadiness, player, status);
    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetTradeMoney(int money1, int money2, char* inventoryName) {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

	Log("SetTradeMoney %d %d", money1, money2);

	jstring jtext = env->NewStringUTF(inventoryName);

    env->CallVoidMethod(activity, s_setTradeMoney, money1, money2, jtext);
	env->DeleteLocalRef(jtext);
    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetTradePlayerName(int player, char* name) {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

	Log("SetTradePlayerName %d %s", player, name);

    jstring jtext = env->NewStringUTF(name);

    env->CallVoidMethod(activity, s_setTradePlayerName, player, jtext);
    env->DeleteLocalRef(jtext);
    EXCEPTION_CHECK(env);
}

#include <GLES2/gl2.h>
extern int g_iStatusDriftChanged;
extern int greenzone;
#include "..//CDebugInfo.h"
#include "graphics/CInventory.h"
#include "graphics/CBuyAuto.h"

extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onInputEnd(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		if (pKeyBoard)
		{
			pKeyBoard->OnNewKeyboardInput(pEnv, thiz, str);
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onEventBackPressed(JNIEnv* pEnv, jobject thiz)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsOpen())
			{
				Log(OBFUSCATE("Closing keyboard"));
				pKeyBoard->Close();
			}
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNativeHeightChanged(JNIEnv* pEnv, jobject thiz, jint orientation, jint height)
	{
		if (pChatWindow)
		{
			pChatWindow->SetLowerBound(height);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeCutoutSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iCutout = b;
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeKeyboardSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iAndroidKeyboard = b;
		}

		if (pKeyBoard && b)
		{
			pKeyBoard->EnableNewKeyboard();
		}
		else if(pKeyBoard)
		{
			pKeyBoard->EnableOldKeyboard();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeFpsCounterSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iFPSCounter = b;
		}

		CDebugInfo::SetDrawFPS(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHpArmourText(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			if (!pSettings->GetWrite().iHPArmourText && b)
			{
				if (CAdjustableHudColors::IsUsingHudColor(HUD_HP_TEXT) == false)
				{
					CAdjustableHudColors::SetHudColorFromRGBA(HUD_HP_TEXT, 255, 0, 0, 255);
				}
				if (CAdjustableHudPosition::GetElementPosition(HUD_HP_TEXT).x == -1 || CAdjustableHudPosition::GetElementPosition(HUD_HP_TEXT).y == -1)
				{
					CAdjustableHudPosition::SetElementPosition(HUD_HP_TEXT, 500, 500);
				}
				if (CAdjustableHudScale::GetElementScale(HUD_HP_TEXT).x == -1 || CAdjustableHudScale::GetElementScale(HUD_HP_TEXT).y == -1)
				{
					CAdjustableHudScale::SetElementScale(HUD_HP_TEXT, 400, 400);
				}

				if (CAdjustableHudColors::IsUsingHudColor(HUD_ARMOR_TEXT) == false)
				{
					CAdjustableHudColors::SetHudColorFromRGBA(HUD_ARMOR_TEXT, 255, 0, 0, 255);
				}
				if (CAdjustableHudPosition::GetElementPosition(HUD_ARMOR_TEXT).x == -1 || CAdjustableHudPosition::GetElementPosition(HUD_ARMOR_TEXT).y == -1)
				{
					CAdjustableHudPosition::SetElementPosition(HUD_ARMOR_TEXT, 300, 500);
				}
				if (CAdjustableHudScale::GetElementScale(HUD_ARMOR_TEXT).x == -1 || CAdjustableHudScale::GetElementScale(HUD_ARMOR_TEXT).y == -1)
				{
					CAdjustableHudScale::SetElementScale(HUD_ARMOR_TEXT, 400, 400);
				}
			}

			pSettings->GetWrite().iHPArmourText = b;
		}

		CInfoBarText::SetEnabled(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iOutfitGuns = b;

			CWeaponsOutFit::SetEnabled(b);
		}
	}

		JNIEXPORT void JNICALL
    	Java_com_nvidia_devtech_NvEventQueueActivity_onKeyboardClose(JNIEnv *pEnv, jobject thiz) {
pKeyBoard->Close();
    															}


	JNIEXPORT void JNICALL
	Java_com_nvidia_devtech_NvEventQueueActivity_responseDialog(JNIEnv *pEnv, jobject thiz,
															jint dialogid, jint response,
															jint listitem, jbyteArray inputtext) {
        jboolean isCopy = true;

        jbyte* pMsg = pEnv->GetByteArrayElements(inputtext, &isCopy);
        jsize length = pEnv->GetArrayLength(inputtext);

        std::string szStr((char*)pMsg, length);

		pNetGame->SendDialogResponse(dialogid, response, listitem, (char*)szStr.c_str());

		if(!CInventory::isShow() && !CBuyAuto::isShow() && showHud) {
            if(pSettings->GetReadOnly().iNewHud)
                g_pJavaWrapper->ShowHUD(true);

            g_pJavaWrapper->CallLauncherActivity(1237);
			g_pJavaWrapper->CallLauncherActivity(1234);
        }

		pEnv->ReleaseByteArrayElements(inputtext, pMsg, JNI_ABORT);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativePcMoney(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iPCMoney = b;
		}

		CGame::SetEnabledPCMoney(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeRadarrect(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iRadarRect = b;

			CRadarRect::SetEnabled(b);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeNameTag(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iNameTag = b;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendCommand(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pMsg, length);

		if(pNetGame) {
			pNetGame->SendChatCommand((char*)szStr.c_str());
		}

		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setUIP(JNIEnv* pEnv, jobject thiz, jbyteArray data)
	{
		jbyte* dataBytes = pEnv->GetByteArrayElements(data, nullptr);
		jsize dataLen = pEnv->GetArrayLength(data);

		char* dataChars = new char[dataLen + 1];
		memcpy(dataChars, dataBytes, dataLen);
		dataChars[dataLen] = '\0';

		//pChatWindow->AddDebugMessage("UIP %s", dataChars);
		pNetGame->SendRPCUIP(dataChars);
		delete[] dataChars;

		pEnv->ReleaseByteArrayElements(data, dataBytes, 0);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setUID(JNIEnv* pEnv, jobject thiz, jbyteArray data)
	{
		jbyte* dataBytes = pEnv->GetByteArrayElements(data, nullptr);
		jsize dataLen = pEnv->GetArrayLength(data);

		char* dataChars = new char[dataLen + 1];
		memcpy(dataChars, dataBytes, dataLen);
		dataChars[dataLen] = '\0';

		//pChatWindow->AddDebugMessage("UID %s", dataChars);
		pNetGame->SendRPCUID(dataChars);
		delete[] dataChars;

		pEnv->ReleaseByteArrayElements(data, dataBytes, 0);
	}


	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNative3DText(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().i3DText = b;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeVoice(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iVoice = b;

		if (pVoice)
		{
			if (b)
			{
				if (pVoice->IsDisconnected())
                    pVoice->Connect(pNetGame->m_szHostOrIp, pNetGame->m_iPort + 100);
			}
			else pVoice->FullDisconnect();
		}
	} 
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeSkyBox(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iSkyBox = b;
	}

        JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeCacheTextDraw(JNIEnv* pEnv, jobject thiz, jboolean b)
        {
            if (pSettings){
                pSettings->GetWrite().iCacheTextDraw = b;
                //pSnapShotHelper->SetActiveCacheTexture(b);
            }

        }
		JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeDialogNew(JNIEnv* pEnv, jobject thiz, jboolean b)
    	{
    		if (pSettings)
    			pSettings->GetWrite().iDialogNew = b;
    	}
		JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeCrossHair(JNIEnv* pEnv, jobject thiz, jboolean b)
    	{
    		if (pSettings)
    			pSettings->GetWrite().iCrossHair = b;
    	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeSkyBox(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iSkyBox;

		return 0;
	}

		JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeDialogNew(JNIEnv* pEnv, jobject thiz)
    	{
    		if (pSettings)
    			return pSettings->GetReadOnly().iDialogNew;

    		return 0;
    	}
		JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeCrossHair(JNIEnv* pEnv, jobject thiz)
    	{
    		if (pSettings)
    			return pSettings->GetReadOnly().iCrossHair;

    		return 0;
    	}

        JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeCacheTextDraw(JNIEnv* pEnv, jobject thiz)
        {
            if (pSettings)
                return pSettings->GetReadOnly().iCacheTextDraw;

            return 0;
        }

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeCutoutSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iCutout;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeKeyboardSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iAndroidKeyboard;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeFpsCounterSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iFPSCounter;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHpArmourText(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iHPArmourText;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iOutfitGuns;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativePcMoney(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iPCMoney;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeRadarrect(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iRadarRect;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNative3DText(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().i3DText;

		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeNameTag(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iNameTag;

		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeVoice(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iVoice;

		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowSave(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			pSettings->Save();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowDefaults(JNIEnv* pEnv, jobject thiz, jint category)
	{
		if (pSettings)
		{
			pSettings->ToDefaults(category);
			if (pChatWindow)
			{
				pChatWindow->m_bPendingReInit = true;
			}
		}
	}

    void DefaultSelectServer(jint b){
		pNetGame = new CNetGame(
				g_sEncryptedAddresses[b].decrypt(),
				g_sEncryptedAddresses[b].getPort(),
				pSettings->GetReadOnly().szNickName,
				pSettings->GetReadOnly().szPassword);
	}
    void OneStageConnect(int b){

		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, OBFUSCATE("accept: application/dns-json"));
		char urlStart[] = "https://1.1.1.1/dns-query?name=";
		char urlRequest[strlen(urlStart)+strlen(g_sEncryptedAddresses[b].decrypt())];
		sprintf(urlRequest, OBFUSCATE("https://1.1.1.1/dns-query?name=%s"), g_sEncryptedAddresses[b].decrypt());
		std::string reqString = curlRequestGet(urlRequest, headers);

		if(strcmp(reqString.c_str(), "") == 0) {
			Log(OBFUSCATE("Not requested dns-query"));
			DefaultSelectServer(b);
			return;
		}

		Json::Value root;
		bool parsingSuccessful = reader.parse( reqString.c_str(), root );

		if(!parsingSuccessful){
			Log(OBFUSCATE("Request dns-query not parsed: %s"), reqString.c_str());
			DefaultSelectServer(b);
			return;
		}

		Json::Value resAnswer = root["Answer"];
		if(resAnswer.size() != 1){
		    Log(OBFUSCATE("Request dns-query Answers count == %d"), resAnswer.size());
			DefaultSelectServer(b);
            return;
		}

		std::string resAnswerNameServer = resAnswer[0]["name"].asString();

		if(strcmp(resAnswerNameServer.c_str(), g_sEncryptedAddresses[b].decrypt()) != 0){
			Log(OBFUSCATE("Request dns-query Answer name != %s server name: %s"),g_sEncryptedAddresses[b].decrypt(), resAnswerNameServer.c_str());
			DefaultSelectServer(b);
			return;
		}

		std::string resAnswerIP = resAnswer[0]["data"].asString();

		if(strcmp(resAnswerIP.c_str(), OBFUSCATE("")) == 0){
			Log(OBFUSCATE("Request dns-query Answer ip == null: %s"), resAnswerNameServer.c_str());
			DefaultSelectServer(b);
			return;
		}

		pNetGame = new CNetGame(
				resAnswerIP.c_str(),
				g_sEncryptedAddresses[b].getPort(),
				pSettings->GetReadOnly().szNickName,
				pSettings->GetReadOnly().szPassword);
    }
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeSelectServer(JNIEnv* pEnv, jobject thiz, jint b)
	{

        if(isActiveSelectedServer){
            return;
        }
        isActiveSelectedServer = true;

		/*if(!FileCheckSum()) {
			g_pJavaWrapper->ShowToastText(OBFUSCATE("� ��� ����������� ���������������� ����� SAMP.\n\n����� ���������� �������� � �����, �������������� ���� �� ��������� ��."));
		    Log(OBFUSCATE("Folder SAMP is modify"));
		}else{
		    Log(OBFUSCATE("Folder SAMP is original"));
		}*/

		/*if(!FileCheckSumWeapon()) {
			g_pJavaWrapper->CallLauncherActivity(123);
	   
	   		pChatWindow->AddDebugMessage(OBFUSCATE("� ��� ����������� ���������������� ����� �������, ����������� � ����!"));
			pChatWindow->AddDebugMessage(OBFUSCATE("� ��� ����������� ���������������� ����� �������, ����������� � ����!"));
			pChatWindow->AddDebugMessage(OBFUSCATE("� ��� ����������� ���������������� ����� �������, ����������� � ����!"));
		    
			Log(OBFUSCATE("Folder SAMP is modify"));
		}
		else 
		{*/
		    Log(OBFUSCATE("Folder SAMP is original"));

			g_pJavaWrapper->CallLauncherActivity(122);

			pNetGame = new CNetGame(
			g_sEncryptedAddresses[b].decrypt(),
			g_sEncryptedAddresses[b].getPort(),
			pSettings->GetReadOnly().szNickName,
			pSettings->GetReadOnly().szPassword);
		//}


        //std::thread connect_thread(OneStageConnect, (int)b);
       // connect_thread.detach();

	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementColor(JNIEnv* pEnv, jobject thiz, jint id, jint a, jint r, jint g, jint b)
	{
		CAdjustableHudColors::SetHudColorFromRGBA((E_HUD_ELEMENT)id, r, g, b, a);
	}

	JNIEXPORT void JNICALL
	Java_com_stage_core_ui_VehicleSpawner_sendCarId(JNIEnv *env, jobject thiz, jint vehicleid) {
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x101);
		bsSend.Write((uint16_t)vehicleid);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}

	JNIEXPORT void JNICALL
	Java_com_stage_core_ui_SkinSpawner_sendSkinId(JNIEnv *env, jobject thiz, jint skin) {
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x102);
		bsSend.Write((uint16_t)skin);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}

	JNIEXPORT void JNICALL
	Java_com_stage_core_ui_Collectors_sendRPCCollectors(JNIEnv *env, jobject thiz) {
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x71);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}

	JNIEXPORT jbyteArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementColor(JNIEnv* pEnv, jobject thiz, jint id)
	{
		char pTemp[9];
		jbyteArray color = pEnv->NewByteArray(sizeof(pTemp));

		if (!color)
		{
			return nullptr;
		}

		pEnv->SetByteArrayRegion(color, 0, sizeof(pTemp), (const jbyte*)CAdjustableHudColors::GetHudColorString((E_HUD_ELEMENT)id).c_str());

		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementPosition(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
	    if(id == -1){
	        if (pSettings){
            	pSettings->GetWrite().fKillListPosX = x;
                pSettings->GetWrite().fKillListPosY = y;
                pKillList->UpdateValuesRender();
                return;
            }
	    }
		if (id == 7)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fChatPosX = x;
				pSettings->GetWrite().fChatPosY = y;
				if (pChatWindow)
				{
					pChatWindow->m_bPendingReInit = true;
				}
				return;
			}
			return;
		}
		if (id == HUD_SNOW)
		{
			if (pSettings)
			{
				pSettings->GetWrite().iSnow = x;
			}
			CSnow::SetCurrentSnow(pSettings->GetReadOnly().iSnow);
			return;
		}
		CAdjustableHudPosition::SetElementPosition((E_HUD_ELEMENT)id, x, y);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
	    if(id == -1){
	    if (pSettings){
            pSettings->GetWrite().fKillListScale = x;
            pKillList->UpdateValuesRender();
            }
	        return;
	    }
		CAdjustableHudScale::SetElementScale((E_HUD_ELEMENT)id, x, y);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}


	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(2);

		if (!color)
		{
			return nullptr;
		}
		int arr[2];
		if(id == -1){
		if (pSettings){
            arr[0] = pSettings->GetReadOnly().fKillListScale;
            }else{
            arr[0] = 0;
            }
            arr[1] = 0;
        }else{
		    arr[0] = CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)id).x;
		    arr[1] = CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)id).y;
		}
		pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)& arr[0]);

		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeWidgetPositionAndScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y, jint scale)
	{
		if (id == 0)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonMicrophoneX = x;
				pSettings->GetWrite().fButtonMicrophoneY = y;
				pSettings->GetWrite().fButtonMicrophoneSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_MICROPHONE))
				{
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetWidth(scale);
				}
			}
		}
		
		if (id == 1)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonEnterPassengerX = x;
				pSettings->GetWrite().fButtonEnterPassengerY = y;
				pSettings->GetWrite().fButtonEnterPassengerSize = scale;
			}
		}

		if (id == 2)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonCameraCycleX = x;
				pSettings->GetWrite().fButtonCameraCycleY = y;
				pSettings->GetWrite().fButtonCameraCycleSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
				{
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetWidth(scale);
				}
			}
		}

		if (id == 3)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonPassengerCycleX = x;
				pSettings->GetWrite().fButtonPassengerCycleY = y;
				pSettings->GetWrite().fButtonPassengerCycleSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_PASSENGER))
				{
					g_pWidgetManager->GetWidget(WIDGET_PASSENGER)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_PASSENGER)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_PASSENGER)->SetWidth(scale);
				}
			}
		}
	}


	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeKillListPosition(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(3);
		if(!color){
			return nullptr;
		}
		int arr[3];
		if(pSettings){
			if(id == 16){
				arr[0] = pSettings->GetReadOnly().fKillListPosX;
				arr[1] = pSettings->GetReadOnly().fKillListPosY;
				arr[2] = pSettings->GetReadOnly().fKillListScale;

				pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)&arr[0]);

				return color;
			}
		}

	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementPosition(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(2);

		if (!color)
		{
			return nullptr;
		}
		int arr[2];

		if (id == 7 && pSettings)
		{
			arr[0] = pSettings->GetReadOnly().fChatPosX;
			arr[1] = pSettings->GetReadOnly().fChatPosY;
		}
		else if (id == HUD_SNOW && pSettings)
		{
			arr[0] = CSnow::GetCurrentSnow();
			arr[1] = CSnow::GetCurrentSnow();
		}else if(id == -1){
		    arr[0] = pSettings->GetReadOnly().fKillListPosX;
        			arr[1] = pSettings->GetReadOnly().fKillListPosY;
		}else
		{
			arr[0] = CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)id).x;
			arr[1] = CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)id).y;
		}

		pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)&arr[0]);

		return color;
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeWidgetPositionAndScale(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(3);

		if (!color)
		{
			return nullptr;
		}
		int arr[3] = { -1, -1, -1 };
		

		if (pSettings)
		{
			if (id == 0)
			{
				arr[0] = pSettings->GetWrite().fButtonMicrophoneX;
				arr[1] = pSettings->GetWrite().fButtonMicrophoneY;
				arr[2] = pSettings->GetWrite().fButtonMicrophoneSize;
			}
			if (id == 1)
			{
				arr[0] = pSettings->GetWrite().fButtonEnterPassengerX;
				arr[1] = pSettings->GetWrite().fButtonEnterPassengerY;
				arr[2] = pSettings->GetWrite().fButtonEnterPassengerSize;
			}
			if (id == 2)
			{
				arr[0] = pSettings->GetWrite().fButtonCameraCycleX;
				arr[1] = pSettings->GetWrite().fButtonCameraCycleY;
				arr[2] = pSettings->GetWrite().fButtonCameraCycleSize;
			}
			if (id == 3)
			{
				arr[0] = pSettings->GetWrite().fButtonPassengerCycleX;
				arr[1] = pSettings->GetWrite().fButtonPassengerCycleY;
				arr[2] = pSettings->GetWrite().fButtonPassengerCycleSize;
			}
		}
		

		pEnv->SetIntArrayRegion(color, 0, 3, (const jint*)& arr[0]);

		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendClick(JNIEnv *env, jobject thiz, jstring inputtext)
	{
		if (pNetGame && pGame)
        {
            pGame->FindPlayerPed()->TogglePlayerControllable(true);
            pNetGame->SendChatCommand((char*)jstring2string(g_pJavaWrapper->GetEnv(), inputtext).c_str());
        }
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onTabClose(JNIEnv *env, jobject thiz, jint questId)
	{
		g_pJavaWrapper->isGlobalShowTab = false;

		pGame->ToggleHUDElement(HUD_ELEMENT_CHAT, true);
		pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, true);
		pGame->ToggleHUDElement(HUD_ELEMENT_MAP, true);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeNewHud(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iNewHud = b;

		*(uint8_t*)(SA_ADDR(0x7165E8)) = !b;

		if (b)
			g_pJavaWrapper->ShowHUD(true);
		else g_pJavaWrapper->HideHUD(true);

		//g_pJavaWrapper->isGlobalShowHUD = b;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeNewHud(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iNewHud;

		return 0;
	}

	/*JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_switchWeapon(JNIEnv* pEnv, jobject thiz)
	{
		auto pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
			pPlayerPed->SwitchWeapon();
	}*/

	JNIEXPORT void JNICALL Java_com_stage_core_ui_notification_Notification_sendClick(JNIEnv *env, jobject thiz, jint action, jint button) {
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x91);
		bsSend.Write((uint16_t)action);
		bsSend.Write((uint8_t)button);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}

	JNIEXPORT void JNICALL Java_com_stage_core_ui_Speedometer_sendClick(JNIEnv* pEnv, jobject thiz, jint id)
	{
		switch(id)
		{
			case 1:
			{
				CPlayerPed *pPlayerPed = CPlayerPool::GetLocalPlayer()->GetPlayerPed();
				CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

				if(pVehicle->m_iTurnState == eTurnState::TURN_LEFT)
				{
					pVehicle->bIsOnLeftPovorotnik = true;
					pVehicle->m_iTurnState = eTurnState::TURN_OFF;
				}
				else {
					pVehicle->bIsOnLeftPovorotnik = false;
					pVehicle->m_iTurnState = eTurnState::TURN_LEFT;
				}
				//g_pJavaWrapper->UpdateSpeedometerTurnlights(pVehicle->m_iTurnState);
				break;
			}
			case 2:
			{
				CPlayerPed *pPlayerPed = CPlayerPool::GetLocalPlayer()->GetPlayerPed();
				CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

				if(pVehicle->m_iTurnState == eTurnState::TURN_RIGHT)
				{
					pVehicle->bIsOnRightPovorotnik = true;
					pVehicle->m_iTurnState = eTurnState::TURN_OFF;
				}
				else {
					pVehicle->bIsOnRightPovorotnik = false;
					pVehicle->m_iTurnState = eTurnState::TURN_RIGHT;
				}
				//g_pJavaWrapper->UpdateSpeedometerTurnlights(pVehicle->m_iTurnState);
				break;
			}
			case 3:
			{
				CPlayerPed *pPlayerPed = CPlayerPool::GetLocalPlayer()->GetPlayerPed();
				CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

				if(pVehicle->m_iTurnState == eTurnState::TURN_ALL)
				{
					pVehicle->bIsOnLeftPovorotnik = true;
					pVehicle->bIsOnRightPovorotnik = true;
					pVehicle->bIsOnAvariyka = true;
					pVehicle->m_iTurnState = eTurnState::TURN_OFF;
				}
				else {
					pVehicle->bIsOnLeftPovorotnik = false;
					pVehicle->bIsOnRightPovorotnik = false;
					pVehicle->bIsOnAvariyka = false;
					pVehicle->m_iTurnState = eTurnState::TURN_ALL;
				}
				//g_pJavaWrapper->UpdateSpeedometerTurnlights(pVehicle->m_iTurnState);
				break;
			}
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_hideCaptcha(JNIEnv *env, jobject thiz, jbyteArray captcha)
	{
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x67);

		jboolean isCopy;
		jbyte* pMsg = env->GetByteArrayElements(captcha, &isCopy);
		jsize length = env->GetArrayLength(captcha);

		std::string szStr((char*)pMsg, length);
		Log("str: %s, len: %d", szStr.c_str(), length);
		if(length != 0)
		{
			bsSend.Write((bool)false);
			bsSend.Write((uint32_t)length);
			bsSend.Write((char*)szStr.c_str(), (uint32_t)length);
		}
		else
		{
			bsSend.Write((bool)true);
		}
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
		env->ReleaseByteArrayElements(captcha, pMsg, JNI_ABORT);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_hideMine(JNIEnv *env, jobject thiz)
	{
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x68);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_hideSawmill(JNIEnv *env, jobject thiz, jint id)
	{
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x69);
		//bsSend.Write((uint8_t)id);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_togglePlayer(JNIEnv* pEnv, jobject thiz, jint toggle) {
        if(toggle)
            pGame->FindPlayerPed()->TogglePlayerControllable(false);
        else
            pGame->FindPlayerPed()->TogglePlayerControllable(true);
    }
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_hideRubbish(JNIEnv *env, jobject thiz)
	{
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x70);
		//bsSend.Write((uint8_t)id);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_hideCollectors(JNIEnv *env, jobject thiz)
	{
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x71);
		//bsSend.Write((uint8_t)id);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_hidePotato(JNIEnv *env, jobject thiz)
	{
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)251);
		bsSend.Write((uint32_t)0x73);
		//bsSend.Write((uint8_t)id);
		pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
	}
}

CJavaWrapper::CJavaWrapper(JNIEnv* env, jobject activity)
{
	this->activity = env->NewGlobalRef(activity);

	jclass nvEventClass = env->GetObjectClass(activity);
	if (!nvEventClass)
	{
		Log(OBFUSCATE("nvEventClass null"));
		return;
	}

	s_CallLauncherActivity = env->GetMethodID(nvEventClass, OBFUSCATE("callLauncherActivity"), OBFUSCATE("(I)V"));

	s_GetClipboardText = env->GetMethodID(nvEventClass, OBFUSCATE("getClipboardText"), OBFUSCATE("()[B"));

	s_ShowInputLayout = env->GetMethodID(nvEventClass, OBFUSCATE("showInputLayout"), OBFUSCATE("()V"));
	s_HideInputLayout = env->GetMethodID(nvEventClass, OBFUSCATE("hideInputLayout"), OBFUSCATE("()V"));

	s_ShowClientSettings = env->GetMethodID(nvEventClass, OBFUSCATE("showClientSettings"), OBFUSCATE("()V"));
	s_SetUseFullScreen = env->GetMethodID(nvEventClass, OBFUSCATE("setUseFullscreen"), OBFUSCATE("(I)V"));

	s_BuildDialog = env->GetMethodID(nvEventClass, OBFUSCATE("BuildDialog"), OBFUSCATE("(I[B[B[B[BI)V"));
    s_ToggleRender = env->GetMethodID(nvEventClass, OBFUSCATE("ToggleRender"), OBFUSCATE("(I)V"));
	s_SetVisibleDialog = env->GetMethodID(nvEventClass, OBFUSCATE("SetVisibleDialog"), OBFUSCATE("(I)V"));


    s_SetVisibleKeyboardStandard = env->GetMethodID(nvEventClass, OBFUSCATE("SetVisibleKeyboardStandard"), OBFUSCATE("(II)V"));

	s_ShowSpeedometer = env->GetMethodID(nvEventClass, "showSpeedometer", "(Z)V");
	s_HideSpeedometer = env->GetMethodID(nvEventClass, "hideSpeedometer", "(Z)V");

	s_SetSpeedometerSpeed = env->GetMethodID(nvEventClass, "setSpeedometerSpeed", "(I)V");
	s_SetSpeedometerMileage = env->GetMethodID(nvEventClass, "setSpeedometerMileage", "(I)V");
	s_SetSpeedometerCarHP = env->GetMethodID(nvEventClass, "setSpeedometerCarHP", "(I)V");
	s_SetSpeedometerCarTurnlights = env->GetMethodID(nvEventClass, "updateTurnlights", "(I)V");

	s_SetSpeedometerEngine = env->GetMethodID(nvEventClass, "setEngineState", "(I)V");
	s_SetSpeedometerLight = env->GetMethodID(nvEventClass, "setLightState", "(I)V");
	s_SetSpeedometerBelt = env->GetMethodID(nvEventClass, "setBeltState", "(I)V");
	s_SetSpeedometerLock = env->GetMethodID(nvEventClass, "setLockState", "(I)V");

	s_ExitGame = env->GetMethodID(nvEventClass, "exitGame", "()V");

	s_ShowTab = env->GetMethodID(nvEventClass, "showTabWindow", "(Z)V");
	s_HideTab = env->GetMethodID(nvEventClass, "hideTabWindow", "()V");
	s_ClearTabStat = env->GetMethodID(nvEventClass, "clearTabStat", "()V");
	s_SetTabStat = env->GetMethodID(nvEventClass, "setTabStat", "(I[BII)V");

	s_ShowHUD = env->GetMethodID(nvEventClass, "showHud", "(Z)V");
	s_ShowLogo = env->GetMethodID(nvEventClass, "showLogo", "(Z)V");
	s_HideHUD = env->GetMethodID(nvEventClass, "hideHud", "(Z)V");
	s_SetMoney = env->GetMethodID(nvEventClass, "setMoney", "(I)V");
	s_SetHP = env->GetMethodID(nvEventClass, "setHP", "(I)V");
	s_SetArmour = env->GetMethodID(nvEventClass, "setArmour", "(I)V");
	s_SetEat = env->GetMethodID(nvEventClass, "setEat", "(I)V");
	s_SetAmmo1 = env->GetMethodID(nvEventClass, "setAmmo1", "(I)V");
	s_SetAmmo2 = env->GetMethodID(nvEventClass, "setAmmo2", "(I)V");
	s_SetWantedLevel = env->GetMethodID(nvEventClass, "setWantedLevel", "(I)V");
	s_SetOnline = env->GetMethodID(nvEventClass, "setOnline", "(I)V");
	s_SetX2 = env->GetMethodID(nvEventClass, "setX2", "(Z)V");
	s_UpdateTime = env->GetMethodID(nvEventClass, "updateTime", "()V");
	s_UpdateHudIcon = env->GetMethodID(nvEventClass, "updateHudIcon", "(I)V");
	s_UpdateHud = env->GetMethodID(nvEventClass, "updateHud", "()V");
	s_SetHudOnline= env->GetMethodID(nvEventClass, "setHudOnline", "(I)V");
	s_setServerID = env->GetMethodID(nvEventClass, "setServerID", "(I)V");
	s_setServerInfo = env->GetMethodID(nvEventClass, "setServerInfo", "(ILjava/lang/String;)V");
	s_showNotification = env->GetMethodID(nvEventClass, "showNotification", "(ILjava/lang/String;IILjava/lang/String;Ljava/lang/String;)V");
	s_hideNotification = env->GetMethodID(nvEventClass, "hideNotification", "()V");

	s_showCaptcha = env->GetMethodID(nvEventClass, "showCaptcha", "(Ljava/lang/String;)V");
	
	s_showMine = env->GetMethodID(nvEventClass, "showMine", "()V");

    s_SetInterface = env->GetMethodID(nvEventClass, "SetInterface", "(I)V");
    s_ShowWin = env->GetMethodID(nvEventClass, "ShowWin", "(I)V");
    s_SetFuel = env->GetMethodID(nvEventClass, "SetFuel", "(I)V");
	s_SetCar = env->GetMethodID(nvEventClass, "SetCar", "(ILjava/lang/String;)V");
	s_showAutoSalon = env->GetMethodID(nvEventClass, "showAutoSalon", "()V");
    s_ShowAd = env->GetMethodID(nvEventClass, "ShowAd", "()V");
    s_ShowCases = env->GetMethodID(nvEventClass, "ShowCases", "()V");

    s_hideAutoSalon = env->GetMethodID(nvEventClass, "hideAutoSalon", "()V");
	s_showSawmill = env->GetMethodID(nvEventClass, "showSawmill", "()V");

	s_showRubbish = env->GetMethodID(nvEventClass, "showRubbish", "()V");

	s_showCollectors = env->GetMethodID(nvEventClass, "showCollectors", "()V");

	s_setLoadingText = env->GetMethodID(nvEventClass, "setLoadingText", "(I)V");

	s_showJobInfo = env->GetMethodID(nvEventClass, "showJobInfo", "(IILjava/lang/String;)V");
	s_hideJobInfo = env->GetMethodID(nvEventClass, "hideJobInfo", "()V");

	s_byteArray = env->GetMethodID(nvEventClass, "showImageFromByte", "([B)V");

	s_showBizWar = env->GetMethodID(nvEventClass, "showBizWar", "(IIIIIFFLjava/lang/String;II)V");
	s_hideBizWar = env->GetMethodID(nvEventClass, "hideBizWar", "()V");

	s_showPotato = env->GetMethodID(nvEventClass, "showPotato", "()V");

	s_nativeRendered = env->GetMethodID(nvEventClass, "onNativeRendered", "(I[BII)V");

	s_showInventory = env->GetMethodID(nvEventClass, "showInventory", "(II)V");
    s_hideInventory = env->GetMethodID(nvEventClass, "hideInventory", "()V");
	s_setInventorySkin = env->GetMethodID(nvEventClass, "setInventorySkin", "(IFFFF)V");
	s_updateSlots = env->GetMethodID(nvEventClass, "updateSlotsInfo", "(IIIIFFFFLjava/lang/String;Ljava/lang/String;)V");
	s_setNullSlot = env->GetMethodID(nvEventClass, "setNullSlotInfo", "(I)V");
	s_updateAcsSlots = env->GetMethodID(nvEventClass, "updateSlotsAcsInfo", "(IIFFFFLjava/lang/String;Ljava/lang/String;)V");
    s_setInventoryInfo = env->GetMethodID(nvEventClass, "setInventoryInfo", "(IIIII)V");
	s_clearInventory = env->GetMethodID(nvEventClass, "clearInventory", "()V");

	s_showInventoryUniversal = env->GetMethodID(nvEventClass, "showUniversalInventory", "(IIII)V");
	s_hideInventoryUniversal = env->GetMethodID(nvEventClass, "hideUniversalInventory", "()V");
    s_setUniversalInventoryInfo = env->GetMethodID(nvEventClass, "setUniversalInventoryInfo", "(IILjava/lang/String;Ljava/lang/String;)V");
	s_updateSlotsLeft = env->GetMethodID(nvEventClass, "updateLeftSlotsInfo", "(IIIIIFFFFLjava/lang/String;Ljava/lang/String;I)V");
	s_updateSlotsRight = env->GetMethodID(nvEventClass, "updateRightSlotsInfo", "(IIIIFFFFLjava/lang/String;Ljava/lang/String;)V");
	s_setNullSlotLeft = env->GetMethodID(nvEventClass, "setNullSlotInfoLeft", "(I)V");
	s_setNullSlotRight = env->GetMethodID(nvEventClass, "setNullSlotInfoRight", "(I)V");
	s_clearInventoryLeft = env->GetMethodID(nvEventClass, "clearLeftInventory", "()V");
	s_clearInventoryRight = env->GetMethodID(nvEventClass, "clearRightInventory", "()V");

	s_showBuyAuto = env->GetMethodID(nvEventClass, "showBuyAuto", "()V");
	s_hideBuyAuto = env->GetMethodID(nvEventClass, "hideBuyAuto", "()V");
	s_addCarToRecycler = env->GetMethodID(nvEventClass, "addCarToRecycler", "(IFFFFIIIFILjava/lang/String;)V");

	s_showTuning = env->GetMethodID(nvEventClass, "showTuning", "()V");
	s_hideTuning = env->GetMethodID(nvEventClass, "hideTuning", "()V");
	s_SetSpeedometerFuel = env->GetMethodID(nvEventClass, "setSpeedometerFuel", "(II)V");
	s_showTrade = env->GetMethodID(nvEventClass, "showInventoryTrade", "(II)V");
	s_hideTrade = env->GetMethodID(nvEventClass, "hideInventoryTrade", "()V");
	s_updateTradeRightSlotsInfo = env->GetMethodID(nvEventClass, "updateRightSlotsInfoTrade", "(IIIIFFFFLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	s_updateTradeLeftSlotsInfo = env->GetMethodID(nvEventClass, "updateLeftSlotsInfoTrade", "(IIIIFFFFLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	s_setNullTradeRightSlotsInfo = env->GetMethodID(nvEventClass, "setRightNullSlotInfoTrade", "(I)V");
	s_clearTradeRightSlotsInfo = env->GetMethodID(nvEventClass, "clearRightInventoryTrade", "()V");
	s_setTradeMaxPages = env->GetMethodID(nvEventClass, "setTradeMaxPages", "(I)V");
	s_clearTradeLeftSlotsInfo = env->GetMethodID(nvEventClass, "clearLeftInventoryTrade", "()V");
    s_setTradeReadiness = env->GetMethodID(nvEventClass, "setTradeReadiness", "(II)V");
    s_setTradePlayerName = env->GetMethodID(nvEventClass, "setTradePlayerName", "(ILjava/lang/String;)V");
    s_setTradeMoney = env->GetMethodID(nvEventClass, "setTradeMoney", "(IILjava/lang/String;)V");

	s_showVehicleSpawner = env->GetMethodID(nvEventClass, "showVehicleSpawner", "()V");
	s_hideVehicleSpawner = env->GetMethodID(nvEventClass, "hideVehicleSpawner", "()V");
	s_showSkinSpawner = env->GetMethodID(nvEventClass, "showSkinSpawner", "()V");
	s_hideSkinSpawner = env->GetMethodID(nvEventClass, "hideSkinSpawner", "()V");

    env->DeleteLocalRef(nvEventClass);

	isGlobalShowTab = false;

	//isGlobalShowHUD = false;
	isLocalShowHUD = false;
}

CJavaWrapper::~CJavaWrapper()
{
	JNIEnv* pEnv = GetEnv();
	if (pEnv)
	{
		pEnv->DeleteGlobalRef(this->activity);
	}
}

CJavaWrapper* g_pJavaWrapper = nullptr;