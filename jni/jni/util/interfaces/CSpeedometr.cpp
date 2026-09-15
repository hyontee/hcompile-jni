#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CSpeedometr.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CSpeedometr::activity;

void CSpeedometr::ShowSpeedometr()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showSpeedometr"), OBFUSCATE("()V"));
   	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CSpeedometr::HideSpeedometr(bool b)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideSpeedometr"), OBFUSCATE("(Z)V"));
   	env->CallVoidMethod(activity, method, b);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CSpeedometr::UpdateDataSpeedometr(int Speed, int Fuel, int Health, int Milleage, bool AlarmStatus, int Lights, int Engine, int Doors)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateDataSpeedometr"), OBFUSCATE("(IIIIZIII)V"));
   	env->CallVoidMethod(activity, method, Speed, Fuel, Health, Milleage, AlarmStatus, Lights, Engine, Doors);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

uint32_t CSpeedometr::CustomTickRate;
extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_speedometr_SpeedometrManager_onSpeedometrOpened(JNIEnv* pEnv, jobject thiz)
	{
		if (GetTickCount() - CSpeedometr::CustomTickRate > 500)
		{
			CSpeedometr::CustomTickRate = GetTickCount();

			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_ModePlayer);
				
			bsParams.Write((uint8_t)1);
			pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		}
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_speedometr_SpeedometrManager_onSpeedometrClicked(JNIEnv* pEnv, jobject thiz)
	{
		if (GetTickCount() - CSpeedometr::CustomTickRate > 500)
		{
			CSpeedometr::CustomTickRate = GetTickCount();

			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_ModePlayer);
				
			bsParams.Write((uint8_t)1);
			pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		}
	}
}

void CSpeedometr::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CSpeedometr class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CSpeedometr done!"));
}