#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CMusic.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CMusic::activity;

void CMusic::ShowAudioSelector(int AudioId)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showAudioSelector"), OBFUSCATE("(I)V"));
    env->CallVoidMethod(activity, method, AudioId);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CMusic::HideAudioSelector()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideAudioSelector"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

int UsedRecorder = 0;
char MusicText[300];
void CMusic::ShowAudioMenu(int usedRecorder, int ButtonHand, int ButtonInventory, int VolumeMusic, bool ActiveMusic, const char* MusicName)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
	UsedRecorder = usedRecorder;
	
	char jByte[300 + 1];
   	jbyteArray bytes = env->NewByteArray(sizeof(jByte));
	env->SetByteArrayRegion(bytes, 0, sizeof(jByte), (jbyte*)MusicName);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showAudioMenu"), OBFUSCATE("(IIIZ[B)V"));
   	env->CallVoidMethod(activity, method, ButtonHand, ButtonInventory, VolumeMusic, ActiveMusic, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CMusic::HideAudioMenu()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideAudioMenu"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_audiosystem_AudioMenuManager_onPressedButton(JNIEnv* pEnv, jobject thiz, jint i)
	{
		if (i == 1)
		{
			CMusic::ShowAudioSelector(0);
		}
		else if (i == 2)
		{
			CMusic::ShowAudioSelector(1);
		}
		else if (i == 3)
		{
			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_PlayAudioStreams);

			uint8_t iStrlen = strlen(MusicText);
			bsParams.Write(i);
			bsParams.Write(iStrlen);
			bsParams.Write(MusicText, iStrlen);
			pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		}
		else if (i == 4)
		{
			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_PlayAudioStreams);

			uint8_t iStrlen = strlen(MusicText);
			bsParams.Write(i);
			bsParams.Write(iStrlen);
			bsParams.Write(MusicText, iStrlen);
			pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		}
		else if (i == 5) 
		{
			if (pSettings->GetWrite().iVolumeStream + 1 < 100)
			{
				int _volume;
				_volume = pSettings->GetWrite().iVolumeStream = pSettings->GetWrite().iVolumeStream + 1;
				if (_volume > 0)
				{
					pNetGame->GetStreamPool()->SetStreamVolumeJava(_volume);
				}
				else
				{
					pNetGame->GetStreamPool()->StopIndividualStream();
					pNetGame->GetStreamPool()->Deactivate();
				}
				
				bool _status = (pSettings->GetWrite().iVolumeActivate == 1) ? (true) : (false);
				CMusic::ShowAudioMenu(UsedRecorder, 0, 0, pSettings->GetWrite().iVolumeStream, _status, MusicText);
				
				pSettings->Save();
			}
		}
		else if (i == 6) 
		{
			if (pSettings->GetWrite().iVolumeStream - 1 > 0)
			{
				int _volume;
				_volume = pSettings->GetWrite().iVolumeStream = pSettings->GetWrite().iVolumeStream - 1;
				if (_volume > 0)
				{
					pNetGame->GetStreamPool()->SetStreamVolumeJava(_volume);
				}
				else
				{
					pNetGame->GetStreamPool()->StopIndividualStream();
					pNetGame->GetStreamPool()->Deactivate();
				}
				
				bool _status = (pSettings->GetWrite().iVolumeActivate == 1) ? (true) : (false);
				CMusic::ShowAudioMenu(UsedRecorder, 0, 0, pSettings->GetWrite().iVolumeStream, _status, MusicText);
				
				pSettings->Save();
			}
		}
		else if (i == 7) 
		{
			if (pSettings->GetWrite().iVolumeActivate == 1) 
			{
				pSettings->GetWrite().iVolumeActivate = 0;
				pNetGame->GetStreamPool()->StopIndividualStream();
				pNetGame->GetStreamPool()->Deactivate();
			}
			else
			{
				pSettings->GetWrite().iVolumeActivate = 1;
				pNetGame->GetStreamPool()->Activate();
			}
			
			bool _status = (pSettings->GetWrite().iVolumeActivate == 1) ? (true) : (false);
			CMusic::ShowAudioMenu(UsedRecorder, 0, 0, pSettings->GetWrite().iVolumeStream, _status, MusicText);
				
			pSettings->Save();
		}
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_audiosystem_AudioSelectManager_onSelectedItem(JNIEnv* pEnv, jobject thiz, jbyteArray bArr, jbyteArray bArr2, jint i, jint i2)
	{
		if (!bArr || !bArr2) return;
		
		jboolean isCopy = true;
		jbyte* pMsg = pEnv->GetByteArrayElements(bArr, &isCopy);
		jsize length = pEnv->GetArrayLength(bArr);
		std::string szStr((char*)pMsg, length);
		
		pEnv->ReleaseByteArrayElements(bArr, pMsg, JNI_ABORT);
		
		jboolean isCopy2 = true;
		jbyte* pMsg2 = pEnv->GetByteArrayElements(bArr2, &isCopy2);
		jsize length2 = pEnv->GetArrayLength(bArr2);
		std::string szStr2((char*)pMsg2, length2);
		
		pEnv->ReleaseByteArrayElements(bArr2, pMsg2, JNI_ABORT);

		char buff[300];
		memset(buff, 0, 300);
		strcpy(buff, szStr.c_str());
		
		char buff2[300];
		memset(buff2, 0, 300);
		strcpy(buff2, szStr2.c_str());
		
		if (UsedRecorder > 0)
		{
			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_PlayAudioStreams);

			uint8_t _zero = 0;
			uint8_t iStrlen = strlen(buff);
			bsParams.Write(_zero);
			bsParams.Write(iStrlen);
			bsParams.Write(buff, iStrlen);
			pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		}
		else
		{
			pNetGame->GetStreamPool()->PlayIndividualStream(&buff[0]);
		}
		
		memset(MusicText, 0, 300);
		strcpy(MusicText, buff2);
	}
}

void CMusic::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CMusic class"));
	memset(MusicText, 0, 300);
	strcpy(MusicText, "-");
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CMusic done!"));
}