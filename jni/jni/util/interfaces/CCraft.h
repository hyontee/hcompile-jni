#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CCraft
{
	
public:
    static jobject activity;
    
	static void CraftShow(char* text);
	static void CraftHide();
	static void rvCraftMenuItem(char* text);
	static void rvCraftItem(int8_t position, int8_t type, uint32_t model, char* name, char* price, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom);
	static void rvResItem(int8_t type, uint32_t model, char* name, char* price, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom);
	static void updateCraftBlocked(int position, bool show, char* text, char* text2);
	static void updateCraftCount(char* text);
	static void updateCraft(int position, int max, int progress, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* text);
	static void craftClearItem(int id);
	static void setActivity(JNIEnv* env, jobject thiz);
};