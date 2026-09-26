#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

static constexpr uintptr_t kRemoteStreamEntryTableOffset = 0x4788E20;
static constexpr uintptr_t kRemoteStreamCurrentIndexOffset = 0x4788E1C;
static constexpr size_t kRemoteStreamEntrySize = 0x1A8;
static constexpr uint16_t kRemoteStreamMaxEntries = 1004;

static bool IsMostlyText(const unsigned char* data, unsigned int len)
{
    if (!data || !len) {
        return false;
    }

    unsigned int printable = 0;
    for (unsigned int i = 0; i < len; ++i) {
        unsigned char c = data[i];
        if (c == 0) {
            return i > 0;
        }
        if (c == '\r' || c == '\n' || c == '\t' || (c >= 0x20 && c < 0x7F) || c >= 0xC0) {
            ++printable;
        }
    }
    return printable >= len - 1;
}

static bool IsLikelyChatText(const unsigned char* text, uint8_t len)
{
    if (!text || len < 2 || len > 144) {
        return false;
    }

    const unsigned char first = text[0];
    const bool validStart =
        (first == '/' || first == '!' || (first >= '0' && first <= '9') ||
         (first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z') ||
         first == 0xA8 || first == 0xB8 || first >= 0xC0);
    if (!validStart) {
        return false;
    }

    for (uint8_t i = 0; i < len; ++i) {
        const unsigned char c = text[i];
        if (c == 0) {
            return false;
        }
        if (!(c == '\r' || c == '\n' || c == '\t' || (c >= 0x20 && c < 0x7F) || c == 0xA8 || c == 0xB8 || c >= 0xC0)) {
            return false;
        }
    }
    return true;
}

static bool ExtractPrefixedText(const unsigned char* data, unsigned int dataBytes, const char** textPtr, uint8_t* textLen)
{
    if (!data || dataBytes < 2 || !textPtr || !textLen) {
        return false;
    }

    const uint8_t prefixedLen = data[0];
    const bool exact = (prefixedLen != 0 && prefixedLen == dataBytes - 1);
    const bool withTerminator = (prefixedLen != 0 && dataBytes > 2 && prefixedLen == dataBytes - 2 && data[dataBytes - 1] == 0);
    if (!exact && !withTerminator) {
        return false;
    }

    if (!IsMostlyText(data + 1, prefixedLen) || !IsLikelyChatText(data + 1, prefixedLen)) {
        return false;
    }

    *textPtr = reinterpret_cast<const char*>(data + 1);
    *textLen = prefixedLen;
    return true;
}

static int ResolveRpcId(int* uniqueID)
{
    if (!uniqueID) {
        return -1;
    }

    const uintptr_t raw = reinterpret_cast<uintptr_t>(uniqueID);
    if (raw <= 0xFFFF) {
        return static_cast<int>(raw);
    }

    return *uniqueID;
}

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    CNetGame::ProcessNetwork();
}

void (*orig_CRemoteStreamEntry__Tick)(void* entry);
void hook_CRemoteStreamEntry__Tick(void* entry)
{
    if (!entry) {
        return;
    }

    auto* rawEntry = static_cast<unsigned char*>(entry);
    const uintptr_t renderObject = *reinterpret_cast<uintptr_t*>(rawEntry);
    const uintptr_t renderObjectStable = *reinterpret_cast<uintptr_t*>(rawEntry);
    if (renderObject < 0x10000 || renderObjectStable < 0x10000 || renderObject != renderObjectStable) {
        rawEntry[228] = 0;
        rawEntry[232] = 0;
        return;
    }

    if (orig_CRemoteStreamEntry__Tick) {
        orig_CRemoteStreamEntry__Tick(entry);
    }
}

void (*orig_CRemoteStreamUpdate)(void* context);
void hook_CRemoteStreamUpdate(void* context)
{
    if (!context) {
        return;
    }

    auto* currentIndex = reinterpret_cast<uint16_t*>(CGameAPI::m_address + kRemoteStreamCurrentIndexOffset);
    auto* table = reinterpret_cast<unsigned char*>(CGameAPI::m_address + kRemoteStreamEntryTableOffset);
    if (!currentIndex || !table) {
        if (orig_CRemoteStreamUpdate) {
            orig_CRemoteStreamUpdate(context);
        }
        return;
    }

    const uint16_t index = *currentIndex;
    if (index >= kRemoteStreamMaxEntries) {
        return;
    }

    auto* entry = table + (static_cast<size_t>(index) * kRemoteStreamEntrySize);
    const uintptr_t renderObject = *reinterpret_cast<uintptr_t*>(entry);
    if (renderObject < 0x10000) {
        // Prevent native null-deref at client offset 0x2040F4 while stream table is racing.
        *reinterpret_cast<uintptr_t*>(static_cast<unsigned char*>(context) + 2216) = 0;
        return;
    }

    if (orig_CRemoteStreamUpdate) {
        orig_CRemoteStreamUpdate(context);
    }
}

