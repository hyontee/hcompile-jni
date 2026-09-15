#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CInventory
{
	
public:
    static jobject activity;
    
	static void showInventory();
	static void invUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2);
	static void acsUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, bool used);
	static void subUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text);
	static void invOpen(int8_t position, char* text, char* text2, bool status, int32_t maxSlot);
	static void invUpdateData(int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, int8_t health, int8_t armour, int8_t satiety);
	static void hideInventory();
	static void upgradeUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text);
	static void warehouseUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2);
	static void buttonAddItem(char* text, int8_t type, bool used);
	static void buttonUpdateItem(int8_t position, char* text, int8_t type, bool used);
	static void walletUpdateItem(int8_t position, int8_t type, int32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, char* text, bool used);
	static void setActivity(JNIEnv* env, jobject thiz);
};