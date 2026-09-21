#include "netgame.h"

#include <new>
#include "xorstr.h"
#include "plugin.h"

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
        const size_t timestampOffset = sizeof(uint8_t) + sizeof(uint32_t);
        if (p->length <= timestampOffset) { return 255; }
		return (uint8_t)p->data[timestampOffset];
	} else {
		return (uint8_t)p->data[0];
	}
}

CPlayerPool* CNetGame::GetPlayerPool()
{
    static uintptr_t playerPoolAddress = 0;
    if (playerPoolAddress == 0) {
        playerPoolAddress = CGameAPI::GetBase(xorstr("CNetGame::m_pPlayerPool"));
    }
    if (playerPoolAddress == 0) { return nullptr; }
    return *(CPlayerPool **)(playerPoolAddress);
}

uint16_t CNetGame::GetOnlinePlayerCount()
{
    CPlayerPool* pool = GetPlayerPool();
    return pool ? pool->GetOnlinePlayerCount() : 0;
}

void CallOnJsonDataIncoming(int guiId, const char* jsonData) {
    if (!g_jvm || !g_jsonTransportClass || !g_onJsonDataMethod) {
//        CChat::AddDebugMessage(xorstr("JNI not initialized"));
        return;
    }
    
    JNIEnv* env;
    int status = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (status == JNI_EDETACHED) {
        if (g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) {
        //    CChat::AddDebugMessage(xorstr("Failed to attach thread"));
            return;
        }
    } else if (status != JNI_OK) {
     //   CChat::AddDebugMessage(xorstr("Failed to get JNIEnv"));
        return;
    }
    
    jsize dataLength = strlen(jsonData);
    jbyteArray jData = env->NewByteArray(dataLength);
    env->SetByteArrayRegion(jData, 0, dataLength, (jbyte*)jsonData);
    
    env->CallStaticVoidMethod(g_jsonTransportClass, g_onJsonDataMethod, guiId, jData);
    
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    
    env->DeleteLocalRef(jData);
    
    if (status == JNI_EDETACHED) {
        g_jvm->DetachCurrentThread();
    }
}

void CNetGame::ProcessNetwork()
{
    if (pRakClient == nullptr) return;
    Packet* pkt = nullptr;
    uint8_t packetIdentifier;
    while ((pkt = pRakClient->Receive()) != nullptr)
    {
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
                 CChat::AddDebugMessage(xorstr(""));
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
                uint8_t packetId = 0;
                uint16_t guiId = 0;
                uint32_t jsonLength = 0;
                
                if (!bsData.Read(packetId) || !bsData.Read(guiId) || !bsData.Read(jsonLength)) {
                    break;
                }
                
                constexpr uint32_t kMaxIncomingJson = 1024u * 1024u;
                if ((guiId == 33 || guiId == 34) && jsonLength <= kMaxIncomingJson &&
                    bsData.GetNumberOfUnreadBits() >= jsonLength * 8u)
                {
                    char* jsonData = new (std::nothrow) char[static_cast<size_t>(jsonLength) + 1u];
                    if (jsonData != nullptr) {
                        if (bsData.Read(jsonData, jsonLength)) {
                            jsonData[jsonLength] = '\0';
                            CallOnJsonDataIncoming(guiId, jsonData);
                        }
                        delete[] jsonData;
                    }
                }
                else if (guiId != 33 && guiId != 34)
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
	CChat::AddDebugMessage(xorstr(""));
	pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);
}
#endif

