#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CTune
{
	
public:
    static jobject activity;
    
	static void showTune(char* text, char* text2, char* text3, char* text4);
	static void hideTune();
	static void carCharacteristicLayout(bool show, char* text);
	static void carCharacteristicAdd(char* text, uint32_t max, uint32_t progress, char* text2, char* text3);
	static void carCharacteristicUpdate(int position, char* text, uint32_t max, uint32_t progress, char* text2, char* text3);
	static void carColorLayout(bool show, char* text, bool show2, bool show3);
	static void colorsAdd(int position, char* text);
	static void carWheelsLayout(bool show, char* text);
	static void carWheelsSettings(int position, bool show, char* text, char* text2, char* text3, char* text4, int seekBar);
	static void carTintSettingsLayout(bool show, char* text);
	static void carTintSettings(int position, bool show, char* text, char* text2, char* text3, int seekBar);
	static void menuAdd(int position, char* text, int icon);
	static void carComponentAdd(char* text, char* text2, char* text3, char* text4, char* text5, char* text6,
								int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom,
								char* text7, char* text8, char* text9);
	static void carComponentUpdate(int position, char* text, char* text2, char* text3, char* text4, char* text5, char* text6,
									int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom,
									char* text7, char* text8, char* text9);
	static void clearMenu(int position);
	static void carComponentClear();
	static void carCartLayout(bool show, char* text);
	static void addCartItem(char* text, char* text2);
	static void clearCartItem();
	static void setActivity(JNIEnv* env, jobject thiz);
};