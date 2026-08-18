// CTabletMusic_JNI.cpp Ч FULL REPLACEMENT
#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>
#include "CTabletMusic.h"
#include "util/CJavaWrapper.h"

#ifndef LOGI
#define LOG_TAG "TabletJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif


#define PACKET_CUSTOMRPC_BB 251   // »ћ≈ЌЌќ 251, не 254
#define RPC_BB_SHOW_TABLET  241
#define RPC_BB_SELECT       242
#define RPC_BB_STOP         243
#define RPC_BB_MODE 244
// === ”же реализовано в твоЄм netgame.cpp (3-арг. верси€) ===
extern bool SendCustomRPCToServer(unsigned char packetId, const void* data, int len);

// === Ќаш враппер (4-арг. верси€) Ч ѕј ”≈“ полезную нагрузку как ждЄт сервер ===
// ‘ормат payload:
//   [u32 rpcId]
//   if (rpcId == RPC_BB_SELECT) -> [u16 len][len bytes URL]
//   if (rpcId == RPC_BB_STOP)   -> (ничего больше)
static bool SendCustomRPCToServer(unsigned char packetId, unsigned int rpcId, const void* data, int len)
{
    // —обираем полезную нагрузку в буфер
    std::vector<unsigned char> buf;
    buf.reserve(4 + (rpcId == RPC_BB_SELECT ? 2 + (len > 0 ? len : 0) : 0));

    // ѕишем u32 rpcId (LE Ч стандартно дл€ наших структур)
    {
        unsigned int v = rpcId;
        buf.push_back((unsigned char)(v & 0xFF));
        buf.push_back((unsigned char)((v >> 8) & 0xFF));
        buf.push_back((unsigned char)((v >> 16) & 0xFF));
        buf.push_back((unsigned char)((v >> 24) & 0xFF));
    }

    if (rpcId == RPC_BB_SELECT) {
        // ожидаетс€: [u16 len][len bytes URL]
        unsigned short ulen = (unsigned short)(len > 0 ? len : 0);
        buf.push_back((unsigned char)(ulen & 0xFF));
        buf.push_back((unsigned char)((ulen >> 8) & 0xFF));
        if (ulen && data) {
            const unsigned char* p = (const unsigned char*)data;
            buf.insert(buf.end(), p, p + ulen);
        }
    }
    // RPC_BB_STOP Ч только u32 rpcId

    return SendCustomRPCToServer(packetId, buf.data(), (int)buf.size());
}

// === ”тилиты JNI ===
static inline std::string JStringToUtf(JNIEnv* env, jstring s){
    if(!env || !s) return {};
    const char* c = env->GetStringUTFChars(s,nullptr);
    std::string out = c?c:"";
    if(c) env->ReleaseStringUTFChars(s,c);
    return out;
}

// ==== JNI из TabletBridge ====

// toggle() Ч как было
extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tablet_TabletBridge_toggle(JNIEnv*, jclass)
{
    CTabletMusic::Toggle();
}

// startBoombox(String url, String title) ? 254/242: [u32 rpc][u16 len][url bytes]
extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tablet_TabletBridge_1startBoombox(
        JNIEnv* env, jclass, jstring jurl, jstring /*jtitle*/)
{
    std::string url = JStringToUtf(env, jurl);
    if (url.empty()) {
        LOGW("startBoombox: empty url");
        return;
    }
    // —ервер ждЄт длину без нулевого терминирующего символа ? len = url.size()
    const int len = (int)url.size();
    if (!SendCustomRPCToServer(PACKET_CUSTOMRPC_BB, RPC_BB_SELECT, url.data(), len)) {
        LOGW("startBoombox: send failed");
    } else {
        LOGI("startBoombox: sent url(len=%d)='%s'", len, url.c_str());
    }
}

// stopBoombox() ? 254/243: [u32 rpc]
extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tablet_TabletBridge_1stopBoombox(
        JNIEnv*, jclass)
{
    if (!SendCustomRPCToServer(PACKET_CUSTOMRPC_BB, RPC_BB_STOP, nullptr, 0)) {
        LOGW("stopBoombox: send failed");
    } else {
        LOGI("stopBoombox: sent");
    }
}

// ¬ход€щий RPC (241) от сервера Ч показать планшет
extern "C" void BB_OnRpcShowTabletBoombox()
{
    JNIEnv* env = g_pJavaWrapper ? g_pJavaWrapper->GetEnv() : nullptr;
    if(!env){ LOGE("BB_OnRpcShowTabletBoombox: env==null"); return; }
    CTabletMusic::Init(env);
    CTabletMusic::Show();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tablet_TabletBridge_1setBoomboxMode(
        JNIEnv* env, jclass clazz, jboolean enabled) {

    const bool state = (enabled == JNI_TRUE);
    // payload = 1 байт: 1 или 0
    unsigned char payload = state ? 1 : 0;
    if (!SendCustomRPCToServer((unsigned char)RPC_BB_MODE, &payload, sizeof(payload))) {
        LOGW("setBoomboxMode: failed to send state=%d", state);
    } else {
        LOGI("setBoomboxMode: sent state=%d", state);
    }
}