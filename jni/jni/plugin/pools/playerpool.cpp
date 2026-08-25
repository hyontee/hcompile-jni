#include "playerpool.h"

CRemotePlayer* CPlayerPool::GetAt(uint16_t id)
{
	if(id >= 1504) {
		return nullptr;
	}
	return m_pPlayers[id];
}
