#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"

#include "CBinders.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CBinders::activity;

void CNetGame::packetBinders(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CBinders::bindShow();
	else if (typeInfo == 1) CBinders::bindHide();
	else if (typeInfo == 2)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
			char text[256];
			memset(text, 0, 256);
				
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
			
			CBinders::bindAddItem(position, text);
		}
	}
	else if (typeInfo == 3)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
			char text[256];
			memset(text, 0, 256);
				
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
				
			CBinders::bindUpdateItem(position, text);
		}
	}
	else if (typeInfo == 4) CBinders::bindEditShow();
	else if (typeInfo == 5) CBinders::bindEditHide();
	else if (typeInfo == 6)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			char text[2][256];
			for (int q = 0; q < 2; q ++)
			{
				memset(text[q], 0, 256);
					
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(text[q], bLen);
				text[q][bLen] = '\0';
			}
				
			CBinders::bindEditAddItem(text[0], text[1]);
		}
	}
	else if (typeInfo == 7)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
			char text[2][256];
			for (int q = 0; q < 2; q ++)
			{
				memset(text[q], 0, 256);
					
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(text[q], bLen);
				text[q][bLen] = '\0';
			}
				
			CBinders::bindEditUpdateItem(position, text[0], text[1]);
		}
	}
}

void CBinders::bindShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bindShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBinders::bindHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bindHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBinders::bindAddItem(int8_t position, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bindAddItem"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, position, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBinders::bindUpdateItem(int8_t position, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bindUpdateItem"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, position, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBinders::bindEditShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bindEditShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBinders::bindEditHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bindEditHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBinders::bindEditAddItem(char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bindEditAddItem"), OBFUSCATE("([B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBinders::bindEditUpdateItem(int8_t position, char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bindEditUpdateItem"), OBFUSCATE("(I[B[B)V"));
	env->CallVoidMethod(activity, method, position, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_binder_BinderManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Binders);
					
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_binder_BinderEditManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jbyteArray bArr)
	{
		jboolean isCopy = true;
		jbyte* pMsg = pEnv->GetByteArrayElements(bArr, &isCopy);
		jsize length = pEnv->GetArrayLength(bArr);
		std::string szStr((char*)pMsg, length);

		char buff[128];
		strcpy(buff, szStr.c_str());
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_BindersChar);
						
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((uint8_t)strlen(buff));
		bsParams.Write(buff, (uint8_t)strlen(buff));
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		pEnv->ReleaseByteArrayElements(bArr, pMsg, JNI_ABORT);
	}
}

void CBinders::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CBinders class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CBinders done!"));
}