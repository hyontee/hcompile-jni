#include "netgame.h"
#include "xorstr.h"
#include "plugin.h"

#include <thread>
#include <chrono>
#include <atomic>

#define NETGAME_VERSION 4057

extern RakClientInterface* pRakClient;

uint16_t CNetGame::m_nLastSAMPDialogID;

static int s_nFailedConnections = 0;
#define VPN_SUGGEST_THRESHOLD 3

static std::atomic<bool> s_bRetryPending(false);

// ─────────────────────────────────────────────
//  Одно красивое сообщение в чат
//  {FFD700}SUPPORT-LIMERUSSIA {808080}» {FFFFFF}текст
// ─────────────────────────────────────────────
static void SupportMsg(const char* text)
{
    char buf[512];
    snprintf(buf, sizeof(buf),
        "{FFD700}SUPPORT-LIMERUSSIA {808080}» {FFFFFF}%s", text);
    CChat::AddDebugMessage(buf);
}

// ─────────────────────────────────────────────
//  Повторная попытка через 15 секунд
// ─────────────────────────────────────────────
static void RetryConnectionThread()
{
    for (int i = 15; i > 0; i--)
    {
        if (CNetGame::GetGameState() == GAMESTATE_CONNECTED)
        {
            s_bRetryPending = false;
            return;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (CNetGame::GetGameState() != GAMESTATE_CONNECTED)
    {
        SupportMsg(xorstr("Выполняется повторное подключение к серверу..."));
        CNetGame::SetGameState(GAMESTATE_WAIT_CONNECT);
    }

    s_bRetryPending = false;
}

static void ScheduleRetry(bool showVPN = false)
{
    if (showVPN)
    {
        SupportMsg(xorstr("Отсутствует соединение. Для решения: попробуйте — перезагрузить роутер, либо сменить интернет, либо воспользоваться VPN."));
        s_nFailedConnections = 0;
    }

    if (!s_bRetryPending.exchange(true))
    {
        std::thread(RetryConnectionThread).detach();
    }
}

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
                SupportMsg(xorstr("Не удалось установить шифрование. Повторите попытку."));
                break;

            case ID_CONNECTION_ATTEMPT_FAILED:
            {
                s_nFailedConnections++;
                bool needVPN = (s_nFailedConnections >= VPN_SUGGEST_THRESHOLD);
                if (!needVPN)
                    SupportMsg(xorstr("Сервер не отвечает. Повторное подключение через 15 секунд."));
                ScheduleRetry(needVPN);
                break;
            }

            case ID_NO_FREE_INCOMING_CONNECTIONS:
            {
                // Учитываем в общем счётчике неудач — если подключиться не выходит
                // раз за разом (сервер полный, сеть нестабильна), предлагаем VPN.
                s_nFailedConnections++;
                bool needVPN = (s_nFailedConnections >= VPN_SUGGEST_THRESHOLD);
                if (!needVPN)
                    SupportMsg(xorstr("Сервер переполнен. Повторное подключение через 15 секунд."));
                ScheduleRetry(needVPN);
                break;
            }

            case ID_CONNECTION_BANNED:
                SupportMsg(xorstr("Вы заблокированы на данном сервере."));
                break;

            case ID_INVALID_PASSWORD:
                SupportMsg(xorstr("Вы ввели неверный пароль от сервера."));
                pRakClient->Disconnect(0);
                break;

            case ID_AUTH_KEY:
                Packet_AuthKey(pkt);
                break;

            case ID_CONNECTION_REQUEST_ACCEPTED:
                s_nFailedConnections = 0;
                s_bRetryPending = false;
                Packet_ConnectionSucceeded(pkt);
                break;

            case ID_CONNECTION_LOST:
            {
                s_nFailedConnections++;
                bool needVPN = (s_nFailedConnections >= VPN_SUGGEST_THRESHOLD);
                if (!needVPN)
                    SupportMsg(xorstr("Вы потеряли соединение с сервером. Повторное подключение через 15 секунд."));
                Packet_ConnectionLost(pkt);
                ScheduleRetry(needVPN);
                break;
            }

            case ID_DISCONNECTION_NOTIFICATION:
                SupportMsg(xorstr("Вы отключились от сервера. Повторное подключение через 15 секунд."));
                pRakClient->Disconnect(2000, 0);
                ScheduleRetry(false);
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
        BYTE byteAuthKeyLen = (BYTE)strlen(auth_key);
        bsKey.Write((BYTE)ID_AUTH_KEY);
        bsKey.Write((BYTE)byteAuthKeyLen);
        bsKey.Write(auth_key, byteAuthKeyLen);
        pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, NULL);
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
    uint8_t pktId; uint16_t playerId;
    uint8_t aimSyncBuffer[31] = {0};
    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);
    bsData.ReadBits((unsigned char *)&aimSyncBuffer, 31 * 8);
    CPlayerPool* pool = GetPlayerPool();
    if (!pool) { return; }
    CRemotePlayer* remote_player = pool->GetAt(playerId);
    if(remote_player) { remote_player->StoreAimSyncData(aimSyncBuffer, 0); }
}