#ifdef __aarch64__
extern char AuthKeyTable[512][2][128];
void CNetGame::Packet_AuthKey(Packet* pkt)
{
    if (pkt == nullptr || pkt->data == nullptr || pkt->length < 3) return;

    const uint8_t authLen = static_cast<uint8_t>(pkt->data[1]);
    if (authLen == 0 || authLen > 127 || pkt->length < static_cast<unsigned int>(2u + authLen)) return;

    char authValue[128] = {};
    memcpy(authValue, pkt->data + 2, authLen);

    char* auth_key = nullptr;
    bool found_key = false;

    for (int x = 0; x < 512; x++)
    {
        if (strlen(AuthKeyTable[x][0]) == authLen && memcmp(authValue, AuthKeyTable[x][0], authLen) == 0)
        {
            auth_key = AuthKeyTable[x][1];
            found_key = true;
            break;
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
        CChat::AddDebugMessage(xorstr(""));
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

	bsSuccAuth.IgnoreBits(8); 
	bsSuccAuth.IgnoreBits(32); 
	bsSuccAuth.IgnoreBits(16); 
	bsSuccAuth.Read(MyPlayerID);
	bsSuccAuth.Read(uiChallenge);

    CPlayerPool* playerPool = GetPlayerPool();
    if (playerPool == nullptr || playerPool->GetLocalPlayer() == nullptr) return;
	playerPool->GetLocalPlayer()->SetLocalPlayerID(MyPlayerID);

	int iVersion = NETGAME_VERSION;
	char byteMod = 0x01;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

	const char* sampVersion = xorstr("0.3.7");
	const char* auth_bs = xorstr("15121F6F18550C00AC4B4F8A167D0379BB0ACA99043");
	
	const char* localPlayerName = (const char *)(playerPool->GetLocalPlayer()->GetLocalPlayerName());
	
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
	CChat::AddDebugMessage(xorstr(""));
	SetGameState(GAMESTATE_AWAIT_JOIN);
}

void CNetGame::Packet_AimSync(Packet* pkt)
{
    if (pkt == nullptr || pkt->data == nullptr || pkt->length < 34) return;
    if (GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint8_t aimSyncBuffer[31] = {0};
    if (!bsData.ReadBits(&pktId, 8) || !bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) ||
        !bsData.ReadBits(aimSyncBuffer, sizeof(aimSyncBuffer) * 8u)) return;
    CPlayerPool* playerPool = GetPlayerPool();
    CRemotePlayer* remote_player = playerPool ? playerPool->GetAt(playerId) : nullptr;
    if (remote_player) remote_player->StoreAimSyncData(aimSyncBuffer, 0);
}

void CNetGame::Packet_PlayerSync(Packet* pkt)
{
    if (pkt == nullptr || pkt->data == nullptr || pkt->length < 3) return;
    if (GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint32_t timestamp = 0;
    int16_t lrAnalog = 0, udAnalog = 0; uint16_t wKeys = 0;
    CVector vecPos = {0.0f, 0.0f, 0.0f};
    float tw = 1.0f, tx = 0.0f, ty = 0.0f, tz = 0.0f;
    uint8_t byteHealthArmour = 0, byteCurrentWeapon = 0, byteSpecialAction = 0;
    CVector vecMoveSpeed = {0.0f, 0.0f, 0.0f};
    uint16_t wSurfInfo = 0; CVector vecSurfOffsets = {0.0f, 0.0f, 0.0f};
    if (pkt->data[0] == ID_TIMESTAMP) {
        if (!bsData.ReadBits(&pktId, 8) || !bsData.ReadBits(reinterpret_cast<unsigned char*>(&timestamp), 32)) return;
    }
    if (!bsData.ReadBits(&pktId, 8) || !bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16)) return;
    bool hasLrAnalog = false, hasUdAnalog = false, hasSurfInfo = false;
    if (!bsData.Read(hasLrAnalog)) return;
    if (hasLrAnalog && !bsData.ReadBits(reinterpret_cast<unsigned char*>(&lrAnalog), 16)) return;
    if (!bsData.Read(hasUdAnalog)) return;
    if (hasUdAnalog && !bsData.ReadBits(reinterpret_cast<unsigned char*>(&udAnalog), 16)) return;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&wKeys), 16) ||
        !bsData.Read(reinterpret_cast<char*>(&vecPos), sizeof(vecPos)) ||
        !bsData.ReadNormQuat<float>(tw, tx, ty, tz) ||
        !bsData.ReadBits(&byteHealthArmour, 8) || !bsData.ReadBits(&byteCurrentWeapon, 8) ||
        !bsData.ReadBits(&byteSpecialAction, 8) ||
        !bsData.ReadVector<float>(vecMoveSpeed.x, vecMoveSpeed.y, vecMoveSpeed.z)) return;
    if (!bsData.Read(hasSurfInfo)) return;
    if (hasSurfInfo && (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&wSurfInfo), 16) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&vecSurfOffsets.x), 32) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&vecSurfOffsets.y), 32) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&vecSurfOffsets.z), 32))) return;
    const uint8_t byteArmTemp = byteHealthArmour & 0x0F, byteHlTemp = byteHealthArmour >> 4;
    const uint8_t byteArmour = (byteArmTemp == 0x0F) ? 100 : (byteArmTemp == 0 ? 0 : static_cast<uint8_t>(byteArmTemp * 7));
    const uint8_t byteHealth = (byteHlTemp == 0x0F) ? 100 : (byteHlTemp == 0 ? 0 : static_cast<uint8_t>(byteHlTemp * 7));
    BROnFootSyncData ofSync = {};
    ofSync.lrAnalogLeftStick = lrAnalog; ofSync.udAnalogLeftStick = udAnalog; ofSync.wKeys = wKeys;
    ofSync.vecPos = vecPos; ofSync.quatw = tw; ofSync.quatx = tx; ofSync.quaty = ty; ofSync.quatz = tz;
    ofSync.health = byteHealth; ofSync.armour = byteArmour; ofSync.byteCurrentWeapon = byteCurrentWeapon;
    ofSync.byteSpecialAction = byteSpecialAction; ofSync.vecMoveSpeed = vecMoveSpeed;
    ofSync.vecSurfOffsets = vecSurfOffsets; ofSync.wSurfInfo = wSurfInfo;
    CPlayerPool* playerPool = GetPlayerPool();
    CRemotePlayer* remote_player = playerPool ? playerPool->GetAt(playerId) : nullptr;
    if (remote_player) remote_player->StoreSyncData(&ofSync, 0);
}
void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    if (pkt == nullptr || pkt->data == nullptr || pkt->length < 3) return;
    if (GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; BRInCarSyncData icsync = {};
    if (!bsData.ReadBits(&pktId, 8) || !bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&icsync.VehicleID), 16) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&icsync.lrAnalogLeftStick), 16) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&icsync.udAnalogLeftStick), 16) ||
        !bsData.ReadBits(reinterpret_cast<unsigned char*>(&icsync.wKeys), 16) ||
        !bsData.ReadNormQuat<float>(icsync.quatw, icsync.quatx, icsync.quaty, icsync.quatz) ||
        !bsData.ReadVector<float>(icsync.vecPos.x, icsync.vecPos.y, icsync.vecPos.z) ||
        !bsData.ReadVector<float>(icsync.vecMoveSpeed.x, icsync.vecMoveSpeed.y, icsync.vecMoveSpeed.z)) return;
    uint16_t wTempCarHealth = 0;
    if (!bsData.ReadBits(reinterpret_cast<unsigned char*>(&wTempCarHealth), 16)) return;
    icsync.fCarHealth = wTempCarHealth;
    uint8_t byteHealthArmour = 0;
    if (!bsData.Read(byteHealthArmour)) return;
    const uint8_t byteArmTemp = byteHealthArmour & 0x0F, byteHlTemp = byteHealthArmour >> 4;
    icsync.playerArmour = (byteArmTemp == 0x0F) ? 100 : (byteArmTemp == 0 ? 0 : byteArmTemp * 7);
    icsync.playerHealth = (byteHlTemp == 0x0F) ? 100 : (byteHlTemp == 0 ? 0 : byteHlTemp * 7);
    uint8_t byteTempWeapon = 0;
    if (!bsData.Read(byteTempWeapon)) return;
    icsync.byteCurrentWeapon = byteTempWeapon & 0x3F;
    bool bCheck = false;
    if (!bsData.ReadCompressed(bCheck)) return; if (bCheck) icsync.byteSirenOn = 1;
    if (!bsData.ReadCompressed(bCheck)) return; if (bCheck) icsync.byteLandingGearState = 1;
    if (!bsData.ReadCompressed(bCheck)) return; if (bCheck && !bsData.Read(icsync.TrailerID)) return;
    CPlayerPool* playerPool = GetPlayerPool();
    CRemotePlayer* remote_player = playerPool ? playerPool->GetAt(playerId) : nullptr;
    if (remote_player) remote_player->StoreInCarSyncData(&icsync, 0);
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
    if (pkt == nullptr || pkt->data == nullptr || pkt->length < 29) return;
    if (GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint8_t passengerSync[26] = {0};
    if (!bsData.ReadBits(&pktId, 8) || !bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) ||
        !bsData.ReadBits(passengerSync, sizeof(passengerSync) * 8u)) return;
    CPlayerPool* playerPool = GetPlayerPool();
    CRemotePlayer* remote_player = playerPool ? playerPool->GetAt(playerId) : nullptr;
    if (remote_player) remote_player->StorePassengerSyncData(passengerSync, 0);
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
    if (pkt == nullptr || pkt->data == nullptr || pkt->length < 43) return;
    if (GetGameState() != GAMESTATE_CONNECTED) return;
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    uint8_t pktId = 0; uint16_t playerId = 0; uint8_t bulletSync[40] = {0};
    if (!bsData.ReadBits(&pktId, 8) || !bsData.ReadBits(reinterpret_cast<unsigned char*>(&playerId), 16) ||
        !bsData.ReadBits(bulletSync, sizeof(bulletSync) * 8u)) return;
    CPlayerPool* playerPool = GetPlayerPool();
    CRemotePlayer* remote_player = playerPool ? playerPool->GetAt(playerId) : nullptr;
    if (remote_player) {
        CLocalPlayer* local_player = playerPool->GetLocalPlayer();
        if (local_player != nullptr && local_player->GetLocalPlayerID() != playerId) remote_player->StoreBulletSyncData(bulletSync, 0);
    }
}

void CNetGame::SendOnData(int guiId, const char* data, uint32_t length)
{
    if (pRakClient == nullptr || data == nullptr || length > (1024u * 1024u)) return;
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
    reinterpret_cast<void(*)(Packet*)>(CGameAPI::GetBase(xorstr("CNetGame::Packet_GUI")))(pkt);
}