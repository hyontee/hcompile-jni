#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CE2y.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CE2y::activity;

void CE2y::e2yShow(int32_t iBalance, char* date)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(date));
	env->SetByteArrayRegion(bytes, 0, strlen(date), (jbyte*)date);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("e2yShow"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, iBalance, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CE2y::e2yHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("e2yHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CE2y::e2yShopItem(int modelId, int renderType, int color1, int color2, int type, int price, char* name)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(name));
	env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("e2yShopItem"), OBFUSCATE("(IIIIII[B)V"));
	env->CallVoidMethod(activity, method, modelId, renderType, color1, color2, type, price, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CE2y::e2yRouletteItem(int modelId, int renderType, int color1, int color2, int type, int price, char* name)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(name));
	env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("e2yRouletteItem"), OBFUSCATE("(IIIIII[B)V"));
	env->CallVoidMethod(activity, method, modelId, renderType, color1, color2, type, price, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CE2y::e2yPaymentOkay(int modelId)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("e2yPaymentOkay"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, modelId);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CE2y::e2yTopPlayer(int position, char* name, bool prem_status, uint32_t count)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(name));
	env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("e2yTopPlayer"), OBFUSCATE("(I[BZI)V"));
	env->CallVoidMethod(activity, method, position, bytes, prem_status, count);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CE2y::e2yQuest(int position, int number, char* mes, char* mes2, char* mes3)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(mes));
	env->SetByteArrayRegion(bytes, 0, strlen(mes), (jbyte*)mes);

   	jbyteArray bytes2 = env->NewByteArray(strlen(mes2));
	env->SetByteArrayRegion(bytes2, 0, strlen(mes2), (jbyte*)mes2);

   	jbyteArray bytes3 = env->NewByteArray(strlen(mes3));
	env->SetByteArrayRegion(bytes3, 0, strlen(mes3), (jbyte*)mes3);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("e2yQuest"), OBFUSCATE("(II[B[B[B)V"));
	env->CallVoidMethod(activity, method, position, number, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_e2y_E2yManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		int32_t _i = i;
		int32_t _i2 = i2;
		int32_t _i3 = i3;
		
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_E2yManager);

		bsParams.Write(_i);
		bsParams.Write(_i2);
		bsParams.Write(_i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CE2y::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CE2y class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CE2y done!"));
}