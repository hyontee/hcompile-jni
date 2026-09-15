#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/game.h"

#include "CTune.h"
#include "CNoty.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;
extern CGame* pGame;

jobject CTune::activity;

void CNetGame::packetTuning(Packet *p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		char text[4][256];
		for (int i = 0; i < 4; i ++)
		{
			memset(text[i], 0, 256);

			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CTune::showTune(text[0], text[1], text[2], text[3]);
	}
	else if (typeInfo == 1) CTune::hideTune();
	else if (typeInfo == 2)
	{
		bool show;
		bs.Read(show);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CTune::carCharacteristicLayout(show, text);
	}
	else if (typeInfo == 3)
	{
		char text[3][256];
		for (int q = 0; q < 3; q ++) memset(text[q], 0, 256);
			
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text[0], bLen);
		text[0][bLen] = '\0';
			
		uint32_t max;
		bs.Read(max);
			
		uint32_t progress;
		bs.Read(progress);
			
		for (int q = 1; q < 3; q ++)
		{
			bs.Read(bLen);
			bs.Read(text[q], bLen);
			text[q][bLen] = '\0';
		}
			
		CTune::carCharacteristicAdd(text[0], max, progress, text[1], text[2]);
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
			
		uint32_t max;
		bs.Read(max);
			
		uint32_t progress;
		bs.Read(progress);
		
		char text2[256];
		memset(text2, 0, 256);
		
		bs.Read(bLen);
		bs.Read(text2, bLen);
		text2[bLen] = '\0';
		
		char text3[256];
		memset(text3, 0, 256);
		
		bs.Read(bLen);
		bs.Read(text3, bLen);
		text3[bLen] = '\0';
		
		CTune::carCharacteristicUpdate(position, text, max, progress, text2, text3);
	}
	else if (typeInfo == 5)
	{
		bool show;
		bs.Read(show);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		bool show2;
		bs.Read(show2);
		
		bool show3;
		bs.Read(show3);
		
		CTune::carColorLayout(show, text, show2, show3);
	}
	else if (typeInfo == 6)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
			char text[256];
			memset(text, 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
			
			CTune::colorsAdd(position, text);
		}
	}
	else if (typeInfo == 7)
	{
		bool show;
		bs.Read(show);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CTune::carWheelsLayout(show, text);
	}
	else if (typeInfo == 8)
	{
		int8_t position;
		bs.Read(position);
		
		bool show;
		bs.Read(show);
		
		char text[4][256];
		uint8_t bLen;
		for (int i = 0; i < 4; i ++)
		{
			memset(text[i], 0, 256);
			
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		int8_t seekPosition;
		bs.Read(seekPosition);
		
		CTune::carWheelsSettings(position, show, text[0], text[1], text[2], text[3], seekPosition);
	}
	else if (typeInfo == 9)
	{
		bool show;
		bs.Read(show);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CTune::carTintSettingsLayout(show, text);
	}
	else if (typeInfo == 10)
	{
		int8_t position;
		bs.Read(position);
		
		bool show;
		bs.Read(show);
		
		char text[3][256];
		uint8_t bLen;
		for (int i = 0; i < 3; i ++)
		{
			memset(text[i], 0, 256);
			
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		int8_t seekPosition;
		bs.Read(seekPosition);
		
		CTune::carTintSettings(position, show, text[0], text[1], text[2], seekPosition);
	}
	else if (typeInfo == 11)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
			char text[256];
			memset(text, 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
			
			int8_t icon;
			bs.Read(icon);
			
			CTune::menuAdd(position, text, icon);
		}
	}
	else if (typeInfo == 12)
	{
		char text[9][256];
		uint8_t bLen;
		for (int q = 0; q < 9; q ++) memset(text[q], 0, 256);
		for (int q = 0; q < 6; q ++)
		{
			bs.Read(bLen);
			bs.Read(text[q], bLen);
			text[q][bLen] = '\0';
		}
			
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
			
		for (int q = 6; q < 9; q ++)
		{
			bs.Read(bLen);
			bs.Read(text[q], bLen);
			text[q][bLen] = '\0';
		}
			
		CTune::carComponentAdd(text[0], text[1], text[2], text[3], text[4], text[5], type, model, color1, color2, rotX, rotY, rotZ, zoom, text[6], text[7], text[8]);
	}
	else if (typeInfo == 13)
	{
		int8_t position;
		bs.Read(position);
		
		char text[9][256];
		uint8_t bLen;
		for (int q = 0; q < 9; q ++) memset(text[q], 0, 256);
		for (int q = 0; q < 6; q ++)
		{
			bs.Read(bLen);
			bs.Read(text[q], bLen);
			text[q][bLen] = '\0';
		}
			
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

		for (int q = 6; q < 9; q ++)
		{
			bs.Read(bLen);
			bs.Read(text[q], bLen);
			text[q][bLen] = '\0';
		}

		CTune::carComponentUpdate(position, text[0], text[1], text[2], text[3], text[4], text[5], type, model, color1, color2, rotX, rotY, rotZ, zoom, text[6], text[7], text[8]);
	}
	else if (typeInfo == 14)
	{
		int8_t position;
		bs.Read(position);
		
		CTune::clearMenu(position);
	}
	else if (typeInfo == 15) CTune::carComponentClear();
	else if (typeInfo == 16)
	{
		bool show;
		bs.Read(show);
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CTune::carCartLayout(show, text);
	}
	else if (typeInfo == 17)
	{
		char text[2][256];
		for (int q = 0; q < 2; q ++)
		{
			memset(text[q], 0, 256);
				
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[q], bLen);
			text[q][bLen] = '\0';
		}
			
		CTune::addCartItem(text[0], text[1]);
	}
	else if (typeInfo == 18) CTune::clearCartItem();
}