void CNetGame::Packet_PlayerSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) { return; }
    bool accepted_sync = false;
    uint8_t pktId; uint16_t playerId; uint32_t timestamp;
    int16_t lrAnalog, udAnalog; uint16_t wKeys;
    CVector vecPos = {0.0f, 0.0f, 0.0f};
    float tw, tx, ty, tz;
    uint8_t byteHealthArmour, byteHealth, byteArmour;
    CVector vecMoveSpeed = {0.0f, 0.0f, 0.0f};
    float mx, my, mz; uint16_t wSurfInfo; CVector vecSurfOffsets;
    uint8_t byteCurrentWeapon, byteSpecialAction;

    if(pkt->data[0] == ID_TIMESTAMP) {
        bsData.ReadBits((unsigned char *)&pktId, 8);
        bsData.ReadBits((unsigned char *)&timestamp, 32);
    }
    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);

    int readOffset = bsData.readOffset;
    int numberOfBitsUsed = bsData.numberOfBitsUsed;
    if(readOffset >= numberOfBitsUsed) { return; }
    int v6 = bsData.data[readOffset >> 3];
    char v7 = readOffset & 7;
    int v8 = readOffset + 1;
    bsData.readOffset = v8;
    if (((v6 << v7) & 0x80) != 0) {
        if (!bsData.ReadBits((unsigned char *)&lrAnalog, 16)) { return; }
        v8 = bsData.readOffset;
        numberOfBitsUsed = bsData.numberOfBitsUsed;
    } else { lrAnalog = 0; }

    if (v8 < numberOfBitsUsed) {
        int v9 = bsData.data[v8 >> 3];
        bsData.readOffset = v8 + 1;
        bool ok = true;
        if (((v9 << (v8 & 7)) & 0x80) != 0) {
            ok = bsData.ReadBits((unsigned char *)&udAnalog, 16);
        } else { udAnalog = 0; }

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
            if (readOffset >= bsData.numberOfBitsUsed) { return; }
            int v7_ = bsData.data[readOffset >> 3];
            bsData.readOffset = readOffset + 1;
            if (((v7_ << (readOffset & 7)) & 0x80) != 0) {
                if (bsData.ReadBits((unsigned char *)&wSurfInfo, 16)
                    && bsData.ReadBits((unsigned char *)&vecSurfOffsets.x, 32)
                    && bsData.ReadBits((unsigned char *)&vecSurfOffsets.y, 32)
                    && bsData.ReadBits((unsigned char *)&vecSurfOffsets.z, 32))
                { /* surf */ }
            }
            accepted_sync = true;
        }
    }

    uint8_t byteArmTemp = (byteHealthArmour & 0x0F);
    uint8_t byteHlTemp  = (byteHealthArmour >> 4);
    byteArmour = (byteArmTemp == 0xF) ? 100 : (byteArmTemp == 0) ? 0 : byteArmTemp * 7;
    byteHealth  = (byteHlTemp  == 0xF) ? 100 : (byteHlTemp  == 0) ? 0 : byteHlTemp  * 7;

    BROnFootSyncData ofSync = {0};
    ofSync.lrAnalogLeftStick = lrAnalog; ofSync.udAnalogLeftStick = udAnalog;
    ofSync.wKeys = wKeys; ofSync.vecPos = vecPos;
    ofSync.quatw = tw; ofSync.quatx = tx; ofSync.quaty = ty; ofSync.quatz = tz;
    ofSync.health = byteHealth; ofSync.armour = byteArmour;
    ofSync.byteCurrentWeapon = byteCurrentWeapon;
    ofSync.byteSpecialAction = byteSpecialAction;
    ofSync.vecMoveSpeed = vecMoveSpeed;
    ofSync.vecSurfOffsets = vecSurfOffsets; ofSync.wSurfInfo = wSurfInfo;

    CPlayerPool* pool = GetPlayerPool();
    if (!pool) { return; }
    CRemotePlayer* remote_player = pool->GetAt(playerId);
    if(remote_player && accepted_sync) { remote_player->StoreSyncData(&ofSync, 0); }
}

