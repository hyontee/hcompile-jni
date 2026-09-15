#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CDonate
{
	
public:
    static jobject activity;
    
	static void openDonate();
	static void closeDonate();
	static void donateAddCategory(int8_t icon, char* text);
	static void donateAddItem(int16_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, bool special, char* text3);
	static void donateItemClear();
	static void donateUpdateInfo(char* text, char* text2, char* text3);
	static void setActivity(JNIEnv* env, jobject thiz);
};