#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CShop
{
	
public:
    static jobject activity;
    
	static void ShopShow(int position, char* text);
	static void ShopHide(int position);
	static void UpdateShopItem(int position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2);
	static void ItemShopReset();
	static void setActivity(JNIEnv* env, jobject thiz);
};