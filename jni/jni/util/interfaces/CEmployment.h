#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CEmployment
{
	
public:
    static jobject activity;
    
	static void empShow(bool status);
	static void empHide();
	static void empHeaderText(char* text, char* text2);
	static void empTopTable(bool status);
	static void empStatistic(bool status, bool status2, int max, int progress, char* text, char* text2, char* text3);
	static void empGlobalInfo(char* text, char* text2);
	static void empTaskItem(int icon, char* text, int max, int progress, char* text2, char* text3, char* text4);
	static void empSubTaskItem(int type, int32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* text);
	static void empHistoryItem(char* text, char* text2, char* text3, char* text4, char* text5);
	static void setActivity(JNIEnv* env, jobject thiz);
};