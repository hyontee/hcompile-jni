//
// Created by Regerds on 29.10.2024.
//

#ifndef BLACK_RUSSIA_NVEVENTQUEUEACTIVITY_H
#define BLACK_RUSSIA_NVEVENTQUEUEACTIVITY_H


#include <jni.h>

class NvEventQueueActivity {
public:
    static jobject nvEventContext;
    static JNIEnv* GetJNIEnv();
    static void CallCarRadialMenu(int params[]);
    static void OnPacketIncoming(int guiid, jbyteArray jbyteArray1);
    static void ShowNotification(int type, int duration, int id, int subId, char *caption, char *btnCaption);
    static void ShowBrCarFuelFillDialog(int prices[], int maxFuel, char* mInitialHint);
    static void ShowNewDonate(int rub, int donate_coins);
    static void ShoRegister(int i);
    static void ShowDinnerDialog();
};


#endif //BLACK_RUSSIA_NVEVENTQUEUEACTIVITY_H
