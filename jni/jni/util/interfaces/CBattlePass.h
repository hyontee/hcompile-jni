#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CBattlePass
{
	
public:
    static jobject activity;
    
	static void bpHide();
	static void bpShow(char* text, bool button, char* text2, char* text3);
	static void buyBattlePass(bool show, char* text, char* text2, char* text3, char* text4);
	static void updateBuyBattlePass(int position, bool status, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2);
	static void selectBattlePass(int selectId);
	static void bpUpdateMainLayout(char* text, char* text2, bool limitButton, char* text3);
	static void bpUpdateDayTaskLayout(int type, char* text, char* text2, char* text3, char* text4, int max, int progress, char* text5, char* text6);
	static void bpUpdateDayTasksPrize(int position, bool status, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* text);
	static void bpAddTasks(char* text, char* text2, char* text3, int max, int progress, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, int rareLevel, char* text4);
	static void bpAddGuideMainLayout(int position, char* text, char* text2);
	static void bpAddBPItem(char* count, char* premCount, char* name, int8_t state, int8_t rare, uint32_t icon, char* premName, int8_t premState, int8_t premRare, uint32_t premIcon, int8_t render, int8_t renderPrem, int8_t headerLvl, int max, int progress, char* exp);
	static void bpUpdateBPItem(int8_t position, char* count, char* premCount, char* name, int8_t state, int8_t rare, uint32_t icon, char* premName, int8_t premState, int8_t premRare, uint32_t premIcon, int8_t render, int8_t renderPrem, int8_t headerLvl, int max, int progress, char* exp);
	
	static void caseRouletteHide();
	static void caseRouletteShow(uint32_t itemLevel, bool canRoll, uint32_t maxItems);
	static void caseRouletteShowPrize(uint32_t model, char* name);
	static void caseRouletteAddItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name);
	static void caseRouletteGlass(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name);
	static void setActivity(JNIEnv* env, jobject thiz);
};