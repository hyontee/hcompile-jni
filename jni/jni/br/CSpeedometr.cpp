#include "CSpeedometr.h"
#include "main.h"

#include "../game/game.h"
#include "../gui/gui.h"

#include "../chatwindow.h"

#include "net/netgame.h"
#include "util/CJavaWrapper.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

extern "C"
{
//                  JNIEXPORT jBitmap JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getTestBit(JNIEnv *env, jobject thiz) 
//                  {
//	                  VECTOR vecOffset;
//	                  vecOffset.X = 0.0f;
//	                  vecOffset.Y = 0.0f;
//	                  vecOffset.Z = 0.0f;
//                                   
//		return pSnapShotHelper->CreateVehicleSnapShot(400, 0xFFFFFFFF, &vecOffset, 5.0f, 3, 1);
//                  }

                  JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_turnVehicle(JNIEnv *env, jobject thiz) 
                  {
                                    if(pNetGame) pNetGame->SendChatCommand("/e");
                                    if(pChatWindow) pChatWindow->AddDebugMessage("{ffff00}|{ffffff} Двигатель неисправен, Вы не можете ехать со скоростью более {ffff00}30 км/ч.");
                  }
}