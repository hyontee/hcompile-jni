#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

void (*orig_CNetTextDrawPool__SetServerLogo)(uintptr_t thiz, std::string url);
void hook_CNetTextDrawPool__SetServerLogo(uintptr_t thiz, std::string url)
{
	//RegisterRPCs(pRakClient);
    orig_CNetTextDrawPool__SetServerLogo(thiz, url);
}

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    CNetGame::ProcessNetwork();
}

bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    return pRakClient->Connect(xorstr("80.242.59.112"), 2221, 0, 0, 5);
}

bool (*orig_RakClient__RPC)( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget );
bool hook_RakClient__RPC( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )
{
	int sampRpcId = ConvertBRIDToSampID(uniqueID);
	if(sampRpcId != -1) {
		if(sampRpcId == RPC_RequestClass && g_bInitGameProcess) {
			return false;
		}
		return pRakClient->RPC(&sampRpcId, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
	}
	return false;
}

// full turnlights by @libplugin

static const int kMaxVehicles = 2000;
static uint8_t  g_TurnState[kMaxVehicles] = {3};           // 0 - выкл, 1 - левый, 2 - правый, 3 - аварийка (по умолчанию аварийка)
static bool     g_LightsOnArr[kMaxVehicles] = {false};
static uint64_t g_LastBlinkTimeArr[kMaxVehicles] = {0};
static uint64_t g_NextAutoCycleArr[kMaxVehicles] = {0};
static bool     g_BlinkPhaseArr[kMaxVehicles] = {false};
static bool     g_ForceUpdateArr[kMaxVehicles] = {false};
static uint16_t g_LastLocalVehicleId = 0xFFFF;

// === Utility ===
inline uint64_t GetTickCountMS()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

// === Глобальная фаза мигания (абсолютная синхронизация) ===
inline bool GetGlobalBlinkPhase(uint64_t now)
{
    // 0.5 сек включено, 0.5 сек выключено (1 сек = полный цикл)
    return ((now / 500) % 2) == 0;
}

// === Инициализация состояния поворотников ===
void SetTurnSignalStateForVehicle(uint16_t vehicleId, uint8_t state)
{
    if (vehicleId >= kMaxVehicles) return;

    g_TurnState[vehicleId] = state;
    g_LightsOnArr[vehicleId] = (state != 0);
    uint64_t now = GetTickCountMS();
    g_LastBlinkTimeArr[vehicleId] = now;
    g_NextAutoCycleArr[vehicleId] = now + 3500;
    g_BlinkPhaseArr[vehicleId] = true;
    g_ForceUpdateArr[vehicleId] = true;
}

// === Обработка поворотников при сетевой синхронизации ===
void ProcessTurnSignals(uint64_t veh, BRInCarSyncData& data)
{
    if (!veh || data.VehicleID >= kMaxVehicles) return;

    const uint16_t id = data.VehicleID;
    const uint64_t now = GetTickCountMS();
    const uint8_t  state = g_TurnState[id];
    if (state == 0) return;

    // Глобальная синхронизация — все клиенты моргают в один такт
    bool globalBlink = GetGlobalBlinkPhase(now);

    // Реалистичный автоцикл (3.5 сек)
    if (now >= g_NextAutoCycleArr[id])
    {
        g_NextAutoCycleArr[id] = now + 3500;
        g_ForceUpdateArr[id] = true;
    }

    uint8_t* pLight = (uint8_t*)(veh + 0x36C);
    if (!pLight) return;

    *pLight = globalBlink ? state : 0;
    g_LightsOnArr[id] = (*pLight != 0);
}

// === Главный тик — работает всегда, даже вне стрима ===
void TickTurnSignals()
{
    if (g_LastLocalVehicleId == 0xFFFF) return;
    uint16_t id = g_LastLocalVehicleId;
    if (id >= kMaxVehicles || g_TurnState[id] == 0) return;

    uint64_t now = GetTickCountMS();
    if (now - g_LastBlinkTimeArr[id] >= 500)
    {
        g_BlinkPhaseArr[id] = !g_BlinkPhaseArr[id];
        g_LastBlinkTimeArr[id] = now;
    }

    if (now >= g_NextAutoCycleArr[id])
    {
        g_BlinkPhaseArr[id] = true;
        g_NextAutoCycleArr[id] = now + 3500;
    }

    uint64_t vehPool = *(uint64_t*)(CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool")));
    if (!vehPool) return;
    uint64_t veh = *(uint64_t*)(vehPool + 8ull * id);
    if (!veh) return;

    uint8_t* pLight = (uint8_t*)(veh + 0x36C);
    *pLight = g_BlinkPhaseArr[id] ? g_TurnState[id] : 0;
}

void ToggleTurnSignal(uint8_t mode)
{
    if (g_LastLocalVehicleId == 0xFFFF) return;
    uint16_t id = g_LastLocalVehicleId;

    if (g_TurnState[id] == mode)
        g_TurnState[id] = 0; // выключаем
    else
        SetTurnSignalStateForVehicle(id, mode);
}

// === ВЫЗОВ ЗВУКА ДЛЯ GTA Vice City Android ===
inline void PlayTurnSignalSound(uint16_t soundId)
{
    // Указатель на функцию проигрывания звука в игре (адрес можно уточнить по версии)
    void (*PlayOneShot)(uint16_t, float, float, float) = (void(*)(uint16_t, float, float, float))0x39B4A4; // примерный адрес для Vice City Android 2.1

    if (PlayOneShot)
        PlayOneShot(soundId, 0.0f, 0.0f, 0.0f);
}

// === Структура для расширений ===
struct VehicleTurnSignal
{
    bool leftOn = false;
    bool rightOn = false;
    bool blinkState = false;
    float timer = 0.0f;
};
std::map<int, VehicleTurnSignal> g_VehicleSignals;

bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel );
bool hook_RakClient__Send( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel )
{
    TickTurnSignals();
    RakNet::BitStream bsCopy(bitStream->GetData(), bitStream->GetNumberOfBytesUsed() + 1, false);
    uint8_t pktId;
    bsCopy.Read(pktId);
    if(pktId == BR_ID_USER_INTERFACE_SYNC) {
        uint16_t guiId;
        uint32_t jsonLen;
        bsCopy.Read(guiId);
        bsCopy.Read(jsonLen);
        if(jsonLen > 0) {
            const uint32_t kMaxJsonSize = 1u << 20; // 1 MB защитный предел
            if (jsonLen > kMaxJsonSize) {
                CNetGame::SendOnData(guiId, "", 0);
                return false;
            }

            char* buff = new char[jsonLen + 1];
            if (!bsCopy.Read(buff, jsonLen)) {
                delete[] buff;
                return false;
            }
            buff[jsonLen] = '\0';


            size_t utfCap = static_cast<size_t>(jsonLen) * 3 + 1;
            char* buffGUI = new char[utfCap];
            cp1251_to_utf8(buffGUI, buff, jsonLen);
            if(guiId == 10) {
                if(nlohmann::json::accept(buffGUI)) {
                    nlohmann::json jsonObj = nlohmann::json::parse(buffGUI);
                    uint8_t btn = jsonObj["r"];
                    int16_t listInput = jsonObj["l"];
                    std::string input = jsonObj["i"];
                    uint8_t inputLen = input.length();
                    RakNet::BitStream bsSend;
                    bsSend.WriteBits((unsigned char *)&CNetGame::m_nLastSAMPDialogID, 16);
                    bsSend.WriteBits((unsigned char *)&btn, 8);
                    bsSend.WriteBits((unsigned char *)&listInput, 16);
                    bsSend.WriteBits((unsigned char *)&inputLen, 8);
                    bsSend.Write(input.c_str(), inputLen);
                bool result = pRakClient->RPC(&RPC_DialogResponse, &bsSend,
                                              HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                              false, UNASSIGNED_NETWORK_ID, NULL);
                if (result) {
                } else {
                }
            }
        }
        else {
            CNetGame::SendOnData(guiId, buffGUI, jsonLen);
        }
            delete[] buffGUI;
            delete[] buff;
        }
    }
	if(pktId == BR_ID_AIM_SYNC) {
		uint8_t pktAimSync = ID_AIM_SYNC;
	    uint8_t aimSyncBuffer[31] = {0};
		bsCopy.ReadBits(aimSyncBuffer, 31 * 8);
		RakNet::BitStream bsSend;
	    bsSend.WriteBits(&pktAimSync, 8);
	    bsSend.WriteBits(aimSyncBuffer, 31 * 8);
	    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_BULLET_SYNC) {
		uint8_t pktBltSync = ID_BULLET_SYNC;
	    uint8_t bltSyncBuffer[40] = {0};
		bsCopy.ReadBits(bltSyncBuffer, 40 * 8);
		RakNet::BitStream bsSend;
	    bsSend.WriteBits(&pktBltSync, 8);
	    bsSend.WriteBits(bltSyncBuffer, 40 * 8);
	    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_PLAYER_SYNC) {
		uint8_t pktPlayerSync = ID_PLAYER_SYNC;
	    BROnFootSyncData data;
	    bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16);
		bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16);
		bsCopy.ReadBits((unsigned char *)&data.wKeys, 16);
		bsCopy.ReadBits((unsigned char *)&data.vecPos, 96);
		bsCopy.ReadBits((unsigned char *)&data.quatw, 32);
		bsCopy.ReadBits((unsigned char *)&data.quatx, 32);
		bsCopy.ReadBits((unsigned char *)&data.quaty, 32);
		bsCopy.ReadBits((unsigned char *)&data.quatz, 32);
		bsCopy.ReadBits((unsigned char *)&data.health, 16);
		bsCopy.ReadBits((unsigned char *)&data.armour, 16);
		bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8);
		bsCopy.ReadBits((unsigned char *)&data.byteSpecialAction, 8);
		bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed, 96);
		bsCopy.ReadBits((unsigned char *)&data.vecSurfOffsets, 96);
		bsCopy.ReadBits((unsigned char *)&data.wSurfInfo, 16);
		bsCopy.ReadBits((unsigned char *)&data.dwAnimation, 32); // 16
	    RakNet::BitStream bsSend;
	    bsSend.WriteBits(&pktPlayerSync, 8);
	    ConvertBROnFootSyncToSampSync(&bsSend, data);
	    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_VEHICLE_SYNC) {
		uint8_t pktVehicleSync = ID_VEHICLE_SYNC;
	    BRInCarSyncData data;
	    bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16);
		bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16);
		bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16);
		bsCopy.ReadBits((unsigned char *)&data.wKeys, 16);
		bsCopy.ReadBits((unsigned char *)&data.quatw, 32);
		bsCopy.ReadBits((unsigned char *)&data.quatx, 32);
		bsCopy.ReadBits((unsigned char *)&data.quaty, 32);
		bsCopy.ReadBits((unsigned char *)&data.quatz, 32);
		bsCopy.ReadBits((unsigned char *)&data.vecPos.x, 32);
		bsCopy.ReadBits((unsigned char *)&data.vecPos.y, 32);
		bsCopy.ReadBits((unsigned char *)&data.vecPos.z, 32);
		bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.x, 32);
		bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.y, 32);
		bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.z, 32);
		bsCopy.ReadBits((unsigned char *)&data.fCarHealth, 32);
		bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16);
		bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16);
		bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8);
		bsCopy.ReadBits((unsigned char *)&data.byteSirenOn, 8);
		bsCopy.ReadBits((unsigned char *)&data.byteLandingGearState, 8);
		bsCopy.ReadBits((unsigned char *)&data.TrailerID, 16);
	static float lastHealth[2000] = {0};
    	if(data.VehicleID < 2000) {
        if(lastHealth[data.VehicleID] != data.fCarHealth) {
            char debugMsg[128];
            snprintf(debugMsg, sizeof(debugMsg), "Vehicle %d health: %.1f -> %.1f", 
                     data.VehicleID, lastHealth[data.VehicleID], data.fCarHealth);
            CChat::AddDebugMessage(debugMsg);
            lastHealth[data.VehicleID] = data.fCarHealth;
        }
    	}
    uintptr_t vehPoolAddr = CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool"));
    if (vehPoolAddr) {
        uint64_t vehPool = *(uint64_t *)(vehPoolAddr);
        if (vehPool && data.VehicleID < 2000) {
            uint64_t veh = *(uint64_t *)(vehPool + 8ull * data.VehicleID);
            ProcessTurnSignals(veh, data);
        }
    }
    
    RakNet::BitStream bsSend;
    bsSend.WriteBits(&pktVehicleSync, 8);
    ConvertBRInCarSyncToSampSync(&bsSend, data);
    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

