#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CParking.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CParking::activity;

void CParking::showParking()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showParking"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CParking::hideParking()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideParking"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CParking::addParking(uint32_t ID, uint8_t Type, uint32_t Model, char* Name, uint8_t Color1, uint8_t Color2, float rotX, float rotY, float rotZ, float Zoom, uint32_t Status, uint8_t Fuel, uint8_t FuelStatus, char* TimeText)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(Name));
	env->SetByteArrayRegion(bytes, 0, strlen(Name), (jbyte*)Name);

   	jbyteArray bytes2 = env->NewByteArray(strlen(TimeText));
	env->SetByteArrayRegion(bytes2, 0, strlen(TimeText), (jbyte*)TimeText);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addParking"), OBFUSCATE("(III[BIIFFFFIII[B)V"));
	env->CallVoidMethod(activity, method, ID, Type, Model, bytes, Color1, Color2, rotX, rotY, rotZ, Zoom, Status, Fuel, FuelStatus, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CParking::parkingDialog(char* caption, char* text, char* buttonNo, char* buttonYes)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(caption));
	env->SetByteArrayRegion(bytes, 0, strlen(caption), (jbyte*)caption);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes2, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes3 = env->NewByteArray(strlen(buttonNo));
	env->SetByteArrayRegion(bytes3, 0, strlen(buttonNo), (jbyte*)buttonNo);

   	jbyteArray bytes4 = env->NewByteArray(strlen(buttonYes));
	env->SetByteArrayRegion(bytes4, 0, strlen(buttonYes), (jbyte*)buttonYes);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("parkingDialog"), OBFUSCATE("([B[B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_parking_ParkingManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		int32_t _i = i;
		int32_t _i2 = i2;
		
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Parking);

		bsParams.Write(_i);
		bsParams.Write(_i2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CParking::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CParking class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CParking done!"));
}