void (*orig_CRemotePlayerStreamLoop)();
void hook_CRemotePlayerStreamLoop()
{
    auto* currentIndex = reinterpret_cast<uint16_t*>(CGameAPI::m_address + kRemoteStreamCurrentIndexOffset);
    auto* table = reinterpret_cast<unsigned char*>(CGameAPI::m_address + kRemoteStreamEntryTableOffset);
    if (!currentIndex || !table) {
        if (orig_CRemotePlayerStreamLoop) {
            orig_CRemotePlayerStreamLoop();
        }
        return;
    }

    const uint16_t index = *currentIndex;
    auto* entry = table + (static_cast<size_t>(index) * kRemoteStreamEntrySize);
    const uintptr_t renderObject = *reinterpret_cast<uintptr_t*>(entry);
    if (renderObject < 0x10000) {
        entry[0xE4] = 0;
        entry[0xE8] = 0;
        return;
    }

    if (orig_CRemotePlayerStreamLoop) {
        orig_CRemotePlayerStreamLoop();
    }
}

int (*orig_CRemoteStreamClassify)(void* entity, void* target, void* playerIdPtr, void* extra);
int hook_CRemoteStreamClassify(void* entity, void* target, void* playerIdPtr, void* extra)
{
    (void)extra;
    // Keep this guard minimal: the last attempt crashed on startup because the
    // function is called from more places than just remote-player stream-in.
    // Only bail out on obviously invalid arguments and let the native code
    // handle everything else.
    if (!entity || !target || !playerIdPtr) {
        return 0;
    }

    if (orig_CRemoteStreamClassify) {
        return orig_CRemoteStreamClassify(entity, target, playerIdPtr, extra);
    }

    return 0;
}

int (*orig_CRemoteEntityRelationCheck)(void* entity, void* targetA, void* targetB, uint16_t targetId);
int hook_CRemoteEntityRelationCheck(void* entity, void* targetA, void* targetB, uint16_t targetId)
{
    if (!entity || !targetA || !targetB) {
        return 0;
    }

    auto rawEntity = reinterpret_cast<uintptr_t>(entity);
    auto rawTargetA = reinterpret_cast<uintptr_t>(targetA);
    auto rawTargetB = reinterpret_cast<uintptr_t>(targetB);
    if (rawEntity < 0x10000 || rawTargetA < 0x10000 || rawTargetB < 0x10000) {
        return 0;
    }

    if (orig_CRemoteEntityRelationCheck) {
        return orig_CRemoteEntityRelationCheck(entity, targetA, targetB, targetId);
    }

    return 0;
}

bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    if (!pRakClient) {
        return false;
    }
    return pRakClient->Connect(xorstr("188.127.241.74"), 1438, 0, 0, 5);
}

