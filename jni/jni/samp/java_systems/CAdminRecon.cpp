//
// Created by Cross on 17.09.2025.
//
#include "CAdminRecon.h"
#include <jni.h>
#include <cmath> // sqrtf

#include "main.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "CHUD.h"

jobject j_mAdminRecon;

// Кэш уровней (обновляется из packetAdminRecon)
int g_PlayerLevel[MAX_PLAYERS] = {0};

bool CAdminRecon::bIsToggle = false;
PLAYERID CAdminRecon::iPlayerID = INVALID_PLAYER_ID;

void CAdminRecon::hide(){
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(j_mAdminRecon);
    jmethodID Hide = env->GetMethodID(clazz, "hide", "()V");

    env->CallVoidMethod(j_mAdminRecon, Hide);

    CAdminRecon::bIsToggle = false;
    CAdminRecon::iPlayerID = INVALID_PLAYER_ID;
}

void CAdminRecon::tempToggle(bool toggle){
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(j_mAdminRecon);
    jmethodID tempToggle = env->GetMethodID(clazz, "tempToggle", "(Z)V");
    env->CallVoidMethod(j_mAdminRecon, tempToggle, toggle);
}

void CAdminRecon::show(int targetId){
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if (!env) return;

    CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
    if (!pPlayerPool || !pPlayerPool->m_pPlayers[targetId]) return;

    Log("AdminRecon::show - targetId=%d, name=%s", targetId, pPlayerPool->GetPlayerName(targetId));

    const char* name = pPlayerPool->GetPlayerName(targetId);
    jstring jName = env->NewStringUTF(name ? name : "");

    jclass clazz = env->GetObjectClass(j_mAdminRecon);
    if (!clazz) {
        env->DeleteLocalRef(jName);
        return;
    }
    jmethodID Show = env->GetMethodID(clazz, "show", "(Ljava/lang/String;I)V");
    if (!Show) {
        env->DeleteLocalRef(jName);
        return;
    }

    env->CallVoidMethod(j_mAdminRecon, Show, jName, (jint)targetId);

    env->DeleteLocalRef(jName);
    env->DeleteLocalRef(clazz);

    CAdminRecon::bIsToggle = true;
    CAdminRecon::iPlayerID = targetId;
}

void CAdminRecon::setStats(int money, int level, float health, float armour)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if (!env) return;

    jclass clazz = env->GetObjectClass(j_mAdminRecon);
    if (!clazz) return;

    // Java: void setStats(int money, int level, float health, float armour)
    jmethodID mid = env->GetMethodID(clazz, "setStats", "(IIFF)V");
    if (!mid) { env->DeleteLocalRef(clazz); return; }

    env->CallVoidMethod(j_mAdminRecon, mid, (jint)money, (jint)level, (jfloat)health, (jfloat)armour);

    env->DeleteLocalRef(clazz);
}

void CAdminRecon::setNet(float speedKmh, int pingMs, float lossPct)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if (!env) return;

    jclass clazz = env->GetObjectClass(j_mAdminRecon);
    if (!clazz) return;

    // Java: void setNet(float speedKmh, int pingMs, float lossPct)
    jmethodID mid = env->GetMethodID(clazz, "setNet", "(FIF)V");
    if (!mid) { env->DeleteLocalRef(clazz); return; }

    env->CallVoidMethod(j_mAdminRecon, mid, (jfloat)speedKmh, (jint)pingMs, (jfloat)lossPct);
    env->DeleteLocalRef(clazz);
}

