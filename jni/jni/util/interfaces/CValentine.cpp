#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CValentine.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CValentine::activity;

void CValentine::valentineShowMainPage(uint32_t i, uint32_t i2, uint32_t i3, uint32_t i4, char* mes, uint32_t i5, char* s, char* s2, char* s3, char* s4, char* s5, char* s6)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(mes));
	env->SetByteArrayRegion(bytes, 0, strlen(mes), (jbyte*)mes);

   	jbyteArray bytes2 = env->NewByteArray(strlen(s));
	env->SetByteArrayRegion(bytes2, 0, strlen(s), (jbyte*)s);

   	jbyteArray bytes3 = env->NewByteArray(strlen(s2));
	env->SetByteArrayRegion(bytes3, 0, strlen(s2), (jbyte*)s2);

   	jbyteArray bytes4 = env->NewByteArray(strlen(s3));
	env->SetByteArrayRegion(bytes4, 0, strlen(s3), (jbyte*)s3);

   	jbyteArray bytes5 = env->NewByteArray(strlen(s4));
	env->SetByteArrayRegion(bytes5, 0, strlen(s4), (jbyte*)s4);

   	jbyteArray bytes6 = env->NewByteArray(strlen(s5));
	env->SetByteArrayRegion(bytes6, 0, strlen(s5), (jbyte*)s5);

   	jbyteArray bytes7 = env->NewByteArray(strlen(s6));
	env->SetByteArrayRegion(bytes7, 0, strlen(s6), (jbyte*)s6);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineShowMainPage"), OBFUSCATE("(IIII[BI[B[B[B[B[B[B)V"));
	env->CallVoidMethod(activity, method, i, i2, i3, i4, bytes, i5, bytes2, bytes3, bytes4, bytes5, bytes6, bytes7);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineShowTaskPage(int i, int i2, int i3, int i4, int i5, int i6, int i7, int i8)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineShowTaskPage"), OBFUSCATE("(IIIIIIII)V"));
	env->CallVoidMethod(activity, method, i, i2, i3, i4, i5, i6, i7, i8);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineShowSendPage(int i, int i2, int i3, int i4, int i5)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineShowSendPage"), OBFUSCATE("(IIIII)V"));
	env->CallVoidMethod(activity, method, i, i2, i3, i4, i5);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineShowShopPage(uint32_t i, uint32_t i2, uint32_t i3, uint32_t i4, uint32_t i5, int i6, uint32_t i7, uint32_t i8, uint32_t i9, uint32_t i10, uint32_t i11, uint32_t i12, uint32_t i13, uint32_t i14)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineShowShopPage"), OBFUSCATE("(IIIIIIIIIIIIII)V"));
	env->CallVoidMethod(activity, method, i, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineShowTopPage()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineShowTopPage"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineShowDescPage()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineShowDescPage"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineShowLobbyPage(int i)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineShowLobbyPage"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, i);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineLobbySetName(int i, char* mes)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineLobbySetName"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, i, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineLobbySetReady(int i, int i2)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineLobbySetReady"), OBFUSCATE("(II)V"));
	env->CallVoidMethod(activity, method, i, i2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineLobbySetState(int i)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineLobbySetState"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, i);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineLobbyHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineLobbyHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CValentine::valentineAddTopItem(int i, int i2, char* mes, int i3)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("valentineAddTopItem"), OBFUSCATE("(II[BI)V"));
	env->CallVoidMethod(activity, method, i, i2, bytes, i3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_valentine_ValentineManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		int32_t _i = i;
		int32_t _i2 = i2;
		int32_t _i3 = i3;
		
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_ValentineManager);

		bsParams.Write(_i);
		bsParams.Write(_i2);
		bsParams.Write(_i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CValentine::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CValentine class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CValentine done!"));
}