#include "netgame.h"
#include "xorstr.h"
#include "plugin.h"
#include <vector>

extern JavaVM* g_jvm;
extern jclass g_jsonTransportClass;
extern jmethodID g_onJsonDataMethod;

#define NETGAME_VERSION 4057

extern RakClientInterface* pRakClient;

uint16_t CNetGame::m_nLastSAMPDialogID;

uint8_t GetPacketID(Packet *p)
{
	if(p == nullptr || p->data == nullptr || p->length == 0) { return 255; }
	if ((uint8_t)p->data[0] == ID_TIMESTAMP) {
		constexpr unsigned int kTimestampPacketPrefix = sizeof(uint8_t) + sizeof(unsigned int);
		if(p->length <= kTimestampPacketPrefix) { return 255; }
		return (uint8_t)p->data[kTimestampPacketPrefix];
	}
	return (uint8_t)p->data[0];
}

CPlayerPool* CNetGame::GetPlayerPool()
{
    static const uintptr_t address = CGameAPI::GetBase(xorstr("CNetGame::m_pPlayerPool"));
    if (!address) {
        return nullptr;
    }
    return *(CPlayerPool **)(address);
}

void CallOnJsonDataIncoming(int guiId, const char* jsonData, uint32_t dataLength) {
    if (!g_jvm || !g_jsonTransportClass || !g_onJsonDataMethod) {
//        CChat::AddDebugMessage(xorstr("JNI not initialized"));
        return;
    }
    
    JNIEnv* env;
    int status = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (status == JNI_EDETACHED) {
        if (g_jvm->AttachCurrentThread((void**)&env, nullptr) != JNI_OK) {
        //    CChat::AddDebugMessage(xorstr("Failed to attach thread"));
            return;
        }
    } else if (status != JNI_OK) {
     //   CChat::AddDebugMessage(xorstr("Failed to get JNIEnv"));
        return;
    }
    
    if (!jsonData || dataLength > static_cast<uint32_t>(INT32_MAX)) {
        if (status == JNI_EDETACHED) {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    jbyteArray jData = env->NewByteArray(static_cast<jsize>(dataLength));
    if (!jData) {
        if (status == JNI_EDETACHED) {
            g_jvm->DetachCurrentThread();
        }
        return;
    }
    env->SetByteArrayRegion(jData, 0, static_cast<jsize>(dataLength), (const jbyte*)jsonData);
    
    env->CallStaticVoidMethod(g_jsonTransportClass, g_onJsonDataMethod, guiId, jData);
    
    if (env->ExceptionCheck()) {
        // Do not call ExceptionDescribe() on the network hot path.
        env->ExceptionClear();
    }
    
    env->DeleteLocalRef(jData);
    
    if (status == JNI_EDETACHED) {
        g_jvm->DetachCurrentThread();
    }
}

void CNetGame::ProcessNetwork()
{
    uint8_t packetIdentifier;
    if (pRakClient == nullptr) {
        return;
    }

    constexpr unsigned int kMaxPacketsPerFrame = 512;
    unsigned int packetsProcessed = 0;

    Packet* pkt = nullptr;
    while(packetsProcessed < kMaxPacketsPerFrame && (pkt = pRakClient->Receive()) != nullptr)
    {
        ++packetsProcessed;
        packetIdentifier = GetPacketID(pkt);
        switch(packetIdentifier)
        {
            case ID_FAILED_INITIALIZE_ENCRIPTION:
                CChat::AddDebugMessage(xorstr("Failed to initialize encryption."));
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                CChat::AddDebugMessage(xorstr("Не удалось подключиться к серверу, попробуйте перезагрузить сеть либо использовать VPN."));
                SetGameState(GAMESTATE_WAIT_CONNECT);
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                CChat::AddDebugMessage(xorstr("Сервер полон"));
                SetGameState(GAMESTATE_WAIT_CONNECT);
                pRakClient->Disconnect(0, 0);
                break;
            case ID_CONNECTION_BANNED:
                CChat::AddDebugMessage(xorstr("{ff6347}Сервер закрыл соединение, можете выйти."));
                break;
            case ID_INVALID_PASSWORD:
                CChat::AddDebugMessage(xorstr("{ff6347}Сервер закрыл соединение, можете выйти."));
                pRakClient->Disconnect(0);
                break;
            case ID_AUTH_KEY:
                Packet_AuthKey(pkt);
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
           //      CChat::AddDebugMessage(xorstr("Вы войшли на сервер, ожидайте загрузки."));
                Packet_ConnectionSucceeded(pkt);
                break;
            case ID_CONNECTION_LOST:
                CChat::AddDebugMessage(xorstr("{ff6347}Вас отключили от сервера, можете выйти."));
                Packet_ConnectionLost(pkt);
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                CChat::AddDebugMessage(xorstr("{ff6347}Вас отключили от сервера, можете выйти."));
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
                RakNet::BitStream bsData(pkt->data, pkt->length, false);
                uint8_t packetId;
                uint16_t guiId;
                uint32_t jsonLength;

                if (!bsData.Read(packetId) || !bsData.Read(guiId) || !bsData.Read(jsonLength)) {
                    break;
                }

                if(guiId == 33 || guiId == 34)
                {
                    const uint32_t kMaxGuiJsonSize = 1u << 20;
                    const uint32_t bytesRemaining = static_cast<uint32_t>(bsData.GetNumberOfUnreadBits() / 8);
                    if (jsonLength > kMaxGuiJsonSize || jsonLength > bytesRemaining) {
                        break;
                    }

                    static thread_local std::vector<char> jsonBuffer;
                    jsonBuffer.resize(static_cast<size_t>(jsonLength) + 1u);
                    if (!bsData.Read(jsonBuffer.data(), jsonLength)) {
                        break;
                    }
                    jsonBuffer[jsonLength] = '\0';
                    CallOnJsonDataIncoming(guiId, jsonBuffer.data(), jsonLength);
                }
                else
                {
                    Packet_GUI(pkt);
                }
                break;
            }
        }
        pRakClient->DeallocatePacket(pkt);  
    }
}


int CNetGame::GetGameState()
{
    static const uintptr_t address = CGameAPI::GetBase(xorstr("CNetGame::m_iGameState"));
    return address ? *(int *)(address) : GAMESTATE_NONE;
}

void CNetGame::SetGameState(int state)
{
    static const uintptr_t address = CGameAPI::GetBase(xorstr("CNetGame::m_iGameState"));
    if (address) {
        *(int *)(address) = state;
    }
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
	//CChat::AddDebugMessage(xorstr("Автoр рeйдж зoменкo @dеr1xtоn"));
	pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);
}
#endif

