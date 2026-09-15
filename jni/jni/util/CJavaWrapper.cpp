#include <GLES2/gl2.h>

#include "CJavaWrapper.h"
#include "../main.h"
#include "../dialog.h"
void DialogWindowInputHandler(const char* str);

extern "C" JavaVM* javaVM;
#include <vector>

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/game.h"
#include "../voice/CVoiceChatClient.h"

#include "../gui/gui.h"
#include "../gui/buttons.h"
#include "..//scoreboard.h"
#include "../game/snapshothelper.h"

#include "/interfaces/CHud.h"
#include "/interfaces/CTab.h"
#include "/interfaces/CNewBattlePass.h"
#include "/interfaces/CBattlePass.h"
#include "/interfaces/CNewBieBattlePass.h"
#include "/interfaces/CDonate.h"
#include "/interfaces/CCasino.h"
#include "/interfaces/CValentine.h"
#include "/interfaces/CInventory.h"
#include "/interfaces/CBoats.h"
#include "/interfaces/CE2y.h"
#include "/interfaces/CChat.h"
#include "/interfaces/CSpeedometr.h"
#include "/interfaces/CNoty.h"
#include "/interfaces/CMusic.h"
#include "/interfaces/CAction.h"
#include "/interfaces/CTune.h"
#include "/interfaces/CFamily.h"
#include "/interfaces/CEmployment.h"
#include "/interfaces/CShop.h"
#include "/interfaces/CTrade.h"
#include "/interfaces/CLoading.h"
#include "/interfaces/CRent.h"
#include "/interfaces/CVoice.h"
#include "/interfaces/CParking.h"
#include "/interfaces/CMarket.h"
#include "/interfaces/CCraft.h"
#include "/interfaces/CRoulette.h"
#include "/interfaces/CEdit.h"
#include "/interfaces/CRecon.h"
#include "/interfaces/CAzs.h"
#include "/interfaces/CAdminTools.h"
#include "/interfaces/CContainer.h"
#include "/interfaces/CFishing.h"
#include "/interfaces/CDialogs.h"
#include "/interfaces/CBinders.h"
#include "/interfaces/CQuests.h"
#include "/interfaces/CAuthReg.h"
#include "/interfaces/CPlates.h"
#include "/interfaces/CSpawnMenu.h"
#include "/interfaces/CBusiness.h"

#include "armhook.h"

extern CVoiceChatClient* pVoice;
extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CScoreBoard* pScoreBoard;
extern CSettings* pSettings;
extern CNetGame* pNetGame;
extern CGame* pGame;
extern CGUI* pGUI;
extern CSnapShotHelper* pSnapShotHelper;
extern CDialogWindow* pDialogWindow;

extern uint8_t m_SelectedServer;
int VoicePlayerID = 0;

void* ButtonsPressed(void* g_button)
{
    for (int i = 0; i < 50; i++)
    {   
    	usleep(5000);
    	*(bool*)g_button = true;  
    }
	pthread_exit(0);
	return (void*)0;
}
pthread_t threadButtons;

JNIEnv* CJavaWrapper::GetEnv()
{
	JNIEnv* env = nullptr;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

	if (getEnvStat == JNI_EDETACHED)
	{
		Log(OBFUSCATE("GetEnv: not attached"));
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		{
			Log(OBFUSCATE("Failed to attach"));
			return nullptr;
		}
	}
	if (getEnvStat == JNI_EVERSION)
	{
		Log(OBFUSCATE("GetEnv: version not supported"));
		return nullptr;
	}

	if (getEnvStat == JNI_ERR)
	{
		Log(OBFUSCATE("GetEnv: JNI_ERR"));
		return nullptr;
	}

	return env;
}

std::string CJavaWrapper::GetClipboardString()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return std::string("");
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("getClipboardText"), OBFUSCATE("()[B"));
	auto retn = (jbyteArray)env->CallObjectMethod(activity, method);
	env->DeleteLocalRef(clazz);

	if ((env)->ExceptionCheck())
	{
		(env)->ExceptionDescribe();
		(env)->ExceptionClear();
		return std::string("");
	}

	if (!retn)
	{
		return std::string("");
	}

	jboolean isCopy = true;

	jbyte* pText = env->GetByteArrayElements(retn, &isCopy);
	jsize length = env->GetArrayLength(retn);

	std::string str((char*)pText, length);

	env->ReleaseByteArrayElements(retn, pText, JNI_ABORT);
	env->DeleteLocalRef(retn);
	return str;
}