void CTune::showTune(char* text, char* text2, char* text3, char* text4)
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

   	jbyteArray bytes3 = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

   	jbyteArray bytes4 = env->NewByteArray(strlen(text4));
	env->SetByteArrayRegion(bytes4, 0, strlen(text4), (jbyte*)text4);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showTune"), OBFUSCATE("([B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::hideTune()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideTune"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carCharacteristicLayout(bool show, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carCharacteristicLayout"), OBFUSCATE("(Z[B)V"));
    env->CallVoidMethod(activity, method, show, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carCharacteristicAdd(char* text, uint32_t max, uint32_t progress, char* text2, char* text3)
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

   	jbyteArray bytes3 = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carCharacteristicAdd"), OBFUSCATE("([BII[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, max, progress, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carCharacteristicUpdate(int position, char* text, uint32_t max, uint32_t progress, char* text2, char* text3)
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

   	jbyteArray bytes3 = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carCharacteristicUpdate"), OBFUSCATE("(I[BII[B[B)V"));
    env->CallVoidMethod(activity, method, position, bytes, max, progress, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carColorLayout(bool show, char* text, bool show2, bool show3)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carColorLayout"), OBFUSCATE("(Z[BZZ)V"));
    env->CallVoidMethod(activity, method, show, bytes, show2, show3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::colorsAdd(int position, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("colorsAdd"), OBFUSCATE("(I[B)V"));
    env->CallVoidMethod(activity, method, position, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carWheelsLayout(bool show, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carWheelsLayout"), OBFUSCATE("(Z[B)V"));
    env->CallVoidMethod(activity, method, show, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carWheelsSettings(int position, bool show, char* text, char* text2, char* text3, char* text4, int seekBar)
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

   	jbyteArray bytes3 = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

   	jbyteArray bytes4 = env->NewByteArray(strlen(text4));
	env->SetByteArrayRegion(bytes4, 0, strlen(text4), (jbyte*)text4);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carWheelsSettings"), OBFUSCATE("(IZ[B[B[B[BI)V"));
    env->CallVoidMethod(activity, method, position, show, bytes, bytes2, bytes3, bytes4, seekBar);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carTintSettingsLayout(bool show, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carTintSettingsLayout"), OBFUSCATE("(Z[B)V"));
    env->CallVoidMethod(activity, method, show, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carTintSettings(int position, bool show, char* text, char* text2, char* text3, int seekBar)
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

   	jbyteArray bytes3 = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carTintSettings"), OBFUSCATE("(IZ[B[B[BI)V"));
    env->CallVoidMethod(activity, method, position, show, bytes, bytes2, bytes3, seekBar);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::menuAdd(int position, char* text, int icon)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("menuAdd"), OBFUSCATE("(I[BI)V"));
    env->CallVoidMethod(activity, method, position, bytes, icon);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carComponentAdd(char* text, char* text2, char* text3, char* text4, char* text5, char* text6,
							int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom,
							char* text7, char* text8, char* text9)
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

   	jbyteArray bytes3 = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

   	jbyteArray bytes4 = env->NewByteArray(strlen(text4));
	env->SetByteArrayRegion(bytes4, 0, strlen(text4), (jbyte*)text4);

   	jbyteArray bytes5 = env->NewByteArray(strlen(text5));
	env->SetByteArrayRegion(bytes5, 0, strlen(text5), (jbyte*)text5);

   	jbyteArray bytes6 = env->NewByteArray(strlen(text6));
	env->SetByteArrayRegion(bytes6, 0, strlen(text6), (jbyte*)text6);

   	jbyteArray bytes7 = env->NewByteArray(strlen(text7));
	env->SetByteArrayRegion(bytes7, 0, strlen(text7), (jbyte*)text7);

   	jbyteArray bytes8 = env->NewByteArray(strlen(text8));
	env->SetByteArrayRegion(bytes8, 0, strlen(text8), (jbyte*)text8);

   	jbyteArray bytes9 = env->NewByteArray(strlen(text9));
	env->SetByteArrayRegion(bytes9, 0, strlen(text9), (jbyte*)text9);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carComponentAdd"), OBFUSCATE("([B[B[B[B[B[BIIIIFFFF[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5, bytes6, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes7, bytes8, bytes9);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(bytes8);
	env->DeleteLocalRef(bytes9);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carComponentUpdate(int position, char* text, char* text2, char* text3, char* text4, char* text5, char* text6,
								int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom,
								char* text7, char* text8, char* text9)
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

   	jbyteArray bytes3 = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

   	jbyteArray bytes4 = env->NewByteArray(strlen(text4));
	env->SetByteArrayRegion(bytes4, 0, strlen(text4), (jbyte*)text4);

   	jbyteArray bytes5 = env->NewByteArray(strlen(text5));
	env->SetByteArrayRegion(bytes5, 0, strlen(text5), (jbyte*)text5);

   	jbyteArray bytes6 = env->NewByteArray(strlen(text6));
	env->SetByteArrayRegion(bytes6, 0, strlen(text6), (jbyte*)text6);

   	jbyteArray bytes7 = env->NewByteArray(strlen(text7));
	env->SetByteArrayRegion(bytes7, 0, strlen(text7), (jbyte*)text7);

   	jbyteArray bytes8 = env->NewByteArray(strlen(text8));
	env->SetByteArrayRegion(bytes8, 0, strlen(text8), (jbyte*)text8);

   	jbyteArray bytes9 = env->NewByteArray(strlen(text9));
	env->SetByteArrayRegion(bytes9, 0, strlen(text9), (jbyte*)text9);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carComponentUpdate"), OBFUSCATE("([B[B[B[B[B[BIIIIFFFF[B[B[B)V"));
    env->CallVoidMethod(activity, method, position, bytes, bytes2, bytes3, bytes4, bytes5, bytes6, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes7, bytes8, bytes9);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(bytes8);
	env->DeleteLocalRef(bytes9);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::clearMenu(int position)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("clearMenu"), OBFUSCATE("(I)V"));
    env->CallVoidMethod(activity, method, position);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carComponentClear()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carComponentClear"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::carCartLayout(bool show, char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("carCartLayout"), OBFUSCATE("(Z[B)V"));
    env->CallVoidMethod(activity, method, show, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::addCartItem(char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addCartItem"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CTune::clearCartItem()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("clearCartItem"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_tune_TuneManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Tuning);
					
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CTune::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CTune class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CTune done!"));
}