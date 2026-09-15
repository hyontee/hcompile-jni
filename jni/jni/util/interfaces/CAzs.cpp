#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CAzs.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CAzs::activity;

void CNetGame::packetAzs(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		bool status;
		bs.Read(status);
		
		int32_t price;
		bs.Read(price);
		
		int32_t fuel;
		bs.Read(fuel);
		
		char text[255];
		memset(text, 0, 255);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CAzs::showAzs(status, price, fuel, text);
	}
	else if (typeInfo == 1) CAzs::hideAzs();
	else if (typeInfo == 2)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int q = 0; q < max_items; q ++)
		{
			char text[2][255];
			for (int i = 0; i < 2; i ++)
			{
				memset(text[i], 0, 255);
						
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(text[i], bLen);
				text[i][bLen] = '\0';
			}

			CAzs::azsAddItem(text[0], text[1]);
		}
	}
}

void CAzs::showAzs(bool status, int32_t price, int32_t fuel, char* valute)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jbyteArray bytes = env->NewByteArray(strlen(valute));
    env->SetByteArrayRegion(bytes, 0, strlen(valute), (jbyte*)valute);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showAzs"), OBFUSCATE("(ZII[B)V"));
	env->CallVoidMethod(activity, method, status, price, fuel, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CAzs::hideAzs()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideAzs"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CAzs::azsAddItem(char* text, char* price)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(price));
    env->SetByteArrayRegion(bytes2, 0, strlen(price), (jbyte*)price);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("azsAddItem"), OBFUSCATE("([B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_azs_AzsManager_sendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_AzsManager);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CAzs::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CAzs class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CAzs done!"));
}