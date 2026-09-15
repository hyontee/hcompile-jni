#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CDialogs.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CDialogs::activity;

void CNetGame::packetDialogs(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		char text[8][4048];
		uint32_t textLen;
		for (int i = 0; i < 8; i ++)
		{
			memset(text[i], 0, 4048);

			bs.Read(textLen);
			bs.Read(text[i], textLen);
			text[i][textLen] = '\0';
		}
		
		CDialogs::showSelectorDialog(text[0], text[1], text[2], text[3], text[4], text[5], text[6], text[7]);
	}
	else if (typeInfo == 1) CDialogs::hideSelectorDialog();
	else if (typeInfo == 2)
	{
		bool status;
		bs.Read(status);
		
		char text[3][4048];
		uint32_t textLen;
		for (int i = 0; i < 3; i ++)
		{
			memset(text[i], 0, 4048);

			bs.Read(textLen);
			bs.Read(text[i], textLen);
			text[i][textLen] = '\0';
		}
		
		int32_t max;
		bs.Read(max);
		
		int32_t progress;
		bs.Read(progress);
		
		CDialogs::dialogSeekBar(status, text[0], text[1], text[2], max, progress);
	}
}

void CDialogs::showSelectorDialog(char* text, char* text2, char* text3, char* text4, char* text5, char* text6, char* text7, char* text8)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showSelectorDialog"), OBFUSCATE("([B[B[B[B[B[B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5, bytes6, bytes7, bytes8);
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

void CDialogs::hideSelectorDialog()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideSelectorDialog"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CDialogs::dialogSeekBar(bool status, char* text, char* text2, char* text3, int32_t max, int32_t progress)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("dialogSeekBar"), OBFUSCATE("(Z[B[B[BII)V"));
	env->CallVoidMethod(activity, method, status, bytes, bytes2, bytes3, max, progress);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_dialogs_SelectorDialogManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_SelectorDialog);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CDialogs::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CDialogs class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CDialogs done!"));
}