#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CBoats
{
	
public:
    static jobject activity;
    
	static void boatsShow(int i);
	static void boatsHide();
	static void boatsColor(int oldcolor, uint32_t cost, int type, int modelId);
	static void boatsUpgrade(int modelId, int type, 
								int lvl, int lvl2, int lvl3, int lvl4, int lvl5, int lvl6,
								int32_t cost, int32_t cost2, int32_t cost3, int32_t cost4, int32_t cost5, int32_t cost6,
								int stats, int stats2, int stats3, int stats4);
	static void boatsRent(int id, int typeVehicle, int speed, uint32_t cost, int type, int modelId);
	static void boatsSelect(uint32_t id, char* mes, int speed, bool isLoaded, int type, int modelId);
	static void setActivity(JNIEnv* env, jobject thiz);
};