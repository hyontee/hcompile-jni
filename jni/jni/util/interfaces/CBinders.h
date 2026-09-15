#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CBinders
{
	
public:
    static jobject activity;
    
	static void bindShow();
	static void bindHide();
	static void bindAddItem(int8_t position, char* text);
	static void bindUpdateItem(int8_t position, char* text);
	
	static void bindEditShow();
	static void bindEditHide();
	static void bindEditAddItem(char* text, char* text2);
	static void bindEditUpdateItem(int8_t position, char* text, char* text2);
	static void setActivity(JNIEnv* env, jobject thiz);
};