#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CAdminTools.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CAdminTools::activity;

void CNetGame::packetAdminTools(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
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
		CAdminTools::adminLogsShow(text[0], text[1], text[2], text[3], text[4]);
	}
	else if (typeInfo == 1) CAdminTools::adminLogsHide();
	else if (typeInfo == 2)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			char text[32];
			memset(text, 0, 32);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
			
			CAdminTools::adminMenuItem(text);
		}
	}
	else if (typeInfo == 3)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int q = 0; q < max_items; q ++)
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
			CAdminTools::adminLogsItem(text[0], text[1], text[2], text[3], text[4]);
		}
	}
}

void CAdminTools::adminLogsShow(char* text, char* text2, char* text3, char* text4, char* text5)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("adminLogsShow"), OBFUSCATE("([B[B[B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CAdminTools::adminLogsHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("adminLogsHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CAdminTools::adminMenuItem(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("adminMenuItem"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CAdminTools::adminLogsItem(char* text, char* text2, char* text3, char* text4, char* text5)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("adminLogsItem"), OBFUSCATE("([B[B[B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_admintools_AdminManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_AdminManager);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i3);
		uint8_t iStrlen = strlen(" ");
		bsParams.Write(iStrlen);
		bsParams.Write(" ", iStrlen);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_admintools_AdminManager_SendResponseByte(JNIEnv* pEnv, jobject thiz, jint i, jbyteArray bArr, jint i3)
	{
		if (!bArr) return;
		
		jboolean isCopy = true;
		jbyte* pMsg = pEnv->GetByteArrayElements(bArr, &isCopy);
		jsize length = pEnv->GetArrayLength(bArr);
		std::string szStr((char*)pMsg, length);
		
		pEnv->ReleaseByteArrayElements(bArr, pMsg, JNI_ABORT);
		
		char buff[128];
		memset(buff, 0, 128);
		strcpy(buff, szStr.c_str());
		
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_AdminManager);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i3);
		uint8_t iStrlen = strlen(buff);
		bsParams.Write(iStrlen);
		bsParams.Write(buff, iStrlen);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CAdminTools::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CAdminTools class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CAdminTools done!"));
}