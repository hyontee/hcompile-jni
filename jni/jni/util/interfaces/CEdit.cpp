#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CEdit.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CEdit::activity;

void CNetGame::packetEdit(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		bool status;
		bs.Read(status);
		
		CEdit::editShow(status);
	}
	else if (typeInfo == 1) CEdit::editHide();
	else if (typeInfo == 2)
	{
		float scale;
		bs.Read(scale);
		
		CEdit::updateEditScale(scale);
	}
}

void CEdit::editShow(bool status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("editShow"), OBFUSCATE("(Z)V"));
	env->CallVoidMethod(activity, method, status);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CEdit::editHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("editHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CEdit::updateEditScale(float scale)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateEditScale"), OBFUSCATE("(F)V"));
	env->CallVoidMethod(activity, method, scale);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_edit_EditManager_sendResponse(JNIEnv* pEnv, jobject thiz, jint i)
	{
		int32_t _i = i;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_EditManager);

		bsParams.Write(_i);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CEdit::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CEdit class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CEdit done!"));
}