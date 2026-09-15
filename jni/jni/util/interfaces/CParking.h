#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CParking
{
	
public:
    static jobject activity;
    
	static void showParking();
	static void hideParking();
	static void addParking(uint32_t ID, uint8_t Type, uint32_t Model, char* Name, uint8_t Color1, uint8_t Color2, float rotX, float rotY, float rotZ, float Zoom, uint32_t Status, uint8_t Fuel, uint8_t FuelStatus, char* TimeText);
	static void parkingDialog(char* caption, char* text, char* buttonNo, char* buttonYes);
	static void setActivity(JNIEnv* env, jobject thiz);
};