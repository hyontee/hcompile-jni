#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CDialogs
{
	
public:
    static jobject activity;
    
	static void showSelectorDialog(char* text, char* text2, char* text3, char* text4, char* text5, char* text6, char* text7, char* text8);
	static void hideSelectorDialog();
	static void dialogSeekBar(bool status, char* text, char* text2, char* text3, int32_t max, int32_t progress);
	static void setActivity(JNIEnv* env, jobject thiz);
};