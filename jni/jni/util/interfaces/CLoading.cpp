#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CLoading.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CLoading::activity;

void CLoading::SetLoadingShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetLoadingShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CLoading::SetLoadingHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetLoadingHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CLoading::SetLoadingStatus(char* status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(status));
	env->SetByteArrayRegion(bytes, 0, strlen(status), (jbyte*)status);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetLoadingStatus"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CLoading::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CLoading class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CLoading done!"));
}