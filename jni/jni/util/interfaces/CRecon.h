#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CRecon
{
	
public:
    static jobject activity;
    
	static void reconShow(bool status);
	static void reconHide(bool status);
	static void updateRecon(char* text, int32_t playerID);
	static void setActivity(JNIEnv* env, jobject thiz);
};