void CJavaWrapper::ShowInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showInputLayout"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideInputLayout"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowClientSettings()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showClientSettings"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText)
{
    JNIEnv* env = GetEnv();
	
    if (!env)
    {
		Log(OBFUSCATE("No env"));
		return;
    }
	
    /*jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");*/
    jbyteArray bytes = env->NewByteArray(strlen(caption));
    env->SetByteArrayRegion(bytes, 0, strlen(caption), (jbyte*)caption);
    //jstring str1 = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
    //
    /*jclass strClass1 = env->FindClass("java/lang/String");
    jmethodID ctorID1 = env->GetMethodID(strClass1, "<init>", "([BLjava/lang/String;)V");
    jstring encoding1 = env->NewStringUTF("UTF-8");*/
    jbyteArray bytes1 = env->NewByteArray(strlen(content));
    env->SetByteArrayRegion(bytes1, 0, strlen(content), (jbyte*)content);
    //jstring str2 = (jstring)env->NewObject(strClass1, ctorID1, bytes1, encoding1);
    //
    /*jclass strClass2 = env->FindClass("java/lang/String");
    jmethodID ctorID2 = env->GetMethodID(strClass2, "<init>", "([BLjava/lang/String;)V");
    jstring encoding2 = env->NewStringUTF("UTF-8");*/
    jbyteArray bytes2 = env->NewByteArray(strlen(leftBtnText));
    env->SetByteArrayRegion(bytes2, 0, strlen(leftBtnText), (jbyte*)leftBtnText);
    //jstring str3 = (jstring)env->NewObject(strClass2, ctorID2, bytes2, encoding2);
    //
    /*jclass strClass3 = env->FindClass("java/lang/String");
    jmethodID ctorID3 = env->GetMethodID(strClass3, "<init>", "([BLjava/lang/String;)V");
    jstring encoding3 = env->NewStringUTF("UTF-8");*/
    jbyteArray bytes3 = env->NewByteArray(strlen(rightBtnText));
    env->SetByteArrayRegion(bytes3, 0, strlen(rightBtnText), (jbyte*)rightBtnText);
    //jstring str4 = (jstring)env->NewObject(strClass3, ctorID3, bytes3, encoding3);

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showDialog"), OBFUSCATE("(II[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, dialogId, dialogTypeId, bytes, bytes1, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes1);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetUseFullScreen(int b)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("setUseFullscreen"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, b);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowButton()
{
	JNIEnv* env = GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showButton"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideButton()
{
	JNIEnv* env = GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideButton"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetPauseState(bool status)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("setPauseState"), OBFUSCATE("(Z)V"));
    env->CallVoidMethod(activity, method, status);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CJavaWrapper::openSelectorServer()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("openSelectorServer"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern uint32_t bCleanJavaModelPool;
static uint8_t g_pPixelsBuff[512 * 512 * sizeof(RwRGBA)];
void CJavaWrapper::UpdateSlot(uint32_t slot, int32_t type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, int32_t scaleX, int32_t scaleY, bool isBig)
{
    if (slot == 255) return;

	if (scaleX < 1 || scaleX > 512 || scaleY < 1 || scaleY > 512)
	{
		if (!isBig) scaleX = 256, scaleY = 256;
		else scaleX = 512, scaleY = 512;
	}
	if (scaleX != scaleY) scaleY = scaleX;
	
    VECTOR vecRotation { rotX, rotY, rotZ };
    RwTexture* pTexture = nullptr;
	memset(g_pPixelsBuff, 0, 512 * 512 * sizeof(RwRGBA));
    if (IsPedModel(model))
	{
		pTexture = pSnapShotHelper->CreatePedSnapShots(model, 0x00000000, &vecRotation, zoom, scaleX, scaleY );
		//if (pTexture) pGame->RemoveModel(model, true);
	}
    else if (model >= 400 && model <= 611 || model >= 3217 && model <= 3500)
	{
		pTexture = pSnapShotHelper->CreateVehicleSnapShots(model, 0x00000000, &vecRotation, zoom, color1, color2, scaleX, scaleY );
		//pGame->RemoveModel(model, true);
	}
	else 
	{
		pTexture = pSnapShotHelper->CreateObjectSnapShots(model, 0x00000000, &vecRotation, zoom, scaleX, scaleY );
		//if (pTexture) pGame->RemoveModel(model, true);
	}

	if (pTexture)
	{
		uintptr_t oldTarget = *(uintptr_t*)(g_libGTASA + 0x0061B3D8);
		uintptr_t RasterExtOffset_local = *(uintptr_t*)(g_libGTASA + 0x00611844);
		uintptr_t renderTarget = *(uintptr_t*)((char*)(pTexture->raster) + RasterExtOffset_local + 24);

		((void (*)(uintptr_t, bool))(g_libGTASA + 0x001A6F98 + 1))(renderTarget, 1); // RQRenderTarget::Select
		((void (*)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)) // emu_glReadPixels
			(g_libGTASA + 0x0018D528 + 1))(0, 0, scaleX, scaleY, GL_RGBA, GL_UNSIGNED_BYTE, g_pPixelsBuff);

		g_pJavaWrapper->JavaSnapshotProcess(slot, scaleX * scaleY * sizeof(RwRGBA), g_pPixelsBuff, scaleX, scaleY);

		((void (*)(uintptr_t, bool))(g_libGTASA + 0x001A6F98 + 1))(oldTarget, 1); // RQRenderTarget::Select

		RwTextureDestroy(pTexture);
		bCleanJavaModelPool = GetTickCount();
		
		CSnapItemDelete item {model};
		g_pJavaWrapper->m_vecSnapItemsDelete.push_back(item);
	}
    return;
}

void CJavaWrapper::JavaSnapshotProcess(uint32_t CountId, int len, unsigned char * buffer, int pixelX, int pixelY)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jbyteArray bArray = env->NewByteArray(len);
    env->SetByteArrayRegion(bArray, 0, len, (jbyte*)buffer);
	
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("onNativeRendered"), OBFUSCATE("(I[BII)V"));
	env->CallVoidMethod(activity, method, CountId, bArray, pixelX, pixelY);
	env->DeleteLocalRef(bArray);
	env->DeleteLocalRef(clazz);
	
	EXCEPTION_CHECK(env);
}

void CJavaWrapper::showFingerPrintAuth(int id, char* text) 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);
	
	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showFingerPrintAuth"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, id, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::CheckAndroidID(int8_t id)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("CheckAndroidID"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, id);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowRadialMenu(int8_t i, int8_t i2, int8_t i3, int8_t i4, int8_t i5, int8_t i6, int8_t i7, int8_t i8, int8_t i9, int8_t i10)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("ShowRadialMenu"), OBFUSCATE("(IIIIIIIIII)V"));
	env->CallVoidMethod(activity, method, i, i2, i3, i4, i5, i6, i7, i8, i9, i10);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideRadialMenu()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("HideRadialMenu"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::setCheckGPU()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("setCheckGPU"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::setCheckFiles()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("setCheckFiles"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::finishActivity()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("finishActivity"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

enum
{
	BUTTON_TAB = 1,
	BUTTON_ALT,
	BUTTON_CTRL,
	BUTTON_F,
	BUTTON_Y,
	BUTTON_N,
	BUTTON_H,
	BUTTON_BIND,
	BUTTON_ACTION,
	BUTTON_ACTION1
} ;

bool ProcessVoiceCommands(const char* str);
int CustomTickRate = GetTickCount();
extern bool m_bChatStarted;
extern uint8_t m_PortVoice;
#include "..//CDebugInfo.h"
extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_input_InputManager_onInputEnd(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		jboolean isCopy = true;
		jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);
		std::string szStr((char*)pMsg, length);

		char buff[144];
		strcpy(buff, szStr.c_str());
		
		if (buff[0] == '/')
		{
			if (ProcessVoiceCommands(buff))
			{
				return;
			}
		}
		
		if (pKeyBoard)
		{
			pKeyBoard->OnNewKeyboardInput(pEnv, thiz, str);
		}
		
		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
	}
	
	/*JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendDialogResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3, jbyteArray str)
	{
		const char *inputText = pEnv->GetStringUTFChars(str, nullptr);
		char inputtext1[2048];
		cp1251_to_utf8(inputtext1, inputText, 2048);

		if(pNetGame) {
			pNetGame->SendDialogResponse(i2, i, i3, (char*)inputtext1);
			pGame->FindPlayerPed()->TogglePlayerControllable(true);
		}

		Log("sendDialogResponse: dialogid - %d, listitem - %d, inputtext1 - %s, inputText - %s", i2, i3, inputtext1, inputText);

		pEnv->ReleaseStringUTFChars(str, inputText);
	}*/
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_dialogs_DialogManager_sendDialogResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pText = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pText, length);
		
		pEnv->ReleaseByteArrayElements(str, pText, JNI_ABORT);

		char inputtext[2048], inputtext1[2048];
		memset(inputtext, 0, 2048);
		memset(inputtext1, 0, 2048);
		strcpy(inputtext, std::string(szStr).c_str());
		cp1251_to_utf8(inputtext1, std::string(szStr).c_str(), 2048);

		if(pNetGame) 
		{
			pNetGame->SendDialogResponse(i2, i, i3, inputtext);
			if (!pNetGame->GetTextDrawPool()->m_bSelectState) pGame->FindPlayerPed()->TogglePlayerControllable(true);
		}

		Log("sendDialogResponse: dialogid - %d, listitem - %d, inputtext1 - %s", i2, i3, inputtext1);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_buttonpanel_ButtonPanelManager_onReturnButtonId(JNIEnv* pEnv, jobject thiz, jint buttonId)
	{
		if (pScoreBoard->GetToggle())
		{
			pScoreBoard->Toggle();
			return;
		}
			
		if (buttonId == BUTTON_TAB) pScoreBoard->Toggle();
		else if (buttonId == BUTTON_ALT)
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed->IsInVehicle()) pthread_create(&threadButtons, nullptr, ButtonsPressed, (void*)&LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]);
			else pthread_create(&threadButtons, nullptr, ButtonsPressed, (void*)&LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]);
		}
		else if (buttonId == BUTTON_CTRL) LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;
		else if (buttonId == BUTTON_F) LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
		else if (buttonId == BUTTON_H) LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;
		else if (buttonId == BUTTON_Y) LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
		else if (buttonId == BUTTON_N) LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
		else if (buttonId == BUTTON_BIND) 
		{
			if (GetTickCount() - CustomTickRate > 500)
			{
				CustomTickRate = GetTickCount();
				uint8_t bit = 1 ;

				RakNet::BitStream bsParams;
				bsParams.Write((uint8_t)ID_CUSTOM_RPC);
				bsParams.Write((uint32_t)RPC_ModeBinder);
				
				bsParams.Write(bit);
				pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
			}
		}
		else if (buttonId == BUTTON_ACTION) 
		{
			if (GetTickCount() - CustomTickRate > 500)
			{
				CustomTickRate = GetTickCount();
				uint8_t bit = 1 ;

				RakNet::BitStream bsParams;
				bsParams.Write((uint8_t)ID_CUSTOM_RPC);
				bsParams.Write((uint32_t)RPC_ModeSettings);
				
				bsParams.Write(bit);
				pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
			}
		}
		else if (buttonId == BUTTON_ACTION1) 
		{
			if (GetTickCount() - CustomTickRate > 500)
			{
				CustomTickRate = GetTickCount();
				uint8_t bit = 1 ;
				
				RakNet::BitStream bsParams;
				bsParams.Write((uint8_t)ID_CUSTOM_RPC);
				bsParams.Write((uint32_t)RPC_ModePlayer);
				
				bsParams.Write(bit);
				pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
			}
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_requestSnapShot(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3, jint i4, jint i5, jfloat f, jfloat f2, jfloat f3, jfloat f4, jint i6, jint i7, jboolean b)
	{
		CSnapItem item {(uint32_t)i2, (int32_t)i, (uint32_t)i3, (uint8_t)i4, (uint8_t)i5, (float)f, (float)f2, (float)f3, (float)f4, (int32_t)i6, (int32_t)i7, (bool)b};
		g_pJavaWrapper->m_vecSnapItemsQueue.push_back(item);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_nativeRequestRenderTexture(JNIEnv* pEnv, jobject thiz, jbyteArray bArr, jint i)
	{
		if (!bArr) return;
		
		jboolean isCopy = true;
		jbyte* pMsg = pEnv->GetByteArrayElements(bArr, &isCopy);
		jsize length = pEnv->GetArrayLength(bArr);
		std::string szStr((char*)pMsg, length);
		
		pEnv->ReleaseByteArrayElements(bArr, pMsg, JNI_ABORT);

		char buff[300];
		memset(buff, 0, 300);
		strcpy(buff, szStr.c_str());
		
		/*RwTexture* rwTexture = (RwTexture*)LoadTextureFromDB("samp", buff);
		Log("rwTexture: %s", rwTexture);
		g_pJavaWrapper->JavaSnapshotProcess(i, (char*)rwTexture, 128, 128);*/
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSelectServer(JNIEnv* pEnv, jobject thiz, jstring serverIP, jint serverPort)
	{
		const char* inputText = pEnv->GetStringUTFChars(serverIP, 0);
		pEnv->ReleaseStringUTFChars(serverIP, inputText);
		
		char inputtext[32];
		memset(inputtext, 0, 32);
		cp1251_to_utf8(inputtext, inputText, 32);
		//Log("Server: %s, serverPort: %d", inputtext, serverPort);
		pNetGame = new CNetGame((const char*)inputtext, serverPort, pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
		m_bChatStarted = true;
			
		g_pJavaWrapper->setCheckFiles();
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onEventBackPressed(JNIEnv* pEnv, jobject thiz)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsOpen())
			{
				pKeyBoard->Close();
			}
		}
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNativeHeightChanged(JNIEnv* pEnv, jobject thiz, jint orientation, jint height)
	{
		if (pChatWindow)
		{
			pChatWindow->SetLowerBound(height);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeCutoutSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iCutout = b;
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeKeyboardSettings(JNIEnv* pEnv, jobject thiz, jint i)
	{
		if (pSettings)
		{
			if (i < 1) i = 1;
			/*if(pSettings->GetReadOnly().iChat == 1)
			{
				if (i < 1) i = 1;
			}*/
			pSettings->GetWrite().iAndroidKeyboard = i;
		}

		if (pKeyBoard && i)
		{
			pKeyBoard->EnableNewKeyboard(pSettings->GetWrite().iAndroidKeyboard);
		}
		else if(pKeyBoard)
		{
			pKeyBoard->EnableOldKeyboard();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeFpsCounterSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iFPSCounter = b;
		}

		CDebugInfo::SetDrawFPS(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHpArmourText(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			if (!pSettings->GetWrite().iHPArmourText && b)
			{
				if (CAdjustableHudColors::IsUsingHudColor(HUD_HP_TEXT) == false)
				{
					CAdjustableHudColors::SetHudColorFromRGBA(HUD_HP_TEXT, 255, 0, 0, 255);
				}
				if (CAdjustableHudPosition::GetElementPosition(HUD_HP_TEXT).X == -1 || CAdjustableHudPosition::GetElementPosition(HUD_HP_TEXT).Y == -1)
				{
					CAdjustableHudPosition::SetElementPosition(HUD_HP_TEXT, 500, 500);
				}
				if (CAdjustableHudScale::GetElementScale(HUD_HP_TEXT).X == -1 || CAdjustableHudScale::GetElementScale(HUD_HP_TEXT).Y == -1)
				{
					CAdjustableHudScale::SetElementScale(HUD_HP_TEXT, 400, 400);
				}

				if (CAdjustableHudColors::IsUsingHudColor(HUD_ARMOR_TEXT) == false)
				{
					CAdjustableHudColors::SetHudColorFromRGBA(HUD_ARMOR_TEXT, 255, 0, 0, 255);
				}
				if (CAdjustableHudPosition::GetElementPosition(HUD_ARMOR_TEXT).X == -1 || CAdjustableHudPosition::GetElementPosition(HUD_ARMOR_TEXT).Y == -1)
				{
					CAdjustableHudPosition::SetElementPosition(HUD_ARMOR_TEXT, 300, 500);
				}
				if (CAdjustableHudScale::GetElementScale(HUD_ARMOR_TEXT).X == -1 || CAdjustableHudScale::GetElementScale(HUD_ARMOR_TEXT).Y == -1)
				{
					CAdjustableHudScale::SetElementScale(HUD_ARMOR_TEXT, 400, 400);
				}
			}

			pSettings->GetWrite().iHPArmourText = b;
		}

		CInfoBarText::SetEnabled(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iOutfitGuns = b;

			CWeaponsOutFit::SetEnabled(b);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativePcMoney(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iPCMoney = b;
		}

		CGame::SetEnabledPCMoney(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeRadarrect(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iRadarRect = b;

			CRadarRect::SetEnabled(b);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeNameTag(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iNameTag = b;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNative3DText(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().i3DText = b;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeVoice(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iVoice = b;
			
			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_VoiceChat);
				
			uint8_t voiceStatus = pSettings->GetWrite().iVoice;
			uint8_t chatStatus = pSettings->GetReadOnly().iChat;
			bsParams.Write(voiceStatus);
			bsParams.Write(chatStatus);
			pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		}
		
		if (pVoice)
		{
			if (b)
			{
				if (pVoice->IsDisconnected())
					pVoice->Connect(pNetGame->m_szHostOrIp, pNetGame->m_iPort + m_PortVoice);
			}
			else pVoice->FullDisconnect();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeSkyBox(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iSkyBox = b;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeSkyBox(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iSkyBox;

		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNative3DLabel(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().bLabelBg;

		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNative3DLabel(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().bLabelBg = b;
			pGUI->bLabelBackground = (b == 1) ? (true) : (false);
		}
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeTimeStamp(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().szTimeStamp;

		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeTimeStamp(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().szTimeStamp = b;
			pGUI->timestamp = (b == 1) ? (true) : (false);
		}
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeAlphaChat(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pChatWindow->GetChatDissappearTimeout();
		}
		
		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeAlphaChat(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			if (pChatWindow)
				pChatWindow->SetChatDissappearTimeout(b, b);
		}
	}
	
	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeDialog(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetWrite().iDialog;
		}
		
		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeDialog(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iDialog = b;
		}
	}
	
	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHud(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iHUD;
		}
		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHud(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			uint8_t i_hud = (b == true) ? (1) : (0);
			pSettings->GetWrite().iHUD = i_hud;
				
			if (CButtons::m_iKey[0] == -1 || pSettings->GetReadOnly().iHUD != 1)
			{
				CButtons::m_bIsShow = false;
				CButtons::m_bIsActive = false;
			}
			else
			{
				CButtons::m_bIsShow = false;
				CButtons::m_bIsActive = true;
			}
			
			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_HudManager);

			bsParams.Write(i_hud);
			pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		}
	}
	
	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeNewChat(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iChat;
		}
		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeNewChat(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iChat = b;
			
			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_VoiceChat);

			uint8_t voiceStatus = pSettings->GetWrite().iVoice;
			uint8_t chatStatus = pSettings->GetReadOnly().iChat;
			bsParams.Write(voiceStatus);
			bsParams.Write(chatStatus);
			pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
		}
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeCutoutSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iCutout;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeKeyboardSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			bool b;
			if (pSettings->GetReadOnly().iAndroidKeyboard > 0) b = true;
			else b = false;
			return b;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeFpsCounterSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iFPSCounter;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHpArmourText(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iHPArmourText;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iOutfitGuns;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativePcMoney(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iPCMoney;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeRadarrect(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iRadarRect;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNative3DText(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().i3DText;

		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeNameTag(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iNameTag;

		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeVoice(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iVoice;

		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowSave(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			pSettings->Save();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowDefaults(JNIEnv* pEnv, jobject thiz, jint category)
	{
		if (pSettings)
		{
			pSettings->ToDefaults(category);
			if (pChatWindow)
			{
				pChatWindow->m_bPendingReInit = true;
			}
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementColor(JNIEnv* pEnv, jobject thiz, jint id, jint a, jint r, jint g, jint b)
	{
		CAdjustableHudColors::SetHudColorFromRGBA((E_HUD_ELEMENT)id, r, g, b, a);
	}

	JNIEXPORT jbyteArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementColor(JNIEnv* pEnv, jobject thiz, jint id)
	{
		char pTemp[9];
		jbyteArray color = pEnv->NewByteArray(sizeof(pTemp));

		if (!color)
		{
			return nullptr;
		}

		pEnv->SetByteArrayRegion(color, 0, sizeof(pTemp), (const jbyte*)CAdjustableHudColors::GetHudColorString((E_HUD_ELEMENT)id).c_str());
		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementPosition(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
		if (id == 7)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fChatPosX = x;
				pSettings->GetWrite().fChatPosY = y;
				if (pChatWindow)
				{
					pChatWindow->m_bPendingReInit = true;
				}
				return;
			}
			return;
		}
		if (id == HUD_SNOW)
		{
			if (pSettings)
			{
				pSettings->GetWrite().iSnow = x;
			}
			CSnow::SetCurrentSnow(pSettings->GetReadOnly().iSnow);
			return;
		}
		CAdjustableHudPosition::SetElementPosition((E_HUD_ELEMENT)id, x, y);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
		CAdjustableHudScale::SetElementScale((E_HUD_ELEMENT)id, x, y);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(2);

		if (!color)
		{
			return nullptr;
		}
		int arr[2];
		arr[0] = CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)id).X;
		arr[1] = CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)id).Y;
		pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)& arr[0]);
		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeWidgetPositionAndScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y, jint scale)
	{
		if (id == 0)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonMicrophoneX = x;
				pSettings->GetWrite().fButtonMicrophoneY = y;
				pSettings->GetWrite().fButtonMicrophoneSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_MICROPHONE))
				{
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetWidth(scale);
				}
			}
		}
		
		if (id == 1)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonEnterPassengerX = x;
				pSettings->GetWrite().fButtonEnterPassengerY = y;
				pSettings->GetWrite().fButtonEnterPassengerSize = scale;
			}
		}

		if (id == 2)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonCameraCycleX = x;
				pSettings->GetWrite().fButtonCameraCycleY = y;
				pSettings->GetWrite().fButtonCameraCycleSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
				{
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetWidth(scale);
				}
			}
		}
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementPosition(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(2);

		if (!color)
		{
			return nullptr;
		}
		int arr[2];

		if (id == 7 && pSettings)
		{
			arr[0] = pSettings->GetReadOnly().fChatPosX;
			arr[1] = pSettings->GetReadOnly().fChatPosY;
		}
		else if (id == HUD_SNOW && pSettings)
		{
			arr[0] = CSnow::GetCurrentSnow();
			arr[1] = CSnow::GetCurrentSnow();
		}
		else
		{
			arr[0] = CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)id).X;
			arr[1] = CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)id).Y;
		}

		pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)&arr[0]);
		return color;
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeWidgetPositionAndScale(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(3);

		if (!color)
		{
			return nullptr;
		}
		int arr[3] = { -1, -1, -1 };
		

		if (pSettings)
		{
			if (id == 0)
			{
				arr[0] = pSettings->GetWrite().fButtonMicrophoneX;
				arr[1] = pSettings->GetWrite().fButtonMicrophoneY;
				arr[2] = pSettings->GetWrite().fButtonMicrophoneSize;
			}
			if (id == 1)
			{
				arr[0] = pSettings->GetWrite().fButtonEnterPassengerX;
				arr[1] = pSettings->GetWrite().fButtonEnterPassengerY;
				arr[2] = pSettings->GetWrite().fButtonEnterPassengerSize;
			}
			if (id == 2)
			{
				arr[0] = pSettings->GetWrite().fButtonCameraCycleX;
				arr[1] = pSettings->GetWrite().fButtonCameraCycleY;
				arr[2] = pSettings->GetWrite().fButtonCameraCycleSize;
			}
		}
	
		pEnv->SetIntArrayRegion(color, 0, 3, (const jint*)& arr[0]);
		return color;
	}
}

