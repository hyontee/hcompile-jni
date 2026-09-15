#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CEdit
{
	
public:
    static jobject activity;
    
	static void editShow(bool status);
	static void editHide();
	static void updateEditScale(float scale);
	static void setActivity(JNIEnv* env, jobject thiz);
};