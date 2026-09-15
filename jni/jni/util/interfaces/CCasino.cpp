#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CCasino.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CCasino::activity;

void CNetGame::packetCasino(Packet *p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CCasino::SetAppleStart();
	else if (typeInfo == 1) CCasino::SetAppleStop();
	else if (typeInfo == 2)
	{
		uint8_t touch_id;
		bs.Read(touch_id);
			
		uint8_t result_id;
		bs.Read(result_id);
			
		bool status;
		bs.Read(status);
			
		uint8_t process_id;
		bs.Read(process_id);
			
		CCasino::SetAppleProcessBid(touch_id, result_id, status, process_id);
	}
	else if (typeInfo == 3) CCasino::SetAppleShow();
	else if (typeInfo == 4) CCasino::SetBinaryShow();
	else if (typeInfo == 5)
	{
		uint32_t oldValue;
		bs.Read(oldValue);

		CCasino::SetBinaryStart(oldValue);
	}
	else if (typeInfo == 6)
	{
		uint32_t newValue;
		bs.Read(newValue);
			
		uint32_t winValue;
		bs.Read(winValue);

		CCasino::SetBinaryValue(newValue, winValue);
	}
	else if (typeInfo == 7) CCasino::SetRouletteShow();
	else if (typeInfo == 8)
	{
		uint8_t winId;
		bs.Read(winId);
			
		uint32_t winMoney;
		bs.Read(winMoney);
			
		CCasino::SetRouletteStart(winId, winMoney);
	}
	else if (typeInfo == 9) CCasino::JackpotShow();
	else if (typeInfo == 10) CCasino::JackpotHide();
	else if (typeInfo == 11)
	{
		int32_t slotCombination;
		bs.Read(slotCombination);
			
		int32_t slotWin;
		bs.Read(slotWin);
			
		CCasino::JackpotStart(slotCombination, slotWin);
	}
	else if (typeInfo == 12)
	{
		uint8_t type;
		bs.Read(type);
			
		char date[128];
		char date1[128];
		memset(date, 0, 128);
		memset(date1, 0, 128);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(date, bLen);
		date[bLen] = '\0';
		cp1251_to_utf8(date1, date, 128);
			
		CCasino::WheelShow(type, date1);
	}
	else if (typeInfo == 13) CCasino::WheelHide();
	else if (typeInfo == 14)
	{
		uint32_t winValue;
		bs.Read(winValue);
			
		uint8_t winType;
		bs.Read(winType);
			
		uint16_t winModel;
		bs.Read(winModel);
			
		uint8_t winColor1;
		bs.Read(winColor1);
			
		uint8_t winColor2;
		bs.Read(winColor2);
			
		char date[128];
		char date1[128];
		memset(date, 0, 128);
		memset(date1, 0, 128);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(date, bLen);
		date[bLen] = '\0';
		cp1251_to_utf8(date1, date, 128);
			
		CCasino::WheelUpdateData(winValue, winType, winModel, winColor1, winColor2, date1);
	}
	else if (typeInfo == 15) CCasino::SetDiceShow();
	else if (typeInfo == 16) CCasino::SetDiceHide();
	else if (typeInfo == 17)
	{
		int8_t type;
		bs.Read(type);
		
		char text[4][256];
		for (int i = 0; i < 4; i ++) memset(text[i], 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text[0], bLen);
		text[0][bLen] = '\0';
		
		int32_t time;
		bs.Read(time);
		
		for (int i = 1; i < 4; i ++)
		{
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		CCasino::updateDice(type, text[0], time, text[1], text[2], text[3]);
	}
	else if (typeInfo == 18)
	{
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		int8_t type;
		bs.Read(type);
		
		bool show;
		bs.Read(show);
		
		char text2[256];
		memset(text2, 0, 256);
		
		bs.Read(bLen);
		bs.Read(text2, bLen);
		text2[bLen] = '\0';
		
		CCasino::addGamerItem(text, type, show, text2);
	}
	else if (typeInfo == 19)
	{
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		int8_t type;
		bs.Read(type);
		
		bool show;
		bs.Read(show);
		
		char text2[256];
		memset(text2, 0, 256);
		
		bs.Read(bLen);
		bs.Read(text2, bLen);
		text2[bLen] = '\0';
		
		CCasino::addGamerUpdate(text, type, show, text2);
	}
	else if (typeInfo == 20)
	{
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';

		CCasino::clearGamerItem(text);
	}
	else if (typeInfo == 21)
	{
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

		CCasino::addChatItem(text, text2);
	}
	else if (typeInfo == 22)
	{
		bool show;
		bs.Read(show);

		CCasino::diceBetScreen(show);
	}
}

void CCasino::SetAppleStart()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetAppleStart"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetAppleStop()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetAppleStop"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetAppleProcessBid(uint8_t touch_id, uint8_t result_id, bool status, uint8_t process_id)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetAppleProcessBid"), OBFUSCATE("(IIZI)V"));
	env->CallVoidMethod(activity, method, touch_id, result_id, status, process_id);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetAppleShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetAppleShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetBinaryShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetBinaryShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetBinaryStart(uint32_t oldValue)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetBinaryStart"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, oldValue);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetBinaryValue(uint32_t newValue, uint32_t winValue)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetBinaryValue"), OBFUSCATE("(II)V"));
	env->CallVoidMethod(activity, method, newValue, winValue);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetRouletteShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetRouletteShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetRouletteStart(uint8_t winId, uint32_t winMoney)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetRouletteStart"), OBFUSCATE("(II)V"));
	env->CallVoidMethod(activity, method, winId, winMoney);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::JackpotShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("JackpotShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::JackpotHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("JackpotHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::JackpotStart(int32_t i, int32_t i2)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("JackpotStart"), OBFUSCATE("(II)V"));
	env->CallVoidMethod(activity, method, i, i2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::WheelShow(int type, char* mes)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(mes));
	env->SetByteArrayRegion(bytes, 0, strlen(mes), (jbyte*)mes);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("WheelShow"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, type, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::WheelHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("WheelHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::WheelUpdateData(uint32_t winValue, int winType, int winModel, int winColor1, int winColor2, char* mes)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(mes));
	env->SetByteArrayRegion(bytes, 0, strlen(mes), (jbyte*)mes);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("WheelUpdateData"), OBFUSCATE("(IIIII[B)V"));
	env->CallVoidMethod(activity, method, winValue, winType, winModel, winColor1, winColor2, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetDiceShow()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetDiceShow"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::SetDiceHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("SetDiceHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::updateDice(int type, char* text, int i, char* text2, char* text3, char* text4)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateDice"), OBFUSCATE("(I[BI[B[B[B)V"));
	env->CallVoidMethod(activity, method, type, bytes, i, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::addGamerItem(char* text, int i, bool b, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addGamerItem"), OBFUSCATE("([BIZ[B)V"));
	env->CallVoidMethod(activity, method, bytes, i, b, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::addGamerUpdate(char* text, int i, bool b, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addGamerUpdate"), OBFUSCATE("([BIZ[B)V"));
	env->CallVoidMethod(activity, method, bytes, i, b, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::clearGamerItem(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("clearGamerItem"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::addChatItem(char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("addChatItem"), OBFUSCATE("([B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

void CCasino::diceBetScreen(bool b)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("diceBetScreen"), OBFUSCATE("(Z)V"));
	env->CallVoidMethod(activity, method, b);
	env->DeleteLocalRef(clazz);

	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_AppleManager_close(JNIEnv* pEnv, jobject thiz)
	{
		uint8_t _id = 255;
		uint32_t _result = 255;
		
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Apple);

		bsParams.Write(_id);
		bsParams.Write(_result);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_AppleManager_selectApple(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		uint8_t _id = i;
		uint32_t _result = i2;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Apple);

		bsParams.Write(_id);
		bsParams.Write(_result);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_AppleManager_startBet(JNIEnv* pEnv, jobject thiz, jint i)
	{
		uint8_t _id = 254;
		uint32_t _result = i;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Apple);

		bsParams.Write(_id);
		bsParams.Write(_result);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_AppleManager_stopBet(JNIEnv* pEnv, jobject thiz)
	{
		uint8_t _id = 253;
		uint32_t _result = 253;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Apple);

		bsParams.Write(_id);
		bsParams.Write(_result);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_BinaryManager_close(JNIEnv* pEnv, jobject thiz)
	{
		uint8_t _id = 255;
		uint32_t _result = 255;
		bool _b_result = false;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Binary);

		bsParams.Write(_id);
		bsParams.Write(_result);
		bsParams.Write(_b_result);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_BinaryManager_selectArrow(JNIEnv* pEnv, jobject thiz, jboolean z)
	{
		uint8_t _id = 252;
		uint32_t _result = 0;
		bool _b_result = z;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Binary);

		bsParams.Write(_id);
		bsParams.Write(_result);
		bsParams.Write(_b_result);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_BinaryManager_startBet(JNIEnv* pEnv, jobject thiz, jint i)
	{
		uint8_t _id = 254;
		uint32_t _result = i;
		bool _b_result = false;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Binary);

		bsParams.Write(_id);
		bsParams.Write(_result);
		bsParams.Write(_b_result);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_BinaryManager_stopBet(JNIEnv* pEnv, jobject thiz)
	{
		uint8_t _id = 253;
		uint32_t _result = 253;
		bool _b_result = false;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Binary);

		bsParams.Write(_id);
		bsParams.Write(_result);
		bsParams.Write(_b_result);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_RouletteManager_close(JNIEnv* pEnv, jobject thiz)
	{
		uint32_t _id = 255;
		uint8_t _result = 255;
		uint8_t _result1 = 0;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Roulette);

		bsParams.Write(_id);
		bsParams.Write(_result);
		bsParams.Write(_result1);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_RouletteManager_startBet(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		uint32_t _id = i;
		uint8_t _result = i2;
		uint8_t _result1 = i3;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Roulette);

		bsParams.Write(_id);
		bsParams.Write(_result);
		bsParams.Write(_result1);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_JackpotManager_returnJackpotManager(JNIEnv* pEnv, jobject thiz, jint i, jint i2)
	{
		int32_t _i = i;
		int32_t _i2 = i2;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_JackpotManager);

		bsParams.Write(_i);
		bsParams.Write(_i2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_WheelManager_returnWheelManager(JNIEnv* pEnv, jobject thiz, jint i)
	{
		int32_t _i = i;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_WheelManager);

		bsParams.Write(_i);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_casino_DiceManager_diceResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pText = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pText, length);
		
		pEnv->ReleaseByteArrayElements(str, pText, JNI_ABORT);

		char inputtext[128], inputtext1[128];
		memset(inputtext, 0, 128);
		memset(inputtext1, 0, 128);
		strcpy(inputtext, std::string(szStr).c_str());
		cp1251_to_utf8(inputtext1, std::string(szStr).c_str(), 128);

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_CasinoDice);

		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((uint8_t)strlen(inputtext1));
		bsParams.Write(inputtext1, (uint8_t)strlen(inputtext1));
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CCasino::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CCasino class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CCasino done!"));
}