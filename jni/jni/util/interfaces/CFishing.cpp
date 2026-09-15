#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"
#include "../game/CFirstPersonCamera.h"

#include "CFishing.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CFishing::activity;

extern bool g_bFirstPersonOnFootEnabled;
void CNetGame::packetFishing(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	
	if (typeInfo == 0) CFishing::fishingShow();
	else if (typeInfo == 1) CFishing::fishingHide();
	else if (typeInfo == 2)
	{
		char text[256];
		memset(text, 0, 256);
					
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		bool status;
		bs.Read(status);
		
		CFishing::fishingUpdate(text, status);
	}
	else if (typeInfo == 3)
	{
		bool status;
		bs.Read(status);
		
		g_bFirstPersonOnFootEnabled = status;
	}
}

void CFishing::fishingShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("fishingShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFishing::fishingHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("fishingHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFishing::fishingUpdate(char* text, bool status)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("fishingUpdate"), OBFUSCATE("([BZ)V"));
	env->CallVoidMethod(activity, method, bytes, status);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_fishing_FishingManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_FishingManager);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CFishing::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CFishing class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CFishing done!"));
}