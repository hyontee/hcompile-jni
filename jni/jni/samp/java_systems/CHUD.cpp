//
// Created by plaka on 28.10.2022.
//

#include "CHUD.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "keyboard.h"
#include "CSettings.h"
#include "chatwindow.h"
#include "CSpeedometr.h"
#include "util/patch.h"
#include "voice/MicroIcon.h"

extern CJavaWrapper *g_pJavaWrapper;
extern CGame *pGame;

bool        CHUD::bIsShow = false;
bool        CHUD::bIsShowPassengerButt = false;
bool        CHUD::bIsShowEnterExitButt = false;
bool        CHUD::bIsShowLockButt = false;
bool        CHUD::bIsShowHornButt = false;
bool        CHUD::bIsShowSirenButt = false;
bool        CHUD::bIsShowChat = true;
int         CHUD::iLocalMoney = 0;
int         CHUD::iWantedLevel = 0;
bool        CHUD::bIsShowMafiaWar = false;
float       CHUD::fLastGiveDamage = 0.0f;
bool        CHUD::bIsTouchCameraButt = false;
bool        CHUD::bIsCamEditGui = false;
int         CHUD::iSatiety = 0;
PLAYERID    CHUD::lastGiveDamagePlayerId = INVALID_PLAYER_ID;

CVector2D CHUD::radarBgPos1;
CVector2D CHUD::radarBgPos2;
CVector2D CHUD::radarPos;

jobject CHUD::thiz = nullptr;
jclass CHUD::clazz = nullptr;


void CHUD::ChangeChatHeight(int height) {
    if(CHUD::thiz == nullptr) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;


    jmethodID method = env->GetMethodID(clazz, "ChangeChatHeight", "(I)V");

    env->CallVoidMethod(thiz, method, height);
}

void CHUD::ChangeChatTextSize(int size) {
    if(CHUD::thiz == nullptr) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;


    jmethodID method = env->GetMethodID(clazz, "ChangeChatFontSize", "(I)V");

    env->CallVoidMethod(thiz, method, size);

}



void CHUD::toggleAll(bool toggle, bool withchat)
{
    if(toggle == bIsShow)
    {
        return;
    }
    if(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->IsInVehicle())
    {
        if(!toggle)
        {
            if(CSpeedometr::bIsShow)
            {
                CSpeedometr::tempToggle(false);
            }
        }
        else
        {
            if(CSpeedometr::bIsShow)
            {
                CSpeedometr::tempToggle(true);
            }
        }
    }

    bIsShow = toggle;
    *(uint8_t *)( g_libGTASA +  (VER_x32 ? 0x819D88 : 0x9FF3A8)) = 0;
    pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, toggle);
    MicroIcon::hasShowed = toggle;
   // pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(toggle, true);
    pGame->ToggleHUDElement(HUD_ELEMENT_FPS, toggle);

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    jmethodID method = env->GetMethodID(CHUD::clazz, "toggleAll", "(ZZ)V");
    env->CallVoidMethod(CHUD::thiz, method, toggle, withchat);


 //   *(uint8_t*)(g_libGTASA+0x7165E8) = 0;//������ ��� ���
  //  ScriptCommand(&toggle_radar_blank, (int)toggle); // ����� ���
}

void CHUD::togglePassengerButton(bool toggle)
{
    CHUD::bIsShowPassengerButt = toggle;
}

void CHUD::toggleEnterExitButton(bool toggle)
{
    CHUD::bIsShowEnterExitButt = toggle;
}

void CHUD::toggleLockButton(bool toggle)
{
    CHUD::bIsShowLockButt = toggle;
}

void CHUD::toggleSirenButton(bool toggle)
{

}

void CHUD::toggleHornButton(bool toggle)
{

}

