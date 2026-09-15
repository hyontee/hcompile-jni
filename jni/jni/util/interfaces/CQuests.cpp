#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CQuests.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CQuests::activity;

void CNetGame::packetQuests(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		bool status;
		bs.Read(status);
		
		CQuests::questsShow(status);
	}
	else if (typeInfo == 1) CQuests::questsHide();
	else if (typeInfo == 2)
	{
		int8_t position;
		bs.Read(position);
		
		int8_t type;
		bs.Read(type);
			
		int32_t model;
		bs.Read(model);
			
		uint8_t color1;
		bs.Read(color1);
		
		uint8_t color2;
		bs.Read(color2);
		
		float rotX;
		bs.Read(rotX);
		
		float rotY;
		bs.Read(rotY);
		
		float rotZ;
		bs.Read(rotZ);
		
		float zoom;
		bs.Read(zoom);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		char text2[256];
		memset(text2, 0, 256);
		
		bs.Read(bLen);
		bs.Read(text2, bLen);
		text2[bLen] = '\0';
		
		int32_t max;
		bs.Read(max);
		
		int32_t progress;
		bs.Read(progress);
		
		CQuests::addQuestsItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text, text2, max, progress);
	}
	else if (typeInfo == 3)
	{
		int8_t type;
		bs.Read(type);
			
		int32_t model;
		bs.Read(model);
		
		uint8_t color1;
		bs.Read(color1);
		
		uint8_t color2;
		bs.Read(color2);
		
		float rotX;
		bs.Read(rotX);
		
		float rotY;
		bs.Read(rotY);
		
		float rotZ;
		bs.Read(rotZ);
		
		float zoom;
		bs.Read(zoom);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CQuests::addQuestRewardsItem(type, model, color1, color2, rotX, rotY, rotZ, zoom, text);
	}
	else if (typeInfo == 4)
	{
		char text[4][256];
		for (int i = 0; i < 4; i ++) memset(text[i], 0, 256);
		for (int i = 0; i < 2; i ++)
		{
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		int32_t max;
		bs.Read(max);
		
		int32_t progress;
		bs.Read(progress);
		
		for (int i = 2; i < 4; i ++)
		{
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CQuests::updateQuestsData(text[0], text[1], max, progress, text[2], text[3]);
	}
}

void CQuests::questsShow(bool status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("questsShow"), OBFUSCATE("(Z)V"));
	env->CallVoidMethod(activity, method, status);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CQuests::questsHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("questsHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CQuests::addQuestsItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, int32_t max, int32_t progress)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addQuestsItem"), OBFUSCATE("(IIIIIFFFF[B[BII)V"));
	env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2, max, progress);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CQuests::addQuestRewardsItem(int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addQuestRewardsItem"), OBFUSCATE("(IIIIFFFF[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CQuests::updateQuestsData(char* text, char* text2, int max, int progress, char* text3, char* text4)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jbyteArray bytes3 = env->NewByteArray(strlen(text3));
    env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jbyteArray bytes4 = env->NewByteArray(strlen(text4));
    env->SetByteArrayRegion(bytes4, 0, strlen(text4), (jbyte*)text4);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateQuestsData"), OBFUSCATE("([B[BII[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, max, progress, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_quest_QuestManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_QuestManager);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CQuests::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CQuests class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CQuests done!"));
}