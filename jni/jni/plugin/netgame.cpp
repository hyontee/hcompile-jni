#include "netgame.h"
#include "xorstr.h"
#include "game/jniutil.h"
#include "vendor/nlohmann/json.hpp"
#include <thread>
#include <chrono>


#include "plugin.h"


#include "game/BRNotification.h"

#define NETGAME_VERSION 4057

extern "C"
JNIEXPORT void JNICALL
Java_com_blackhub_bronline_RadialMenuReporter_sendRadialClick(JNIEnv *env, jobject thiz, jint viewId) {
    char buffer[64];
    sprintf(buffer, "radial = %d", viewId);
    CChat::AddDebugMessage(buffer);  // выводим в чат
}

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
	return *(CPlayerPool **)(CGameAPI::GetBase(xorstr("CNetGame::m_pPlayerPool")));
}

void CNetGame::ProcessNetwork()
{
    Packet* pkt = nullptr;
    uint8_t packetIdentifier;
    while (pkt = pRakClient->Receive())
    {
        packetIdentifier = GetPacketID(pkt);
        switch (packetIdentifier)
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
            case ID_AUTH_KEY: // ID_AUTH_KEY
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
                Packet_ConnectionLost(pkt);
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
            case 251:
                Packet_CustomRPC(pkt);
                break;
            case 252:
                Packet_CustomRPC(pkt);
                break;
        }
        pRakClient->DeallocatePacket(pkt);
    }
}

bool g_playerGUIOpened[256] = { false };

void FLog1(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;
	const char* pszStorage = "/storage/emulated/0/Android/data/com.bless.client/";


	if (flLog == nullptr && pszStorage != nullptr)
	{
		sprintf(buffer, "%s/edgar_log.txt", pszStorage);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}
bool is = true;
extern CJavaWrapper* pJavaWrapper;
void ShowDebugMessage(const char* msg);
void CallGuiRPC(int guiID, const char* jsonData);

void CNetGame::Packet_CustomRPC(Packet* p) {
    if (!p || !p->data || p->length < 2) return;

    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(8);

    uint16_t rpcID = 0;
    bs.Read(rpcID);

    uint32_t length = 0;
    bs.Read(length);
    if (length == 0 || length > 65536) {
        ShowDebugMessage("Invalid RPC length");
        return;
    }

    std::string jsonString(length, '\0');
    bs.Read(&jsonString[0], length);

    CallGuiRPC(rpcID, jsonString.c_str());

    if (rpcID != 252 && rpcID != 22) {
        ShowDebugMessage("Unknown GUI RPC");
    }
}

void CallGuiRPC(int guiID, const char* jsonData) {
    if (!pJavaWrapper) return;

    JNIEnv* env = pJavaWrapper->GetEnv();
    if (!env || !pJavaWrapper->s_JSONTransportClass) return;

    jclass jsonTransportClass = pJavaWrapper->s_JSONTransportClass;

    jmethodID onJsonDataIncoming = env->GetStaticMethodID(
        jsonTransportClass,
        "onJsonDataIncoming",
        "(I[B)V"
    );
    if (!onJsonDataIncoming) {
        CChat::AddDebugMessage(xorstr("Failed to find onJsonDataIncoming method"));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        return;
    }

    size_t len = strlen(jsonData);
    jbyteArray arr = env->NewByteArray(len);
    env->SetByteArrayRegion(arr, 0, len, (jbyte*)jsonData);

    env->CallStaticVoidMethod(jsonTransportClass, onJsonDataIncoming, guiID, arr);

    env->DeleteLocalRef(arr);

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

// Объявляем функцию обычным C++ именем
void CNetGame::SendOnData(int guiId, const char* data, uint32_t length)
{
    if (!pRakClient) {
        ShowDebugMessage("SendOnData: pRakClient is null");
        return;
    }
    if (!data || length == 0) {
        ShowDebugMessage("SendOnData: data is null or length == 0");
        return;
    }
    if (length > 65535) {
        ShowDebugMessage("SendOnData: data too long for GUI RPC");
        return;
    }

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)252);         // кастомный packet ID
    bsSend.Write((uint16_t)guiId);      // GUI ID
    bsSend.Write((uint32_t)length);     // длина данных
    bsSend.Write(data, length);         // данные JSON

    bool success = pRakClient->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
    if (!success) {
        ShowDebugMessage("SendOnData: failed to send packet via RakClient");
    }
}

// JNI версия остаётся для вызова из Java, но теперь можно вызывать C++ функцию напрямую
extern "C" JNIEXPORT void JNICALL Java_com_blackhub_bronline_game_core_JNIJSONTransport_sendJsonData(
    JNIEnv* env, jclass clazz, jint guiId, jbyteArray data)
{
    if (!data) {
        ShowDebugMessage("sendJsonData: data array is null");
        return;
    }

    jsize length = env->GetArrayLength(data);
    if (length <= 0) {
        ShowDebugMessage("sendJsonData: data length <= 0");
        return;
    }

    jbyte* byteArray = env->GetByteArrayElements(data, nullptr);
    if (!byteArray) {
        ShowDebugMessage("sendJsonData: failed to get byte array elements");
        return;
    }

    // Игнорируем guiId == 10 (старый функционал)
    if (guiId == 10) {
        ShowDebugMessage("sendJsonData: guiId 10 ignored (handled separately)");
        env->ReleaseByteArrayElements(data, byteArray, JNI_ABORT);
        return;
    }

    // Просто вызываем C++ функцию
    CNetGame::SendOnData(guiId, (const char*)byteArray, length);

    env->ReleaseByteArrayElements(data, byteArray, JNI_ABORT);
}