int getPlayerGPU;
extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_fingerprint_FingerPrintAuth_sendFingerPrintResult(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jbyteArray bArr)
	{
		if (!bArr) return;
		
		jboolean isCopy = true;
		jbyte* pMsg = pEnv->GetByteArrayElements(bArr, &isCopy);
		jsize length = pEnv->GetArrayLength(bArr);
		std::string szStr((char*)pMsg, length);
		
		pEnv->ReleaseByteArrayElements(bArr, pMsg, JNI_ABORT);
		
		char buff[256];
		memset(buff, 0, 256);
		strcpy(buff, szStr.c_str());

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Finger);

		uint8_t _id = i;
		uint8_t _result = i2;
		bsParams.Write(_id);
		bsParams.Write(_result);
		
		uint8_t iStrlen = strlen(buff);
		bsParams.Write(iStrlen);
		bsParams.Write(buff, iStrlen);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendAndroidIDResult(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jbyteArray bArr)
	{
		if (!bArr) return;
		
		jboolean isCopy = true;
		jbyte* pMsg = pEnv->GetByteArrayElements(bArr, &isCopy);
		jsize length = pEnv->GetArrayLength(bArr);
		std::string szStr((char*)pMsg, length);
		
		pEnv->ReleaseByteArrayElements(bArr, pMsg, JNI_ABORT);
		
		char buff[256];
		memset(buff, 0, 256);
		strcpy(buff, szStr.c_str());
		
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Finger);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((uint8_t)strlen(buff));
		bsParams.Write(buff, (uint8_t)strlen(buff));
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_spawnmenu_SpawnMenuManager_sendResponse(JNIEnv* pEnv, jobject thiz, jint i)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_CustomSpawn);

		uint8_t spawnId = i;
		bsParams.Write(spawnId);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_radial_RadialManager_onRadialClicked(JNIEnv* pEnv, jobject thiz, jint i)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_RadialMenu);

		uint8_t actionId = i;
		bsParams.Write(actionId);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getKeyboard(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iAndroidKeyboard;
		}
		return 0;
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getReturnGPU(JNIEnv* pEnv, jobject thiz, jint i)
	{
		Log("getReturnGPU: %d", i);
		getPlayerGPU = i;
		if (getPlayerGPU == 3)
		{
			// pvr
			unProtect(g_libGTASA + 0x573670);
			*(char*)(g_libGTASA + 0x573670 + 12) = 'd';
			*(char*)(g_libGTASA + 0x573670 + 13) = 'x';
			*(char*)(g_libGTASA + 0x573670 + 14) = 't';

			unProtect(g_libGTASA + 0x5736CC);
			*(char*)(g_libGTASA + 0x5736CC + 12) = 'd';
			*(char*)(g_libGTASA + 0x5736CC + 13) = 'x';
			*(char*)(g_libGTASA + 0x5736CC + 14) = 't';

			// etc
			unProtect(g_libGTASA + 0x573684);
			*(char*)(g_libGTASA + 0x573684 + 12) = 'd';
			*(char*)(g_libGTASA + 0x573684 + 13) = 'x';
			*(char*)(g_libGTASA + 0x573684 + 14) = 't';

			unProtect(g_libGTASA + 0x5736DC);
			*(char*)(g_libGTASA + 0x5736DC + 12) = 'd';
			*(char*)(g_libGTASA + 0x5736DC + 13) = 'x';
			*(char*)(g_libGTASA + 0x5736DC + 14) = 't';
		}
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getReturnFiles(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		Log("getReturnFiles: %d", b);
		if (!b)
		{
			Log(OBFUSCATE("Not accept files"));
			exit(0);
		}
	}
}

