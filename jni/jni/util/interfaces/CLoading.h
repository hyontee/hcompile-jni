#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CLoading
{
	
public:
    static jobject activity;
    
	static void SetLoadingShow();
	static void SetLoadingHide();
	static void SetLoadingStatus(char* status);
	static void setActivity(JNIEnv* env, jobject thiz);
};