int tickUpdate;
void CHUD::UpdateHudInfo()
{
    if(CHUD::thiz == nullptr) return;

    tickUpdate++;
    if(tickUpdate < 20) return;

    tickUpdate = 0;
    CPlayerPed* pPed = pGame->FindPlayerPed();

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }

    jmethodID method = env->GetMethodID(CHUD::clazz, "UpdateHudInfo", "(IIIIII)V");

    env->CallVoidMethod(thiz, method,
                        (int)pPed->GetHealth(),
                        (int)pPed->GetArmour(),
                        (int)CHUD::iSatiety,
                        (int)pPed->m_pPed->WeaponSlots[pPed->m_pPed->byteCurWeaponSlot].dwType,
                        (int)pPed->m_pPed->WeaponSlots[pPed->m_pPed->byteCurWeaponSlot].dwAmmo,
                        (int)pPed->m_pPed->WeaponSlots[pPed->m_pPed->byteCurWeaponSlot].dwAmmoInClip
                        );

}
void CHUD::UpdateWanted()
{
    if(CHUD::thiz == nullptr) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }

    jmethodID method = env->GetMethodID(clazz, "UpdateWanted", "(I)V");

    env->CallVoidMethod(thiz, method, iWantedLevel);
}

void CHUD::updateLevelInfo(int level, int currentexp, int maxexp)
{
    if(CHUD::thiz == nullptr) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }

    jmethodID method = env->GetMethodID(clazz, "updateLevelInfo", "(III)V");

    env->CallVoidMethod(thiz, method, level, currentexp, maxexp);
}

void CHUD::showUpdateTargetNotify(int type, char *text)
{

}

void CHUD::hideBusInfo()
{

}

void CHUD::showBusInfo(int time)
{

}

void CHUD::hideTargetNotify()
{

}

void CHUD::toggleGps(bool toggle)
{
    /*JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }

    jmethodID method = env->GetMethodID(clazz, "toggleGps", "(Z)V");

    env->CallVoidMethod(thiz, method, toggle);*/
}

void CHUD::toggleServerLogo(bool toggle)
{

}

void CHUD::toggleGreenZone(bool toggle)
{

}

