#include "netgame.h"
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
	return *(CPlayerPool **)(CGameAPI::GetBase(("CNetGame::m_pPlayerPool")));
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
				CChat::AddDebugMessage(("Failed to initialize encryption."));
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				CChat::AddDebugMessage(("Сервер не отвечает. Переподключение..."));
				SetGameState(GAMESTATE_WAIT_CONNECT);
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				CChat::AddDebugMessage(("Сервер полон. Переподключение..."));
				SetGameState(GAMESTATE_WAIT_CONNECT);
				pRakClient->Disconnect(0, 0);
				break;
			case ID_CONNECTION_BANNED:
				CChat::AddDebugMessage(("Вы были забанены/отключены на этом сервере."));
				break;
			case ID_INVALID_PASSWORD:
				CChat::AddDebugMessage(("Wrong server password."));
				pRakClient->Disconnect(0);
				break;
			case ID_AUTH_KEY:
				Packet_AuthKey(pkt);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				Packet_ConnectionSucceeded(pkt);
				break;
			case ID_CONNECTION_LOST:
				CChat::AddDebugMessage(("Переподключение через 15 секунд..."));
				Packet_ConnectionLost(pkt);
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				CChat::AddDebugMessage(("Переподключение через 15 секунд..."));
				pRakClient->Disconnect(2000, 0);
				// Packet_DisconnectionNotification(pkt);
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

			case PACKET_BLACKGUI:
                Packet_GUI(pkt);
                break;
		}

		pRakClient->DeallocatePacket(pkt);
	}
}

int CNetGame::GetGameState()
{
	return *(int *)(CGameAPI::GetBase(("CNetGame::m_iGameState")));
}

void CNetGame::SetGameState(int state)
{
	*(int *)(CGameAPI::GetBase(("CNetGame::m_iGameState"))) = state;
}


#ifdef __arm__
extern char AuthKeyTable[512][2][128];

void CNetGame::Packet_AuthKey(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length <= 2)
        return;

    const char* rawKey = (const char*)pkt->data + 2;
    int rawLen = pkt->length - 2;

    char packetKey[128];
    int copyLen = rawLen < 127 ? rawLen : 127;
    memcpy(packetKey, rawKey, copyLen);
    packetKey[copyLen] = '\0';

    const char* auth_key = nullptr;

    for (int x = 0; x < 512; x++)
    {
        if (AuthKeyTable[x][0][0] == '\0')
            continue;

        if (strcmp(packetKey, AuthKeyTable[x][0]) == 0)
        {
            auth_key = AuthKeyTable[x][1];
            break;
        }
    }

    if (!auth_key)
        return;

    size_t keyLen = strnlen(auth_key, 127);
    if (keyLen == 0)
        return;

    RakNet::BitStream bsKey;
    bsKey.Write((BYTE)ID_AUTH_KEY);
    bsKey.Write((BYTE)keyLen);
    bsKey.Write(auth_key, (int)keyLen);

    pRakClient->Send(
        &bsKey,
        SYSTEM_PRIORITY,
        RELIABLE,
        0
    );
}


#endif

#ifdef __aarch64__
extern char AuthKeyTable[512][2][128];

void CNetGame::Packet_AuthKey(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length <= 2)
        return;

    const char* rawKey = (const char*)pkt->data + 2;
    int rawLen = pkt->length - 2;

    char packetKey[128];
    int copyLen = rawLen < 127 ? rawLen : 127;
    memcpy(packetKey, rawKey, copyLen);
    packetKey[copyLen] = '\0';

    const char* auth_key = nullptr;

    for (int x = 0; x < 512; x++)
    {
        if (AuthKeyTable[x][0][0] == '\0')
            continue;

        if (strcmp(packetKey, AuthKeyTable[x][0]) == 0)
        {
            auth_key = AuthKeyTable[x][1];
            break;
        }
    }

    if (!auth_key)
        return;

    size_t keyLen = strnlen(auth_key, 127);
    if (keyLen == 0)
        return;

    RakNet::BitStream bsKey;
    bsKey.Write((BYTE)ID_AUTH_KEY);
    bsKey.Write((BYTE)keyLen);
    bsKey.Write(auth_key, (int)keyLen);
    pRakClient->Send(
        &bsKey,
        SYSTEM_PRIORITY,
        RELIABLE,
        0
    );
}


