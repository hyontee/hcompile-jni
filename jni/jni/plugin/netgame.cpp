#include "netgame.h"
#include "xorstr.h"
#include "plugin.h"

#define NETGAME_VERSION 4057

extern RakClientInterface* pRakClient;

uint16_t CNetGame::m_nLastSAMPDialogID;

uint8_t GetPacketID(Packet *p)
{
	if(p == 0) { return 255; }
	if ((uint8_t)p->data[0] == ID_TIMESTAMP) {
		return (uint8_t)p->data[sizeof(uint8_t) + sizeof(unsigned long)];
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
	while(pkt = pRakClient->Receive())
	{
		packetIdentifier = GetPacketID(pkt);
		switch(packetIdentifier)
		{
			case ID_FAILED_INITIALIZE_ENCRIPTION:
				CChat::AddDebugMessage(xorstr("Failed to initialize encryption."));
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				CChat::AddDebugMessage(xorstr("Сервер не отвечает. Переподключение"));
				SetGameState(GAMESTATE_WAIT_CONNECT);
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				CChat::AddDebugMessage(xorstr("Сервер полон. Переподключение"));
				SetGameState(GAMESTATE_WAIT_CONNECT);
				pRakClient->Disconnect(0, 0);
				break;
			case ID_NEW_INCOMING_CONNECTION:
				CChat::AddDebugMessage(xorstr("NEW_INCOMING_CONNECTION"));
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
				CChat::AddDebugMessage(xorstr("Потеряно соединение к серверу. Переподключение через 15 секунд"));
				Packet_ConnectionLost(pkt);
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				CChat::AddDebugMessage(xorstr("Сервер оборвал соединение. Перезайдите"));
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
			
			case ID_USER_INTERFACE_SYNC: 
				Packet_GUI(pkt);
				break;
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
	
	int sampRpcId = ConvertBRIDToSampID(static_cast<BRRpcIds>(295));
	
	if(sampRpcId == -1) {
		sampRpcId = 25;
	}
	
	if(static_cast<BRRpcIds>(295) == static_cast<BRRpcIds>(295)) {
		sampRpcId = 25;
	}
	
	pRakClient->RPC(&sampRpcId, &bsSend, HIGH_PRIORITY, 
	               RELIABLE,
	               0, false, UNASSIGNED_NETWORK_ID, NULL);
	
	SetGameState(GAMESTATE_AWAIT_JOIN);
}

void CNetGame::Packet_AimSync(Packet* pkt)
{
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    if(!pkt || !pkt->data) return;

    RakNet::BitStream bsData(pkt->data, pkt->length, false);

    uint8_t pktId = 0;
    uint16_t playerId = 0;
    uint8_t aimSyncBuffer[31]{};

    if(!bsData.Read(pktId)) return;
    if(!bsData.Read(playerId)) return;

    for(int i = 0; i < 31; ++i)
        if(!bsData.Read(aimSyncBuffer[i])) return;

    CPlayerPool* pool = GetPlayerPool();
    if(!pool) return;

    CRemotePlayer* remote = pool->GetAt(playerId);
    if(remote)
        remote->StoreAimSyncData(aimSyncBuffer, 0);
}

void CNetGame::Packet_PlayerSync(Packet* pkt)
{
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    if(!pkt || !pkt->data) return;

    RakNet::BitStream bs(pkt->data, pkt->length, false);

    uint8_t pktId = 0;
    uint16_t playerId = 0;
    uint32_t timestamp = 0;

    if(pkt->data[0] == ID_TIMESTAMP)
    {
        if(!bs.Read(pktId)) return;
        if(!bs.Read(timestamp)) return;
    }

    if(!bs.Read(pktId)) return;
    if(!bs.Read(playerId)) return;

    int16_t lrAnalog = 0;
    int16_t udAnalog = 0;
    uint16_t wKeys = 0;

    CVector vecPos{};
    float qw = 0, qx = 0, qy = 0, qz = 0;

    uint8_t byteHealthArmour = 0;
    uint8_t byteWeapon = 0;
    uint8_t byteSpecial = 0;

    CVector vecMove{};
    uint16_t surfInfo = 0;
    CVector surfOffset{};

    bool hasLR = false;
    if(!bs.ReadCompressed(hasLR)) return;
    if(hasLR && !bs.Read(lrAnalog)) return;

    bool hasUD = false;
    if(!bs.ReadCompressed(hasUD)) return;
    if(hasUD && !bs.Read(udAnalog)) return;

    if(!bs.Read(wKeys)) return;

    if(!bs.Read(vecPos.x)) return;
    if(!bs.Read(vecPos.y)) return;
    if(!bs.Read(vecPos.z)) return;

    if(!bs.ReadNormQuat<float>(qw,qx,qy,qz)) return;

    if(!bs.Read(byteHealthArmour)) return;
    if(!bs.Read(byteWeapon)) return;
    if(!bs.Read(byteSpecial)) return;

    if(!bs.ReadVector<float>(vecMove.x, vecMove.y, vecMove.z)) return;

    bool hasSurf = false;
    if(bs.ReadCompressed(hasSurf) && hasSurf)
    {
        if(!bs.Read(surfInfo)) return;
        if(!bs.Read(surfOffset.x)) return;
        if(!bs.Read(surfOffset.y)) return;
        if(!bs.Read(surfOffset.z)) return;
    }

    uint8_t armNib = byteHealthArmour & 0x0F;
    uint8_t hpNib  = byteHealthArmour >> 4;

    uint8_t armour = (armNib == 0xF) ? 100 : armNib * 7;
    uint8_t health = (hpNib  == 0xF) ? 100 : hpNib  * 7;

    BROnFootSyncData sync{};

    sync.lrAnalogLeftStick = lrAnalog;
    sync.udAnalogLeftStick = udAnalog;
    sync.wKeys = wKeys;
    sync.vecPos = vecPos;

    sync.quatw = qw;
    sync.quatx = qx;
    sync.quaty = qy;
    sync.quatz = qz;

    sync.health = health;
    sync.armour = armour;

    sync.byteCurrentWeapon = byteWeapon;
    sync.byteSpecialAction = byteSpecial;

    sync.vecMoveSpeed = vecMove;
    sync.vecSurfOffsets = surfOffset;
    sync.wSurfInfo = surfInfo;

    CPlayerPool* pool = GetPlayerPool();
    if(!pool) return;

    CRemotePlayer* remote = pool->GetAt(playerId);
    if(remote)
        remote->StoreSyncData(&sync, 0);
}
void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    if(!pkt || !pkt->data) return;

    RakNet::BitStream bs(pkt->data, pkt->length, false);

    uint8_t pktId = 0;
    uint16_t playerId = 0;

    if(!bs.Read(pktId)) return;
    if(!bs.Read(playerId)) return;

    BRInCarSyncData sync{};

    if(!bs.Read(sync.VehicleID)) return;
    if(!bs.Read(sync.lrAnalogLeftStick)) return;
    if(!bs.Read(sync.udAnalogLeftStick)) return;
    if(!bs.Read(sync.wKeys)) return;

    if(!bs.ReadNormQuat<float>(sync.quatw, sync.quatx, sync.quaty, sync.quatz)) return;

    if(!bs.Read(sync.vecPos.x)) return;
    if(!bs.Read(sync.vecPos.y)) return;
    if(!bs.Read(sync.vecPos.z)) return;

    if(!bs.ReadVector<float>(sync.vecMoveSpeed.x, sync.vecMoveSpeed.y, sync.vecMoveSpeed.z)) return;

    uint16_t tempHealth = 0;
    if(!bs.Read(tempHealth)) return;
    sync.fCarHealth = static_cast<float>(tempHealth);

    uint8_t hpArm = 0;
    if(!bs.Read(hpArm)) return;

    uint8_t arm = hpArm & 0x0F;
    uint8_t hp  = hpArm >> 4;

    sync.playerArmour = (arm == 0xF) ? 100 : arm * 7;
    sync.playerHealth = (hp  == 0xF) ? 100 : hp  * 7;

    uint8_t weapon = 0;
    if(!bs.Read(weapon)) return;
    sync.byteCurrentWeapon = weapon & 0x3F;

    bool flag = false;

    if(bs.ReadCompressed(flag) && flag)
        sync.byteSirenOn = 1;

    if(bs.ReadCompressed(flag) && flag)
        sync.byteLandingGearState = 1;

    if(bs.ReadCompressed(flag) && flag)
        bs.Read(sync.TrailerID);

    CPlayerPool* pool = GetPlayerPool();
    if(!pool) return;

    CRemotePlayer* remote = pool->GetAt(playerId);
    if(remote)
        remote->StoreInCarSyncData(&sync, 0);
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    if(!pkt || !pkt->data) return;

    RakNet::BitStream bs(pkt->data, pkt->length, false);

    uint8_t pktId = 0;
    uint16_t playerId = 0;
    uint8_t data[26]{};

    if(!bs.Read(pktId)) return;
    if(!bs.Read(playerId)) return;

    for(int i = 0; i < 26; ++i)
        if(!bs.Read(data[i])) return;

    CPlayerPool* pool = GetPlayerPool();
    if(!pool) return;

    CRemotePlayer* remote = pool->GetAt(playerId);
    if(remote)
        remote->StorePassengerSyncData(data, 0);
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    if(!pkt || !pkt->data) return;

    RakNet::BitStream bs(pkt->data, pkt->length, false);

    uint8_t pktId = 0;
    uint16_t playerId = 0;
    uint8_t data[40]{};

    if(!bs.Read(pktId)) return;
    if(!bs.Read(playerId)) return;

    for(int i = 0; i < 40; ++i)
        if(!bs.Read(data[i])) return;

    CPlayerPool* pool = GetPlayerPool();
    if(!pool) return;

    CRemotePlayer* remote = pool->GetAt(playerId);
    if(!remote) return;

    CLocalPlayer* local = pool->GetLocalPlayer();
    if(local && local->GetLocalPlayerID() != playerId)
        remote->StoreBulletSyncData(data, 0);
}

void CNetGame::SendOnData(int guiId, const char* data, uint32_t length)
{
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)252);         // packet ID
    bsSend.Write((uint16_t)guiId);      
    bsSend.Write((uint32_t)length);     
    bsSend.Write(data, length);        

    bool success = pRakClient->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C" JNIEXPORT void JNICALL Java_com_blackhub_bronline_game_core_JNIJSONTransport_sendJsonData(
    JNIEnv* env, jclass clazz, jint guiId, jbyteArray data)
{
    jbyte* byteArray = env->GetByteArrayElements(data, nullptr);
    jsize length = env->GetArrayLength(data);

    CNetGame::SendOnData(guiId, reinterpret_cast<const char*>(byteArray), length);

    env->ReleaseByteArrayElements(data, byteArray, JNI_ABORT);
}

void CNetGame::Packet_GUI(Packet* pkt)
{
    reinterpret_cast<void(*)(Packet*)>(CGameAPI::GetBase(xorstr("CNetGame::Packet_GUI")))(pkt);
}

void CNetGame::Packet_Turnlights(Packet* pkt)
{
    reinterpret_cast<void(*)(Packet*)>(CGameAPI::GetBase(xorstr("CNetGame::Packet_Turnlights")))(pkt);
}
