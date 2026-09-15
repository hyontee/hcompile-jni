#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CContainer
{
	
public:
    static jobject activity;
    
	static void containerShow();
	static void containerHide();
	static void containerUpdateData(char* text, char* text2, char* text3, char* text4, char* text5);
	static void containerShowInput(bool b, char* text, char* text2, char* text3);
	static void containerAddItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name);
    
	static void containerPrizeShow(char* text);
	static void containerPrizeHide();
	static void containerPrizeClear();
	static void containerPrizeAddItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name);
	static void setActivity(JNIEnv* env, jobject thiz);
};