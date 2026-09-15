//
// Created by AM0R3M1O on 28.02.2021.
//


#include <list>
#include <string>

#ifndef CLIENT_KILLLIST_H
#define CLIENT_KILLLIST_H

// Sturct
struct KillListStrutct
{
    std::string playerName;
    uint32_t playercolor;
	uint16_t playerid;
    std::string killerName;
    uint32_t killercolor;
	uint16_t killerid;
    uint8_t reason;
};


class KillList {

public:
    KillList();
    ~KillList();

    // Functions
public:
    void MakeRecord(const char* playername, unsigned int playercolor, unsigned short playerid, const char* killername, unsigned int killercolor, unsigned short killerid, uint8_t reason);
    void Render();

private:
    const char* SpriteIDForWeapon(uint8_t byteWeaponID);

    // Vars
private:
    std::list<KillListStrutct*> m_pKillList;

};


#endif //CLIENT_KILLLIST_H