#endif

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
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

	const char* sampVersion = ("0.3.7");
	const char* auth_bs = ("15121F6F18550C00AC4B4F8A167D0379BB0ACA99043");
	
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
	RakNet::BitStream bsData(pkt->data, pkt->length, false);
	
	if(GetGameState() != GAMESTATE_CONNECTED) { return; }
	
	uint8_t pktId;
	uint16_t playerId;
	uint8_t aimSyncBuffer[31] = {0};
	
	bsData.ReadBits((unsigned char *)&pktId, 8);
	bsData.ReadBits((unsigned char *)&playerId, 16);
	bsData.ReadBits((unsigned char *)&aimSyncBuffer, 31 * 8);
	
	CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
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
	uint32_t timestamp;

	int16_t lrAnalog, udAnalog;
	uint16_t wKeys;
	CVector vecPos = {0.0f, 0.0f, 0.0f};
	float tw, tx, ty, tz;
	uint8_t byteHealthArmour, byteHealth, byteArmour;
	CVector vecMoveSpeed = {0.0f, 0.0f, 0.0f};
	float mx, my, mz;
	uint16_t wSurfInfo;
	CVector vecSurfOffsets;
	uint8_t byteCurrentWeapon, byteSpecialAction;

	if(pkt->data[0] == ID_TIMESTAMP) {
		bsData.ReadBits((unsigned char *)&pktId, 8);
		bsData.ReadBits((unsigned char *)&timestamp, 32);
	}
	bsData.ReadBits((unsigned char *)&pktId, 8);
	bsData.ReadBits((unsigned char *)&playerId, 16);

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

	CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
	if(remote_player && accepted_sync) {
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
	
	bsData.ReadBits((unsigned char *)&pktId, 8);
	bsData.ReadBits((unsigned char *)&playerId, 16);
	
	bsData.ReadBits((unsigned char *)&icsync.VehicleID, 16);
	bsData.ReadBits((unsigned char *)&icsync.lrAnalogLeftStick, 16);
	bsData.ReadBits((unsigned char *)&icsync.udAnalogLeftStick, 16);
	bsData.ReadBits((unsigned char *)&icsync.wKeys, 16);
	bsData.ReadNormQuat<float>(icsync.quatw, icsync.quatx, icsync.quaty, icsync.quatz);
	bsData.ReadBits((unsigned char *)&icsync.vecPos, 96);
	bsData.ReadVector<float>(icsync.vecMoveSpeed.x, icsync.vecMoveSpeed.y, icsync.vecMoveSpeed.z);
	uint16_t wTempCarHealth;
	bsData.ReadBits((unsigned char *)&wTempCarHealth, 16);
	icsync.fCarHealth = wTempCarHealth;
	
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp = 0, byteHlTemp = 0;

	bsData.Read(byteHealthArmour);
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
	bsData.Read(byteTempWeapon);
	icsync.byteCurrentWeapon ^= (byteTempWeapon ^ icsync.byteCurrentWeapon) & 0x3F;
	
	bool bCheck;
	bsData.ReadCompressed(bCheck);
	if(bCheck) { icsync.byteSirenOn = 1; }
	bsData.ReadCompressed(bCheck);
	if(bCheck) { icsync.byteLandingGearState = 1; }
	bsData.ReadCompressed(bCheck);
	if(bCheck) { bsData.Read(icsync.TrailerID);  }

	CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
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
	uint8_t passengerSync[26] = {0};
	
	bsData.ReadBits((unsigned char *)&pktId, 8);
	bsData.ReadBits((unsigned char *)&playerId, 16);
	bsData.ReadBits((unsigned char *)&passengerSync, 26 * 8);
	
	CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
	if(remote_player) {
		remote_player->StorePassengerSyncData(passengerSync, 0);
	}
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);
	
	if(GetGameState() != GAMESTATE_CONNECTED) { return; }
	
	uint8_t pktId;
	uint16_t playerId;
	uint8_t bulletSync[40] = {0};
	
	bsData.ReadBits((unsigned char *)&pktId, 8);
	bsData.ReadBits((unsigned char *)&playerId, 16);
	bsData.ReadBits((unsigned char *)&bulletSync, 40 * 8);
	
	CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
	if(remote_player) {
		CLocalPlayer* local_player = GetPlayerPool()->GetLocalPlayer();
		if(local_player->GetLocalPlayerID() != playerId) {
			remote_player->StoreBulletSyncData(bulletSync, 0);
		}
	}
}

void CNetGame::Packet_GUI(Packet* pkt)
{    
    reinterpret_cast<void(*)(Packet*)>(CGameAPI::GetBase(("CNetGame::Packet_GUI")))(pkt);
}