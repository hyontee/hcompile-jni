#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CCasino
{
	
public:
    static jobject activity;
    
	static void SetAppleStart();
	static void SetAppleStop();
	static void SetAppleProcessBid(uint8_t touch_id, uint8_t result_id, bool status, uint8_t process_id);
	static void SetAppleShow();
	static void SetBinaryShow();
	static void SetBinaryStart(uint32_t oldValue);
	static void SetBinaryValue(uint32_t newValue, uint32_t winValue);
	static void SetRouletteShow();
	static void SetRouletteStart(uint8_t winId, uint32_t winMoney);
	static void JackpotShow();
	static void JackpotHide();
	static void JackpotStart(int32_t i, int32_t i2);
	static void WheelShow(int type, char* mes);
	static void WheelHide();
	static void WheelUpdateData(uint32_t winValue, int winType, int winModel, int winColor1, int winColor2, char* mes);
	
	static void SetDiceShow();
	static void SetDiceHide();
	static void updateDice(int type, char* text, int i, char* text2, char* text3, char* text4);
	static void addGamerItem(char* text, int i, bool b, char* text2);
	static void addGamerUpdate(char* text, int i, bool b, char* text2);
	static void clearGamerItem(char* text);
	static void addChatItem(char* text, char* text2);
	static void diceBetScreen(bool b);
	static void setActivity(JNIEnv* env, jobject thiz);
};