bool (*orig_RakClient__RPC)( uintptr_t thiz, int* uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget );
bool hook_RakClient__RPC( uintptr_t thiz, int* uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )
{
    if (!pRakClient) {
        return false;
    }

    const int rpcId = ResolveRpcId(uniqueID);
    const int sampRpcId = ConvertBRIDToSampID(static_cast<BRRpcIds>(rpcId));
    const unsigned char* rpcData = bitStream ? bitStream->GetData() : nullptr;
    const unsigned int rpcBytes = bitStream ? bitStream->GetNumberOfBytesUsed() : 0;
    const char* textPtr = nullptr;
    uint8_t textLen = 0;
    const bool hasPrefixedText = ExtractPrefixedText(rpcData, rpcBytes, &textPtr, &textLen);

    if (sampRpcId != -1) {
        if (sampRpcId == RPC_RequestClass && g_bInitGameProcess) {
            return false;
        }

        if (sampRpcId == RPC_Chat && hasPrefixedText && textLen > 1 && textPtr[0] == '/') {
            int rpcMapped = sampRpcId;
            bool sent = pRakClient->RPC(&rpcMapped, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID, replyFromTarget);

            RakNet::BitStream bsCommand;
            bsCommand.Write(textLen);
            bsCommand.Write(textPtr, textLen);
            int rpcServerCommand = RPC_ServerCommand;
            sent = pRakClient->RPC(&rpcServerCommand, &bsCommand, priority, reliability,
                                   orderingChannel, shiftTimestamp, networkID, replyFromTarget) || sent;

            RakNet::BitStream bsCommandNoSlash;
            const uint8_t commandLenNoSlash = static_cast<uint8_t>(textLen - 1);
            bsCommandNoSlash.Write(commandLenNoSlash);
            bsCommandNoSlash.Write(textPtr + 1, commandLenNoSlash);
            sent = pRakClient->RPC(&rpcServerCommand, &bsCommandNoSlash, priority, reliability,
                                   orderingChannel, shiftTimestamp, networkID, replyFromTarget) || sent;
            return sent;
        }

        if (sampRpcId == RPC_ServerCommand && hasPrefixedText && textLen > 1 && textPtr[0] == '/') {
            int rpcMapped = sampRpcId;
            bool sent = pRakClient->RPC(&rpcMapped, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID, replyFromTarget);

            RakNet::BitStream bsCommandNoSlash;
            const uint8_t commandLenNoSlash = static_cast<uint8_t>(textLen - 1);
            bsCommandNoSlash.Write(commandLenNoSlash);
            bsCommandNoSlash.Write(textPtr + 1, commandLenNoSlash);
            int rpcServerCommand = RPC_ServerCommand;
            sent = pRakClient->RPC(&rpcServerCommand, &bsCommandNoSlash, priority, reliability,
                                   orderingChannel, shiftTimestamp, networkID, replyFromTarget) || sent;
            return sent;
        }

        int rpcMapped = sampRpcId;
        return pRakClient->RPC(&rpcMapped, bitStream, priority, reliability,
                               orderingChannel, shiftTimestamp, networkID, replyFromTarget);
    }

    if (hasPrefixedText && textLen > 1) {
        if (textPtr[0] == '/') {
            bool sent = pRakClient->RPC(uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID, replyFromTarget);

            RakNet::BitStream bsCommand;
            bsCommand.Write(textLen);
            bsCommand.Write(textPtr, textLen);
            int rpcServerCommand = RPC_ServerCommand;
            sent = pRakClient->RPC(&rpcServerCommand, &bsCommand, HIGH_PRIORITY, RELIABLE_ORDERED,
                                   orderingChannel, shiftTimestamp, networkID, replyFromTarget) || sent;

            RakNet::BitStream bsCommandNoSlash;
            const uint8_t commandLenNoSlash = static_cast<uint8_t>(textLen - 1);
            bsCommandNoSlash.Write(commandLenNoSlash);
            bsCommandNoSlash.Write(textPtr + 1, commandLenNoSlash);
            sent = pRakClient->RPC(&rpcServerCommand, &bsCommandNoSlash, HIGH_PRIORITY, RELIABLE_ORDERED,
                                   orderingChannel, shiftTimestamp, networkID, replyFromTarget) || sent;
            return sent;
        }

        RakNet::BitStream bsChat;
        bsChat.Write(textLen);
        bsChat.Write(textPtr, textLen);
        int rpcChat = RPC_Chat;
        return pRakClient->RPC(&rpcChat, &bsChat, HIGH_PRIORITY, RELIABLE_ORDERED,
                               orderingChannel, shiftTimestamp, networkID, replyFromTarget);
    }

    if (rpcData && rpcBytes >= 2 && rpcBytes <= 144 && IsLikelyChatText(rpcData, static_cast<uint8_t>(rpcBytes)) && rpcData[0] == '/') {
        bool sent = pRakClient->RPC(uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID, replyFromTarget);

        RakNet::BitStream bsCommand;
        const uint8_t commandLen = static_cast<uint8_t>(rpcBytes);
        bsCommand.Write(commandLen);
        bsCommand.Write(reinterpret_cast<const char*>(rpcData), commandLen);
        int rpcServerCommand = RPC_ServerCommand;
        sent = pRakClient->RPC(&rpcServerCommand, &bsCommand, HIGH_PRIORITY, RELIABLE_ORDERED,
                               orderingChannel, shiftTimestamp, networkID, replyFromTarget) || sent;

        RakNet::BitStream bsCommandNoSlash;
        const uint8_t commandLenNoSlash = static_cast<uint8_t>(rpcBytes - 1);
        bsCommandNoSlash.Write(commandLenNoSlash);
        bsCommandNoSlash.Write(reinterpret_cast<const char*>(rpcData + 1), commandLenNoSlash);
        sent = pRakClient->RPC(&rpcServerCommand, &bsCommandNoSlash, HIGH_PRIORITY, RELIABLE_ORDERED,
                               orderingChannel, shiftTimestamp, networkID, replyFromTarget) || sent;
        return sent;
    }

    return pRakClient->RPC(uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID, replyFromTarget);
}
bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel );
static bool ForwardSyncRawPacket(RakNet::BitStream* source, uint8_t targetId, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
    if (!pRakClient || !source) {
        return false;
    }

    const unsigned int bitsUsed = source->GetNumberOfBitsUsed();
    if (bitsUsed < 8) {
        return false;
    }

    const unsigned char* sourceData = source->GetData();
    if (!sourceData) {
        return false;
    }

    if (sourceData[0] == targetId) {
        return pRakClient->Send(source, priority, reliability, orderingChannel);
    }

    RakNet::BitStream bsSend;
    bsSend.WriteBits(&targetId, 8);
    bsSend.WriteBits(sourceData + 1, bitsUsed - 8);
    return pRakClient->Send(&bsSend, priority, reliability, orderingChannel);
}

