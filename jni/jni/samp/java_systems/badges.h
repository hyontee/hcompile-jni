#pragma once
#include <cstdint>
#include "../main.h"
#include "../vendor/raknet/BitStream.h"

class CNetGame;
extern CNetGame* pNetGame;

// --- RPC ids (держим в одном месте; если уже есть в netgame.h — не переопределяем)
#ifndef RPC_BADGES_QUERY
#define RPC_BADGES_QUERY   201   // client -> server: [uint16 pid]
#endif
#ifndef RPC_BADGES_ANSWER
#define RPC_BADGES_ANSWER  202   // server -> client: [uint16 pid][level][admin]
#endif

// Кэш клиентской стороны
extern int      g_BadgeLevel[MAX_PLAYERS];    // 0..max
extern uint8_t  g_BadgeIsAdmin[MAX_PLAYERS];  // 0/1
extern uint8_t  g_BadgePending[MAX_PLAYERS];  // флаг активного запроса
extern uint32_t g_BadgeLastTick[MAX_PLAYERS]; // GetTickCount последнего запроса/ответа

// Инициализация/сброс всего кэша
void Badges_Init();

// Отправляем на сервер тихий запрос инфы по pid
void Badges_Request(PLAYERID pid);

// Обрабатываем ответ сервера (вызвать из Packet_CustomRPC при RPC_BADGES_ANSWER)
void Badges_HandleAnswer(RakNet::BitStream& bs);

// --- Новое: точечный сброс кэша для слота (звать при освобождении/неактивности PID)
void Badges_ResetFor(PLAYERID pid);

// --- Новое: мягкий авто-рефреш — если данные «старше» maxAgeMs, тихо перепросит
void Badges_MaybeRefresh(PLAYERID pid, uint32_t maxAgeMs = 5000);

// Утилиты доступа
inline int Badges_GetLevel(PLAYERID pid) {
    return (pid < MAX_PLAYERS) ? g_BadgeLevel[pid] : 0;
}
inline bool Badges_IsAdmin(PLAYERID pid) {
    return (pid < MAX_PLAYERS) ? (g_BadgeIsAdmin[pid] != 0) : false;
}
