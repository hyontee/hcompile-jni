#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CNoty
{
	
public:
    static jobject activity;
    
	static void ShowNotification(char* notifText, int32_t duration, int type, int tID, int subtype, int pictureId, char* btnText1, char* btnText2);
	static void HideNotification(int i);
	static void visibilityNotification(bool b);
	
	static void showNotyDialog(char* text, char* promo, char* btnUse, char* btnClose, char* caption);
	static void notyImageBitMap(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom);
	static void notyImageResource(char* text);
	
	static void showNotyBig(char* text);
	static void hideNotyBig();
	static void setActivity(JNIEnv* env, jobject thiz);
};