if(pktId == BR_ID_VEHICLE_SYNC) {
    uint8_t pktVehicleSync = ID_VEHICLE_SYNC;
    BRInCarSyncData data;
    bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16);
    bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16);
    bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16);
    bsCopy.ReadBits((unsigned char *)&data.wKeys, 16);
    bsCopy.ReadBits((unsigned char *)&data.quatw, 32);
    bsCopy.ReadBits((unsigned char *)&data.quatx, 32);
    bsCopy.ReadBits((unsigned char *)&data.quaty, 32);
    bsCopy.ReadBits((unsigned char *)&data.quatz, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecPos.x, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecPos.y, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecPos.z, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.x, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.y, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.z, 32);
    bsCopy.ReadBits((unsigned char *)&data.fCarHealth, 32);
    bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16);
    bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16);
    bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8);
    bsCopy.ReadBits((unsigned char *)&data.byteSirenOn, 8);
    bsCopy.ReadBits((unsigned char *)&data.byteLandingGearState, 8);
    bsCopy.ReadBits((unsigned char *)&data.TrailerID, 16);

    uintptr_t vehPoolAddr = CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool"));
    if (vehPoolAddr) {
        uint64_t vehPool = *(uint64_t *)(vehPoolAddr);
        if (vehPool && data.VehicleID < 2000) {
            uint64_t veh = *(uint64_t *)(vehPool + 8ull * data.VehicleID);
            ProcessTurnSignals(veh, data);
        }
    }

    RakNet::BitStream bsSend;
    bsSend.WriteBits(&pktVehicleSync, 8);
    ConvertBRInCarSyncToSampSync(&bsSend, data);
    g_LastLocalVehicleId = data.VehicleID;
    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

	if (pktId == BR_ID_TURNLIGHTS_SYNC)
{
    uint16_t vehicleId = 0;
    uint8_t state = 0;

    // Безопасное чтение
    if (!bsCopy.Read(vehicleId))
        vehicleId = g_LastLocalVehicleId < 2000 ? g_LastLocalVehicleId : 0;

    if (!bsCopy.Read(state))
        state = 0;

    // Установка состояния поворотников
    SetTurnSignalStateForVehicle(vehicleId, state);

    // Локальный звук при изменении
    uint64_t vehPoolAddr = CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool"));
    if (vehPoolAddr)
    {
        uint64_t vehPool = *(uint64_t*)(vehPoolAddr);
        uint64_t pVehicle = *(uint64_t*)(vehPool + 8ull * vehicleId);

        int localPlayerVeh = *(int*)(CGameAPI::GetBase(xorstr("FindPlayerVehicle")));
        if (localPlayerVeh && localPlayerVeh == pVehicle)
        {
            uint16_t soundId = (state == 3) ? 0x14 : 0x13; // 3 — аварийка
            PlayTurnSignalSound(soundId); // звук клика поворотника
        }
    }

    return orig_RakClient__Send(thiz, bitStream, priority, reliability, orderingChannel);
}
	if(pktId == BR_ID_PASSENGER_SYNC) {
		uint8_t pktPassengerSync = ID_PASSENGER_SYNC;
		BRPassengerSyncData data;
		bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16);
		uint8_t tempByte;
		bsCopy.ReadBits((unsigned char *)&tempByte, 7);
		data.byteSeatFlags = tempByte;
		bsCopy.ReadBits((unsigned char *)&tempByte, 1);
		data.byteDriveBy = tempByte;
		bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8);
		bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16);
		bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16);
		bsCopy.ReadBits((unsigned char *)&data.lrAnalog, 16);
		bsCopy.ReadBits((unsigned char *)&data.udAnalog, 16);
		bsCopy.ReadBits((unsigned char *)&data.wKeys, 16);
		bsCopy.ReadBits((unsigned char *)&data.vecPos, 96);
		RakNet::BitStream bsSend;
	    bsSend.WriteBits(&pktPassengerSync, 8);
	    ConvertBRPassengerSyncToSampSync(&bsSend, data);
	    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	
	return false;
}