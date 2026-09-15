#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CSpeedometr
{
	
public:
    static jobject activity;
	static uint32_t CustomTickRate;
    
	static void ShowSpeedometr();
	static void HideSpeedometr(bool b);
	static void UpdateDataSpeedometr(int Speed, int Fuel, int Health, int Milleage, bool AlarmStatus, int Lights, int Engine, int Doors);
	static void setActivity(JNIEnv* env, jobject thiz);
};