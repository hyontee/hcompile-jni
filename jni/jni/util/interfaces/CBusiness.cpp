#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"

#include "CBusiness.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CBusiness::activity;

void CNetGame::packetBusiness(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		char text[256];
		memset(text, 0, 256);
			
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CBusiness::bizShow(text);
	}
	else if (typeInfo == 1) CBusiness::bizHide();
	else if (typeInfo == 2)
	{
		int8_t id;
		bs.Read(id);
		
		CBusiness::bizLayoutPage(id);
	}
	else if (typeInfo == 3)
	{
		int32_t position;
		bs.Read(position);
		
		int8_t type;
		bs.Read(type);
		
		bool status;
		bs.Read(status);
		
		char text[8][256];
		for (int i = 0; i < 8; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CBusiness::bizAddItem(position, type, status, text[0], text[1], text[2], text[3], text[4], text[5], text[6], text[7]);
	}
	else if (typeInfo == 4)
	{
		char text[4][256];
		for (int i = 0; i < 4; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		int8_t type;
		bs.Read(type);
		
		bool status;
		bs.Read(status);
		
		CBusiness::bizLayoutUpdate(text[0], text[1], text[2], text[3], type, status);
	}
	else if (typeInfo == 5)
	{
		int32_t position;
		bs.Read(position);
		
		char text[256];
		memset(text, 0, 256);
			
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
			
		CBusiness::bizMenuItem(position, text);
	}
	else if (typeInfo == 6)
	{
		char text[4][256];
		for (int i = 0; i < 4; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CBusiness::bizMainItem(text[0], text[1], text[2], text[3]);
	}
	else if (typeInfo == 7)
	{
		int32_t position;
		bs.Read(position);
		
		char text[256];
		memset(text, 0, 256);
			
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		bool status;
		bs.Read(status);
		
		CBusiness::bizParentItem(position, text, status);
	}
	else if (typeInfo == 8)
	{
		int32_t position;
		bs.Read(position);
		
		int32_t id;
		bs.Read(id);
		
		char text[2][256];
		for (int i = 0; i < 2; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CBusiness::bizChildItem(position, id, text[0], text[1]);
	}
}

void CBusiness::bizShow(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizShow"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBusiness::bizHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBusiness::bizLayoutPage(int8_t i)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizLayoutPage"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, i);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBusiness::bizAddItem(int32_t position, int8_t type, bool status, char* text, char* text2, char* text3, char* text4, char* text5, char* text6, char* text7, char* text8)
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

    jbyteArray bytes5 = env->NewByteArray(strlen(text5));
    env->SetByteArrayRegion(bytes5, 0, strlen(text5), (jbyte*)text5);

    jbyteArray bytes6 = env->NewByteArray(strlen(text6));
    env->SetByteArrayRegion(bytes6, 0, strlen(text6), (jbyte*)text6);

    jbyteArray bytes7 = env->NewByteArray(strlen(text7));
    env->SetByteArrayRegion(bytes7, 0, strlen(text7), (jbyte*)text7);

    jbyteArray bytes8 = env->NewByteArray(strlen(text8));
    env->SetByteArrayRegion(bytes8, 0, strlen(text8), (jbyte*)text8);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizAddItem"), OBFUSCATE("(IIZ[B[B[B[B[B[B[B[B)V"));
	env->CallVoidMethod(activity, method, position, type, status, bytes, bytes2, bytes3, bytes4, bytes5, bytes6, bytes7, bytes8);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(bytes8);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBusiness::bizLayoutUpdate(char* text, char* text2, char* text3, char* text4, int8_t type, bool status)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizLayoutUpdate"), OBFUSCATE("([B[B[B[BIZ)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, type, status);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBusiness::bizMenuItem(int32_t position, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizMenuItem"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, position, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBusiness::bizMainItem(char* text, char* text2, char* text3, char* text4)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizMainItem"), OBFUSCATE("([B[B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBusiness::bizParentItem(int32_t position, char* text, bool status)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizParentItem"), OBFUSCATE("(I[BZ)V"));
	env->CallVoidMethod(activity, method, position, bytes, status);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBusiness::bizChildItem(int32_t position, int32_t id, char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bizChildItem"), OBFUSCATE("(II[B[B)V"));
	env->CallVoidMethod(activity, method, position, id, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_business_BusinessManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Business);
					
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CBusiness::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CBusiness class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CBusiness done!"));
}