#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CAction
{
	
public:
    static jobject activity;
    
	static void actionShow(char* text, int32_t time);
	static void actionHide();
	static void actionClickShow(int32_t clickPercent);
	static void actionClickHide();
	static void setActivity(JNIEnv* env, jobject thiz);
};