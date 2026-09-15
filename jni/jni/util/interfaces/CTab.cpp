#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//scoreboard.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CTab.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CScoreBoard* pScoreBoard;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CTab::activity;

void CTab::ShowTAB()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showTabWindow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CTab::HideTAB()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideTabWindow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CTab::ClearTAB()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("clearTabStat"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CTab::SetTABStats(int playerID, char* playerName, int playerScore, int playerPing)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
	
    if (!env)
    {
		Log(OBFUSCATE("No env"));
		return;
    }
	
    /*jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");*/
    jbyteArray bytes = env->NewByteArray(strlen(playerName));
    env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);
    //jstring str1 = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("setTabStat"), OBFUSCATE("(III[B)V"));
    env->CallVoidMethod(activity, method, playerID, playerScore, playerPing, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

    EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_tab_TabManager_onTabClose(JNIEnv* pEnv, jobject thiz)
	{
		pScoreBoard->Toggle();
		CTab::HideTAB();
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_tab_TabManager_onReturnTabId(JNIEnv* pEnv, jobject thiz, jint playerId)
	{
		pScoreBoard->Toggle();
		CTab::HideTAB();
		
		RakNet::BitStream bsSend;
		bsSend.Write(playerId);
		bsSend.Write(0); // 0 from scoreboard - 1 from ? - 2 from ?
		pNetGame->GetRakClient()->RPC(&RPC_ClickPlayer, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}
}

void CTab::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CTab class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CTab done!"));
}