void ShowDebugMessage(const char* msg) {
    CChat::AddDebugMessage(xorstr("%s"), msg);
}


int CNetGame::GetGameState()
{
	return *(int *)(CGameAPI::GetBase(xorstr("CNetGame::m_iGameState")));
}

void CNetGame::SetGameState(int state)
{
	*(int *)(CGameAPI::GetBase(xorstr("CNetGame::m_iGameState"))) = state;
}

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
	CVector vecPos;
	float tw, tx, ty, tz;
	uint8_t byteHealthArmour, byteHealth, byteArmour;
	CVector vecMoveSpeed;
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
	if( ((v6 << v7) & 0x80) != 0 )
	{
	    if(!bsData.ReadBits((unsigned char *)&lrAnalog, 16)) 
	    {
	        return;
	    }
	    v8 = bsData.readOffset;
	    numberOfBitsUsed = bsData.numberOfBitsUsed;
	}
	if(v8 < numberOfBitsUsed)
	{
	    int v9 = bsData.data[v8 >> 3];
	    bsData.readOffset = v8 + 1;
	    if( (((v9 << (v8 & 7)) & 0x80) == 0 ||
	            bsData.ReadBits((unsigned char *)&udAnalog, 16))
		    && bsData.ReadBits((unsigned char *)&wKeys, 16)
			&& bsData.Read((char *)&vecPos, 12)
			&& bsData.ReadNormQuat<float>(tw, tx, ty, tz)
			&& bsData.ReadBits((unsigned char *)&byteHealthArmour, 8)
			&& bsData.ReadBits((unsigned char *)&byteCurrentWeapon, 8)
			&& bsData.ReadBits((unsigned char *)&byteSpecialAction, 8)
			&& bsData.ReadVector<float>(mx, my, mz))
		{
			readOffset = bsData.readOffset;
			if(readOffset >= bsData.numberOfBitsUsed) {
				return;
			}
			int v7_ = bsData.data[readOffset >> 3];
			bsData.readOffset = readOffset + 1;
			if(((v7_ << (readOffset & 7)) & 0x80) != 0)
			{
				if(bsData.ReadBits((unsigned char *)&wSurfInfo, 16)
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
	
	CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
	if(remote_player && accepted_sync) {
		remote_player->StoreSyncData(&ofSync, 0);
	}
}

void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);

    if (GetGameState() != GAMESTATE_CONNECTED) return;

    uint8_t pktId;
    bsData.ReadBits(&pktId, 8);

    uint16_t playerId;
    unsigned char tmp2[2];
    bsData.ReadBits(tmp2, 16);
    playerId = tmp2[0] | (tmp2[1] << 8);

    BRInCarSyncData icsync = {0};

    bsData.ReadBits(tmp2, 16);
    icsync.VehicleID = tmp2[0] | (tmp2[1] << 8);

    bsData.ReadBits(tmp2, 16);
    icsync.lrAnalogLeftStick = tmp2[0] | (tmp2[1] << 8);

    bsData.ReadBits(tmp2, 16);
    icsync.udAnalogLeftStick = tmp2[0] | (tmp2[1] << 8);

    bsData.ReadBits(tmp2, 16);
    icsync.wKeys = tmp2[0] | (tmp2[1] << 8);

    unsigned char tmp4[4];
    bsData.ReadBits(tmp4, 32); icsync.quatw = *(float*)tmp4;
    bsData.ReadBits(tmp4, 32); icsync.quatx = *(float*)tmp4;
    bsData.ReadBits(tmp4, 32); icsync.quaty = *(float*)tmp4;
    bsData.ReadBits(tmp4, 32); icsync.quatz = *(float*)tmp4;

    unsigned char tmp12[12];
    bsData.ReadBits(tmp12, 96);
    icsync.vecPos.x = *(float*)(tmp12);
    icsync.vecPos.y = *(float*)(tmp12+4);
    icsync.vecPos.z = *(float*)(tmp12+8);

    bsData.ReadBits(tmp12, 96);
    icsync.vecMoveSpeed.x = *(float*)(tmp12);
    icsync.vecMoveSpeed.y = *(float*)(tmp12+4);
    icsync.vecMoveSpeed.z = *(float*)(tmp12+8);

    bsData.ReadBits(tmp2, 16);
    icsync.fCarHealth = tmp2[0] | (tmp2[1] << 8);

    bsData.ReadBits(tmp2, 16);
    icsync.EngineStatus = tmp2[0] | (tmp2[1] << 8);

    uint8_t byteHealthArmour;
    bsData.Read(byteHealthArmour);
    uint8_t byteArmTemp = byteHealthArmour & 0x0F;
    uint8_t byteHlTemp = byteHealthArmour >> 4;

    icsync.playerArmour = (byteArmTemp == 0xF) ? 100 : (byteArmTemp * 7);
    icsync.playerHealth = (byteHlTemp == 0xF) ? 100 : (byteHlTemp * 7);

    uint8_t byteTempWeapon;
    bsData.Read(byteTempWeapon);
    icsync.byteCurrentWeapon ^= (byteTempWeapon ^ icsync.byteCurrentWeapon) & 0x3F;

    bool bCheck;
    bsData.ReadCompressed(bCheck);
    if (bCheck) icsync.byteSirenOn = 1;

    bsData.ReadCompressed(bCheck);
    if (bCheck) icsync.byteLandingGearState = 1;

    bsData.ReadCompressed(bCheck);
    if (bCheck) bsData.Read(icsync.TrailerID);

    int vehPool = *(int *)(CGameAPI::GetBase("CNetGame::m_pVehiclePool"));

    CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
    if (remote_player) {
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