#ifdef __aarch64__
extern char AuthKeyTable[512][2][128];
void CNetGame::Packet_AuthKey(Packet* pkt)
{
    if (pkt == nullptr || pkt->data == nullptr || pkt->length <= 2) {
        return;
    }

    const char* incomingKey = reinterpret_cast<const char*>(pkt->data) + 2;
    char* auth_key = nullptr;

    for (int x = 0; x < 512; x++)
    {
        if (!strcmp(incomingKey, AuthKeyTable[x][0]))
        {
            auth_key = AuthKeyTable[x][1];
            break;
        }
    }

    if (auth_key != nullptr)
    {
        RakNet::BitStream bsKey;
        BYTE byteAuthKeyLen;

        byteAuthKeyLen = (BYTE)strlen(auth_key);

        bsKey.Write((BYTE)ID_AUTH_KEY);
        bsKey.Write((BYTE)byteAuthKeyLen);
        bsKey.Write(auth_key, byteAuthKeyLen);

        pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);
        //CChat::AddDebugMessage(xorstr("Автoр рeйдж зoменкo @dеr1xtоn"));
    }
    else
    {   
    }
}
#endif

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
    using Fn = void(*)();
    static Fn fn = reinterpret_cast<Fn>(CGameAPI::GetBase(xorstr("CNetGame::Packet_ConnectionLost")));
    if (fn) fn();
}

void CNetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length < 15 || !pRakClient) return;
	RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
	uint16_t MyPlayerID;
	unsigned int uiChallenge;

	bsSuccAuth.IgnoreBits(8); 
	bsSuccAuth.IgnoreBits(32); 
	bsSuccAuth.IgnoreBits(16); 
	if (!bsSuccAuth.Read(MyPlayerID) || !bsSuccAuth.Read(uiChallenge)) return;

    CPlayerPool* pool = GetPlayerPool();
    if (!pool || !pool->GetLocalPlayer()) return;
	pool->GetLocalPlayer()->SetLocalPlayerID(MyPlayerID);

	int iVersion = NETGAME_VERSION;
	char byteMod = 0x01;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

	const char* sampVersion = xorstr("0.3.7");
	const char* auth_bs = xorstr("15121F6F18550C00AC4B4F8A167D0379BB0ACA99043");
	
	const char* localPlayerName = (const char *)(pool->GetLocalPlayer()->GetLocalPlayerName());
    if (!localPlayerName) return;
	
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
	//CChat::AddDebugMessage(xorstr("Автoр рeйдж зoменкo @dеr1xtоn"));
	SetGameState(GAMESTATE_AWAIT_JOIN);
}

void CNetGame::Packet_AimSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length < 37) return;
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
    if (!pkt || !pkt->data || pkt->length < 3) return;
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
	uint8_t byteHealthArmour = 0, byteHealth = 0, byteArmour = 0;
	CVector vecMoveSpeed = {0.0f, 0.0f, 0.0f};
	float mx, my, mz;
	uint16_t wSurfInfo;
	CVector vecSurfOffsets;
	uint8_t byteCurrentWeapon = 0, byteSpecialAction = 0;

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
		lrAnalog = 0; 
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
			udAnalog = 0; 
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

    if (!accepted_sync) return;
    CPlayerPool* pool = GetPlayerPool();
    if (!pool) return;
	CRemotePlayer* remote_player = pool->GetAt(playerId);
	if(remote_player) {
		remote_player->StoreSyncData(&ofSync, 0);
	}
}
void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    if (!pkt || !pkt->data || pkt->length < 3) return;
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
    if (!pkt || !pkt->data || pkt->length < 29) return;
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
    if (!pkt || !pkt->data || pkt->length < 43) return;
	RakNet::BitStream bsData(pkt->data, pkt->length, false);
	
	if(GetGameState() != GAMESTATE_CONNECTED) { return; }
	
	uint8_t pktId;
	uint16_t playerId;
	uint8_t bulletSync[40] = {0};
	
	bsData.ReadBits((unsigned char *)&pktId, 8);
	bsData.ReadBits((unsigned char *)&playerId, 16);
	bsData.ReadBits((unsigned char *)&bulletSync, 40 * 8);
	
    CPlayerPool* pool = GetPlayerPool();
    if (!pool) return;
	CRemotePlayer* remote_player = pool->GetAt(playerId);
	if(remote_player) {
		CLocalPlayer* local_player = pool->GetLocalPlayer();
		if(local_player && local_player->GetLocalPlayerID() != playerId) {
			remote_player->StoreBulletSyncData(bulletSync, 0);
		}
	}
}

void CNetGame::SendOnData(int guiId, const char* data, uint32_t length)
{
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)252);        
    bsSend.Write((uint16_t)guiId);      
    bsSend.Write((uint32_t)length);     
    bsSend.Write(data, length);        

    bool success = pRakClient->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
    if (!success) {
    } else {
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_blackhub_bronline_game_core_JNIJSONTransport_sendJsonData(
    JNIEnv* env, jclass clazz, jint guiId, jbyteArray data)
{
    if (!data) {
       // CChat::AddDebugMessage(xorstr("sendJsonData: data array is null"));
        return;
    }

    jsize length = env->GetArrayLength(data);
    if (length <= 0) {
     //   CChat::AddDebugMessage(xorstr("sendJsonData: data length <= 0"));
        return;
    }

    jbyte* byteArray = env->GetByteArrayElements(data, nullptr);
    if (!byteArray) {
     //   CChat::AddDebugMessage(xorstr("sendJsonData: failed to get byte array elements"));
        return;
    }

    if (guiId == 10) {
     //   CChat::AddDebugMessage(xorstr("sendJsonData: guiId 10 ignored (handled separately)"));
        env->ReleaseByteArrayElements(data, byteArray, JNI_ABORT);
        return;
    }

    CNetGame::SendOnData(guiId, (const char*)byteArray, length);

    env->ReleaseByteArrayElements(data, byteArray, JNI_ABORT);
}

void CNetGame::Packet_GUI(Packet* pkt)
{
    using Fn = void(*)(Packet*);
    static Fn fn = reinterpret_cast<Fn>(CGameAPI::GetBase(xorstr("CNetGame::Packet_GUI")));
    if (fn && pkt) fn(pkt);
}