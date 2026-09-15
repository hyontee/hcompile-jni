#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CNewBattlePass
{
	
public:
    static jobject activity;
    
	static void nbpHide();
	static void nbpShowMainPage(char* strEnd, char* lvl, uint32_t score, uint32_t maxScore, char* txtPrize);
	static void nbpShowTasksPage(char* strEnd, char* lvl, uint32_t score, uint32_t maxScore, char* txtPrize);
	static void nbpShowExpPage(char* strEnd, char* lvl, uint32_t score, uint32_t maxScore, char* txtPrize);
	static void nbpShowStatPage(char* strEnd, char* lvl, uint32_t score, uint32_t maxScore, char* txtPrize);
	static void nbpShowRoulette(uint32_t itemLevel, bool canRoll, uint32_t cost);
	static void nbpShowCasePage(uint32_t rouletteID);
	static void nbpShowPrize(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, char* name);
	static void nbpTaskAdd(char* taskName, char* taskProgress, int8_t taskBlock);
	static void nbpSetExpInfo(int8_t position, char* lvl, char* price);
	static void nbpSetCaseInfo(int8_t position, int8_t type, uint32_t model, char* name, int8_t rare, int8_t blocked);
	static void nbpAddBPItem(char* count, char* premCount, char* name, int8_t state, int8_t rare, uint32_t icon, char* premName, int8_t premState, int8_t premRare, uint32_t premIcon, int8_t render, int8_t renderPrem, int8_t headerLvl);
	static void nbpUpdateBPItem(int8_t position, char* count, char* premCount, char* name, int8_t state, int8_t rare, uint32_t icon, char* premName, int8_t premState, int8_t premRare, uint32_t premIcon, int8_t render, int8_t renderPrem, int8_t headerLvl);
	static void nbpAddStatItem(char* name, int8_t premium, char* lvl, char* score);
	static void nbpAddRouletteItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel);
	static void nbpDialog(char* caption, char* text, char* btnUse, char* btnCancel);
	static void setActivity(JNIEnv* env, jobject thiz);
};