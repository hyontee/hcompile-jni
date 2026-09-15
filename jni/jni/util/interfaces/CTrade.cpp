#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CTrade.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CTrade::activity;

void CNetGame::packetTrade(Packet *p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CTrade::showTrade();
	else if (typeInfo == 1) CTrade::hideTrade();
	else if (typeInfo == 2)
	{
		char text[3][256];
		for (int i = 0; i < 3; i ++) memset(text[i], 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text[0], bLen);
		text[0][bLen] = '\0';
		
		bool accept;
		bs.Read(accept);
		
		bool accept2;
		bs.Read(accept2);
		
		bool accept3;
		bs.Read(accept3);
		
		for (int i = 1; i < 3; i ++)
		{
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CTrade::tradeUpdate(text[0], accept, accept2, accept3, text[1], text[2]);
	}
	else if (typeInfo == 3)
	{
		int32_t position;
		bs.Read(position);
		
		int32_t slot;
		bs.Read(slot);
		
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
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		bool changed;
		bs.Read(changed);
		
		int8_t itemtype;
		bs.Read(itemtype);
		
		CTrade::tradeUpdateItem(position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, text[0], text[1], changed, itemtype);
	}
	else if (typeInfo == 4)
	{
		int32_t position;
		bs.Read(position);
		
		int32_t slot;
		bs.Read(slot);
		
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
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		bool changed;
		bs.Read(changed);
		
		int8_t itemtype;
		bs.Read(itemtype);
		
		CTrade::tradeAddSendItem(position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, text[0], text[1], changed, itemtype);
	}
	else if (typeInfo == 5)
	{
		int32_t position;
		bs.Read(position);
		
		int32_t slot;
		bs.Read(slot);
		
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
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		bool changed;
		bs.Read(changed);
		
		int8_t itemtype;
		bs.Read(itemtype);
		
		CTrade::tradeUpdateSendItem(position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, text[0], text[1], changed, itemtype);
	}
	else if (typeInfo == 6)
	{
		int32_t position;
		bs.Read(position);
		
		CTrade::tradeDeleteSendItem(position);
	}
	else if (typeInfo == 7)
	{
		int32_t position;
		bs.Read(position);
		
		int32_t slot;
		bs.Read(slot);
		
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
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		bool changed;
		bs.Read(changed);
		
		int8_t itemtype;
		bs.Read(itemtype);
		
		CTrade::tradeAddReceiveItem(position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, text[0], text[1], changed, itemtype);
	}
	else if (typeInfo == 8)
	{
		int32_t position;
		bs.Read(position);
		
		int32_t slot;
		bs.Read(slot);
		
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
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		bool changed;
		bs.Read(changed);
		
		int8_t itemtype;
		bs.Read(itemtype);
		
		CTrade::tradeUpdateReceiveItem(position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, text[0], text[1], changed, itemtype);
	}
	else if (typeInfo == 9)
	{
		int32_t position;
		bs.Read(position);
		
		CTrade::tradeDeleteReceiveItem(position);
	}
}

void CTrade::showTrade()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showTrade"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::hideTrade()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideTrade"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::tradeUpdate(char* text, bool accept, bool accept2, bool accept3, char* text2, char* text3)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

   	jbyteArray bytes3 = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("tradeUpdate"), OBFUSCATE("([BZZZ[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, accept, accept2, accept3, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::tradeUpdateItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("tradeUpdateItem"), OBFUSCATE("(IIIIIIFFFF[B[BZI)V"));
    env->CallVoidMethod(activity, method, position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2, changed, itemtype);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::tradeAddSendItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("tradeAddSendItem"), OBFUSCATE("(IIIIIIFFFF[B[BZI)V"));
    env->CallVoidMethod(activity, method, position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2, changed, itemtype);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::tradeUpdateSendItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("tradeUpdateSendItem"), OBFUSCATE("(IIIIIIFFFF[B[BZI)V"));
    env->CallVoidMethod(activity, method, position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2, changed, itemtype);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::tradeDeleteSendItem(int32_t position)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("tradeDeleteSendItem"), OBFUSCATE("(I)V"));
    env->CallVoidMethod(activity, method, position);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::tradeAddReceiveItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("tradeAddReceiveItem"), OBFUSCATE("(IIIIIIFFFF[B[BZI)V"));
    env->CallVoidMethod(activity, method, position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2, changed, itemtype);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::tradeUpdateReceiveItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("tradeUpdateReceiveItem"), OBFUSCATE("(IIIIIIFFFF[B[BZI)V"));
    env->CallVoidMethod(activity, method, position, slot, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2, changed, itemtype);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTrade::tradeDeleteReceiveItem(int32_t position)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("tradeDeleteReceiveItem"), OBFUSCATE("(I)V"));
    env->CallVoidMethod(activity, method, position);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_trade_TradeManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Trade);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CTrade::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CTrade class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CTrade done!"));
}