void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) { return; }
    uint8_t pktId; uint16_t playerId;
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
    bsData.Read(byteHealthArmour);
    uint8_t byteArmTemp = (byteHealthArmour & 0x0F);
    uint8_t byteHlTemp  = (byteHealthArmour >> 4);
    icsync.playerArmour = (byteArmTemp == 0xF) ? 100 : (byteArmTemp == 0) ? 0 : byteArmTemp * 7;
    icsync.playerHealth  = (byteHlTemp  == 0xF) ? 100 : (byteHlTemp  == 0) ? 0 : byteHlTemp  * 7;
    uint8_t byteTempWeapon;
    bsData.Read(byteTempWeapon);
    icsync.byteCurrentWeapon ^= (byteTempWeapon ^ icsync.byteCurrentWeapon) & 0x3F;
    bool bCheck;
    bsData.ReadCompressed(bCheck); if(bCheck) { icsync.byteSirenOn = 1; }
    bsData.ReadCompressed(bCheck); if(bCheck) { icsync.byteLandingGearState = 1; }
    bsData.ReadCompressed(bCheck); if(bCheck) { bsData.Read(icsync.TrailerID); }
    CPlayerPool* pool = GetPlayerPool();
    if (!pool) { return; }
    CRemotePlayer* remote_player = pool->GetAt(playerId);
    if(remote_player) { remote_player->StoreInCarSyncData(&icsync, 0); }
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) { return; }

    uint8_t pktId;
    uint16_t playerId;
    BRPassengerSyncData brSync = {0};

    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);

    // Читаем поля BR-формата по отдельности — точно так же, как в исходящем пути
    // (hook_RakClient__Send / BR_ID_PASSENGER_SYNC).
    //
    // Нельзя читать весь struct одним ReadBits: побитовые поля byteSeatFlags:7
    // и byteDriveBy:1 записаны в память в порядке LSB→MSB, а BitStream отдаёт
    // биты MSB→LSB. При массовом чтении биты в этом байте переставляются —
    // seatFlags и driveBy получают неверные значения, и все данные после
    // (lrAnalog, wKeys, vecPos) смещаются. Движок видит неправильный VehicleID
    // или номер сиденья и показывает пассажира вне машины либо «на паузе».
    bsData.ReadBits((unsigned char *)&brSync.VehicleID,         16);
    uint8_t tempByte = 0;
    bsData.ReadBits((unsigned char *)&tempByte,                  7);
    brSync.byteSeatFlags = tempByte;
    bsData.ReadBits((unsigned char *)&tempByte,                  1);
    brSync.byteDriveBy   = tempByte;
    bsData.ReadBits((unsigned char *)&brSync.byteCurrentWeapon,  8);
    bsData.ReadBits((unsigned char *)&brSync.playerHealth,       16);
    bsData.ReadBits((unsigned char *)&brSync.playerArmour,       16);
    bsData.ReadBits((unsigned char *)&brSync.lrAnalog,           16);
    bsData.ReadBits((unsigned char *)&brSync.udAnalog,           16);
    bsData.ReadBits((unsigned char *)&brSync.wKeys,              16);
    bsData.ReadBits((unsigned char *)&brSync.vecPos,             96);

    // Конвертируем BR-формат в SAMP-формат (uint16_t health/armour → uint8_t)
    RakNet::BitStream bsConverted;
    ConvertBRPassengerSyncToSampSync(&bsConverted, brSync);

    CPlayerPool* pool = GetPlayerPool();
    if (!pool) { return; }
    CRemotePlayer* remote_player = pool->GetAt(playerId);
    if(remote_player) {
        remote_player->StorePassengerSyncData(bsConverted.GetData(), 0);
    }
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) { return; }
    uint8_t pktId; uint16_t playerId;
    uint8_t bulletSync[40] = {0};
    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);
    bsData.ReadBits((unsigned char *)&bulletSync, 40 * 8);
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
    if (!data) return;
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
