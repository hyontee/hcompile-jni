#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CAuthReg
{
	
public:
    static jobject activity;
    
	static void authShow();
	static void authHide();
	static void authLoadedBar(bool status);
	static void authCheckedParams(int8_t params, bool status);
	static void authUpdateParams(char* text, char* text2, char* text3, char* text4);
	static void setAuthRemember(bool status);
	static void authMessage(char* text);
	
	static void regShow();
	static void regHide();
	static void regCheckedParams(int8_t params, bool status);
	static void regMessage(char* text);
	
	static void regPedShow();
	static void regPedHide();
	static void regPedClothes(char* text);
	static void regPedMessage(char* text);
	static void setActivity(JNIEnv* env, jobject thiz);
};