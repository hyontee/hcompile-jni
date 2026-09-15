#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"

#include "CContainer.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CContainer::activity;

void CNetGame::packetContainer(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	
	if (typeInfo == 0) CContainer::containerShow();
	else if (typeInfo == 1) CContainer::containerHide();
	else if (typeInfo == 2)
	{
		char text[5][256];
		for (int i = 0; i < 5; i ++)
		{
			memset(text[i], 0, 256);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CContainer::containerUpdateData(text[0], text[1], text[2], text[3], text[4]);
	}
	else if (typeInfo == 3)
	{
		bool show;
		bs.Read(show);
		
		char text[3][256];
		for (int i = 0; i < 3; i ++)
		{
			memset(text[i], 0, 256);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CContainer::containerShowInput(show, text[0], text[1], text[2]);
	}
	else if (typeInfo == 4)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t type;
			bs.Read(type);
			
			uint32_t model;
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
			
			uint8_t rareLevel;
			bs.Read(rareLevel);
				
			char text[256];
			memset(text, 0, 256);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
					
			CContainer::containerAddItem(type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, text);
		}
	}
	else if (typeInfo == 5)
	{
		char text[256];
		memset(text, 0, 256);
					
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CContainer::containerPrizeShow(text);
	}
	else if (typeInfo == 6) CContainer::containerPrizeHide();
	else if (typeInfo == 7) CContainer::containerPrizeClear();
	else if (typeInfo == 8)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t type;
			bs.Read(type);
			
			uint32_t model;
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
			
			uint8_t rareLevel;
			bs.Read(rareLevel);
				
			char text[256];
			memset(text, 0, 256);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
					
			CContainer::containerPrizeAddItem(type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, text);
		}
	}
}

void CContainer::containerShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CContainer::containerHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CContainer::containerPrizeClear()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerPrizeClear"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CContainer::containerUpdateData(char* text, char* text2, char* text3, char* text4, char* text5)
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

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerUpdateData"), OBFUSCATE("([B[B[B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CContainer::containerShowInput(bool b, char* text, char* text2, char* text3)
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

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerShowInput"), OBFUSCATE("(Z[B[B[B)V"));
	env->CallVoidMethod(activity, method, b, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CContainer::containerAddItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerAddItem"), OBFUSCATE("(IIIIFFFFI[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CContainer::containerPrizeShow(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerPrizeShow"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CContainer::containerPrizeHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerPrizeHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CContainer::containerPrizeAddItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("containerPrizeAddItem"), OBFUSCATE("(IIIIFFFFI[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_container_ContainerManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_ContainerManager);

		bsParams.Write((int32_t)1);
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_container_ContainerPrizeManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_ContainerManager);

		bsParams.Write((int32_t)2);
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CContainer::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CContainer class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CContainer done!"));
}