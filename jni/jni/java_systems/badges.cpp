#include "badges.h"

// forward-declarations, чтобы net/*.h не тянули game/*.h
class CPlayerPed;
class CVehicle;
class CVehicleGta;
class CActorPed;

#include "../net/netgame.h"
#include "../vendor/raknet/RakClientInterface.h"

// extern CNetGame* pNetGame; — объявлен в badges.h

int      g_BadgeLevel[MAX_PLAYERS]    = {0};
uint8_t  g_BadgeIsAdmin[MAX_PLAYERS]  = {0};
uint8_t  g_BadgePending[MAX_PLAYERS]  = {0};
uint32_t g_BadgeLastTick[MAX_PLAYERS] = {0};

static inline uint32_t Now() { return GetTickCount(); }

void Badges_Init() {
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        g_BadgeLevel[i]    = 0;
        g_BadgeIsAdmin[i]  = 0;
        g_BadgePending[i]  = 0;
        g_BadgeLastTick[i] = 0;
    }
}

void Badges_Request(PLAYERID pid)
{
    if (!pNetGame || !pNetGame->GetRakClient()) return;
    if (pid >= MAX_PLAYERS) return;

    const uint32_t now = Now();
    // Антиспам: один активный запрос и не чаще раза в ~0.9 сек
    if (g_BadgePending[pid] || (now - g_BadgeLastTick[pid] < 900)) return;

    RakNet::BitStream bs;
    bs.Write((uint8_t)ID_CUSTOM_RPC);
    bs.Write((uint32_t)RPC_BADGES_QUERY);
    bs.Write((uint16_t)pid);

    pNetGame->GetRakClient()->Send(&bs, HIGH_PRIORITY, RELIABLE, 0);

    g_BadgePending[pid]  = 1;
    g_BadgeLastTick[pid] = now;
}

// Парсер payload-а: [uint16 pid][uint16 level][uint8 admin]
void Badges_HandleAnswer(RakNet::BitStream& bs)
{
    uint16_t pid   = 0;
    uint16_t level = 0;
    uint8_t  admin = 0;

    if (!bs.Read(pid))   return;
    if (!bs.Read(level)) return;
    if (!bs.Read(admin)) return;

    if (pid < MAX_PLAYERS) {
        g_BadgeLevel[pid]    = (int)level;
        g_BadgeIsAdmin[pid]  = admin;
        g_BadgePending[pid]  = 0;
        g_BadgeLastTick[pid] = Now();
    }
}

// --- Точечный сброс кэша для слота
void Badges_ResetFor(PLAYERID pid)
{
    if (pid >= MAX_PLAYERS) return;
    g_BadgeLevel[pid]    = 0;
    g_BadgeIsAdmin[pid]  = 0;
    g_BadgePending[pid]  = 0;
    g_BadgeLastTick[pid] = 0;
}

// --- Мягкий авто-рефреш
void Badges_MaybeRefresh(PLAYERID pid, uint32_t maxAgeMs)
{
    if (pid >= MAX_PLAYERS) return;
    const uint32_t now = Now();
    if (now - g_BadgeLastTick[pid] >= maxAgeMs) {
        Badges_Request(pid); // внутри уже есть pending/900ms антиспам
    }
}
