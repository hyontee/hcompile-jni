#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CNoty.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CNoty::activity;

void CNetGame::packetNoty(Packet *p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0)
	{
		char szText[2048];
		char szText2[2048];
		memset(szText, 0, 2048);
		memset(szText2, 0, 2048);
			
		uint16_t bLen;
		bs.Read(bLen);	
		bs.Read(szText, bLen);
		szText[bLen] = '\0';
		cp1251_to_utf8(szText2, szText, 2048);
			
		uint8_t bType;
		bs.Read(bType);
			
		int32_t bProgress;
		bs.Read(bProgress);
		
		uint16_t bID;
		bs.Read(bID);
		
		uint16_t bSend;
		bs.Read(bSend);
		
		uint8_t bPictureId;
		bs.Read(bPictureId);
		
		char btn_text_1[256];
		char btn_text_11[256];
		memset(btn_text_1, 0, 256);
		memset(btn_text_11, 0, 256);
			
		uint8_t bLen_uint8;
		bs.Read(bLen_uint8);
		bs.Read(btn_text_1, bLen_uint8);
		btn_text_1[bLen_uint8] = '\0';
		cp1251_to_utf8(btn_text_11, btn_text_1, 256);
			
		char btn_text_2[256];
		char btn_text_22[256];
		memset(btn_text_2, 0, 256);
		memset(btn_text_22, 0, 256);
			
		bs.Read(bLen_uint8);
		bs.Read(btn_text_2, bLen_uint8);
		btn_text_2[bLen_uint8] = '\0';
		cp1251_to_utf8(btn_text_22, btn_text_2, 256);
			
		if (bProgress != -1) CNoty::ShowNotification(szText2, bProgress / 100, bType, bID, bSend, bPictureId, btn_text_11, btn_text_22);
		else CNoty::ShowNotification(szText2, bProgress, bType, bID, bSend, bPictureId, btn_text_11, btn_text_22);
	}
	else if (typeInfo == 1)
	{
		uint8_t notyId;
		bs.Read(notyId);
			
		CNoty::HideNotification(notyId);
	}
	else if (typeInfo == 2)
	{
		int8_t type;
		bs.Read(type);
		if (type == 1)
		{
			char text[2048];
			memset(text, 0, 2048);
				
			uint16_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
			
			char promo[256];
			memset(promo, 0, 256);
				
			bs.Read(bLen);
			bs.Read(promo, bLen);
			promo[bLen] = '\0';
			
			char btnUse[256];
			memset(btnUse, 0, 256);
			
			bs.Read(bLen);
			bs.Read(btnUse, bLen);
			btnUse[bLen] = '\0';
			
			char btnClose[256];
			memset(btnClose, 0, 256);
			
			bs.Read(bLen);
			bs.Read(btnClose, bLen);
			btnClose[bLen] = '\0';
			
			char caption[256];
			memset(caption, 0, 256);
			
			bs.Read(bLen);
			bs.Read(caption, bLen);
			caption[bLen] = '\0';
			
			CNoty::showNotyDialog(text, promo, btnUse, btnClose, caption);
		}
		else if (type == 2)
		{
			int8_t types;
			bs.Read(types);
			
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
			
			CNoty::notyImageBitMap(types, model, color1, color2, rotX, rotY, rotZ, zoom);
		}
		else if (type == 3)
		{
			char text[256];
			memset(text, 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
			
			CNoty::notyImageResource(text);
		}
	}
	else if (typeInfo == 3)
	{
		char text[2048];
		char text2[2048];
		memset(text, 0, 2048);
		memset(text2, 0, 2048);
		
		uint16_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		cp1251_to_utf8(text2, text, 2048);
		
		CNoty::showNotyBig(text);
	}
	else if (typeInfo == 4) CNoty::hideNotyBig();
}

void CNoty::ShowNotification(char* notifText, int32_t duration, int type, int tID, int subtype, int pictureId, char* btnText1, char* btnText2)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }
	
    /*jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");*/
    jbyteArray bytes = env->NewByteArray(strlen(notifText));
    env->SetByteArrayRegion(bytes, 0, strlen(notifText), (jbyte*)notifText);
    //jstring str = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
	
    /*jclass strClass1 = env->FindClass("java/lang/String");
    jmethodID ctorID1 = env->GetMethodID(strClass1, "<init>", "([BLjava/lang/String;)V");
    jstring encoding1 = env->NewStringUTF("UTF-8");*/
    jbyteArray bytes1 = env->NewByteArray(strlen(btnText1));
    env->SetByteArrayRegion(bytes1, 0, strlen(btnText1), (jbyte*)btnText1);
    //jstring str1 = (jstring)env->NewObject(strClass1, ctorID1, bytes1, encoding1);
	
    /*jclass strClass2 = env->FindClass("java/lang/String");
    jmethodID ctorID2 = env->GetMethodID(strClass2, "<init>", "([BLjava/lang/String;)V");
    jstring encoding2 = env->NewStringUTF("UTF-8");*/
    jbyteArray bytes2 = env->NewByteArray(strlen(btnText2));
    env->SetByteArrayRegion(bytes2, 0, strlen(btnText2), (jbyte*)btnText2);
    //jstring str2 = (jstring)env->NewObject(strClass2, ctorID2, bytes2, encoding2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showNotification"), OBFUSCATE("(IIIII[B[B[B)V"));
	env->CallVoidMethod(activity, method, duration, type, tID, subtype, pictureId, bytes, bytes1, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes1);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNoty::HideNotification(int i)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideNotification"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, i);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNoty::visibilityNotification(bool b)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("visibilityNotification"), OBFUSCATE("(Z)V"));
	env->CallVoidMethod(activity, method, b);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNoty::showNotyDialog(char* text, char* promo, char* btnUse, char* btnClose, char* caption)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }
	
    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);
	
    jbyteArray bytes2 = env->NewByteArray(strlen(promo));
    env->SetByteArrayRegion(bytes2, 0, strlen(promo), (jbyte*)promo);
	
    jbyteArray bytes3 = env->NewByteArray(strlen(btnUse));
    env->SetByteArrayRegion(bytes3, 0, strlen(btnUse), (jbyte*)btnUse);
	
    jbyteArray bytes4 = env->NewByteArray(strlen(btnClose));
    env->SetByteArrayRegion(bytes4, 0, strlen(btnClose), (jbyte*)btnClose);
	
    jbyteArray bytes5 = env->NewByteArray(strlen(caption));
    env->SetByteArrayRegion(bytes5, 0, strlen(caption), (jbyte*)caption);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showNotyDialog"), OBFUSCATE("([B[B[B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNoty::notyImageBitMap(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("notyImageBitMap"), OBFUSCATE("(IIIIFFFF)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNoty::notyImageResource(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("notyImageResource"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNoty::showNotyBig(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("showNotyBig"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNoty::hideNotyBig()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("hideNotyBig"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_noty_NotyItem_sendNotifyResponse(JNIEnv* pEnv, jobject thiz, jint i, jint mID, jboolean b)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_CInfo);
				
		uint16_t _i = i;
		uint16_t _mID = mID;
		bool _b = b;
		bsParams.Write(_i);
		bsParams.Write(_mID);
		bsParams.Write(_b);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_noty_NotyDialogManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_CInfo);
	
		bsParams.Write((uint16_t)32654);
		bsParams.Write((uint16_t)i);
		bsParams.Write((bool)false);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CNoty::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CNoty class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CNoty done!"));
}