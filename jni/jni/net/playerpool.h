#pragma once

#include <unordered_map>
#include "main.h"

class CPlayerPool
{
public:
	static void Init();
	static void Free();

	static bool Process();

	static void UpdateScore(PLAYERID playerId, int iScore);
	static void UpdatePing(PLAYERID playerId, uint32_t dwPing);

	static int GetLocalPlayerScore() { return m_iLocalPlayerScore; }
	static uint32_t GetLocalPlayerPing() { return m_dwLocalPlayerPing; }

	static int GetRemotePlayerScore(PLAYERID playerId)
	{
		if (playerId > MAX_PLAYERS) return 0;
		return m_iPlayerScores[playerId];
	}

	static uint32_t GetRemotePlayerPing(PLAYERID playerId)
	{
		if (playerId > MAX_PLAYERS) return 0;
		return m_dwPlayerPings[playerId];
	}

	static void SetLocalPlayerName(const char* szName) { strcpy(m_szLocalPlayerName, szName); }
	static char* GetLocalPlayerName() { return m_szLocalPlayerName; }
	static void SetLocalPlayerID(PLAYERID MyPlayerID)
	{
		strcpy(m_szPlayerNames[MyPlayerID], m_szLocalPlayerName);
		m_LocalPlayerID = MyPlayerID;
	}
	static PLAYERID GetLocalPlayerID() { return m_LocalPlayerID; }
	static CLocalPlayer* GetLocalPlayer() { return m_pLocalPlayer; }
	// remote
	static bool New(PLAYERID playerId, char* szPlayerName, bool bIsNPC);
	static bool Delete(PLAYERID playerId, uint8_t byteReason);

	static CRemotePlayer* GetAt(PLAYERID playerId)
	{
		auto it = list.find(playerId);
		if(it != list.end())
			return list[playerId];

		return nullptr;
	}

	static void SetPlayerName(PLAYERID playerId, char* szName) { strcpy(m_szPlayerNames[playerId], szName); }
	static char* GetPlayerName(PLAYERID playerId) {
		if(playerId == GetLocalPlayerID()) {
			return GetLocalPlayerName();
		}
		if(GetAt(playerId)) {
			return m_szPlayerNames[playerId];
		}
		return "None";
	}

	static PLAYERID FindRemotePlayerIDFromGtaPtr(PED_TYPE * pActor);

	static void ResetCollisionChecking();
	static void ApplyCollisionChecking();

	static CRemotePlayer* GetSpawnedPlayer(PLAYERID playerid)
	{
		auto it = spawnedPlayers.find(playerid);
		if(it != spawnedPlayers.end())
			return spawnedPlayers[playerid];

		return nullptr;
	}

	// REMOTE
	//static inline CRemotePlayer	*m_pPlayers[MAX_PLAYERS];
	static inline std::unordered_map<PLAYERID, CRemotePlayer*> list {};
	static inline std::unordered_map<PLAYERID, CRemotePlayer*> spawnedPlayers {};
private:
	// LOCAL
	static inline PLAYERID		m_LocalPlayerID;
	static inline CLocalPlayer	*m_pLocalPlayer;
	static inline char			m_szLocalPlayerName[MAX_PLAYER_NAME+1];
	static inline int			m_iLocalPlayerScore;
	static inline uint32_t		m_dwLocalPlayerPing;

	//static inline bool			m_bPlayerSlotState[MAX_PLAYERS];
	static inline char			m_szPlayerNames[MAX_PLAYERS][MAX_PLAYER_NAME+1];
	static inline int			m_iPlayerScores[MAX_PLAYERS];
	static inline uint32_t		m_dwPlayerPings[MAX_PLAYERS];
	static inline bool 			m_bCollisionChecking[MAX_PLAYERS];

};