void CHUD::UpdateMoney()
{
    if(CHUD::thiz == nullptr) return;
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    Log("UpdateMoney");


    jmethodID method = env->GetMethodID(clazz, "updateMoney", "(I)V");

    Log("UpdateMoney");

    env->CallVoidMethod(thiz, method, iLocalMoney);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_ClickEnterPassengerButton(JNIEnv *env, jobject thiz) {
    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(pPlayer != nullptr) {
        pPlayer->GoEnterVehicle(true);
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_ClickEnterExitVehicleButton(JNIEnv *env, jobject thiz) {
    if(!pNetGame)return;
    if(!pNetGame->GetPlayerPool())return;

    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(pPed->IsInVehicle())
    {
        VEHICLEID vehicleId = pPed->GetCurrentSampVehicleID();

        if(vehicleId == INVALID_VEHICLE_ID) return;

        pPlayer->GetPlayerPed()->ExitCurrentVehicle();

        pPlayer->SendExitVehicleNotification(vehicleId);
    }
    else {
        pPlayer->GoEnterVehicle(false);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_ClickLockVehicleButton(JNIEnv *env, jobject thiz) {
    pNetGame->SendChatCommand("/lock");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_PressedHorn(JNIEnv *env, jobject thiz, jboolean pressed) {
    pGame->isHornActive = pressed;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_Speedometer_ClickSpedometr(JNIEnv *env, jobject thiz, jint turn_id,
                                                      jboolean toggle) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_TURN_SIGNAL;
    uint8_t button = turn_id;
    uint8_t toggle_ = toggle;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    bsSend.Write(toggle_);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

void CNetGame::packetSalary(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint32_t salary;
    uint32_t lvl;
    float exp;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(salary);
    bs.Read(lvl);
    bs.Read(exp);


    CHUD::UpdateSalary(salary, lvl, exp);
}

void CNetGame::packetMafiaWar(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint16_t time, attack_score, def_score;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(time);
    bs.Read(attack_score);
    bs.Read(def_score);

    CHUD::updateOpgWarLayout(time, attack_score, def_score);
}

void CHUD::updateOpgWarLayout(int time, int attack_score, int def_score)
{

}

void CHUD::UpdateSalary(int salary, int lvl, float exp)
{

}

void CHUD::SetChatInput(const char ch[])
{
    if(CHUD::thiz == nullptr) return;

    char msg_utf[255];
    cp1251_to_utf8(msg_utf, ch);
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jstring jch = env->NewStringUTF(msg_utf);


    jmethodID AddToChatInput = env->GetMethodID(clazz, "AddToChatInput", "(Ljava/lang/String;)V");

    env->CallVoidMethod(thiz, AddToChatInput, jch);
    env->DeleteLocalRef(jch);
}

void CHUD::ToggleChatInput(bool toggle)
{
    if(CHUD::thiz == nullptr) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();



    jmethodID ToggleChatInput = env->GetMethodID(clazz, "ToggleChatInput", "(Z)V");

    env->CallVoidMethod(thiz, ToggleChatInput, toggle);
}

void CHUD::AddChatMessage(const char msg[])
{
    if(CHUD::thiz == nullptr) return;

    if(!thiz)return;

    char msg_utf[1024];
    cp1251_to_utf8(msg_utf, msg);
    //CChatWindow::FilterInvalidChars(msg_utf);

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    //
    jstring jmsg = env->NewStringUTF( ConvertColorToHtml(msg_utf) );


    jmethodID AddChatMessage = env->GetMethodID(clazz, "AddChatMessage", "(Ljava/lang/String;)V");

    env->CallVoidMethod(thiz, AddChatMessage, jmsg);
    env->DeleteLocalRef(jmsg);
}

void CHUD::addGiveDamageNotify(PLAYERID Id, int weaponId, float damage)
{


}

void CHUD::addTakeDamageNotify(char nick[], int weaponId, float damage)
{

}

void CHUD::Render() {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(CHUD::thiz == nullptr) {

        jmethodID constructor = env->GetMethodID(CHUD::clazz, "<init>",
                                                 "()V");
        CHUD::thiz = env->NewObject(CHUD::clazz, constructor);
        CHUD::thiz = env->NewGlobalRef(CHUD::thiz);
        CHUD::bIsShow = true;
    }
}

void CHUD::ToggleHpText(bool toggle)
{
    if(CHUD::thiz == nullptr) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();


    jmethodID method = env->GetMethodID(clazz, "ToggleHpText", "(Z)V");
    env->CallVoidMethod(thiz, method, toggle);
}

void CHUD::ToggleChat(bool toggle){
    if(CHUD::thiz == nullptr) return;

    bIsShowChat = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();


    jmethodID ToggleChat = env->GetMethodID(clazz, "ToggleChat" , "(Z)V");
    env->CallVoidMethod(thiz, ToggleChat, toggle);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_SetRadarBgPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1,
                                                    jfloat x2, jfloat y2) {
    CHUD::radarBgPos1.x = x1;
    CHUD::radarBgPos1.y = y1;

    CHUD::radarBgPos2.x = x2;
    CHUD::radarBgPos2.y = y2;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_SetRadarPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1) {

    CHUD::radarPos.x = x1;
    CHUD::radarPos.y = y1;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_SendChatMessage(JNIEnv *env, jobject thiz, jbyteArray str) {
    jbyte* pMsg = env->GetByteArrayElements(str, nullptr);
    jsize length = env->GetArrayLength(str);

    std::string szStr((char*)pMsg, length);
    //const char *inputText = pEnv->GetStringUTFChars(str, nullptr);

    if(pNetGame) {
        CKeyBoard::m_sInput = szStr;
        CKeyBoard::Send();
    }

    env->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_ToggleKeyBoard(JNIEnv *env, jobject thiz, jboolean toggle) {
    if (toggle) {
        CKeyBoard::Open();
    } else {
        CKeyBoard::Close();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_clickCameraMode(JNIEnv *env, jobject thiz) {
//
    if(!pNetGame)return;
    if(!pNetGame->GetPlayerPool())return;

    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(pPed->IsInVehicle()) {
        CHUD::bIsTouchCameraButt = true;
    }
    else {
        CFirstPersonCamera::Toggle();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_clickMultText(JNIEnv *env, jobject thiz) {
    pNetGame->SendChatCommand("/action");
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_clickSiren(JNIEnv *env, jobject thiz) {
    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(pPed->IsInVehicle())
    {
        CVehicle* pVehicle = pPed->GetCurrentVehicle();
        if(pVehicle)
        {
            pVehicle->m_bIsSirenOn = !pVehicle->m_bIsSirenOn;
        }

    }
}

void CNetGame::packetUpdateSatiety(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t value;

    bs.Read(value);

    CHUD::iSatiety = value;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_clickHudBtn(JNIEnv *env, jobject thiz, jint btn) {
    // TODO: implement clickHudBtn()
    switch (btn) {
        case 1: {
            pNetGame->SendChatCommand("/inv");
            break;
        }
        case 2: {
            pNetGame->SendChatCommand("/donate");
            break;
        }
    }
}