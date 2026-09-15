#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CPlates.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CPlates::activity;

void CNetGame::packetPlates(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CPlates::platesShow();
	else if (typeInfo == 1) CPlates::platesHide();
	else if (typeInfo == 2)
	{
		int8_t id;
		bs.Read(id);
		
		CPlates::platesLayout(id);
	}
	else if (typeInfo == 3)
	{
		char text[3][256];
		for (int i = 0; i < 3; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CPlates::platesMainLayout(text[0], text[1], text[2]);
	}
	else if (typeInfo == 4)
	{
		int8_t id;
		bs.Read(id);
		
		bool status;
		bs.Read(status);
		
		CPlates::platesTabLayout(id, status);
	}
	else if (typeInfo == 5)
	{
		int8_t id;
		bs.Read(id);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CPlates::platesTabDescription(id, text);
	}
	else if (typeInfo == 6)
	{
		int8_t id;
		bs.Read(id);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CPlates::platesTabButton(id, text);
	}
	else if (typeInfo == 7)
	{
		int8_t id;
		bs.Read(id);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CPlates::platesTabButtonText(id, text);
	}
}

void CPlates::platesShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("platesShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CPlates::platesHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("platesHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CPlates::platesLayout(int8_t id)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("platesLayout"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, id);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CPlates::platesMainLayout(char* text, char* text2, char* text3)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("platesMainLayout"), OBFUSCATE("([B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CPlates::platesTabLayout(int8_t id, bool status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("platesTabLayout"), OBFUSCATE("(IZ)V"));
	env->CallVoidMethod(activity, method, id, status);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CPlates::platesTabDescription(int8_t id, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("platesTabDescription"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, id, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CPlates::platesTabButton(int8_t id, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("platesTabButton"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, id, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CPlates::platesTabButtonText(int8_t id, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("platesTabButtonText"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, id, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_plates_PlatesManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Plates);

		bsParams.Write((int32_t)0);
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_plates_PlatesManager_SendResponseArr(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3, jbyteArray bArr)
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
		bsParams.Write((uint32_t)RPC_Plates);

		bsParams.Write((int32_t)1);
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		bsParams.Write((uint8_t)strlen(buff));
		bsParams.Write(buff, (uint8_t)strlen(buff));
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CPlates::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CPlates class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CPlates done!"));
}