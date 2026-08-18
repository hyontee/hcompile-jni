//
// Created by Error on 16.01.2025.
//

#include "CJavaGui.h"
#include "raknet/BitStream.h"
#include "util.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jclass CJavaGui::clazz = nullptr;
bool CJavaGui::drawHud = false;
bool CJavaGui::drawChat = false;

void CJavaGui::Create(int id){
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring ss = env->NewStringUTF("");

    jmethodID method = env->GetStaticMethodID(CJavaGui::clazz, "receiveUiPacket", "(IIILjava/lang/String;)V");
    env->CallStaticVoidMethod(CJavaGui::clazz, method, id, -2, 0, ss);

    env->DeleteLocalRef(ss);
}

void CJavaGui::ReceivePacket(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    //bs.IgnoreBits(40);
    uint8_t packetID;
    int8 type;
    uint16_t id, rpcid, datalen;
    bs.Read(packetID);
    bs.Read(id);
    bs.Read(type);
    bs.Read(rpcid);

    bs.Read(datalen);
    char data[datalen];
    bs.Read(data, datalen);
    data[datalen] = '\0';

    char utf8_1[datalen];
    cp1251_to_utf8(utf8_1, data);


    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring ss = env->NewStringUTF(utf8_1);

    Log("id: %d, type: %d, rpcid: %d", id,type, rpcid);

    jmethodID method = env->GetStaticMethodID(CJavaGui::clazz, "receiveUiPacket", "(IIILjava/lang/String;)V");
    env->CallStaticVoidMethod(CJavaGui::clazz, method, id, type, rpcid, ss);

    env->DeleteLocalRef(ss);
}

void CJavaGui::DestroyAll() {
    for(int i = 0; i < MaxId - 1; i++) {
        Log("CJavaGui destroy id: %d", i);
        CJavaGui::Destroy(i);
    }
}

void CJavaGui::Destroy(int id) {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring ss = env->NewStringUTF("");

    jmethodID method = env->GetStaticMethodID(CJavaGui::clazz, "receiveUiPacket", "(IIILjava/lang/String;)V");
    env->CallStaticVoidMethod(CJavaGui::clazz, method, id, -3,0, ss);

    env->DeleteLocalRef(ss);
}

void CJavaGui::TempToggle(int id, bool toggle) {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring ss = env->NewStringUTF("");

    jmethodID method = env->GetStaticMethodID(CJavaGui::clazz, "receiveUiPacket", "(IIILjava/lang/String;)V");
    env->CallStaticVoidMethod(CJavaGui::clazz, method, id, toggle ? -2 : -1,0, ss);

    env->DeleteLocalRef(ss);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_NativeGuiManager_onSendDataServer(JNIEnv *env, jclass clazz, jint id,
                                                           jstring data) {

    const char *inputData = env->GetStringUTFChars(data, nullptr);
    uint16_t byteInputData = strlen(inputData);
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)52);
    bsSend.Write((uint16_t)id);
    bsSend.Write(byteInputData);
    bsSend.Write(inputData, byteInputData);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);

    env->ReleaseStringUTFChars(data, inputData);


}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_NativeGui_00024Companion_nativeUpdateHudState(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jboolean ishud,
                                                                           jboolean ischat) {
    // TODO: implement nativeUpdateHudState()
    CJavaGui::drawHud = ishud;
    CJavaGui::drawChat = ischat;
}