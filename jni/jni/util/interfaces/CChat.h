#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CChat
{
	
public:
    static jobject activity;
    
	static void ShowChat();
	static void HideChat();
	static void ShowChatInput();
	static void HideChatInput();
	static void PushChatMessage(uint32_t color, char* msg, char wordType, int dwType, int dwNumber);
	
	static void newShowChat();
	static void newHideChat();
	static void newShowChatInput();
	static void newHideChatInput();
	static void newPushChatMessage(uint32_t color, char* msg, char wordType, int dwType, int dwNumber);
	static void setActivity(JNIEnv* env, jobject thiz);
};