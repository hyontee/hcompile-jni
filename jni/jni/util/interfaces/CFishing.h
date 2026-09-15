#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CFishing
{
	
public:
    static jobject activity;
    
	static void fishingShow();
	static void fishingHide();
	static void fishingUpdate(char* text, bool status);
	static void setActivity(JNIEnv* env, jobject thiz);
};