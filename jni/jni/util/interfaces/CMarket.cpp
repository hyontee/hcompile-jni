#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CMarket.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CMarket::activity;

void CNetGame::packetMarket(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 1)
	{
		char text[256];
		memset(text, 0, 256);
				
		uint8_t textLen;
		bs.Read(textLen);
		bs.Read(text, textLen);
		text[textLen] = '\0';
		
		CMarket::marketShow(text);
	}
	else if (typeInfo == 2) CMarket::marketHide();
	else if (typeInfo == 3)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int32_t position;
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
			
			char text[2][256];
			for (int i = 0; i < 2; i ++)
			{
				memset(text[i], 0, 256);
						
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}
			
			CMarket::marketInvUpdateItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text[0], text[1]);
		}
	}
	else if (typeInfo == 4)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int32_t position;
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
			
			char text[2][256];
			for (int i = 0; i < 2; i ++)
			{
				memset(text[i], 0, 256);
						
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}
			
			CMarket::marketUpdateItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text[0], text[1]);
		}
	}
}

void CMarket::marketShow(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("marketShow"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CMarket::marketHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("marketHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CMarket::marketInvUpdateItem(int32_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("marketInvUpdateItem"), OBFUSCATE("(IIIIIFFFF[B[B)V"));
	env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CMarket::marketUpdateItem(int32_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("marketUpdateItem"), OBFUSCATE("(IIIIIFFFF[B[B)V"));
	env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_market_MarketManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Market);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CMarket::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CMarket class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CMarket done!"));
}