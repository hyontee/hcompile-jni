#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CTab
{
	
public:
    static jobject activity;
    
	static void ShowTAB();
	static void HideTAB();
	static void ClearTAB();
	static void SetTABStats(int playerID, char* playerName, int playerScore, int playerPing);
	static void setActivity(JNIEnv* env, jobject thiz);
};