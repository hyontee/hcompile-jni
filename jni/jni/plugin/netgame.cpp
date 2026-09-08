#include "netgame.h"
#include "xorstr.h"
#include "plugin.h"
#include "common.h"
#include <cstring>

#define NETGAME_VERSION 4057

extern RakClientInterface* pRakClient;

uint16_t CNetGame::m_nLastSAMPDialogID;
bool CNetGame::m_bDialogActive = false;
bool CNetGame::m_bRemotePlayerReady[1504] = { false };

namespace
{
void CallNativePacketHandler(const char* offsetName, Packet* pkt);

bool SkipTimestampIfPresent(Packet* pkt, RakNet::BitStream& bsData)
{
    if(!pkt || !pkt->data || pkt->length <= 0) {
        return false;
    }

    if((uint8_t)pkt->data[0] == ID_TIMESTAMP) {
        uint32_t timestamp = 0;
        bsData.IgnoreBits(8);
        if(!bsData.Read(timestamp)) {
            return false;
        }
    }

    return true;
}

bool ReadSyncPacketHeader(Packet* pkt, RakNet::BitStream& bsData, uint8_t expectedPacketId, uint16_t& playerId, uint32_t& packetTime)
{
    packetTime = 0;

    if(!SkipTimestampIfPresent(pkt, bsData)) {
        return false;
    }

    uint8_t packetId = 0;
    if(!bsData.Read(packetId) || packetId != expectedPacketId) {
        return false;
    }

    if(!bsData.Read(playerId)) {
        return false;
    }

    if(pkt && pkt->data && pkt->length > 0 && (uint8_t)pkt->data[0] == ID_TIMESTAMP) {
        memcpy(&packetTime, pkt->data + 1, sizeof(packetTime));
    }

    return true;
}

bool IsUsableRemotePlayer(uint16_t playerId, CRemotePlayer** outRemotePlayer)
{
    if(outRemotePlayer) {
        *outRemotePlayer = nullptr;
    }

    if(playerId >= 1504 || !CNetGame::IsRemotePlayerReady(playerId)) {
        return false;
    }

    CPlayerPool* pool = CNetGame::GetPlayerPool();
    if(!pool) {
        return false;
    }

    CRemotePlayer* remotePlayer = pool->GetAt(playerId);
    if(!remotePlayer || reinterpret_cast<uintptr_t>(remotePlayer) < 0x10000) {
        return false;
    }

    if(outRemotePlayer) {
        *outRemotePlayer = remotePlayer;
    }

    return true;
}

bool HandleRemoteOnFootSync(Packet* pkt)
{
    if(!pkt || !pkt->data || pkt->length <= 0) {
        return false;
    }

    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint16_t playerId = 0;
    uint32_t packetTime = 0;
    if(!ReadSyncPacketHeader(pkt, bsData, ID_PLAYER_SYNC, playerId, packetTime)) {
        return false;
    }

    CRemotePlayer* remotePlayer = nullptr;
    if(!IsUsableRemotePlayer(playerId, &remotePlayer)) {
        return true;
    }

    BROnFootSyncData syncData = {};
    if(!bsData.Read(reinterpret_cast<char*>(&syncData), sizeof(syncData))) {
        return false;
    }

    RakNet::BitStream bsRepacked;
    uint8_t packetId = ID_PLAYER_SYNC;
    bsRepacked.Write(packetId);
    bsRepacked.Write(playerId);
    ConvertBROnFootSyncToSampSync(&bsRepacked, syncData);

    Packet repairedPacket = *pkt;
    repairedPacket.data = bsRepacked.GetData();
    repairedPacket.length = bsRepacked.GetNumberOfBytesUsed();
    repairedPacket.bitSize = bsRepacked.GetNumberOfBitsUsed();
    repairedPacket.deleteData = false;

    CallNativePacketHandler(xorstr("CNetGame::Packet_PlayerSync_207"), &repairedPacket);
    return true;
}

bool HandleRemoteInCarSync(Packet* pkt)
{
    if(!pkt || !pkt->data || pkt->length <= 0) {
        return false;
    }

    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint16_t playerId = 0;
    uint32_t packetTime = 0;
    if(!ReadSyncPacketHeader(pkt, bsData, ID_VEHICLE_SYNC, playerId, packetTime)) {
        return false;
    }

    CRemotePlayer* remotePlayer = nullptr;
    if(!IsUsableRemotePlayer(playerId, &remotePlayer)) {
        return true;
    }

    BRInCarSyncData syncData = {};
    if(!bsData.Read(reinterpret_cast<char*>(&syncData), sizeof(syncData))) {
        return false;
    }

    remotePlayer->StoreInCarSyncData(&syncData, packetTime);
    return true;
}

bool HandleRemotePassengerSync(Packet* pkt)
{
    if(!pkt || !pkt->data || pkt->length <= 0) {
        return false;
    }

    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint16_t playerId = 0;
    uint32_t packetTime = 0;
    if(!ReadSyncPacketHeader(pkt, bsData, ID_PASSENGER_SYNC, playerId, packetTime)) {
        return false;
    }

    CRemotePlayer* remotePlayer = nullptr;
    if(!IsUsableRemotePlayer(playerId, &remotePlayer)) {
        return true;
    }

    BRPassengerSyncData syncData = {};
    if(!bsData.Read(reinterpret_cast<char*>(&syncData), sizeof(syncData))) {
        return false;
    }

    remotePlayer->StorePassengerSyncData(reinterpret_cast<uint8_t*>(&syncData), packetTime);
    return true;
}

void CallNativePacketHandler(const char* offsetName, Packet* pkt)
{
    if (!pkt) {
        return;
    }

    uintptr_t handler = CGameAPI::GetBase(offsetName);
    if (!handler) {
        return;
    }

    reinterpret_cast<void(*)(Packet*)>(handler)(pkt);
}
}

