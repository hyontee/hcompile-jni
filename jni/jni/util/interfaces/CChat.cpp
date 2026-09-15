#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CChat.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CChat::activity;

void CNetGame::packetChat(Packet *p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CChat::newShowChatInput();
	else if (typeInfo == 1) CChat::newHideChatInput();
}

void CChat::ShowChat()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showChat"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::HideChat()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideChat"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::ShowChatInput()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showChatInput"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::HideChatInput()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideChatInput"), OBFUSCATE("()V"));
   	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::PushChatMessage(uint32_t color, char* msg, char wordType, int dwType, int dwNumber)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(msg));
    env->SetByteArrayRegion(bytes, 0, strlen(msg), (jbyte*)msg);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("pushChatMessage"), OBFUSCATE("(I[BCII)V"));
    env->CallVoidMethod(activity, method, color, bytes, (jchar) wordType, dwType, dwNumber);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::newShowChat()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newShowChat"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::newHideChat()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newHideChat"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::newShowChatInput()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newShowChatInput"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::newHideChatInput()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newHideChatInput"), OBFUSCATE("()V"));
   	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CChat::newPushChatMessage(uint32_t color, char* msg, char wordType, int dwType, int dwNumber)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(msg));
    env->SetByteArrayRegion(bytes, 0, strlen(msg), (jbyte*)msg);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newPushChatMessage"), OBFUSCATE("(I[BCII)V"));
    env->CallVoidMethod(activity, method, color, bytes, (jchar) wordType, dwType, dwNumber);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

bool ProcessVoiceCommands(const char* str);
extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_chat_ChatManager_onClickChatBox(JNIEnv* pEnv, jobject thiz)
	{
		if(pSettings->GetReadOnly().iChat == 1)
		{
			
		}
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_chat_ChatManager_onChatInputEndMethod(JNIEnv* pEnv, jobject thiz, jbyteArray bArr)
	{
		if (!bArr) return;

		if(pSettings->GetReadOnly().iChat == 1)
		{
			jboolean isCopy = true;
			jbyte* pMsg = pEnv->GetByteArrayElements(bArr, &isCopy);
			jsize length = pEnv->GetArrayLength(bArr);
			std::string szStr((char*)pMsg, length);

			char buff[144];
			strcpy(buff, szStr.c_str());
		
			if (buff[0] == '/')
			{
				if (ProcessVoiceCommands(buff))
				{
					return;
				}
				pNetGame->SendChatCommand(buff);
			}
			else
			{
				pNetGame->SendChatMessage(buff);
			}
			pEnv->ReleaseByteArrayElements(bArr, pMsg, JNI_ABORT);
		}
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_chat_NewChatManager_onChatInputEndMethod(JNIEnv* pEnv, jobject thiz, jbyteArray bArr, jint i)
	{
		jboolean isCopy = true;
		jbyte* pMsg = pEnv->GetByteArrayElements(bArr, &isCopy);
		jsize length = pEnv->GetArrayLength(bArr);
		std::string szStr((char*)pMsg, length);

		char buff[128];
		strcpy(buff, szStr.c_str());
		if (strlen(buff) > 0)
		{
			if (i == 0)
			{
				if (buff[0] == '/')
				{
					if (ProcessVoiceCommands(buff))
					{
						return;
					}
					pNetGame->SendChatCommand(buff);
				}
				else
				{
					pNetGame->SendChatMessage(buff);
				}
			}
			else
			{
				RakNet::BitStream bsParams;
				bsParams.Write((uint8_t)ID_CUSTOM_RPC);
				bsParams.Write((uint32_t)RPC_ChatMessage);
					
				bsParams.Write((int32_t)i);
				bsParams.Write((uint8_t)strlen(buff));
				bsParams.Write(buff, (uint8_t)strlen(buff));
				pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
			}
		}
		pEnv->ReleaseByteArrayElements(bArr, pMsg, JNI_ABORT);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_chat_NewChatManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_ChatMessage);

		bsParams.Write((int32_t)i);
		bsParams.Write((uint8_t)strlen(" "));
		bsParams.Write(" ", (uint8_t)strlen(" "));
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CChat::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CChat class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CChat done!"));
}