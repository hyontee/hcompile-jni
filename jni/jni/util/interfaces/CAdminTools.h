#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CAdminTools
{
	
public:
    static jobject activity;
    
	static void adminLogsShow(char* text, char* text2, char* text3, char* text4, char* text5);
	static void adminLogsHide();
	static void adminMenuItem(char* text);
	static void adminLogsItem(char* text, char* text2, char* text3, char* text4, char* text5);
	static void setActivity(JNIEnv* env, jobject thiz);
};