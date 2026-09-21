#pragma once

#include <cstdint>

#include "../localplayer.h"
#include "../remoteplayer.h"

class CPlayerPool
{
public:
	static constexpr uint16_t MAX_STABLE_PLAYERS = 200;
	CLocalPlayer* GetLocalPlayer() { return m_pLocalPlayer; }
	CRemotePlayer* GetAt(uint16_t playerId);
	uint16_t GetOnlinePlayerCount() const;
	void* GetPlayerPed(uint16_t playerId);
public:
	CLocalPlayer* m_pLocalPlayer;
	CRemotePlayer* m_pPlayers[1504];
	int m_iLocalPlayerScore;
	uint32_t m_dwLocalPlayerPing;
	int m_iPlayerScores[1504];
	uint32_t m_dwPlayerPings[1504];
};

