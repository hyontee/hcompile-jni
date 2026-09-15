#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CBusiness
{
	
public:
    static jobject activity;
    
	static void bizShow(char* text);
	static void bizHide();
	static void bizLayoutPage(int8_t i);
	static void bizAddItem(int32_t position, int8_t type, bool status, char* text, char* text2, char* text3, char* text4, char* text5, char* text6, char* text7, char* text8);
	static void bizLayoutUpdate(char* text, char* text2, char* text3, char* text4, int8_t type, bool status);
	static void bizMenuItem(int32_t position, char* text);
	static void bizMainItem(char* text, char* text2, char* text3, char* text4);
	static void bizParentItem(int32_t position, char* text, bool status);
	static void bizChildItem(int32_t position, int32_t id, char* text, char* text2);
	static void setActivity(JNIEnv* env, jobject thiz);
};