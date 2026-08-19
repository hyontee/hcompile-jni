#include <jni.h>
#include "main.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "CAntiCheat.h"
extern CGame *pGame;
extern CNetGame* pNetGame;

#define RPC_ANTICHEAT      0x33

void CAntiCheat::sendMod()
{
    Log("CAntiCheat: sendMod");

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint32_t)RPC_ANTICHEAT);
    bsSend.Write((uint32_t)237);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);

}

/*extern "C" {

    JNIEXPORT void JNICALL Java_ru_edgar_space_core_ui_chatedgar_ChatManager_sendChatMessages(JNIEnv* pEnv, jobject thiz, jbyteArray str) {
    }
}*/