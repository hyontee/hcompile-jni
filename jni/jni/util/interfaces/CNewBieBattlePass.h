#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CNewBieBattlePass
{
	
public:
    static jobject activity;
    
	static void bpHide();
	static void bpShow(char* text);
	static void selectBattlePass(int selectId);
	static void bpUpdateMainLayout(char* text, char* text2, bool limitButton, char* text3);
	static void bpAddTasks(char* text, char* text2, char* text3, int max, int progress, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, int rareLevel, char* text4);
	static void bpAddGuideMainLayout(int position, char* text, char* text2);
	static void bpAddBPItem(char* count, char* name, int8_t state, int8_t rare, uint32_t icon, int8_t render, int8_t headerLvl, int max, int progress, char* exp);
	static void bpUpdateBPItem(int8_t position, char* count, char* name, int8_t state, int8_t rare, uint32_t icon, int8_t render, int8_t headerLvl, int max, int progress, char* exp);
	static void newbieMiniPrize(int8_t id);
	static void setActivity(JNIEnv* env, jobject thiz);
};