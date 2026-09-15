#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CE2y
{
	
public:
    static jobject activity;
    
	static void e2yShow(int32_t iBalance, char* date);
	static void e2yHide();
	static void e2yShopItem(int modelId, int renderType, int color1, int color2, int type, int price, char* name);
	static void e2yRouletteItem(int modelId, int renderType, int color1, int color2, int type, int price, char* name);
	static void e2yPaymentOkay(int modelId);
	static void e2yTopPlayer(int position, char* name, bool prem_status, uint32_t count);
	static void e2yQuest(int position, int number, char* mes, char* mes2, char* mes3);
	static void setActivity(JNIEnv* env, jobject thiz);
};