#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CShop.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CShop::activity;

void CNetGame::packetShop(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		int8_t position;
		bs.Read(position);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CShop::ShopShow(position, text);
	}
	else if (typeInfo == 1)
	{
		int8_t position;
		bs.Read(position);
		
		CShop::ShopHide(position);
	}
	else if (typeInfo == 2)
	{
		uint8_t max_items;
		bs.Read(max_items);
		
		int8_t position;
		bs.Read(position);
		
		for (int i = 0; i < max_items; i ++)
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
			
			char text[2][256];
			for (int q = 0; q < 2; q ++)
			{
				memset(text[q], 0, 256);
				
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(text[q], bLen);
				text[q][bLen] = '\0';
			}
			
			CShop::UpdateShopItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text[0], text[1]);
		}
	}
	else if (typeInfo == 3) CShop::ItemShopReset();
}

void CShop::ShopShow(int position, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("ShopShow"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, position, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CShop::ShopHide(int position)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("ShopHide"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, position);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CShop::UpdateShopItem(int position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("UpdateShopItem"), OBFUSCATE("(IIIIIFFFF[B[B)V"));
	env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CShop::ItemShopReset()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("ItemShopReset"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_shopmanager_SkinShopManager_onReturnShop(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_ShopManager);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)0);
		bsParams.Write((int32_t)1);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_shopmanager_ShopManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_ShopManager);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		bsParams.Write((int32_t)2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CShop::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CShop class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CShop done!"));
}