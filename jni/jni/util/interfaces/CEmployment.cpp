#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"
#include "../game/CFirstPersonCamera.h"

#include "CEmployment.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CEmployment::activity;

void CNetGame::packetEmployment(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		bool status;
		bs.Read(status);
		
		CEmployment::empShow(status);
	}
	else if (typeInfo == 1) CEmployment::empHide();
	else if (typeInfo == 2)
	{
		char text[2][256];
		for (int i = 0; i < 2; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CEmployment::empHeaderText(text[0], text[1]);
	}
	else if (typeInfo == 3)
	{
		bool status;
		bs.Read(status);
		
		CEmployment::empTopTable(status);
	}
	else if (typeInfo == 4)
	{
		bool status;
		bs.Read(status);
		
		bool status2;
		bs.Read(status2);
		
		int32_t max;
		bs.Read(max);
		
		int32_t progress;
		bs.Read(progress);
		
		char text[3][256];
		for (int i = 0; i < 3; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CEmployment::empStatistic(status, status2, max, progress, text[0], text[1], text[2]);
	}
	else if (typeInfo == 5)
	{
		char text[2][256];
		for (int i = 0; i < 2; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CEmployment::empGlobalInfo(text[0], text[1]);
	}
	else if (typeInfo == 6)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int q = 0; q < max_items; q ++)
		{
			int8_t icon;
			bs.Read(icon);
			
			char text[4][256];
			for (int i = 0; i < 4; i ++) memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[0], bLen);
			text[0][bLen] = '\0';
			
			int32_t max;
			bs.Read(max);
			
			int32_t progress;
			bs.Read(progress);
			
			for (int i = 1; i < 4; i ++)
			{
				bs.Read(bLen);
				bs.Read(text[i], bLen);
				text[i][bLen] = '\0';
			}
			
			CEmployment::empTaskItem(icon, text[0], max, progress, text[1], text[2], text[3]);
		}
	}
	else if (typeInfo == 7)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int q = 0; q < max_items; q ++)
		{
			int8_t type;
			bs.Read(type);
			
			int32_t model;
			bs.Read(model);
			
			uint8_t color1;
			bs.Read(color1);
			
			uint8_t color2;
			bs.Read(color2);
			
			float rotX;
			bs.Read(rotX);
			
			float rotY;
			bs.Read(rotY);
			
			float rotZ;
			bs.Read(rotZ);
			
			float zoom;
			bs.Read(zoom);
			
			char text[256];
			memset(text, 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
			
			CEmployment::empSubTaskItem(type, model, color1, color2, rotX, rotY, rotZ, zoom, text);
		}
	}
	else if (typeInfo == 8)
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
			
			CEmployment::empHistoryItem(text[0], text[1], text[2], text[3], text[4]);
		}
	}
}

void CEmployment::empShow(bool status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empShow"), OBFUSCATE("(Z)V"));
	env->CallVoidMethod(activity, method, status);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CEmployment::empHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CEmployment::empHeaderText(char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empHeaderText"), OBFUSCATE("([B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CEmployment::empTopTable(bool status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empTopTable"), OBFUSCATE("(Z)V"));
	env->CallVoidMethod(activity, method, status);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CEmployment::empStatistic(bool status, bool status2, int max, int progress, char* text, char* text2, char* text3)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empStatistic"), OBFUSCATE("(ZZII[B[B[B)V"));
	env->CallVoidMethod(activity, method, status, status2, max, progress, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CEmployment::empGlobalInfo(char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empGlobalInfo"), OBFUSCATE("([B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CEmployment::empTaskItem(int icon, char* text, int max, int progress, char* text2, char* text3, char* text4)
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

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empTaskItem"), OBFUSCATE("(I[BII[B[B[B)V"));
	env->CallVoidMethod(activity, method, icon, bytes, max, progress, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CEmployment::empSubTaskItem(int type, int32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empSubTaskItem"), OBFUSCATE("(IIIIFFFF[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CEmployment::empHistoryItem(char* text, char* text2, char* text3, char* text4, char* text5)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("empHistoryItem"), OBFUSCATE("([B[B[B[B[B)V"));
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
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_fraction_EmploymentManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Fraction);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CEmployment::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CEmployment class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CEmployment done!"));
}