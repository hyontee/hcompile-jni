#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CSpawnMenu
{
	
public:
    static jobject activity;
    
	static void spawnShow();
	static void spawnHide();
	static void spawnAddItem(int32_t position, int32_t position2, char* text);
	static void setActivity(JNIEnv* env, jobject thiz);
};