uint8_t GetPacketID(Packet *p)
{
	if(p == 0) { return 255; }
	if ((uint8_t)p->data[0] == ID_TIMESTAMP) {
		return (uint8_t)p->data[sizeof(uint8_t) + sizeof(RakNetTime)];
	} else {
		return (uint8_t)p->data[0];
	}
}

CPlayerPool* CNetGame::GetPlayerPool()
{
    uintptr_t addr = CGameAPI::GetBase(xorstr("CNetGame::m_pPlayerPool"));
    if (!addr) return nullptr;
    return *(CPlayerPool **)(addr);
}

void CNetGame::SetDialogActive(bool active)
{
	m_bDialogActive = active;
}

bool CNetGame::IsDialogActive()
{
	return m_bDialogActive;
}

void CNetGame::SetRemotePlayerReady(uint16_t playerId, bool ready)
{
    if(playerId >= 1504) {
        return;
    }
    m_bRemotePlayerReady[playerId] = ready;
}

bool CNetGame::IsRemotePlayerReady(uint16_t playerId)
{
    if(playerId >= 1504) {
        return false;
    }
    return m_bRemotePlayerReady[playerId];
}

void CNetGame::ProcessNetwork()
{
	Packet* pkt = nullptr;
	uint8_t packetIdentifier;
	while(pkt = pRakClient->Receive())
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
				CChat::AddDebugMessage(xorstr("Wrong server password."));
				pRakClient->Disconnect(0);
				break;
			case ID_AUTH_KEY:
				Packet_AuthKey(pkt);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				Packet_ConnectionSucceeded(pkt);
				break;
			case ID_CONNECTION_LOST:
				CChat::AddDebugMessage(xorstr("Переподключение через 15 секунд..."));
				Packet_ConnectionLost(pkt);
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				CChat::AddDebugMessage(xorstr("Переподключение через 15 секунд..."));
				pRakClient->Disconnect(2000, 0);
				break;
			case ID_AIM_SYNC:
				Packet_AimSync(pkt);
				break;
				
			case ID_PLAYER_SYNC:
				Packet_PlayerSync(pkt);
				break;

			case ID_UNOCCUPIED_SYNC:
				CallNativePacketHandler(xorstr("CNetGame::Packet_PlayerSync_209"), pkt);
				break;

			case ID_VEHICLE_SYNC:
				Packet_VehicleSync(pkt);
				break;

			case ID_TRAILER_SYNC:
				CallNativePacketHandler(xorstr("CNetGame::Packet_VehicleSync_210"), pkt);
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
			case 253:
			{
				CallNativePacketHandler(xorstr("CNetGame::Packet_253"), pkt);
				break;
			}
		}

		pRakClient->DeallocatePacket(pkt);
	}
}

int CNetGame::GetGameState()
{
	return *(int *)(CGameAPI::GetBase(xorstr("CNetGame::m_iGameState")));
}

void CNetGame::SetGameState(int state)
{
	*(int *)(CGameAPI::GetBase(xorstr("CNetGame::m_iGameState"))) = state;
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
	memset(m_bRemotePlayerReady, 0, sizeof(m_bRemotePlayerReady));

	RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
	uint16_t MyPlayerID;
	unsigned int uiChallenge;

	bsSuccAuth.IgnoreBits(8); // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port
	bsSuccAuth.Read(MyPlayerID);
	bsSuccAuth.Read(uiChallenge);

	GetPlayerPool()->GetLocalPlayer()->SetLocalPlayerID(MyPlayerID);

	int iVersion = NETGAME_VERSION;
	char byteMod = 0x01;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

	const char* sampVersion = xorstr("0.3.7");
	const char* auth_bs = xorstr("15121F6F18550C00AC4B4F8A167D0379BB0ACA99043");
	
	const char* localPlayerName = (const char *)(GetPlayerPool()->GetLocalPlayer()->GetLocalPlayerName());
	
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
    (void)pkt;
}

void CNetGame::Packet_PlayerSync(Packet* pkt)
{
    CallNativePacketHandler(xorstr("CNetGame::Packet_PlayerSync_207"), pkt);
}
void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    CallNativePacketHandler(xorstr("CNetGame::Packet_VehicleSync_200"), pkt);
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
    CallNativePacketHandler(xorstr("CNetGame::Packet_PassengerSync_211"), pkt);
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
    CallNativePacketHandler(xorstr("CNetGame::Packet_BulletSync_206"), pkt);
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
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(!SkipTimestampIfPresent(pkt, bsData)) {
        return;
    }

    uint8_t pktId = 0;
    uint16_t guiId = 0;
    if(bsData.Read(pktId) && bsData.Read(guiId)) {
        if(guiId == 10) {
            CNetGame::SetDialogActive(true);
        }
    }

    reinterpret_cast<void(*)(Packet*)>(CGameAPI::GetBase(xorstr("CNetGame::Packet_GUI")))(pkt);
}
