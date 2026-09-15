#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"

#include "CRoulette.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CRoulette::activity;

void CNetGame::packetRoulette(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	
	if (typeInfo == 0) CRoulette::roulleteHide();
	else if (typeInfo == 1)
	{
		uint32_t itemLevel;
		bs.Read(itemLevel);
		
		bool canRoll;
		bs.Read(canRoll);
		
		uint32_t maxItems;
		bs.Read(maxItems);
		
		CRoulette::showRoulette(itemLevel, canRoll, maxItems);
	}
	else if (typeInfo == 2)
	{
		int8_t type;
		bs.Read(type);
				
		uint32_t model;
		bs.Read(model);
				
		uint8_t color1;
		bs.Read(color1);
				
		uint8_t color2;
		bs.Read(color2);
				
		char text[256];
		memset(text, 0, 256);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CRoulette::rouletteShowPrize(type, model, color1, color2, text);
	}
	else if (typeInfo == 3)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t type;
			bs.Read(type);
			
			uint32_t model;
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
			
			uint8_t rareLevel;
			bs.Read(rareLevel);
					
			CRoulette::addRouletteItem(type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel);
		}
	}
	else if (typeInfo == 4)
	{
		int8_t type;
		bs.Read(type);
		
		char text[3][256];
		for (int i = 0; i < 3; i ++)
		{
			memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		CRoulette::rouletteUpdate(type, text[0], text[1], text[2]);
	}
	
	else if (typeInfo == 5) CRoulette::roulleteJobHide();
	else if (typeInfo == 6)
	{
		uint32_t itemLevel;
		bs.Read(itemLevel);
		
		bool canRoll;
		bs.Read(canRoll);
		
		uint32_t maxItems;
		bs.Read(maxItems);
		
		CRoulette::showRouletteJob(itemLevel, canRoll, maxItems);
	}
	else if (typeInfo == 7)
	{
		uint32_t model;
		bs.Read(model);
				
		char text[256];
		memset(text, 0, 256);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CRoulette::rouletteJobShowPrize(model, text);
	}
	else if (typeInfo == 8)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t type;
			bs.Read(type);
			
			uint32_t model;
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
			
			uint8_t rareLevel;
			bs.Read(rareLevel);
				
			char text[256];
			memset(text, 0, 256);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
					
			CRoulette::addRouletteJobItem(type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, text);
		}
	}
}

void CRoulette::roulleteHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("roulleteHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CRoulette::showRoulette(uint32_t itemLevel, bool canRoll, uint32_t maxItems)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showRoulette"), OBFUSCATE("(IZI)V"));
	env->CallVoidMethod(activity, method, itemLevel, canRoll, maxItems);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CRoulette::rouletteShowPrize(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, char* name)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("rouletteShowPrize"), OBFUSCATE("(IIII[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CRoulette::addRouletteItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addRouletteItem"), OBFUSCATE("(IIIIFFFFI)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CRoulette::rouletteUpdate(int8_t type, char* bronze, char* silver, char* gold)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(bronze));
    env->SetByteArrayRegion(bytes, 0, strlen(bronze), (jbyte*)bronze);

    jbyteArray bytes2 = env->NewByteArray(strlen(silver));
    env->SetByteArrayRegion(bytes2, 0, strlen(silver), (jbyte*)silver);

    jbyteArray bytes3 = env->NewByteArray(strlen(gold));
    env->SetByteArrayRegion(bytes3, 0, strlen(gold), (jbyte*)gold);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("rouletteUpdate"), OBFUSCATE("(I[B[B[B)V"));
	env->CallVoidMethod(activity, method, type, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CRoulette::roulleteJobHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("roulleteJobHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CRoulette::showRouletteJob(uint32_t itemLevel, bool canRoll, uint32_t maxItems)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showRouletteJob"), OBFUSCATE("(IZI)V"));
	env->CallVoidMethod(activity, method, itemLevel, canRoll, maxItems);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CRoulette::rouletteJobShowPrize(uint32_t model, char* name)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("rouletteJobShowPrize"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, model, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CRoulette::addRouletteJobItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addRouletteJobItem"), OBFUSCATE("(IIIIFFFFI[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_roulette_RoulettePlayerManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_PlayerRoulette);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_roulette_RouletteJobManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_PlayerRoulette);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CRoulette::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CRoulette class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CRoulette done!"));
}