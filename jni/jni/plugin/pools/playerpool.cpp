#include "playerpool.h"

CRemotePlayer* CPlayerPool::GetAt(uint16_t id)
{
    if (id >= MAX_PLAYERS) {
        return nullptr;
    }
    return m_pPlayers[id];
}

uint16_t CPlayerPool::GetOnlinePlayerCount() const
{
    uint16_t count = 0;
    for (uint16_t i = 0; i < MAX_PLAYERS; ++i) {
        if (m_pPlayers[i] != nullptr) {
            ++count;
        }
    }

    // The local player is not necessarily represented in m_pPlayers[].
    if (m_pLocalPlayer != nullptr) {
        const uint16_t localId = m_pLocalPlayer->GetLocalPlayerID();
        if (localId >= MAX_PLAYERS || m_pPlayers[localId] == nullptr) {
            ++count;
        }
    }

    return (count > MAX_ONLINE_PLAYERS) ? MAX_ONLINE_PLAYERS : count;
}
