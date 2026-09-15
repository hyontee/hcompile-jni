#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CAzs
{
	
public:
    static jobject activity;
    
	static void showAzs(bool status, int32_t price, int32_t fuel, char* valute);
	static void hideAzs();
	static void azsAddItem(char* text, char* price);
	static void setActivity(JNIEnv* env, jobject thiz);
};