#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CCraft.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CCraft::activity;

void CNetGame::packetCraft(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 1)
	{
		char text[256];
		memset(text, 0, 256);
			
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
			
		CCraft::CraftShow(text);
	}
	else if (typeInfo == 2) CCraft::CraftHide();
	else if (typeInfo == 3)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			char text[256];
			memset(text, 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
			
			CCraft::rvCraftMenuItem(text);
		}
	}
	else if (typeInfo == 4)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
			int8_t type;
			bs.Read(type);
			
			uint32_t model;
			bs.Read(model);
			
			char text[2][256];
			for (int i = 0; i < 2; i ++)
			{
				memset(text[i], 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}
			
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
			
			CCraft::rvCraftItem(position, type, model, text[0], text[1], color1, color2, rotX, rotY, rotZ, zoom);
		}
	}
	else if (typeInfo == 5)
	{
		int8_t position;
		bs.Read(position);
		
		bool show;
		bs.Read(show);
		
		char text[256];
		memset(text, 0, 256);

		uint8_t textLen;
		bs.Read(textLen);
		bs.Read(text, textLen);
		text[textLen] = '\0';
		
		char text2[256];
		memset(text2, 0, 256);

		bs.Read(textLen);
		bs.Read(text2, textLen);
		text2[textLen] = '\0';
		
		CCraft::updateCraftBlocked(position, show, text, text2);
	}
	else if (typeInfo == 6)
	{
		char text[256];
		memset(text, 0, 256);

		uint8_t textLen;
		bs.Read(textLen);
		bs.Read(text, textLen);
		text[textLen] = '\0';
		
		CCraft::updateCraftCount(text);
	}
	else if (typeInfo == 7)
	{
		int8_t position;
		bs.Read(position);
		
		int32_t max;
		bs.Read(max);
		
		int32_t progress;
		bs.Read(progress);
		
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
		
		char text[256];
		memset(text, 0, 256);

		uint8_t textLen;
		bs.Read(textLen);
		bs.Read(text, textLen);
		text[textLen] = '\0';
		
		CCraft::updateCraft(position, max, progress, type, model, color1, color2, rotX, rotY, rotZ, zoom, text);
	}
	else if (typeInfo == 8)
	{
		int8_t type;
		bs.Read(type);
			
		uint32_t model;
		bs.Read(model);
			
		char text[2][256];
		for (int i = 0; i < 2; i ++)
		{
			memset(text[i], 0, 256);
				
			uint8_t textLen;
			bs.Read(textLen);
			bs.Read(text[i], textLen);
			text[i][textLen] = '\0';
		}
			
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
			
		CCraft::rvResItem(type, model, text[0], text[1], color1, color2, rotX, rotY, rotZ, zoom);
	}
	else if (typeInfo == 9)
	{
		int8_t id;
		bs.Read(id);
		
		CCraft::craftClearItem(id);
	}
}

void CCraft::CraftShow(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("CraftShow"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCraft::CraftHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("CraftHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCraft::rvCraftMenuItem(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("rvCraftMenuItem"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCraft::rvCraftItem(int8_t position, int8_t type, uint32_t model, char* name, char* price, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(name));
	env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

   	jbyteArray bytes2 = env->NewByteArray(strlen(price));
	env->SetByteArrayRegion(bytes2, 0, strlen(price), (jbyte*)price);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("rvCraftItem"), OBFUSCATE("(III[B[BIIFFFF)V"));
	env->CallVoidMethod(activity, method, position, type, model, bytes, bytes2, color1, color2, rotX, rotY, rotZ, zoom);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCraft::rvResItem(int8_t type, uint32_t model, char* name, char* price, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(name));
	env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

   	jbyteArray bytes2 = env->NewByteArray(strlen(price));
	env->SetByteArrayRegion(bytes2, 0, strlen(price), (jbyte*)price);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("rvResItem"), OBFUSCATE("(II[B[BIIFFFF)V"));
	env->CallVoidMethod(activity, method, type, model, bytes, bytes2, color1, color2, rotX, rotY, rotZ, zoom);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCraft::updateCraftBlocked(int position, bool show, char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateCraftBlocked"), OBFUSCATE("(IZ[B[B)V"));
	env->CallVoidMethod(activity, method, position, show, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCraft::updateCraftCount(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateCraftCount"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCraft::updateCraft(int position, int max, int progress, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateCraft"), OBFUSCATE("(IIIIIIIFFFF[B)V"));
	env->CallVoidMethod(activity, method, position, max, progress, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCraft::craftClearItem(int id)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("craftClearItem"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, id);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_craft_CraftManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Craft);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CCraft::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CCraft class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CCraft done!"));
}