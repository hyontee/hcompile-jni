#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"
#include <time.h>
#include <cstdlib>
#include <cstring>

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    CNetGame::ProcessNetwork();
}

bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    return pRakClient->Connect(xorstr("188.127.241.8"), 1311, 0, 0, 5); TGK @qoprd
}

void (*orig_RakClient__RegisterAsRemoteProcedureCall)(uintptr_t thiz, int* id, void (*functionPointer)(RPCParameters* rpcParams));
void hook_RakClient__RegisterAsRemoteProcedureCall(uintptr_t thiz, int* id, void (*functionPointer)(RPCParameters* rpcParams))
{
    if (orig_RakClient__RegisterAsRemoteProcedureCall) {
        orig_RakClient__RegisterAsRemoteProcedureCall(thiz, id, functionPointer);
        return;
    }

    if (id && functionPointer) {
        pRakClient->RegisterAsRemoteProcedureCall(id, functionPointer);
    }
}

bool (*orig_RakClient__RPC)(uintptr_t thiz, int* uniqueID, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream* replyFromTarget);
bool hook_RakClient__RPC(uintptr_t thiz, int* uniqueID, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream* replyFromTarget)
{
    if (!uniqueID) {
        return false;
    }

    int sampRpcId = ConvertBRIDToSampID(static_cast<BRRpcIds>(*uniqueID));

    if (sampRpcId != -1) {
        if (sampRpcId == RPC_RequestClass && g_bInitGameProcess) {
            return false;
        }

        return pRakClient->RPC(&sampRpcId, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
    }

    // Forward unmapped RPCs through our connected client as-is.
    return pRakClient->RPC(uniqueID, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
}

static uint8_t g_LastTurnLightState = 0;
static uint64_t g_LastBlinkTime = 0;
static bool g_bLightsOn = false;

uint64_t GetTickCountMS()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

void ProcessTurnSignals(uint64_t veh, BRInCarSyncData& data)
{
    if (!veh) return;

    uint8_t currentStatus = *(uint8_t*)(veh + 0x36C);
    uint64_t currentTime = GetTickCountMS();

    if (currentStatus != 0 && g_LastTurnLightState == 0) {
        g_LastTurnLightState = currentStatus;
        g_LastBlinkTime = currentTime;
        g_bLightsOn = true;
    }

    if (g_LastTurnLightState != 0) {
        if (currentTime - g_LastBlinkTime > 500) {
            g_bLightsOn = !g_bLightsOn;
            g_LastBlinkTime = currentTime;
            *(uint8_t*)(veh + 0x36C) = g_bLightsOn ? g_LastTurnLightState : 0;
        }
    }
}

bool (*orig_RakClient__Send)(uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel);
static bool RemapAndForwardRawPacket(RakNet::BitStream* source, bool hasTimestamp, uint8_t targetId, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
    if (!source || !pRakClient) {
        return false;
    }

    const unsigned int bitsUsed = source->GetNumberOfBitsUsed();
    const unsigned char* sourceData = source->GetData();
    if (!sourceData || bitsUsed < 8) {
        return false;
    }

    if (!hasTimestamp) {
        if (sourceData[0] == targetId) {
            return pRakClient->Send(source, priority, reliability, orderingChannel);
        }

        RakNet::BitStream bsSend;
        bsSend.WriteBits(&targetId, 8);
        if (bitsUsed > 8) {
            bsSend.WriteBits(sourceData + 1, bitsUsed - 8);
        }
        return pRakClient->Send(&bsSend, priority, reliability, orderingChannel);
    }

    const unsigned int timestampBits = 8 + static_cast<unsigned int>(sizeof(RakNetTime) * 8) + 8;
    if (bitsUsed < timestampBits) {
        return false;
    }

    const size_t pktIdOffset = 1 + sizeof(RakNetTime);
    if (sourceData[pktIdOffset] == targetId) {
        return pRakClient->Send(source, priority, reliability, orderingChannel);
    }

    RakNetTime timestamp = 0;
    memcpy(&timestamp, sourceData + 1, sizeof(RakNetTime));

    RakNet::BitStream bsSend;
    uint8_t timestampId = ID_TIMESTAMP;
    bsSend.Write(timestampId);
    bsSend.Write(timestamp);
    bsSend.WriteBits(&targetId, 8);
    if (bitsUsed > timestampBits) {
        bsSend.WriteBits(sourceData + pktIdOffset + 1, bitsUsed - timestampBits);
    }
    return pRakClient->Send(&bsSend, priority, reliability, orderingChannel);
}

bool hook_RakClient__Send(uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
    if (!bitStream || bitStream->GetNumberOfBytesUsed() == 0) {
        return false;
    }

    RakNet::BitStream bsCopy(bitStream->GetData(), bitStream->GetNumberOfBytesUsed() + 1, false);

    uint8_t outerPktId = 0;
    if (!bsCopy.Read(outerPktId)) {
        return false;
    }

    bool hasTimestamp = false;
    RakNetTime packetTimestamp = 0;
    uint8_t pktId = outerPktId;

    if (outerPktId == ID_TIMESTAMP) {
        hasTimestamp = true;
        if (!bsCopy.Read(packetTimestamp)) {
            return false;
        }
        if (!bsCopy.Read(pktId)) {
            return false;
        }
    }

    auto writeSyncHeader = [&](RakNet::BitStream& bsSend, uint8_t mappedPktId) {
        if (hasTimestamp) {
            uint8_t timestampId = ID_TIMESTAMP;
            bsSend.Write(timestampId);
            bsSend.Write(packetTimestamp);
        }
        bsSend.WriteBits(&mappedPktId, 8);
    };

    if (pktId == BR_ID_USER_INTERFACE_SYNC || pktId == 252) {
        uint16_t localGuiId = 0;
        uint32_t localJsonLen = 0;
        if (!bsCopy.Read(localGuiId) || !bsCopy.Read(localJsonLen)) {
            return false;
        }

        auto sendDialogResponse = [&](uint8_t btn, int16_t listInput, const std::string& input) -> bool {
            uint8_t inputLen = static_cast<uint8_t>(input.size() > 255 ? 255 : input.size());

            RakNet::BitStream bsSend;
            bsSend.WriteBits((unsigned char*)&CNetGame::m_nLastSAMPDialogID, 16);
            bsSend.WriteBits((unsigned char*)&btn, 8);
            bsSend.WriteBits((unsigned char*)&listInput, 16);
            bsSend.WriteBits((unsigned char*)&inputLen, 8);
            bsSend.Write(input.c_str(), inputLen);

            bool result = pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
            if (result) {
                CNetGame::SetDialogActive(false);
            }
            return result;
        };

        if (localJsonLen == 0) {
            if (localGuiId == 10) {
                return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
            }
            CNetGame::SendJsonData(localGuiId, "", 0);
            return true;
        }

        const uint32_t kMaxJsonSize = 1u << 20;
        if (localJsonLen > kMaxJsonSize) {
            CNetGame::SendJsonData(localGuiId, "", 0);
            return true;
        }

        char* buff = new char[localJsonLen + 1];
        if (!bsCopy.Read(buff, localJsonLen)) {
            delete[] buff;
            return false;
        }
        buff[localJsonLen] = '\0';

        if (localGuiId == 10) {
            bool handled = false;
            const size_t utfCap = static_cast<size_t>(localJsonLen) * 3 + 1;
            char* buffGUI = new char[utfCap];
            cp1251_to_utf8(buffGUI, buff, localJsonLen);

            if (nlohmann::json::accept(buffGUI)) {
                nlohmann::json jsonObj = nlohmann::json::parse(buffGUI, nullptr, false);
                if (!jsonObj.is_discarded() && jsonObj.is_object()) {
                    const uint8_t btn = static_cast<uint8_t>(jsonObj.value("r", 0));
                    const int16_t listInput = static_cast<int16_t>(jsonObj.value("l", -1));
                    std::string input = jsonObj.value("i", std::string());
                    handled = sendDialogResponse(btn, listInput, input);
                }
            }

            delete[] buffGUI;
            delete[] buff;

            if (handled) {
                return true;
            }
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        CNetGame::SendJsonData(localGuiId, buff, localJsonLen);
        delete[] buff;

        return true;
    }

    if (pktId == BR_ID_AIM_SYNC) {
        return RemapAndForwardRawPacket(bitStream, hasTimestamp, ID_AIM_SYNC, priority, reliability, orderingChannel);
    }

    if (pktId == BR_ID_BULLET_SYNC) {
        return RemapAndForwardRawPacket(bitStream, hasTimestamp, ID_BULLET_SYNC, priority, reliability, orderingChannel);
    }

    if (pktId == BR_ID_PLAYER_SYNC) {
        return RemapAndForwardRawPacket(bitStream, hasTimestamp, ID_PLAYER_SYNC, priority, reliability, orderingChannel);
    }

    if (pktId == BR_ID_VEHICLE_SYNC) {
        return RemapAndForwardRawPacket(bitStream, hasTimestamp, ID_VEHICLE_SYNC, priority, reliability, orderingChannel);
    }

    if (pktId == BR_ID_PASSENGER_SYNC) {
        return RemapAndForwardRawPacket(bitStream, hasTimestamp, ID_PASSENGER_SYNC, priority, reliability, orderingChannel);
    }

    return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
}

void (*orig_CGame__TickCrashPoint)(uintptr_t thiz) = nullptr;
void hook_CGame__TickCrashPoint(uintptr_t thiz)
{
    if (!orig_CGame__TickCrashPoint) {
        return;
    }

    // Guard crash at libblackrussia-client.so+0x245e18:
    // ldrb w10, [x8, #1248] with x8 loaded from [thiz].
    if (thiz < 0x10000) {
        return;
    }

    uintptr_t inner = *(uintptr_t*)thiz;
    if (inner < 0x10000) {
        return;
    }

    orig_CGame__TickCrashPoint(thiz);
}

void (*orig_CGame__LocalStateTick)(uintptr_t thiz) = nullptr;
void hook_CGame__LocalStateTick(uintptr_t thiz)
{
    if (!orig_CGame__LocalStateTick) {
        return;
    }

    if (thiz < 0x10000) {
        return;
    }

    uintptr_t base = CGameAPI::GetBase();
    if (!base) {
        return;
    }

    // libblackrussia-client.so globals used by routine around 0x2040f4.
    uint16_t* localPlayerId = *(uint16_t**)(base + 0x4C69F0);
    uintptr_t playerArrayBase = *(uintptr_t*)(base + 0x4C69F8);
    if (!localPlayerId || playerArrayBase < 0x10000) {
        return;
    }

    uintptr_t playerEntry = *(uintptr_t*)(playerArrayBase + (static_cast<uintptr_t>(*localPlayerId) * 424));
    if (playerEntry < 0x10000) {
        return;
    }

    orig_CGame__LocalStateTick(thiz);
}
