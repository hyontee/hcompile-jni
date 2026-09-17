#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"
#include "../gui/gui.h"
#include "../chatwindow.h"
#include "HUD.h"
#include "../util/patch.h"
#include <string>
#include "../util/CJavaWrapper.h"
#include "../obfuscate/str_obfuscate.hpp"

extern CJavaWrapper* g_pJavaWrapper;
extern CGame* pGame;
extern CChatWindow *pChatWindow;
extern CNetGame *pNetGame;

CGtaRect CHUD::radarBgPos1;
CGtaRect CHUD::radarBgPos2;
CRadarBrRect CHUD::radar1;
CSprite2d* CHUD::radar = nullptr;
CSprite2d* CHUD::logo = nullptr;
CGtaRect CHUD::logoPos;
bool CHUD::m_bShow = false;

// --- RADAR ---
void CHUD::EditRadarBios(CRect* rect)
{
    rect->left      = CHUD::radar1.x1;
    rect->bottom    = CHUD::radar1.y1;

    rect->right = CHUD::radar1.x2;
    rect->top = CHUD::radar1.y2;
}
// --- END RADAR ---

extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_sendJson(JNIEnv *env, jobject thiz, jint json_id) {
    switch(json_id)
    {
        case 1:
        {
            pNetGame->SendChatCommand("/mm");
            break;
        }
    }
}

extern "C"
JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendG(JNIEnv* pEnv, jobject thiz) {
    CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
    CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
    if (pVehiclePool)
    {
        VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
        if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
        {
            CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
            if (pVehicle)
            {
                CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
                if (pPlayerPool)
                {
                    CLocalPlayer* pLocalPlayer;
                    pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
                    pLocalPlayer->SendEnterVehicleNotification(ClosetVehicleID, true);
                }
            }
        }
    }
}

// --- CHAT ---
extern "C" {
JNIEXPORT void JNICALL Java_com_rockstargames_gtasa_gui_ChatManager_sendChatMessages(JNIEnv* pEnv, jobject thiz, jbyteArray str) {
    jboolean isCopy = true;

    jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
    jsize length = pEnv->GetArrayLength(str);

    std::string szStr((char*)pMsg, length);

    pChatWindow->ChatWindowInputHandler((char*)szStr.c_str());

    pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
    }
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_rockstargames_gtasa_gui_ChatManager_ChatOpenPlus(JNIEnv *pEnv, jobject thiz) {
    pChatWindow->OpenKeyboard();
    }
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_rockstargames_gtasa_gui_ChatManager_ChatClosePlus(JNIEnv *pEnv, jobject thiz) {
    pChatWindow->CloseKeyboard();
    }
}
// --- END CHAT ---