#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CPlates
{
	
public:
    static jobject activity;
    
	static void platesShow();
	static void platesHide();
	static void platesLayout(int8_t id);
	static void platesMainLayout(char* text, char* text2, char* text3);
	static void platesTabLayout(int8_t id, bool status);
	static void platesTabDescription(int8_t id, char* text);
	static void platesTabButton(int8_t id, char* text);
	static void platesTabButtonText(int8_t id, char* text);
	static void setActivity(JNIEnv* env, jobject thiz);
};