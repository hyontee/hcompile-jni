//слил мемари крутой ок @WhereMyMemory

#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"
#include "plugin/features/br_features.h"
#include "plugin/features/br_sync_state.h"

#include <cstring>
#include <exception>
#include <cctype>
#include <climits>
#include <cstdlib>
#include <string>
#include <vector>

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    CNetGame::ProcessNetwork();
}

int64_t (*orig_CChat__AddDebugMessage)(const char* format, ...);
int64_t hook_CChat__AddDebugMessage(const char* format, ...)
{
    if(!orig_CChat__AddDebugMessage) {
        return 0;
    }
    if(!format) {
        return orig_CChat__AddDebugMessage("%s", "");
    }

    char formatted[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(formatted, sizeof(formatted), format, args);
    va_end(args);

    return orig_CChat__AddDebugMessage("%s", formatted);
}

bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    if (!pRakClient || !host || host[0] == '\0' || serverPort == 0)
        return false;
    return pRakClient->Connect(host, serverPort, clientPort, depreciated, threadSleepTimer);
}

bool (*orig_RakClient__RPC)( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget );
bool hook_RakClient__RPC( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )
{
    if (!pRakClient)
        return false;

    int sampRpcId = ConvertBRIDToSampID(uniqueID);
    if (sampRpcId == -1)
        return false;

    if (sampRpcId == RPC_RequestClass && g_bInitGameProcess)
        return false;

    return pRakClient->RPC(&sampRpcId, bitStream, priority,
                           ConvertBRToSampReliability(reliability), orderingChannel,
                           shiftTimestamp, networkID, replyFromTarget);
}

namespace
{
    constexpr uint32_t kMaxGuiJsonSize = 1024u * 1024u;
    constexpr uint32_t kMaxDialogInputSize = 255u;
    constexpr uint32_t kPlayerSyncPayloadBits = 560u;
    constexpr uint32_t kVehicleSyncPayloadBits = 488u;
    constexpr uint32_t kPassengerSyncPayloadBits = 192u;
    constexpr uint32_t kTrailerSyncPayloadBits = 54u * 8u;
    constexpr uint32_t kUnoccupiedSyncPayloadBits = 67u * 8u;
    constexpr uint32_t kSpectatorSyncPayloadBits = 18u * 8u;
    constexpr uint32_t kStatsSyncPayloadBits = 8u * 8u;
    static_assert(sizeof(BRPassengerSyncData) == 24u, "Unexpected BRPassengerSyncData layout");
    constexpr uint32_t kAimSyncPayloadBits = 248u;
    constexpr uint32_t kBulletSyncPayloadBits = 320u;

    bool ReadBitsExact(RakNet::BitStream& bs, unsigned char* dst, size_t bits)
    {
        return dst && bs.GetNumberOfUnreadBits() >= static_cast<int>(bits) &&
               bs.ReadBits(dst, static_cast<unsigned int>(bits), true);
    }

    template <typename T>
    bool ReadBitsExact(RakNet::BitStream& bs, T& value, size_t bits)
    {
        return ReadBitsExact(bs, reinterpret_cast<unsigned char*>(&value), bits);
    }

    bool ReadRawBytes(RakNet::BitStream& bs, void* dst, size_t bytes)
    {
        return dst && bs.GetNumberOfUnreadBits() >= static_cast<int>(bytes * 8u) &&
               bs.Read(static_cast<char*>(dst), static_cast<unsigned int>(bytes));
    }


    bool SkipWhitespace(const std::string& text, size_t& pos)
    {
        while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos;
        return pos < text.size();
    }

    bool FindJsonKey(const std::string& text, const char* key, size_t& valuePos)
    {
        if (!key) return false;
        const std::string token = std::string("\"") + key + "\"";
        size_t pos = 0;
        while ((pos = text.find(token, pos)) != std::string::npos)
        {
            size_t cursor = pos + token.size();
            SkipWhitespace(text, cursor);
            if (cursor < text.size() && text[cursor] == ':')
            {
                ++cursor;
                if (SkipWhitespace(text, cursor)) { valuePos = cursor; return true; }
            }
            pos += token.size();
        }
        return false;
    }

