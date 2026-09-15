#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CBoats.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CBoats::activity;

void CBoats::boatsShow(int i)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("boatsShow"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, i);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CBoats::boatsHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("boatsHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CBoats::boatsColor(int oldcolor, uint32_t cost, int type, int modelId)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("boatsColor"), OBFUSCATE("(IIII)V"));
	env->CallVoidMethod(activity, method, oldcolor, cost, type, modelId);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CBoats::boatsUpgrade(int modelId, int type, 
								int lvl, int lvl2, int lvl3, int lvl4, int lvl5, int lvl6,
								int32_t cost, int32_t cost2, int32_t cost3, int32_t cost4, int32_t cost5, int32_t cost6,
								int stats, int stats2, int stats3, int stats4)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("boatsUpgrade"), OBFUSCATE("(IIIIIIIIIIIIIIIIII)V"));
	env->CallVoidMethod(activity, method, modelId, type, 
											lvl, lvl2, lvl3, lvl4, lvl5, lvl6,
											cost, cost2, cost3, cost4, cost5, cost6,
											stats, stats2, stats3, stats4);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}
							 
void CBoats::boatsRent(int id, int typeVehicle, int speed, uint32_t cost, int type, int modelId)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("boatsRent"), OBFUSCATE("(IIIIII)V"));
	env->CallVoidMethod(activity, method, id, typeVehicle, speed, cost, type, modelId);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}
							 
void CBoats::boatsSelect(uint32_t id, char* mes, int speed, bool isLoaded, int type, int modelId)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("boatsSelect"), OBFUSCATE("(I[BIZII)V"));
	env->CallVoidMethod(activity, method, id, bytes, speed, isLoaded, type, modelId);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_boats_BoatsManager_returnBoats(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		int32_t _i = i;
		int32_t _i2 = i2;
		int32_t _i3 = i3;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_BoatsManager);

		bsParams.Write(_i);
		bsParams.Write(_i2);
		bsParams.Write(_i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CBoats::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CBoats class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CBoats done!"));
}