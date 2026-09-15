#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CRent
{
	
public:
    static jobject activity;
    
	static void SetRentShow();
	static void SetRentHide();
	static void SetRentUp(int modelId, int maxSpeed, int rentPrice);
	static void setActivity(JNIEnv* env, jobject thiz);
};