// ===== RPC «админ-рекон/статы» (здесь же прилетает level) =====
void CNetGame::packetAdminRecon(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    uint8_t  toggle = 0;
    uint32_t targetID = 0;

    bs.Read(toggle);
    bs.Read(targetID);

    if (toggle == 1)
    {
        int32_t money = 0, level = 0;
        float health = 0.0f, armour = 0.0f;

        float   speed = 0.0f;  // км/ч
        int32_t ping  = 0;     // мс
        float   loss  = 0.0f;  // %

        bs.Read(money);
        bs.Read(level);
        bs.Read(health);
        bs.Read(armour);

        // сеть:
        bs.Read(speed);
        bs.Read(ping);
        bs.Read(loss);

        // Обновляем кэш уровня — пригодится для бейджа
        if (targetID < MAX_PLAYERS) {
            g_PlayerLevel[targetID] = (int)level;
        }

        // Обычное поведение админ-рекона (вызвано вручную)
        CAdminRecon::show((int)targetID);
        CAdminRecon::setStats((int)money, (int)level, health, armour);
        CAdminRecon::setNet(speed, (int)ping, loss);
    }
    else
    {
        CAdminRecon::hide();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_AdminRecon_init(JNIEnv *env, jobject thiz) {
    j_mAdminRecon = env->NewGlobalRef(thiz);
}

// ===== Клиентская оценка скорости (сглаживание) =====
static DWORD sLastTick = 0;
static float sLastX = 0.0f, sLastY = 0.0f, sLastZ = 0.0f;
static int   sLastForId = -1;
static float sSmoothed = 0.0f;

static bool GetTargetWorldPos(int playerId, float& x, float& y, float& z)
{
    if (!pNetGame) return false;
    CPlayerPool* pool = pNetGame->GetPlayerPool();
    if (!pool) return false;
    return pool->GetPosition((PLAYERID)playerId, x, y, z);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_criminal_moscow_gui_AdminRecon_getClientSpeed(JNIEnv* env, jobject thiz, jint playerId)
{
    float x,y,z;
    if (!GetTargetWorldPos((int)playerId, x,y,z)) {
        sLastForId = -1; sLastTick = 0; sSmoothed = 0.0f;
        return 0.0f;
    }

    DWORD now = GetTickCount();
    if (sLastForId != (int)playerId || sLastTick == 0) {
        sLastForId = (int)playerId;
        sLastTick  = now;
        sLastX = x; sLastY = y; sLastZ = z;
        sSmoothed = 0.0f;
        return 0.0f;
    }

    float dt = (now - sLastTick) / 1000.0f;
    if (dt <= 0.0f) return sSmoothed;

    float dx = x - sLastX, dy = y - sLastY, dz = z - sLastZ;
    float dist = sqrtf(dx*dx + dy*dy + dz*dz);

    sLastX = x; sLastY = y; sLastZ = z;
    sLastTick = now;

    // м/с -> км/ч
    float kmh = (dist / dt) * 3.6f;

    // сглаживание
    const float alpha = 0.35f;
    sSmoothed = alpha*kmh + (1.0f - alpha)*sSmoothed;
    if (sSmoothed < 0.01f) sSmoothed = 0.0f;

    return sSmoothed;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_AdminRecon_clickButton(JNIEnv *env, jobject thiz, jint button_id) {
    switch (button_id) {
        case CAdminRecon::Buttons::EXIT_BUTTON:{
            pNetGame->SendChatCommand("/reoff");
            CAdminRecon::hide();
            CAdminRecon::bIsToggle = false;
            CAdminRecon::iPlayerID = INVALID_PLAYER_ID;
            break;
        }
        case CAdminRecon::Buttons::STATS_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/getstats %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::FREEZE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/freeze %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::UNFREEZE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/unfreeze %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::SPAWN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/spawn %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::SLAP_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/slap %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::REFRESH_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::MUTE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/mute %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::JAIL_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/jail %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::KICK_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/kick %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::BAN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/ban %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::WARN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/warn %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::NEXT_BUTTON:{
            PLAYERID playerid = CAdminRecon::iPlayerID + 1;
            while(!pNetGame->GetPlayerPool()->GetAt(playerid))
            {
                playerid++;
                if(playerid > MAX_PLAYERS) playerid = 0;
            }
            CAdminRecon::iPlayerID = playerid;

            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::PREV_BUTTON:{
            PLAYERID playerid = CAdminRecon::iPlayerID - 1;
            while(!pNetGame->GetPlayerPool()->GetAt(playerid))
            {
                playerid--;
                if(playerid < 0) playerid = MAX_PLAYERS;
            }
            CAdminRecon::iPlayerID = playerid;

            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::FLIP_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/flip %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        default: return;
    }
}
