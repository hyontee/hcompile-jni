#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../voice/CVoiceChatClient.h"

#include "CVoice.h"

extern CVoiceChatClient* pVoice;
extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CVoice::activity;

void CVoice::ShowVoice()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}
		
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showVoice"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT jint JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_getGlobalVolume(JNIEnv* pEnv, jobject thiz)
	{
		if (pVoice)
		{
			return pVoice->GetVolume();
		}
		return 0;
	}

	JNIEXPORT jbyteArray JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_getPlayerName(JNIEnv* pEnv, jobject thiz, jint playerId)
	{
		char pTemp[24];
		jbyteArray nickname = pEnv->NewByteArray(sizeof(pTemp));

		if (!nickname)
		{
			return nullptr;
		}

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		pEnv->SetByteArrayRegion(nickname, 0, sizeof(pTemp), (const jbyte*)pPlayerPool->GetPlayerName(playerId));
		return nickname;
	}
	
	JNIEXPORT jint JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_getPlayerVolume(JNIEnv* pEnv, jobject thiz, jint playerId)
	{
		if (pVoice)
		{
			return pVoice->GetVolumePlayer(playerId);
		}
		return 0;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_isPlayerExists(JNIEnv* pEnv, jobject thiz, jint playerId)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (!pPlayerPool->GetSlotState(playerId)) return false;
		else return true;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_isPlayerHasVoice(JNIEnv* pEnv, jobject thiz, jint playerId)
	{
		if (!pVoice) return false;
		
		return pVoice->GetPlayerVoiceStatus(playerId);
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_isPlayerMuted(JNIEnv* pEnv, jobject thiz, jint playerId)
	{
		if (pVoice)
		{
			if (pVoice->StatusMutePlayer(playerId) == 1) return false;
		}
		else return true;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_isVoiceActive(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetWrite().iVoice;
		}
		else return 0;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_isVoiceListActive(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetWrite().iVoiceList;
		}
		else return 0;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_setGlobalVolume(JNIEnv* pEnv, jobject thiz, jint volume)
	{
		if (pVoice)
		{
			pVoice->SetVolume(volume);
			pSettings->Save();
			return true;
		}
		return false;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_setPlayerMuted(JNIEnv* pEnv, jobject thiz, jint playerId, jboolean b)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (!pPlayerPool->GetSlotState(playerId)) return false;
		
		if (pVoice)
		{
			if (b) pVoice->MutePlayer(playerId);
			else pVoice->UnMutePlayer(playerId);
			return true;
		}
		return false;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_setPlayerVolume(JNIEnv* pEnv, jobject thiz, jint playerId, jint volume)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (!pPlayerPool->GetSlotState(playerId)) return false;
		
		if (pVoice) 
		{
			pVoice->SetVolumePlayer(playerId, volume);
			return true;
		}
		return false;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_setVoiceActive(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (!pVoice) return false;
		if (!pSettings) return false;
		
		if (b)
		{
			pSettings->GetWrite().iVoice = 1;
			pSettings->Save();
			
			pVoice->SetNetworkState(VOICECHAT_WAIT_CONNECT);
			return true;
		}
		else
		{
			pSettings->GetWrite().iVoice = 0;
			pSettings->Save();
			
			pVoice->FullDisconnect();
			return true;
		}
		return false;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_rockstargames_gui_voice_DialogVoiceSettings_setVoiceListActive(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (!pVoice) return false;
		if (!pSettings) return false;
		
		pSettings->GetWrite().iVoiceList = b;
		pSettings->Save();
		return true;
	}
}

void CVoice::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CVoice class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CVoice done!"));
}