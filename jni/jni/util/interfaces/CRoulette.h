#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CRoulette
{
	
public:
    static jobject activity;
    
	static void roulleteHide();
	static void showRoulette(uint32_t itemLevel, bool canRoll, uint32_t maxItems);
	static void rouletteShowPrize(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, char* name);
	static void addRouletteItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel);
	static void rouletteUpdate(int8_t type, char* bronze, char* silver, char* gold);
    
	static void roulleteJobHide();
	static void showRouletteJob(uint32_t itemLevel, bool canRoll, uint32_t maxItems);
	static void rouletteJobShowPrize(uint32_t model, char* name);
	static void addRouletteJobItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name);
	static void setActivity(JNIEnv* env, jobject thiz);
};