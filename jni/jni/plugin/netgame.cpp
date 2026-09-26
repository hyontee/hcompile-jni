#include "netgame.h"
#include "xorstr.h"
#include "plugin.h"
#include "common.h"
#include "remoteplayer.h"
#include "vendor/RakNet/GetTime.h"
#include <vector>

#define NETGAME_VERSION 4057

extern RakClientInterface* pRakClient;

uint16_t CNetGame::m_nLastSAMPDialogID;

static constexpr uintptr_t kGetRemotePlayerOffset = 0x2AC4B4;

static void CallNativePacketHandler(const char* offsetName, Packet* pkt)
{
    if (!pkt) {
        return;
    }
    const uintptr_t handler = CGameAPI::GetBase(offsetName);
    if (!handler) {
        return;
    }
    reinterpret_cast<void(*)(Packet*)>(handler)(pkt);
}

static CRemotePlayer* GetNativeRemotePlayerFromPacket(Packet* pkt)
{
    if (!pkt || pkt->playerIndex == UNASSIGNED_PLAYER_INDEX) {
        return nullptr;
    }

    CPlayerPool* pool = CNetGame::GetPlayerPool();
    if (!pool) {
        return nullptr;
    }

    return reinterpret_cast<CRemotePlayer*(*)(CPlayerPool*, uint16_t)>(
        CGameAPI::m_address + kGetRemotePlayerOffset)(pool, pkt->playerIndex);
}

static bool GetPacketPayload(Packet* pkt, uint8_t expectedId, const unsigned char** payload, unsigned int* payloadBytes)
{
    if (!pkt || !pkt->data || !payload || !payloadBytes || pkt->length == 0) {
        return false;
    }

    unsigned int offset = 0;
    if (pkt->data[0] == ID_TIMESTAMP) {
        if (pkt->length <= sizeof(uint8_t) + sizeof(uint32_t)) {
            return false;
        }
        offset = sizeof(uint8_t) + sizeof(uint32_t);
    }

    if (offset >= pkt->length || pkt->data[offset] != expectedId) {
        return false;
    }

    ++offset;
    if (offset > pkt->length) {
        return false;
    }

    *payload = reinterpret_cast<const unsigned char*>(pkt->data + offset);
    *payloadBytes = pkt->length - offset;
    return true;
}

uint8_t GetPacketID(Packet *p)
{
	if(p == 0) { return 255; }
	if ((uint8_t)p->data[0] == ID_TIMESTAMP) {
		return (uint8_t)p->data[sizeof(uint8_t) + sizeof(uint32_t)];
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

void CNetGame::ProcessNetwork()
{
	Packet* pkt = nullptr;
	uint8_t packetIdentifier;
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

        pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);

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
    reinterpret_cast<void(*)(Packet*)>(CGameAPI::GetBase(xorstr("CNetGame::Packet_GUI")))(pkt);
}

