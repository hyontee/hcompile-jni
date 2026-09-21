//
// Created by Error on 24.04.2024.
//

#ifndef BR_CFAMILY_H
#define BR_CFAMILY_H

#include <jni.h>

static class CFamily {
public:
    static jobject thiz;
    static jclass clazz;
    static bool bIsShow;

    static void startRender();
    static void show(bool toggle, const char name[], int money, int coin, bool ifLeader);
    static void LoadCar(const char name[], uint16_t vehid, uint16_t access, bool isspawn);
    static void LoadNotif(const char text[], const char name[]);


    static void LoadRaiting(const char name[], uint32_t rep, uint16_t capture);

    static void
    setInfoMainMenu(int status, int rep, int people, int upd_sklad, int upd_weapon, int upd_people,
                    int money, int mats, int mask, int oils, int ammo, int armor, int skin_id,
                    const char playername[]);

    static void LoadPlayers(int id, const char name[], int lvl, const char nameRang[], int rang, int warn, int mute, int phone,
                     bool isOnline);

    static void UpdateAdapterCar(int access, int id, bool isspawn);

    static void UpdateAdapterPlayer(int id, int warn, int rang, int mute);
    static void LoadNameRang(const char rang[]);


    static void
    SetAccessRang(uint16_t sklad,
                  uint16_t invite, uint16_t uninvite, uint16_t mute, uint16_t giverang);


    static void AddInfoStock(int allmoney, int mats, int mask, int oil, int armor);
};


#endif //BR_CFAMILY_H
