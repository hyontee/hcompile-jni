#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CRecon.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CRecon::activity;

void CNetGame::packetRecon(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CRecon::reconShow(true);
	else if (typeInfo == 1) CRecon::reconHide(true);
	else if (typeInfo == 2)
	{
		char text[32];
		memset(text, 0, 32);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		int32_t playerID;
		bs.Read(playerID);
		
		CRecon::updateRecon(text, playerID);
	}
}

void CRecon::reconShow(bool status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("reconShow"), OBFUSCATE("(Z)V"));
	env->CallVoidMethod(activity, method, status);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CRecon::reconHide(bool status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("reconHide"), OBFUSCATE("(Z)V"));
	env->CallVoidMethod(activity, method, status);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CRecon::updateRecon(char* text, int32_t playerID)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateRecon"), OBFUSCATE("([BI)V"));
	env->CallVoidMethod(activity, method, bytes, playerID);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_recon_ReconManager_sendResponse(JNIEnv* pEnv, jobject thiz, jint i)
	{
		int32_t _i = i;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_ReconManager);

		bsParams.Write(_i);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CRecon::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CRecon class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CRecon done!"));
}