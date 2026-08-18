#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

#include "..//chatwindow.h"
#include "..//game/game.h"
#include "..//net/netgame.h"
#include "../game/RW/RenderWare.h"

#include <cstring> // memset, strcpy
#include <cmath>   // sqrtf

extern CGame*    pGame;
extern CNetGame* pNetGame;

CPlayerPool::CPlayerPool()
{
    m_pLocalPlayer = new CLocalPlayer();

    // Обнулим указатели на игроков
    for (auto& m_pPlayer : m_pPlayers)
        m_pPlayer = nullptr;

    // Базовая инициализация локальных полей
    m_LocalPlayerID      = INVALID_PLAYER_ID;
    m_iLocalPlayerScore  = 0;
    m_dwLocalPlayerPing  = 0;
    std::memset(m_szLocalPlayerName, 0, sizeof(m_szLocalPlayerName));

    // Инициализация массивов для всех игроков
    std::memset(m_szPlayerNames,      0, sizeof(m_szPlayerNames));
    std::memset(m_iPlayerScores,      0, sizeof(m_iPlayerScores));
    std::memset(m_dwPlayerPings,      0, sizeof(m_dwPlayerPings));
    std::memset(m_bCollisionChecking, 0, sizeof(m_bCollisionChecking));
    std::memset(m_iPlayerLevels,      0, sizeof(m_iPlayerLevels)); // уровни
}

CPlayerPool::~CPlayerPool()
{
    delete m_pLocalPlayer;
    m_pLocalPlayer = nullptr;

    for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
        Delete(playerId, 0);
}

void CPlayerPool::ApplyCollisionChecking()
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        CRemotePlayer* pPlayer = GetAt(i);
        if (!pPlayer) continue;

        CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
        if (!pPlayerPed) continue;

        if (!pPlayerPed->IsInVehicle())
        {
            m_bCollisionChecking[i] = pPlayerPed->GetCollisionChecking();
            pPlayerPed->SetCollisionChecking(true);
        }
    }
}

void CPlayerPool::ResetCollisionChecking()
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        CRemotePlayer* pPlayer = GetAt(i);
        if (!pPlayer) continue;

        CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
        if (!pPlayerPed) continue;

        if (!pPlayerPed->IsInVehicle())
            pPlayerPed->SetCollisionChecking(m_bCollisionChecking[i]);
    }
}

void CPlayerPool::UpdateScore(PLAYERID playerId, int iScore)
{
    if (playerId == m_LocalPlayerID)
    {
        m_iLocalPlayerScore = iScore;
    }
    else
    {
        if (playerId > MAX_PLAYERS - 1) return;
        m_iPlayerScores[playerId] = iScore;
    }
}

void CPlayerPool::UpdatePing(PLAYERID playerId, uint32_t dwPing)
{
    if (playerId == m_LocalPlayerID)
    {
        m_dwLocalPlayerPing = dwPing;
    }
    else
    {
        if (playerId > MAX_PLAYERS - 1) return;
        m_dwPlayerPings[playerId] = dwPing;
    }
}

void CPlayerPool::Process()
{
    for (const auto& player : m_pPlayers)
    {
        if (!player) continue;
        player->Process();
    }

    if (m_pLocalPlayer)
        m_pLocalPlayer->Process();
}

PLAYERID CPlayerPool::GetCount()
{
    PLAYERID byteCount = 0;
    for (PLAYERID p = 0; p < MAX_PLAYERS; p++)
        if (m_pPlayers[p]) byteCount++;
    return byteCount;
}

bool CPlayerPool::New(PLAYERID playerId, char* szPlayerName, bool IsNPC)
{
    if (playerId > MAX_PLAYERS - 1) return false;

    m_pPlayers[playerId] = new CRemotePlayer();

    if (m_pPlayers[playerId])
    {
        std::strcpy(m_szPlayerNames[playerId], szPlayerName ? szPlayerName : "");
        m_pPlayers[playerId]->SetID(playerId);
        m_pPlayers[playerId]->SetNPC(IsNPC);

        // Сброс кэшированных данных по игроку
        m_iPlayerScores[playerId]      = 0;
        m_dwPlayerPings[playerId]      = 0;
        m_bCollisionChecking[playerId] = false;
        m_iPlayerLevels[playerId]      = 0; // уровень по умолчанию

        return true;
    }

    return false;
}

bool CPlayerPool::Delete(PLAYERID playerId, uint8_t /*byteReason*/)
{
    if (playerId > MAX_PLAYERS - 1) return false;

    Log("CPlayerPool::Delete %d", playerId);
    if (!m_pPlayers[playerId])
        return false;

    if (GetLocalPlayer()->IsSpectating() && GetLocalPlayer()->m_SpectateID == playerId)
        GetLocalPlayer()->ToggleSpectating(false);

    delete m_pPlayers[playerId];
    m_pPlayers[playerId] = nullptr;

    // Сброс кэшированных данных по игроку
    m_iPlayerScores[playerId]      = 0;
    m_dwPlayerPings[playerId]      = 0;
    m_bCollisionChecking[playerId] = false;
    m_iPlayerLevels[playerId]      = 0;
    std::memset(m_szPlayerNames[playerId], 0, sizeof(m_szPlayerNames[playerId]));

    return true;
}

PLAYERID CPlayerPool::FindRemotePlayerIDFromGtaPtr(CPedGta* pActor)
{
    CPlayerPed* pPlayerPed;

    for (auto& m_pPlayer : m_pPlayers)
    {
        if (!m_pPlayer) continue;

        pPlayerPed = m_pPlayer->GetPlayerPed();
        if (!pPlayerPed) continue;

        CPedGta* pTestActor = pPlayerPed->GetGtaActor();
        if (pTestActor != NULL && pActor == pTestActor) // found it
            return m_pPlayer->GetID();
    }

    return INVALID_PLAYER_ID;
}

bool CPlayerPool::GetPosition(PLAYERID id, float& x, float& y, float& z)
{
    CRemotePlayer* pRemote = GetAt(id);
    if (!pRemote) return false;

    CPlayerPed* pPed = pRemote->GetPlayerPed();
    if (!pPed) return false;

    RwMatrix mat{};
    pPed->GetMatrix(&mat);

    x = mat.pos.x;
    y = mat.pos.y;
    z = mat.pos.z;
    return true;
}

// ===== Уровень игрока (LVL) =====
void CPlayerPool::SetLevel(PLAYERID playerId, int level)
{
    if (playerId > MAX_PLAYERS - 1) return;
    if (level < 0) level = 0;
    m_iPlayerLevels[playerId] = level;
}

int CPlayerPool::GetLevel(PLAYERID playerId) const
{
    if (playerId > MAX_PLAYERS - 1) return 0;
    return m_iPlayerLevels[playerId];
}
