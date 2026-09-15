#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CMusic
{
	
public:
    static jobject activity;
    
	static void ShowAudioSelector(int AudioId);
	static void HideAudioSelector();
	static void ShowAudioMenu(int usedRecorder, int ButtonHand, int ButtonInventory, int VolumeMusic, bool ActiveMusic, const char* MusicName);
	static void HideAudioMenu();
	static void setActivity(JNIEnv* env, jobject thiz);
};