    bool ParseJsonInt(const std::string& text, const char* key, int& out)
    {
        size_t pos = 0;
        if (!FindJsonKey(text, key, pos)) return false;
        const char* begin = text.c_str() + pos;
        char* end = nullptr;
        const long value = std::strtol(begin, &end, 10);
        if (end == begin || value < INT_MIN || value > INT_MAX) return false;
        while (*end && std::isspace(static_cast<unsigned char>(*end))) ++end;
        if (*end == ',' || *end == '}') { out = static_cast<int>(value); return true; }
        return false;
    }

    bool ParseJsonString(const std::string& text, const char* key, std::string& out)
    {
        size_t pos = 0;
        if (!FindJsonKey(text, key, pos) || pos >= text.size() || text[pos] != '"') return false;
        ++pos; out.clear();
        while (pos < text.size())
        {
            const char c = text[pos++];
            if (c == '"') return true;
            if (c != '\\') { out.push_back(c); continue; }
            if (pos >= text.size()) return false;
            const char escaped = text[pos++];
            switch (escaped)
            {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                default: return false;
            }
        }
        return false;
    }

    bool ParseGui10Response(const std::string& text, int& button, int& listValue, std::string& input)
    {
        return ParseJsonInt(text, "r", button) && ParseJsonInt(text, "l", listValue) && ParseJsonString(text, "i", input);
    }

    bool SendConvertedPacket(uint8_t packetId, RakNet::BitStream& payload,
                             PacketPriority priority = HIGH_PRIORITY,
                             PacketReliability reliability = UNRELIABLE_SEQUENCED)
    {
        if (!pRakClient)
            return false;

        RakNet::BitStream packet;
        packet.WriteBits(&packetId, 8);
        const unsigned int payloadBits = static_cast<unsigned int>(payload.GetNumberOfBitsUsed());
        if (payloadBits > 0)
            packet.WriteBits(payload.GetData(), payloadBits, false);
        return pRakClient->Send(&packet, priority, reliability, 0);
    }
}

bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel );
bool hook_RakClient__Send( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel )
{
    if (!pRakClient || !bitStream || !bitStream->GetData())
        return false;

    const unsigned int bytesUsed = bitStream->GetNumberOfBytesUsed();
    if (bytesUsed == 0 || bitStream->GetNumberOfUnreadBits() < 8)
        return false;

    RakNet::BitStream bsCopy(bitStream->GetData(), bytesUsed, false);
    uint8_t pktId = 0;
    if (!bsCopy.Read(pktId))
        return false;

    if (pktId == BR_ID_USER_INTERFACE_SYNC)
    {
        uint16_t guiId = 0;
        uint32_t jsonLen = 0;
        if (bsCopy.GetNumberOfUnreadBits() < 48 || !bsCopy.Read(guiId) || !bsCopy.Read(jsonLen))
            return false;
        if (jsonLen > kMaxGuiJsonSize || bsCopy.GetNumberOfUnreadBits() < static_cast<int>(jsonLen * 8ull))
            return false;

        std::string rawJson(jsonLen, '\0');
        if (jsonLen > 0 && !bsCopy.Read(rawJson.data(), jsonLen))
            return false;

        std::string utf8Json(static_cast<size_t>(jsonLen) * 3u + 1u, '\0');
        cp1251_to_utf8(utf8Json.data(), rawJson.data(), jsonLen);
        utf8Json.resize(std::strlen(utf8Json.c_str()));

        if (guiId == 10)
        {
            int buttonValue = 0;
            int listValue = 0;
            std::string input;
            if (!ParseGui10Response(utf8Json, buttonValue, listValue, input)) return false;
            if (buttonValue < 0 || buttonValue > 255 || listValue < -32768 || listValue > 32767) return false;
            if (input.size() > kMaxDialogInputSize) input.resize(kMaxDialogInputSize);

            const uint8_t button = static_cast<uint8_t>(buttonValue);
            const int16_t listInput = static_cast<int16_t>(listValue);
            const uint8_t inputLen = static_cast<uint8_t>(input.size());

            RakNet::BitStream bsSend;
            bsSend.WriteBits(reinterpret_cast<unsigned char*>(&CNetGame::m_nLastSAMPDialogID), 16);
            bsSend.WriteBits(reinterpret_cast<const unsigned char*>(&button), 8);
            bsSend.WriteBits(reinterpret_cast<const unsigned char*>(&listInput), 16);
            bsSend.WriteBits(reinterpret_cast<const unsigned char*>(&inputLen), 8);
            if (inputLen > 0) bsSend.Write(input.data(), inputLen);

            return pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                   false, UNASSIGNED_NETWORK_ID, nullptr);
        }

        CNetGame::SendJsonData(guiId, utf8Json.data(), static_cast<uint32_t>(utf8Json.size()));
        return false;
    }

    if (pktId == BR_ID_AIM_SYNC)
    {
        uint8_t payload[31] = {};
        if (!ReadBitsExact(bsCopy, payload, kAimSyncPayloadBits))
            return false;
        RakNet::BitStream payloadBs;
        payloadBs.WriteBits(payload, kAimSyncPayloadBits);
        return SendConvertedPacket(ID_AIM_SYNC, payloadBs);
    }

    if (pktId == BR_ID_BULLET_SYNC)
    {
        uint8_t payload[40] = {};
        if (!ReadBitsExact(bsCopy, payload, kBulletSyncPayloadBits))
            return false;
        RakNet::BitStream payloadBs;
        payloadBs.WriteBits(payload, kBulletSyncPayloadBits);
        return SendConvertedPacket(ID_BULLET_SYNC, payloadBs);
    }

    if (pktId == BR_ID_PLAYER_SYNC)
    {
        BROnFootSyncData data = {};
        if (!ReadBitsExact(bsCopy, data.lrAnalogLeftStick, 16) ||
            !ReadBitsExact(bsCopy, data.udAnalogLeftStick, 16) ||
            !ReadBitsExact(bsCopy, data.wKeys, 16) ||
            !ReadBitsExact(bsCopy, reinterpret_cast<unsigned char*>(&data.vecPos), 96) ||
            !ReadBitsExact(bsCopy, data.quatw, 32) ||
            !ReadBitsExact(bsCopy, data.quatx, 32) ||
            !ReadBitsExact(bsCopy, data.quaty, 32) ||
            !ReadBitsExact(bsCopy, data.quatz, 32) ||
            !ReadBitsExact(bsCopy, data.health, 16) ||
            !ReadBitsExact(bsCopy, data.armour, 16) ||
            !ReadBitsExact(bsCopy, data.byteCurrentWeapon, 8) ||
            !ReadBitsExact(bsCopy, data.byteSpecialAction, 8) ||
            !ReadBitsExact(bsCopy, reinterpret_cast<unsigned char*>(&data.vecMoveSpeed), 96) ||
            !ReadBitsExact(bsCopy, reinterpret_cast<unsigned char*>(&data.vecSurfOffsets), 96) ||
            !ReadBitsExact(bsCopy, data.wSurfInfo, 16) ||
            !ReadBitsExact(bsCopy, data.dwAnimation, 32))
            return false;

        br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
        if (!syncRegistry.NormalizeOnFoot(data))
            return false;
        RakNet::BitStream payloadBs;
        ConvertBROnFootSyncToSampSync(&payloadBs, data);
        return SendConvertedPacket(ID_PLAYER_SYNC, payloadBs);
    }

    if (pktId == BR_ID_VEHICLE_SYNC)
    {
        BRInCarSyncData data = {};
        if (!ReadBitsExact(bsCopy, data.VehicleID, 16) ||
            !ReadBitsExact(bsCopy, data.lrAnalogLeftStick, 16) ||
            !ReadBitsExact(bsCopy, data.udAnalogLeftStick, 16) ||
            !ReadBitsExact(bsCopy, data.wKeys, 16) ||
            !ReadBitsExact(bsCopy, data.quatw, 32) ||
            !ReadBitsExact(bsCopy, data.quatx, 32) ||
            !ReadBitsExact(bsCopy, data.quaty, 32) ||
            !ReadBitsExact(bsCopy, data.quatz, 32) ||
            !ReadBitsExact(bsCopy, reinterpret_cast<unsigned char*>(&data.vecPos), 96) ||
            !ReadBitsExact(bsCopy, reinterpret_cast<unsigned char*>(&data.vecMoveSpeed), 96) ||
            !ReadBitsExact(bsCopy, data.fCarHealth, 32) ||
            !ReadBitsExact(bsCopy, data.playerHealth, 16) ||
            !ReadBitsExact(bsCopy, data.playerArmour, 16) ||
            !ReadBitsExact(bsCopy, data.byteCurrentWeapon, 8) ||
            !ReadBitsExact(bsCopy, data.byteSirenOn, 8) ||
            !ReadBitsExact(bsCopy, data.byteLandingGearState, 8) ||
            !ReadBitsExact(bsCopy, data.TrailerID, 16))
            return false;

        br::FeatureState::Instance().SetVehicleHealth(data.VehicleID, data.fCarHealth);

        br::SyncRegistry& syncRegistry = br::SyncRegistry::Instance();
        if (!syncRegistry.NormalizeInCar(data))
            return false;
        RakNet::BitStream payloadBs;
        ConvertBRInCarSyncToSampSync(&payloadBs, data);
        return SendConvertedPacket(ID_VEHICLE_SYNC, payloadBs);
    }

    if (pktId == BR_ID_PASSENGER_SYNC)
    {
        uint8_t payload[24] = {};
        if (!ReadBitsExact(bsCopy, payload, kPassengerSyncPayloadBits))
            return false;
        RakNet::BitStream payloadBs;
        payloadBs.WriteBits(payload, kPassengerSyncPayloadBits);
        return SendConvertedPacket(ID_PASSENGER_SYNC, payloadBs);
    }

    if (pktId == BR_ID_TURNLIGHTS_SYNC)
    {
        BRTurnSignalData data = {};
        if (!ReadRawBytes(bsCopy, &data, sizeof(data)))
            return false;
        RakNet::BitStream payloadBs;
        ConvertBRTurnSignalToSampSync(&payloadBs, data);
        return SendConvertedPacket(BR_ID_TURNLIGHTS_SYNC, payloadBs);
    }

    if (pktId == BR_ID_TRAILER_SYNC || pktId == BR_ID_UNOCCUPIED_SYNC || pktId == BR_ID_SPECTATOR_SYNC || pktId == BR_ID_STATS_UPDATE)
    {
        const uint32_t requiredBits =
            pktId == BR_ID_TRAILER_SYNC ? kTrailerSyncPayloadBits :
            pktId == BR_ID_UNOCCUPIED_SYNC ? kUnoccupiedSyncPayloadBits :
            pktId == BR_ID_SPECTATOR_SYNC ? kSpectatorSyncPayloadBits : kStatsSyncPayloadBits;
        if (bsCopy.GetNumberOfUnreadBits() < static_cast<int>(requiredBits))
            return false;
        uint8_t payload[67] = {};
        const size_t payloadBytes = requiredBits / 8u;
        if (!ReadRawBytes(bsCopy, payload, payloadBytes))
            return false;
        RakNet::BitStream payloadBs;
        payloadBs.Write(reinterpret_cast<const char*>(payload), static_cast<int>(payloadBytes));
        const uint8_t sampId =
            pktId == BR_ID_TRAILER_SYNC ? static_cast<uint8_t>(ID_TRAILER_SYNC) :
            pktId == BR_ID_UNOCCUPIED_SYNC ? static_cast<uint8_t>(ID_UNOCCUPIED_SYNC) :
            pktId == BR_ID_SPECTATOR_SYNC ? static_cast<uint8_t>(ID_SPECTATOR_SYNC) : static_cast<uint8_t>(ID_STATS_UPDATE);
        return SendConvertedPacket(sampId, payloadBs);
    }

    if (pktId == BR_ID_WEAPONS_UPDATE)
    {
        const int unreadBits = bsCopy.GetNumberOfUnreadBits();
        if (unreadBits < 32 || ((unreadBits - 32) % 32) != 0)
            return false;
        const size_t payloadBytes = static_cast<size_t>(unreadBits / 8);
        std::vector<uint8_t> payload(payloadBytes);
        if (!ReadRawBytes(bsCopy, payload.data(), payload.size()))
            return false;
        RakNet::BitStream payloadBs;
        payloadBs.Write(reinterpret_cast<const char*>(payload.data()), static_cast<int>(payload.size()));
        return SendConvertedPacket(ID_WEAPONS_UPDATE, payloadBs);
    }

    // Non-BR packets must keep working. The old hook returned false here and
    // silently swallowed unrelated packets (including ordinary weapon/stats).
    return orig_RakClient__Send ? orig_RakClient__Send(thiz, bitStream, priority, reliability, orderingChannel) : false;
}

