#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "../game/CRadarRect.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//scoreboard.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CHud.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CScoreBoard* pScoreBoard;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CHud::activity;

void CHud::ShowHud()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showHud"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CHud::HideHud()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}
		
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideHud"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CHud::UpdateDataHud(int health, int armour, int weaponid, int ammo, int ammoclip, int money, int wanted) {
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateHudInfo"), OBFUSCATE("(IIIIIII)V"));
	env->CallVoidMethod(activity, method, health, armour, weaponid, ammo, ammoclip, money, wanted);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

/*void CHud::UpdateDataHud(int iHP, int iArmour, int iHunger, int iWanted, int iWeapon, int iAllAmmo, int iAmmo, int iMoney, int iEventCoins, int iDonate, bool bEventUsed, bool bGZ, bool bE2y)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateDataHud"), OBFUSCATE("(IIIIIIIIIIZZZ)V"));
	env->CallVoidMethod(activity, method, iHP, iArmour, iHunger, iWanted, iWeapon, iAllAmmo, iAmmo, iMoney, iEventCoins, iDonate, bEventUsed, bGZ, bE2y);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}*/

void CHud::hudButton(int i, int i2, int i3, int i4, int i5, int i6, int i7)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}
	
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hudButton"), OBFUSCATE("(IIIIIII)V"));
	env->CallVoidMethod(activity, method, i, i2, i3, i4, i5, i6, i7);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CHud::buttonExtra(bool bVisible, bool bExtra, char* mes, int icon)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(mes));
    env->SetByteArrayRegion(bytes, 0, strlen(mes), (jbyte*)mes);
	
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("buttonExtra"), OBFUSCATE("(ZZ[BI)V"));
	env->CallVoidMethod(activity, method, bVisible, bExtra, bytes, icon);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CHud::giftLayout(int iconID, char* mes)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(mes));
    env->SetByteArrayRegion(bytes, 0, strlen(mes), (jbyte*)mes);
	
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("giftLayout"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, iconID, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CHud::giftHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}
		
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("giftHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CHud::questShow(char* questCaption, char* questText, char* questButton, bool buttonCancel, bool showQuest)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(questCaption));
	env->SetByteArrayRegion(bytes, 0, strlen(questCaption), (jbyte*)questCaption);

   	jbyteArray bytes2 = env->NewByteArray(strlen(questText));
	env->SetByteArrayRegion(bytes2, 0, strlen(questText), (jbyte*)questText);

   	jbyteArray bytes3 = env->NewByteArray(strlen(questButton));
	env->SetByteArrayRegion(bytes3, 0, strlen(questButton), (jbyte*)questButton);
	
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("questShow"), OBFUSCATE("([B[B[BZZ)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, buttonCancel, showQuest);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CHud::questHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("questHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_hud_HudManager_init(JNIEnv *env, jobject thiz)
	{
		Log("activity true");
		CHud::activity = env->NewGlobalRef(thiz);
	}
}

CRect radarPosition{ 0, 0, 0, 0 };
static uint8_t g_pPixelsToBuff[512 * 512 * sizeof(RwRGBA)];
int SendHudCommand = GetTickCount();
extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_hud_HudManager_sendResponse(JNIEnv* pEnv, jobject thiz, jint i)
	{
		if (GetTickCount() - SendHudCommand > 500)
		{
			SendHudCommand = GetTickCount();
			
			if (pScoreBoard->GetToggle())
			{
				pScoreBoard->Toggle();
				return;
			}
			
			if (i == 0) pNetGame->SendChatCommand("/donate");
			else if (i == 1) pNetGame->SendChatCommand("/340340changegun___fdje");
			else if (i == 2)
			{
				char buffer[32] = "";
				sprintf(buffer, "/hud_cmd_%d", 2);
				pNetGame->SendChatCommand(buffer);
			}
			else if (i == 3)
			{
				char buffer[32] = "";
				sprintf(buffer, "/hud_cmd_%d", 3);
				pNetGame->SendChatCommand(buffer);
			}
			else if (i == 10)
			{
				char buffer[32] = "";
				sprintf(buffer, "/hud_cmd_%d", 10);
				pNetGame->SendChatCommand(buffer);
			}
			else if (i == 11)
			{
				char buffer[32] = "/mm";
				pNetGame->SendChatCommand(buffer);
			}
			else if (i == 12)
			{
				//char buffer[32] = "/inv";
				char buffer[32] = "/gps"; //нето
				pNetGame->SendChatCommand(buffer);
			}
			else if (i == 13)
			{
				//char buffer[32] = "/quest";
				//char buffer[32] = "/hud_cmd_10";
				char buffer[32] = "/gps";
				pNetGame->SendChatCommand(buffer);
			}
			else if (i == 14)
			{
				char buffer[32] = "/inv";
				pNetGame->SendChatCommand(buffer);
			}

			else
			{
				char buffer[32] = "";
				sprintf(buffer, "/hud_cmd_%d", i);
				pNetGame->SendChatCommand(buffer);
			}
		}
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_hud_HudManager_setRadarSizes(JNIEnv* pEnv, jobject thiz, jfloat left, jfloat bottom, jfloat right, jfloat top)
	{
		radarPosition.left = left;
		radarPosition.bottom = bottom;
		radarPosition.right = right;
		radarPosition.top = top;
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_hud_NewHudManager_setRadarSizes(JNIEnv* pEnv, jobject thiz, jfloat left, jfloat bottom, jfloat right, jfloat top)
	{
		radarPosition.left = left;
		radarPosition.bottom = bottom;
		radarPosition.right = right;
		radarPosition.top = top;
		
		CRadarRect::ChangeTextures(0, "radardisc_new");
	}
}

void CHud::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CHud class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CHud done!"));
}