
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "CDonate.h"
#include "util/CJavaWrapper.h"
#include "chatwindow.h"

extern CNetGame* pNetGame;
extern CGame *pGame;

jobject CDonate::thiz = nullptr;
bool CDonate::bIsToggle = false;


void CDonate::ShowDonate(int money, int donate)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(CDonate::thiz);
    jmethodID ShowDonate = env->GetMethodID(clazz, "showdonate", "(II)V");


    env->CallVoidMethod(CDonate::thiz, ShowDonate, money, donate);
}
void CDonate::UpdateDonate(int money, int donate)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(CDonate::thiz);
    jmethodID UpdateDonate = env->GetMethodID(clazz, "upddonate", "(II)V");


    env->CallVoidMethod(CDonate::thiz, UpdateDonate, money, donate);
}


void CNetGame::packetDonateShow(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    int money = pGame->GetLocalMoney();
    int donate;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(money);
    bs.Read(donate);

    CDonate::ShowDonate(money, donate);
}

void CNetGame::packetDonateUpdate(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    int money = pGame->GetLocalMoney();
    int donate;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(money);
    bs.Read(donate);

    CDonate::UpdateDonate(money, donate);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_grand_game_gui_donate_Donate_DonateInit(JNIEnv *env, jobject thiz) {
    CDonate::thiz = env->NewGlobalRef(thiz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_grand_game_gui_donate_Donate_DonateClick(JNIEnv *env, jobject thiz, jint id) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)0x26);
    bsSend.Write((uint8_t)RPC_DONATE_CLICK);
    bsSend.Write((uint16_t)id);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_grand_game_gui_donate_Donate_BuyDonate(JNIEnv *env, jobject thiz, jint id, jint itemid, jint cost) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)0x25);
    bsSend.Write((uint8_t)RPC_DONATE_BUY);
    bsSend.Write((uint16_t)id);
    bsSend.Write((uint16_t)itemid);
    bsSend.Write((uint32_t)cost);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}