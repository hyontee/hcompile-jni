#include "playerpool.h"

CRemotePlayer* CPlayerPool::GetAt(uint16_t id)
{
	if (id >= MAX_STABLE_PLAYERS) {
		return nullptr;
	}
	return m_pPlayers[id];
}

uint16_t CPlayerPool::GetOnlinePlayerCount() const
{
	uint16_t count = 0;
	for (uint16_t i = 0; i < MAX_STABLE_PLAYERS; ++i) {
		if (m_pPlayers[i] != nullptr) {
			++count;
		}
	}
	return count;
}
