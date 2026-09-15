#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CValentine
{
	
public:
    static jobject activity;
    
	static void valentineShowMainPage(uint32_t i, uint32_t i2, uint32_t i3, uint32_t i4, char* mes, uint32_t i5, char* s, char* s2, char* s3, char* s4, char* s5, char* s6);
	static void valentineShowTaskPage(int i, int i2, int i3, int i4, int i5, int i6, int i7, int i8);
	static void valentineShowSendPage(int i, int i2, int i3, int i4, int i5);
	static void valentineShowShopPage(uint32_t i, uint32_t i2, uint32_t i3, uint32_t i4, uint32_t i5, int i6, uint32_t i7, uint32_t i8, uint32_t i9, uint32_t i10, uint32_t i11, uint32_t i12, uint32_t i13, uint32_t i14);
	static void valentineShowTopPage();
	static void valentineShowDescPage();
	static void valentineShowLobbyPage(int i);
	static void valentineLobbySetName(int i, char* mes);
	static void valentineLobbySetReady(int i, int i2);
	static void valentineLobbySetState(int i);
	static void valentineLobbyHide();
	static void valentineHide();
	static void valentineAddTopItem(int i, int i2, char* mes, int i3);
	static void setActivity(JNIEnv* env, jobject thiz);
};