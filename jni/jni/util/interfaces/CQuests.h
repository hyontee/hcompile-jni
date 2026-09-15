#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CQuests
{
	
public:
    static jobject activity;
    
	static void questsShow(bool status);
	static void questsHide();
	static void addQuestsItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2, int32_t max, int32_t progress);
	static void addQuestRewardsItem(int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text);
	static void updateQuestsData(char* text, char* text2, int max, int progress, char* text3, char* text4);
	static void setActivity(JNIEnv* env, jobject thiz);
};