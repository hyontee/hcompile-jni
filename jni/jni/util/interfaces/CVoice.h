#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CVoice
{
	
public:
    static jobject activity;
    
	static void ShowVoice();
	static void setActivity(JNIEnv* env, jobject thiz);
};