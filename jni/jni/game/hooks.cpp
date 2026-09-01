#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "ANTICRASH", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "ANTICRASH", __VA_ARGS__)

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    CNetGame::ProcessNetwork();
}

#include "..//settings.h"
bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    return pRakClient->Connect(xorstr("178.130.53.117"), 1189, 0, 0, 5);
}

bool (*orig_RakClient__RPC)( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget );
bool hook_RakClient__RPC( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )
{
    int sampRpcId = ConvertBRIDToSampID(uniqueID);
    
    if(uniqueID == 313) {
        sampRpcId = 97; 
    }
    
    if(sampRpcId == 97) {
        int currentOffset = bitStream->GetReadOffset();
        
        int actionType = -1;
        unsigned char actionSubtype = 0;  
        
        if(bitStream->GetNumberOfUnreadBits() >= 32) {
            bitStream->ReadBits((unsigned char*)&actionType, 32, true);
        }
        if(bitStream->GetNumberOfUnreadBits() >= 8) {
            bitStream->ReadBits(&actionSubtype, 8, true); 
        }
        
        bitStream->SetReadOffset(currentOffset);
    }
    
    if(sampRpcId != -1) {
        if(sampRpcId == RPC_RequestClass && g_bInitGameProcess) {
            return false;
        }
        
        return pRakClient->RPC(&sampRpcId, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
    }
    
    return false;
}

bool bVehicleSyncSent = false;

static uint8_t g_LastTurnLightState = 0;
static uint64_t g_LastBlinkTime = 0;
static bool g_bLightsOn = false;
static bool g_bForceUpdate = false;

uint64_t GetTickCountMS() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

void ProcessTurnSignals(uint64_t veh, BRInCarSyncData& data) {
    if (!veh) return;

    uint8_t currentStatus = *(uint8_t*)(veh + 0x36C);
    uint64_t currentTime = GetTickCountMS();

    // если только что включили поворотники (с 0)
    if (currentStatus != 0 && g_LastTurnLightState == 0) {
        g_LastTurnLightState = currentStatus;
        g_LastBlinkTime = currentTime;
        g_bLightsOn = true;
    }

    // мигание, если поворотники были включены хоть раз
    if (g_LastTurnLightState != 0) {
        if (currentTime - g_LastBlinkTime > 500) {
            g_bLightsOn = !g_bLightsOn;
            g_LastBlinkTime = currentTime;

            // применяем только локально, без синка
            *(uint8_t*)(veh + 0x36C) = g_bLightsOn ? g_LastTurnLightState : 0;
        }
    }
}

void FLog(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;
	const char* pszStorage = "/storage/emulated/0/Android/data/com.bless.client/";


	if (flLog == nullptr && pszStorage != nullptr)
	{
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

// === simple_safe_create_object.h ===
// легкий, надёжный и минималистичный набор проверок для HandleCreateObjectRPC

#pragma once

#include <cmath>      // std::isfinite, std::fabs
#include <cstdint>    // uint32_t
#include <cstdio>     // snprintf если нужно
#include <cstdlib>    // NULL

// Заглушки логов — замени на свои реализации
#ifndef LOGE
 #define LOGE(fmt, ...) do { printf("[ERROR] " fmt "\n", ##__VA_ARGS__); } while(0)
#endif
#ifndef LOGW
 #define LOGW(fmt, ...) do { printf("[WARN] " fmt "\n", ##__VA_ARGS__); } while(0)
#endif
#ifndef LOGI
 #define LOGI(fmt, ...) do { printf("[INFO] " fmt "\n", ##__VA_ARGS__); } while(0)
#endif

// -----------------------------
// Настройки (подогнать под сервер)
// -----------------------------
static constexpr int MAX_NETID_DEFAULT   = 1000000; // твой старый предел
static constexpr int MAX_MODELID_DEFAULT = 5000;    // твой старый предел
static constexpr uint32_t MAX_BITSTREAM_BYTES = (1u << 20); // 1 MB

// -----------------------------
// Валидации
// -----------------------------
static bool IsValidBitStream(RakNet::BitStream* bs) {
    if (!bs) return false;
    uint32_t used = bs->GetNumberOfBytesUsed();
    if (used == 0 || used > MAX_BITSTREAM_BYTES) return false;
    return true;
}

static inline bool IsFiniteFloat(float v) {
    // std::isfinite проверяет NaN/Inf
    // Дополнительно можно ограничить диапазон при желании
    return std::isfinite(v);
}

static inline bool IsValidNetId(int netId, int maxNetId = MAX_NETID_DEFAULT) {
    return netId > 0 && netId <= maxNetId;
}

static inline bool IsValidModelId(int modelId, int maxModelId = MAX_MODELID_DEFAULT) {
    return modelId > 0 && modelId <= maxModelId;
}

// -----------------------------
// Основной обработчик (прост и безопасен)
// -----------------------------
void HandleCreateObjectRPC(int netId, int modelId, float x, float y, float z)
{
    // быстрые проверки id
    if (!IsValidNetId(netId)) {
        LOGE("Bad netId %d", netId);
        return;
    }

    if (!IsValidModelId(modelId)) {
        LOGE("Bad modelId %d", modelId);
        return;
    }

    // проверка координат на NaN / Inf
    if (!IsFiniteFloat(x) || !IsFiniteFloat(y) || !IsFiniteFloat(z)) {
        LOGE("Bad coords (NaN/Inf) %f %f %f", x, y, z);
        return;
    }

    // опциональная проверка world bounds (если хочешь защититься от диких чисел)
    const float WORLD_LIMIT = 20000.0f; // можно уменьшить до 18000/8000 в зависимости от движка
    if (std::fabs(x) > WORLD_LIMIT || std::fabs(y) > WORLD_LIMIT || std::fabs(z) > WORLD_LIMIT) {
        LOGE("Coords out of bounds %f %f %f", x, y, z);
        return;
    }

    // --- теперь безопасно выполнять создание объекта ---
    // Здесь должна быть твоя логика создания объекта (CObject, регистрация в ObjectPool и т.д.)
    // Пример-заглушка:
    LOGI("CreateObject accepted: netId=%d modelId=%d pos=%.3f %.3f %.3f",
         netId, modelId, x, y, z);

    // TODO: вызвать функцию создания/регистрации объекта:
    // CObject* obj = CreateObjectForModel(modelId);
    // if (!obj) { LOGE("Create failed, model load error"); return; }
    // obj->SetPosition(x,y,z);
    // g_ObjectPool.Register(netId, obj);

    // Для простоты — ничего не создаём здесь, чтобы не трогать твою архитектуру.
}

bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel );
bool hook_RakClient__Send( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel )
{
	bVehicleSyncSent = false;
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
                LOGE("JSON size is too big: %d", jsonLen);
                CNetGame::SendOnData(guiId, "", 0);
                return false;
            }

            char* buff = new char[jsonLen + 1];
            if (!bsCopy.Read(buff, jsonLen)) {
                LOGE("Failed to read JSON buffer");
                delete[] buff;
                return false;
            }
            buff[jsonLen] = '\0';


            size_t utfCap = static_cast<size_t>(jsonLen) * 3 + 1;
            char* buffGUI = new char[utfCap];
            cp1251_to_utf8(buffGUI, buff, jsonLen);
            if(guiId == 10) {
                if(nlohmann::json::accept(buffGUI)) {
                    LOGI("Parsing JSON for GUI ID 10");
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
                        LOGI("RPC_DialogResponse sent successfully");
                    } else {
                        LOGE("Failed to send RPC_DialogResponse");
                    }
                } else {
                    LOGE("Invalid JSON format for GUI ID 10: %s", buffGUI);
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
		if (bVehicleSyncSent) return false;
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
		bVehicleSyncSent = true;
		LOGI("Sending BR_ID_VEHICLE_SYNC");
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
	if(pktId == BR_ID_PASSENGER_SYNC) {
		bVehicleSyncSent = true;
		LOGI("Sending BR_ID_PASSENGER_SYNC");
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
