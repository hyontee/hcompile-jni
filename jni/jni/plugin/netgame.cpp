#include "netgame.h"
#include "xorstr.h"
#include "plugin.h"
#include <cmath>

#define NETGAME_VERSION 4057

extern RakClientInterface* pRakClient;

uint16_t CNetGame::m_nLastSAMPDialogID;

namespace
{
constexpr uint16_t kPlayerPoolSize = 1504;
constexpr uint32_t kMaxPacketsPerProcessTick = 1024;

inline uintptr_t ResolveCachedAddress(uintptr_t& cached, const char* name)
{
    if (!cached) {
        cached = CGameAPI::GetBase(name);
    }
    return cached;
}

inline bool IsFiniteVector(const CVector& vec)
{
    return std::isfinite(vec.x) && std::isfinite(vec.y) && std::isfinite(vec.z);
}

inline bool IsFiniteQuaternion(float w, float x, float y, float z)
{
    return std::isfinite(w) && std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

inline CRemotePlayer* GetRemotePlayerForSync(uint16_t playerId)
{
    if (playerId >= kPlayerPoolSize) {
        return nullptr;
    }

    CPlayerPool* pool = CNetGame::GetPlayerPool();
    if (!pool) {
        return nullptr;
    }

    return pool->GetAt(playerId);
}
}

uint8_t GetPacketID(Packet *p)
{
	if(p == 0 || !p->data || p->length == 0) { return 255; }
	if ((uint8_t)p->data[0] == ID_TIMESTAMP) {
		if (p->length <= sizeof(uint8_t) + sizeof(RakNetTime)) {
			return 255;
		}
		return (uint8_t)p->data[sizeof(uint8_t) + sizeof(RakNetTime)];
	} else {
		return (uint8_t)p->data[0];
	}
}

static bool ReadSyncPacketHeader(RakNet::BitStream& bsData, const Packet* pkt, uint8_t& pktId, uint16_t& playerId)
{
    if (!pkt || !pkt->data || pkt->length == 0) {
        return false;
    }

    if (pkt->data[0] == ID_TIMESTAMP) {
        uint8_t timestampId = 0;
        RakNetTime timestamp = 0;
        if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&timestampId), 8)) {
            return false;
        }
        if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&timestamp), sizeof(RakNetTime) * 8)) {
            return false;
        }
    }

    return bsData.ReadBits(reinterpret_cast<unsigned char*>(&pktId), 8) &&
           bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16);
}

CPlayerPool* CNetGame::GetPlayerPool()
{
    static uintptr_t addr = 0;
    if (!ResolveCachedAddress(addr, xorstr("CNetGame::m_pPlayerPool"))) {
        return nullptr;
    }
    return *(CPlayerPool **)(addr);
}

void CNetGame::ProcessNetwork()
{
	Packet* pkt = nullptr;
	uint8_t packetIdentifier;
    uint32_t processedPackets = 0;
	while((pkt = pRakClient->Receive()) != nullptr)
	{
		packetIdentifier = GetPacketID(pkt);
		switch(packetIdentifier)
		{
			case ID_FAILED_INITIALIZE_ENCRIPTION:
				CChat::AddDebugMessage(xorstr("Failed to initialize encryption."));
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				CChat::AddDebugMessage(xorstr("Сервер не отвечает. Переподключение..."));
				SetGameState(GAMESTATE_WAIT_CONNECT);
				
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				CChat::AddDebugMessage(xorstr("Сервер полон. Переподключение..."));
				SetGameState(GAMESTATE_WAIT_CONNECT);
				pRakClient->Disconnect(0, 0);
				break;
			case ID_CONNECTION_BANNED:
				CChat::AddDebugMessage(xorstr("Вы были заблокированы на этом сервере."));
				break;
			case ID_INVALID_PASSWORD:
				CChat::AddDebugMessage(xorstr("На данный момент сервер закрыт на тех.работы..."));
				pRakClient->Disconnect(0);
				break;
			case ID_AUTH_KEY:
				Packet_AuthKey(pkt);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				Packet_ConnectionSucceeded(pkt);
				break;
			case ID_CONNECTION_LOST:
				CChat::AddDebugMessage(xorstr("Потеряно соединение к серверу. Переподключение через 15 секунд..."));
				Packet_ConnectionLost(pkt);
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				CChat::AddDebugMessage(xorstr("Потеряно соединение к серверу. Переподключение через 15 секунд..."));
				pRakClient->Disconnect(2000, 0);
				break;
			case ID_AIM_SYNC:
				Packet_AimSync(pkt);
				break;
				
			case ID_PLAYER_SYNC:
				Packet_PlayerSync(pkt);
				break;

			case ID_VEHICLE_SYNC:
				Packet_VehicleSync(pkt);
				break;

			case ID_PASSENGER_SYNC:
				Packet_PassengerSync(pkt);
				break;
			
			case ID_BULLET_SYNC:
				Packet_BulletSync(pkt);
				break;
			
			case 252: 
			{
		       Packet_GUI(pkt);
   			 break;
			}
		}

		pRakClient->DeallocatePacket(pkt);

        if (++processedPackets >= kMaxPacketsPerProcessTick) {
            break;
        }
	}
}

