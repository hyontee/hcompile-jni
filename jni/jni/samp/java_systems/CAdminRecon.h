//
// Created by Cross on 17.09.2025.
//
#ifndef LIVERUSSIA_CADMINRECON_H
#define LIVERUSSIA_CADMINRECON_H

#include "game/common.h"
#include "main.h"

// ===== Глобальные хранилища уровней и статусов «тихих» запросов =====
extern int   g_PlayerLevel[MAX_PLAYERS];
extern bool  g_LevelProbePending[MAX_PLAYERS];
extern DWORD g_LastLevelProbeTick[MAX_PLAYERS];

// Запросить уровень у сервера для игрока pid.
// silent=true — не открывать UI рекона по приходу пакета.
void RequestPlayerLevel(PLAYERID pid, bool silent = true);

class CAdminRecon {
public:
    static bool     bIsToggle;
    static PLAYERID iPlayerID;

    static void setStats(int money, int level, float health, float armour);
    static void setNet(float speedKmh, int pingMs, float lossPct);

    static void show(int targetID);
    static void hide();

    enum Buttons{
        EXIT_BUTTON,
        STATS_BUTTON,
        FREEZE_BUTTON,
        UNFREEZE_BUTTON,
        SPAWN_BUTTON,
        SLAP_BUTTON,
        REFRESH_BUTTON,
        MUTE_BUTTON,
        JAIL_BUTTON,
        KICK_BUTTON,
        BAN_BUTTON,
        WARN_BUTTON,
        NEXT_BUTTON,
        PREV_BUTTON,
        FLIP_BUTTON
    };

    static void tempToggle(bool toggle);
};

#endif // LIVERUSSIA_CADMINRECON_H
