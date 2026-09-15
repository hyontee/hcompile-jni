#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CInventory.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CInventory::activity;

void CNetGame::packetInventory(Packet *p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CInventory::showInventory();
	else if (typeInfo == 1)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
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
			
			char text2[256];
			memset(text2, 0, 256);
			
			bs.Read(bLen);
			bs.Read(text2, bLen);
			text2[bLen] = '\0';
			
			CInventory::invUpdateItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text, text2);
		}
	}
	else if (typeInfo == 2)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
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
			
			bool used;
			bs.Read(used);
			
			CInventory::acsUpdateItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text, used);
		}
	}
	else if (typeInfo == 3)
	{
		int8_t position;
		bs.Read(position);
			
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

		CInventory::subUpdateItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text);
	}
	else if (typeInfo == 4)
	{
		int8_t position;
		bs.Read(position);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		char text2[256];
		memset(text2, 0, 256);
		
		bs.Read(bLen);
		bs.Read(text2, bLen);
		text2[bLen] = '\0';
		
		bool status;
		bs.Read(status);
		
		int32_t maxSlot;
		bs.Read(maxSlot);
		
		CInventory::invOpen(position, text, text2, status, maxSlot);
	}
	else if (typeInfo == 5)
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
		
		int8_t health;
		bs.Read(health);
		
		int8_t armour;
		bs.Read(armour);
		
		int8_t satiety;
		bs.Read(satiety);
		
		CInventory::invUpdateData(type, model, color1, color2, rotX, rotY, rotZ, zoom, health, armour, satiety);
	}
	else if (typeInfo == 6) CInventory::hideInventory();
	else if (typeInfo == 7)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
				
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

			CInventory::upgradeUpdateItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text);
		}
	}
	else if (typeInfo == 8)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
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
			
			char text2[256];
			memset(text2, 0, 256);
			
			bs.Read(bLen);
			bs.Read(text2, bLen);
			text2[bLen] = '\0';
			
			CInventory::warehouseUpdateItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text, text2);
		}
	}
	else if (typeInfo == 9)
	{
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		
		int8_t type;
		bs.Read(type);
		
		bool used;
		bs.Read(used);
		
		CInventory::buttonAddItem(text, type, used);
	}
	else if (typeInfo == 10)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
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
			
			bool used;
			bs.Read(used);
			
			CInventory::walletUpdateItem(position, type, model, color1, color2, rotX, rotY, rotZ, zoom, text, used);
		}
	}
	else if (typeInfo == 11)
	{
		int8_t position;
		bs.Read(position);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		
		int8_t type;
		bs.Read(type);
		
		bool used;
		bs.Read(used);
		
		CInventory::buttonUpdateItem(position, text, type, used);
	}
}

void CInventory::showInventory()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showInventory"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::invUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("invUpdateItem"), OBFUSCATE("(IIIIIFFFF[B[B)V"));
    env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::acsUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, bool used)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("acsUpdateItem"), OBFUSCATE("(IIIIIFFFF[BZ)V"));
    env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, used);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::subUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("subUpdateItem"), OBFUSCATE("(IIIIIFFFF[B)V"));
    env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::invOpen(int8_t position, char* text, char* text2, bool status, int32_t maxSlot)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("invOpen"), OBFUSCATE("(I[B[BZI)V"));
    env->CallVoidMethod(activity, method, position, bytes, bytes2, status, maxSlot);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::invUpdateData(int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, int8_t health, int8_t armour, int8_t satiety)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("invUpdateData"), OBFUSCATE("(IIIIFFFFIII)V"));
    env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, health, armour, satiety);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::hideInventory()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideInventory"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::upgradeUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("upgradeUpdateItem"), OBFUSCATE("(IIIIIFFFF[B)V"));
    env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::warehouseUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

   	jbyteArray bytes2 = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("warehouseUpdateItem"), OBFUSCATE("(IIIIIFFFF[B[B)V"));
    env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::buttonAddItem(char* text, int8_t type, bool used)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("buttonAddItem"), OBFUSCATE("([BIZ)V"));
    env->CallVoidMethod(activity, method, bytes, type, used);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::buttonUpdateItem(int8_t position, char* text, int8_t type, bool used)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("buttonUpdateItem"), OBFUSCATE("(I[BIZ)V"));
    env->CallVoidMethod(activity, method, position, bytes, type, used);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CInventory::walletUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, bool used)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("walletUpdateItem"), OBFUSCATE("(IIIIIFFFF[BZ)V"));
    env->CallVoidMethod(activity, method, position, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, used);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_inventory_InventoryManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Inventory);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CInventory::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CInventory class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CInventory done!"));
}