CJavaWrapper::CJavaWrapper(JNIEnv* env, jobject activity)
{
	Log(OBFUSCATE("CJavaWrapper"));
	
	this->activity = env->NewGlobalRef(activity);

	jclass nvEventClass = env->GetObjectClass(activity);
	if (!nvEventClass)
	{
		Log(OBFUSCATE("nvEventClass null"));
		return;
	}

	CHud::setActivity(env, activity);
	CTab::setActivity(env, activity);
	CNewBattlePass::setActivity(env, activity);
	CBattlePass::setActivity(env, activity);
	CDonate::setActivity(env, activity);
	CCasino::setActivity(env, activity);
	CValentine::setActivity(env, activity);
	CInventory::setActivity(env, activity);
	CBoats::setActivity(env, activity);
	CE2y::setActivity(env, activity);
	CChat::setActivity(env, activity);
	CSpeedometr::setActivity(env, activity);
	CNoty::setActivity(env, activity);
	CMusic::setActivity(env, activity);
	CAction::setActivity(env, activity);
	CTune::setActivity(env, activity);
	CFamily::setActivity(env, activity);
	CEmployment::setActivity(env, activity);
	CShop::setActivity(env, activity);
	CTrade::setActivity(env, activity);
	CLoading::setActivity(env, activity);
	CRent::setActivity(env, activity);
	CVoice::setActivity(env, activity);
	CParking::setActivity(env, activity);
	CMarket::setActivity(env, activity);
	CCraft::setActivity(env, activity);
	CRoulette::setActivity(env, activity);
	CEdit::setActivity(env, activity);
	CRecon::setActivity(env, activity);
	CAzs::setActivity(env, activity);
	CAdminTools::setActivity(env, activity);
	CContainer::setActivity(env, activity);
	CFishing::setActivity(env, activity);
	CDialogs::setActivity(env, activity);
	CBinders::setActivity(env, activity);
	CQuests::setActivity(env, activity);
	CNewBieBattlePass::setActivity(env, activity);
	CAuthReg::setActivity(env, activity);
	CPlates::setActivity(env, activity);
	CSpawnMenu::setActivity(env, activity);
	CBusiness::setActivity(env, activity);
	
	setCheckGPU();
	//s_CallLauncherActivity = env->GetMethodID(nvEventClass, OBFUSCATE("callLauncherActivity"), OBFUSCATE("(I)V"));

	env->DeleteLocalRef(nvEventClass);
	
	Log(OBFUSCATE("CJavaWrapper done!"));
}

CJavaWrapper::~CJavaWrapper()
{
	JNIEnv* pEnv = GetEnv();
	if (pEnv)
	{
		pEnv->DeleteGlobalRef(this->activity);
	}
}

CJavaWrapper* g_pJavaWrapper = nullptr;