bool hook_RakClient__Send( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel )
{
    if (!bitStream || bitStream->GetNumberOfBytesUsed() == 0 || !pRakClient) {
        return orig_RakClient__Send(thiz, bitStream, priority, reliability, orderingChannel);
    }

    RakNet::BitStream bsCopy(bitStream->GetData(), bitStream->GetNumberOfBytesUsed() + 1, false);
    uint8_t pktId = 0;
    if (!bsCopy.Read(pktId)) {
        return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
    }

    if (pktId == BR_ID_USER_INTERFACE_SYNC) {
        uint16_t guiPacketId = 0;
        uint32_t guiJsonLen = 0;
        if (!bsCopy.Read(guiPacketId) || !bsCopy.Read(guiJsonLen)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        if (guiPacketId != 10) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        const uint32_t kMaxJsonSize = 1u << 20;
        const unsigned int packetBytes = bitStream->GetNumberOfBytesUsed();
        const unsigned int headerBytes = 1u + sizeof(guiPacketId) + sizeof(guiJsonLen);
        const unsigned int remainingBytes = packetBytes > headerBytes ? packetBytes - headerBytes : 0;
        if (guiJsonLen == 0 || guiJsonLen > kMaxJsonSize || guiJsonLen > remainingBytes) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        char* buff = new char[guiJsonLen + 1];
        if (!bsCopy.Read(buff, guiJsonLen)) {
            delete[] buff;
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }
        buff[guiJsonLen] = '\0';

        bool handled = false;
        const size_t utfCap = static_cast<size_t>(guiJsonLen) * 3 + 1;
        char* buffGUI = new char[utfCap];
        cp1251_to_utf8(buffGUI, buff, guiJsonLen);

        if(nlohmann::json::accept(buffGUI)) {
            nlohmann::json jsonObj = nlohmann::json::parse(buffGUI, nullptr, false);
            if (!jsonObj.is_discarded()) {
                const uint8_t btn = static_cast<uint8_t>(jsonObj.value("r", 0));
                const int16_t listInput = static_cast<int16_t>(jsonObj.value("l", -1));
                std::string input = jsonObj.value("i", std::string());
                if (input.size() > 255) {
                    input.resize(255);
                }
                const uint8_t inputLen = static_cast<uint8_t>(input.size());

                RakNet::BitStream bsSend;
                bsSend.WriteBits((unsigned char *)&CNetGame::m_nLastSAMPDialogID, 16);
                bsSend.WriteBits((unsigned char *)&btn, 8);
                bsSend.WriteBits((unsigned char *)&listInput, 16);
                bsSend.WriteBits((unsigned char *)&inputLen, 8);
                if (inputLen > 0) {
                    bsSend.Write(input.c_str(), inputLen);
                }

                int rpcDialogResponse = RPC_DialogResponse;
                handled = pRakClient->RPC(&rpcDialogResponse, &bsSend,
                                          HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                          false, UNASSIGNED_NETWORK_ID, NULL);
            }
        }

        delete[] buffGUI;
        delete[] buff;

        // Keep BR UI packet flow alive for servers that rely on native UI sync,
        // while also sending SA-MP dialog response when we could parse JSON.
        const bool rawSent = pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        return handled || rawSent;
    }

    if (pktId == BR_ID_AIM_SYNC) {
        uint8_t pktAimSync = ID_AIM_SYNC;
        uint8_t aimSyncBuffer[31] = {0};
        if (!bsCopy.ReadBits(aimSyncBuffer, 31 * 8)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktAimSync, 8);
        bsSend.WriteBits(aimSyncBuffer, 31 * 8);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }
    if (pktId == BR_ID_BULLET_SYNC) {
        uint8_t pktBltSync = ID_BULLET_SYNC;
        uint8_t bltSyncBuffer[40] = {0};
        if (!bsCopy.ReadBits(bltSyncBuffer, 40 * 8)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktBltSync, 8);
        bsSend.WriteBits(bltSyncBuffer, 40 * 8);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }
    if (pktId == BR_ID_PLAYER_SYNC) {
        uint8_t pktPlayerSync = ID_PLAYER_SYNC;
        BROnFootSyncData data{};
        if (!bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.wKeys, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecPos, 96) ||
            !bsCopy.ReadBits((unsigned char *)&data.quatw, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.quatx, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.quaty, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.quatz, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.health, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.armour, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8) ||
            !bsCopy.ReadBits((unsigned char *)&data.byteSpecialAction, 8) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed, 96) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecSurfOffsets, 96) ||
            !bsCopy.ReadBits((unsigned char *)&data.wSurfInfo, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.dwAnimation, 32)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktPlayerSync, 8);
        ConvertBROnFootSyncToSampSync(&bsSend, data);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }
    if (pktId == BR_ID_VEHICLE_SYNC) {
        uint8_t pktVehicleSync = ID_VEHICLE_SYNC;
        BRInCarSyncData data{};
        if (!bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.wKeys, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.quatw, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.quatx, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.quaty, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.quatz, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecPos.x, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecPos.y, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecPos.z, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.x, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.y, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.z, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.fCarHealth, 32) ||
            !bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8) ||
            !bsCopy.ReadBits((unsigned char *)&data.byteSirenOn, 8) ||
            !bsCopy.ReadBits((unsigned char *)&data.byteLandingGearState, 8) ||
            !bsCopy.ReadBits((unsigned char *)&data.TrailerID, 16)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktVehicleSync, 8);
        ConvertBRInCarSyncToSampSync(&bsSend, data);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }
    if (pktId == BR_ID_PASSENGER_SYNC) {
        uint8_t pktPassengerSync = ID_PASSENGER_SYNC;
        BRPassengerSyncData data{};
        if (!bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        uint8_t tempByte = 0;
        if (!bsCopy.ReadBits((unsigned char *)&tempByte, 7)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }
        data.byteSeatFlags = tempByte;

        tempByte = 0;
        if (!bsCopy.ReadBits((unsigned char *)&tempByte, 1)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }
        data.byteDriveBy = tempByte;

        if (!bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8) ||
            !bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.lrAnalog, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.udAnalog, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.wKeys, 16) ||
            !bsCopy.ReadBits((unsigned char *)&data.vecPos, 96)) {
            return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
        }

        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktPassengerSync, 8);
        ConvertBRPassengerSyncToSampSync(&bsSend, data);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }
    if (pktId == BR_ID_TRAILER_SYNC) {
        return ForwardSyncRawPacket(bitStream, ID_TRAILER_SYNC, priority, reliability, orderingChannel);
    }
    if (pktId == BR_ID_UNOCCUPIED_SYNC) {
        return ForwardSyncRawPacket(bitStream, ID_UNOCCUPIED_SYNC, priority, reliability, orderingChannel);
    }
    if (pktId == BR_ID_SPECTATOR_SYNC) {
        return ForwardSyncRawPacket(bitStream, ID_SPECTATOR_SYNC, priority, reliability, orderingChannel);
    }
    if (pktId == BR_ID_WEAPONS_UPDATE) {
        return ForwardSyncRawPacket(bitStream, ID_WEAPONS_UPDATE, priority, reliability, orderingChannel);
    }
    if (pktId == BR_ID_STATS_UPDATE) {
        return ForwardSyncRawPacket(bitStream, ID_STATS_UPDATE, priority, reliability, orderingChannel);
    }

    return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
}
