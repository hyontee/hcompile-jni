#pragma once

#include <cstdint>

#include "../localplayer.h"
#include "../remoteplayer.h"

class CPlayerPool
{
public:
	static constexpr uint16_t MAX_PLAYERS = 1504;
	static constexpr uint16_t MAX_ONLINE_PLAYERS = 200;

	CLocalPlayer* GetLocalPlayer() { return m_pLocalPlayer; }
	CRemotePlayer* GetAt(uint16_t playerId);
	uint16_t GetOnlinePlayerCount() const;
	void* GetPlayerPed(uint16_t playerId);
public:
	CLocalPlayer* m_pLocalPlayer;
	CRemotePlayer* m_pPlayers[MAX_PLAYERS];
	int m_iLocalPlayerScore;
	uint32_t m_dwLocalPlayerPing;
	int m_iPlayerScores[MAX_PLAYERS];
	uint32_t m_dwPlayerPings[MAX_PLAYERS];
};

