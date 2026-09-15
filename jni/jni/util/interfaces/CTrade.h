#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CTrade
{
	
public:
    static jobject activity;
    
	static void showTrade();
	static void hideTrade();
	static void tradeUpdate(char* text, bool accept, bool accept2, bool accept3, char* text2, char* text3);
	static void tradeUpdateItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype);
	static void tradeAddSendItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype);
	static void tradeUpdateSendItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype);
	static void tradeDeleteSendItem(int32_t position);
	static void tradeAddReceiveItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype);
	static void tradeUpdateReceiveItem(int32_t position, int32_t slot, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool changed, int itemtype);
	static void tradeDeleteReceiveItem(int32_t position);
	static void setActivity(JNIEnv* env, jobject thiz);
};