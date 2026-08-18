
#ifndef CRIMINAL_MOSCOW_STATISTICS_H
#define CRIMINAL_MOSCOW_STATISTICS_H

#endif

#pragma once
#include <jni.h>

class CStatistics {
public:
    static jobject j_statistics;

    static void show();
    static void hide();
    static void updateJson(const char* jsonBytesCp1251);

    static void renderSkin(int skinId);
};

extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_stats_StatisticsView_init(JNIEnv* env, jclass clazz, jobject thiz);


extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_StatisticsOverlay_init(JNIEnv* env, jclass self, jobject overlayObj);