int CNetGame::GetGameState()
{
    static uintptr_t addr = 0;
    if (!ResolveCachedAddress(addr, xorstr("CNetGame::m_iGameState"))) {
        return GAMESTATE_NONE;
    }
	return *(int *)(addr);
}

void CNetGame::SetGameState(int state)
{
    static uintptr_t addr = 0;
    if (!ResolveCachedAddress(addr, xorstr("CNetGame::m_iGameState"))) {
        return;
    }
	*(int *)(addr) = state;
}

#ifdef __arm__
void gen_auth_key(char buf[260], char* auth_in);
void CNetGame::Packet_AuthKey(Packet* pkt)
{
	RakNet::BitStream bsAuth((unsigned char *)pkt->data, pkt->length, false);

	uint8_t byteAuthLen;
	char szAuth[260];

	bsAuth.IgnoreBits(8);
	bsAuth.Read(byteAuthLen);
	bsAuth.Read(szAuth, byteAuthLen);
	szAuth[byteAuthLen] = '\0';

	char szAuthKey[260];
	gen_auth_key(szAuthKey, szAuth);

	RakNet::BitStream bsKey;
	uint8_t byteAuthKeyLen = (uint8_t)strlen(szAuthKey);

	bsKey.Write((uint8_t)ID_AUTH_KEY);
	bsKey.Write((uint8_t)byteAuthKeyLen);
	bsKey.Write(szAuthKey, byteAuthKeyLen);
	pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);
}
#endif

#ifdef __aarch64__
extern char AuthKeyTable[512][2][128];
void CNetGame::Packet_AuthKey(Packet* pkt)
{
    char* auth_key;
    bool found_key = false;

    for (int x = 0; x < 512; x++)
    {
        if (!strcmp(((char*)pkt->data + 2), AuthKeyTable[x][0]))
        {
            auth_key = AuthKeyTable[x][1];
            found_key = true;
        }
    }

    if (found_key)
    {
        RakNet::BitStream bsKey;
        BYTE byteAuthKeyLen;

        byteAuthKeyLen = (BYTE)strlen(auth_key);

        bsKey.Write((BYTE)ID_AUTH_KEY);
        bsKey.Write((BYTE)byteAuthKeyLen);
        bsKey.Write(auth_key, byteAuthKeyLen);

        pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, NULL);

    }
    else
    {
    
    }
}
#endif

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
	reinterpret_cast<void(*)()>(CGameAPI::GetBase(xorstr("CNetGame::Packet_ConnectionLost")))();
}

void CNetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
	RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
	uint16_t MyPlayerID;
	unsigned int uiChallenge;

	bsSuccAuth.IgnoreBits(8); // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port
	bsSuccAuth.Read(MyPlayerID);
	bsSuccAuth.Read(uiChallenge);

    CPlayerPool* pool = GetPlayerPool();
    if (!pool) {
        return;
    }
    CLocalPlayer* localPlayer = pool->GetLocalPlayer();
    if (!localPlayer) {
        return;
    }

	localPlayer->SetLocalPlayerID(MyPlayerID);

	int iVersion = NETGAME_VERSION;
	char byteMod = 0x01;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

	const char* sampVersion = xorstr("0.3.7");
	const char* auth_bs = xorstr("15121F6F18550C00AC4B4F8A167D0379BB0ACA99043");
	
	const char* localPlayerName = (const char *)(localPlayer->GetLocalPlayerName());
    if (!localPlayerName) {
        localPlayerName = "";
    }
	
	char byteAuthBSLen = (char)strlen(auth_bs);
	char byteNameLen = (char)strlen(localPlayerName);
	char byteClientverLen = (char)strlen(sampVersion);

	RakNet::BitStream bsSend;
	bsSend.Write(iVersion);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(localPlayerName, byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(auth_bs, byteAuthBSLen);
	bsSend.Write(byteClientverLen);
	bsSend.Write(sampVersion, byteClientverLen);
	pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	
	SetGameState(GAMESTATE_AWAIT_JOIN);
}

