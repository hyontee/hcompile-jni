#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CMarket
{
	
public:
    static jobject activity;
    
	static void marketShow(char* text);
	static void marketHide();
	static void marketInvUpdateItem(int32_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2);
	static void marketUpdateItem(int32_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2);
	static void setActivity(JNIEnv* env, jobject thiz);
};