void CNetGame::Packet_AimSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    
    if(GetGameState() != GAMESTATE_CONNECTED) { return; }
    
    uint8_t pktId;
    uint16_t playerId;
    uint8_t aimSyncBuffer[31] = {0};
    
    if (!ReadSyncPacketHeader(bsData, pkt, pktId, playerId)) { return; }
    if (playerId >= kPlayerPoolSize) { return; }
    if (!bsData.ReadBits((unsigned char *)&aimSyncBuffer, 31 * 8)) { return; }
    
    CRemotePlayer* remote_player = GetRemotePlayerForSync(playerId);
    if(remote_player) {
        remote_player->StoreAimSyncData(aimSyncBuffer, 0);
    }
}

void CNetGame::Packet_PlayerSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);

	if(GetGameState() != GAMESTATE_CONNECTED) { return; }

	bool accepted_sync = false;

	uint8_t pktId;
	uint16_t playerId;
	int16_t lrAnalog, udAnalog;
	uint16_t wKeys;
	CVector vecPos = {0.0f, 0.0f, 0.0f};
	float tw = 0.0f, tx = 0.0f, ty = 0.0f, tz = 0.0f;
	uint8_t byteHealthArmour = 0, byteHealth = 0, byteArmour = 0;
	CVector vecMoveSpeed = {0.0f, 0.0f, 0.0f};
	float mx, my, mz;
	uint16_t wSurfInfo = 0;
	CVector vecSurfOffsets = {0.0f, 0.0f, 0.0f};
	uint8_t byteCurrentWeapon = 0, byteSpecialAction = 0;

	if (!ReadSyncPacketHeader(bsData, pkt, pktId, playerId)) { return; }
    if (playerId >= kPlayerPoolSize) { return; }

	int readOffset = bsData.readOffset;
	int numberOfBitsUsed = bsData.numberOfBitsUsed;
	if(readOffset >= numberOfBitsUsed) {
		return;
	}
	int v6 = bsData.data[readOffset >> 3];
	char v7 = readOffset & 7;
	int v8 = readOffset + 1;
	bsData.readOffset = v8;
	if (((v6 << v7) & 0x80) != 0)
	{
		if (!bsData.ReadBits((unsigned char *)&lrAnalog, 16))
		{
			return;
		}
		v8 = bsData.readOffset;
		numberOfBitsUsed = bsData.numberOfBitsUsed;
	} 
	else 
	{
		lrAnalog = 0; // fix игрок идет вперед
	}
	if (v8 < numberOfBitsUsed)
	{
		int v9 = bsData.data[v8 >> 3];
		bsData.readOffset = v8 + 1;

		bool ok = true;

		if (((v9 << (v8 & 7)) & 0x80) != 0)
		{
			ok = bsData.ReadBits((unsigned char *)&udAnalog, 16);
		}
		else
		{
			udAnalog = 0; // fix игрок идет вперед
		}

		if (ok
			&& bsData.ReadBits((unsigned char *)&wKeys, 16)
			&& bsData.Read((char *)&vecPos, 12)
			&& bsData.ReadNormQuat<float>(tw, tx, ty, tz)
			&& bsData.ReadBits((unsigned char *)&byteHealthArmour, 8)
			&& bsData.ReadBits((unsigned char *)&byteCurrentWeapon, 8)
			&& bsData.ReadBits((unsigned char *)&byteSpecialAction, 8)
			&& bsData.ReadVector<float>(mx, my, mz))
		{
			vecMoveSpeed = CVector(mx, my, mz);

			readOffset = bsData.readOffset;
			if (readOffset >= bsData.numberOfBitsUsed) {
				return;
			}
			int v7_ = bsData.data[readOffset >> 3];
			bsData.readOffset = readOffset + 1;
			if (((v7_ << (readOffset & 7)) & 0x80) != 0)
			{
				if (bsData.ReadBits((unsigned char *)&wSurfInfo, 16)
					&& bsData.ReadBits((unsigned char *)&vecSurfOffsets.x, 32)
					&& bsData.ReadBits((unsigned char *)&vecSurfOffsets.y, 32)
					&& bsData.ReadBits((unsigned char *)&vecSurfOffsets.z, 32))
				{
					// surf
				}
			}
			accepted_sync = true;
		}
	}

    if (!accepted_sync) {
        return;
    }
    if (!IsFiniteVector(vecPos) || !IsFiniteQuaternion(tw, tx, ty, tz) ||
        !IsFiniteVector(vecMoveSpeed) || !IsFiniteVector(vecSurfOffsets)) {
        return;
    }

	uint8_t byteArmTemp = 0, byteHlTemp = 0;
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) {
	byteArmour = 100;
	} else if(byteArmTemp == 0) {
	byteArmour = 0;
	} else {
	byteArmour = byteArmTemp * 7;
	}

	if(byteHlTemp == 0xF) {
	byteHealth = 100;
	} else if(byteHlTemp == 0) {
	byteHealth = 0;
	} else {
	byteHealth = byteHlTemp * 7;
	}

	BROnFootSyncData ofSync = {0};
	ofSync.lrAnalogLeftStick = lrAnalog;
	ofSync.udAnalogLeftStick = udAnalog;
	ofSync.wKeys = wKeys;
	ofSync.vecPos = vecPos;
	ofSync.quatw = tw;
	ofSync.quatx = tx;
	ofSync.quaty = ty;
	ofSync.quatz = tz;
	ofSync.health = byteHealth;
	ofSync.armour = byteArmour;
	ofSync.byteCurrentWeapon = byteCurrentWeapon;
	ofSync.byteSpecialAction = byteSpecialAction;
	ofSync.vecMoveSpeed = vecMoveSpeed;
	ofSync.vecSurfOffsets = vecSurfOffsets;
	ofSync.wSurfInfo = wSurfInfo;

    CRemotePlayer* remote_player = GetRemotePlayerForSync(playerId);
    if(remote_player) {
        remote_player->StoreSyncData(&ofSync, 0);
    }
}
void CNetGame::Packet_VehicleSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);
	
	if(GetGameState() != GAMESTATE_CONNECTED) { return; }
	
	uint8_t pktId;
	uint16_t playerId;
	
	BRInCarSyncData icsync = {0};
	
	if (!ReadSyncPacketHeader(bsData, pkt, pktId, playerId)) { return; }
    if (playerId >= kPlayerPoolSize) { return; }
	
	if (!bsData.ReadBits((unsigned char *)&icsync.VehicleID, 16)) { return; }
	if (!bsData.ReadBits((unsigned char *)&icsync.lrAnalogLeftStick, 16)) { return; }
	if (!bsData.ReadBits((unsigned char *)&icsync.udAnalogLeftStick, 16)) { return; }
	if (!bsData.ReadBits((unsigned char *)&icsync.wKeys, 16)) { return; }
	if (!bsData.ReadNormQuat<float>(icsync.quatw, icsync.quatx, icsync.quaty, icsync.quatz)) { return; }
	if (!bsData.ReadBits((unsigned char *)&icsync.vecPos, 96)) { return; }
	if (!bsData.ReadVector<float>(icsync.vecMoveSpeed.x, icsync.vecMoveSpeed.y, icsync.vecMoveSpeed.z)) { return; }
	uint16_t wTempCarHealth;
	if (!bsData.ReadBits((unsigned char *)&wTempCarHealth, 16)) { return; }
	icsync.fCarHealth = wTempCarHealth;
	
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp = 0, byteHlTemp = 0;

	if (!bsData.Read(byteHealthArmour)) { return; }
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) {
		icsync.playerArmour = 100;
	} else if(byteArmTemp == 0) {
		icsync.playerArmour = 0;
	} else {
		icsync.playerArmour = byteArmTemp * 7;
	}
	
	if(byteHlTemp == 0xF) {
		icsync.playerHealth = 100;
	} else if(byteHlTemp == 0) {
		icsync.playerHealth = 0;
	} else {
		icsync.playerHealth = byteHlTemp * 7;
	}
	
	uint8_t byteTempWeapon;
	if (!bsData.Read(byteTempWeapon)) { return; }
	icsync.byteCurrentWeapon ^= (byteTempWeapon ^ icsync.byteCurrentWeapon) & 0x3F;
	
	bool bCheck;
	if (!bsData.ReadCompressed(bCheck)) { return; }
	if(bCheck) { icsync.byteSirenOn = 1; }
	if (!bsData.ReadCompressed(bCheck)) { return; }
	if(bCheck) { icsync.byteLandingGearState = 1; }
	if (!bsData.ReadCompressed(bCheck)) { return; }
	if(bCheck && !bsData.Read(icsync.TrailerID)) { return;  }

    if (!IsFiniteQuaternion(icsync.quatw, icsync.quatx, icsync.quaty, icsync.quatz) ||
        !IsFiniteVector(icsync.vecPos) || !IsFiniteVector(icsync.vecMoveSpeed) ||
        !std::isfinite(icsync.fCarHealth)) {
        return;
    }

    CRemotePlayer* remote_player = GetRemotePlayerForSync(playerId);
    if(remote_player) {
        remote_player->StoreInCarSyncData(&icsync, 0);
    }
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);
	
	if(GetGameState() != GAMESTATE_CONNECTED) { return; }
	
	uint8_t pktId;
	uint16_t playerId;
	BRPassengerSyncData passengerSync = {0};
	
	if (!ReadSyncPacketHeader(bsData, pkt, pktId, playerId)) { return; }
    if (playerId >= kPlayerPoolSize) { return; }

    uint8_t seatAndDriveBy = 0;
    uint8_t playerHealth = 0;
    uint8_t playerArmour = 0;

    if (!bsData.ReadBits((unsigned char *)&passengerSync.VehicleID, 16)) { return; }
    if (!bsData.ReadBits((unsigned char *)&seatAndDriveBy, 8)) { return; }
    passengerSync.byteSeatFlags = (seatAndDriveBy & 0x7F);
    passengerSync.byteDriveBy = ((seatAndDriveBy >> 7) & 0x01);
    if (!bsData.ReadBits((unsigned char *)&passengerSync.byteCurrentWeapon, 8)) { return; }
    if (!bsData.ReadBits((unsigned char *)&playerHealth, 8)) { return; }
    if (!bsData.ReadBits((unsigned char *)&playerArmour, 8)) { return; }
    passengerSync.playerHealth = playerHealth;
    passengerSync.playerArmour = playerArmour;
    if (!bsData.ReadBits((unsigned char *)&passengerSync.lrAnalog, 16)) { return; }
    if (!bsData.ReadBits((unsigned char *)&passengerSync.udAnalog, 16)) { return; }
    if (!bsData.ReadBits((unsigned char *)&passengerSync.wKeys, 16)) { return; }
    if (!bsData.ReadBits((unsigned char *)&passengerSync.vecPos, 96)) { return; }
    if (!IsFiniteVector(passengerSync.vecPos)) { return; }
	
    CRemotePlayer* remote_player = GetRemotePlayerForSync(playerId);
    if(remote_player) {
        remote_player->StorePassengerSyncData(reinterpret_cast<uint8_t*>(&passengerSync), 0);
    }
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);
	
	if(GetGameState() != GAMESTATE_CONNECTED) { return; }
	
	uint8_t pktId;
	uint16_t playerId;
	uint8_t bulletSync[40] = {0};
	
	if (!ReadSyncPacketHeader(bsData, pkt, pktId, playerId)) { return; }
    if (playerId >= kPlayerPoolSize) { return; }
	if (!bsData.ReadBits((unsigned char *)&bulletSync, 40 * 8)) { return; }
	
    CPlayerPool* pool = GetPlayerPool();
    if (!pool) { return; }
    CRemotePlayer* remote_player = pool->GetAt(playerId);
    if(remote_player) {
        CLocalPlayer* local_player = pool->GetLocalPlayer();
        if(local_player && local_player->GetLocalPlayerID() != playerId) {
            remote_player->StoreBulletSyncData(bulletSync, 0);
        }
    }
}

void CNetGame::SendJsonData(int guiId, const char* data, uint32_t length)
{
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)252);
    bsSend.Write((uint16_t)guiId);
    bsSend.Write((uint32_t)length);
    bsSend.Write(data, length);

    pRakClient->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C" JNIEXPORT void JNICALL Java_com_blackhub_bronline_game_core_JNIJSONTransport_sendJsonData(
    JNIEnv* env, jclass clazz, jint guiId, jbyteArray data)
{
    if (!data)
        return;

    jbyte* byteArray = env->GetByteArrayElements(data, nullptr);
    jsize length = env->GetArrayLength(data);

    if (guiId != 10)
        CNetGame::SendJsonData(guiId, (const char*)byteArray, (uint32_t)length);

    env->ReleaseByteArrayElements(data, byteArray, JNI_ABORT);
}


void CNetGame::Packet_GUI(Packet* pkt)
{
    reinterpret_cast<void(*)(Packet*)>(CGameAPI::GetBase(xorstr("CNetGame::Packet_GUI")))(pkt);
}
