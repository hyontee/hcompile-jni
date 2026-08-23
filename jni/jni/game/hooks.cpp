#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"
#include "game/CEntity.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <unordered_set>
#include <vector>

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

namespace
{
constexpr int kMaxObjectsInPool = 1000;
constexpr int kMaxMaterialsPerGeometry = 64;
constexpr int kMaxTexturesToPrint = 80;
constexpr int kMaxAtomicInClump = 16;

struct RwLayout
{
    size_t atomicGeometryOffset;
    size_t geometryMaterialListOffset;
    size_t geometryMaterialCountOffset;
    size_t textureNameOffset;
    size_t clumpAtomicListOffset;
    size_t atomicInClumpLinkOffset;
};

constexpr RwLayout kRwLayout = (sizeof(uintptr_t) == 8)
    ? RwLayout{56, 40, 48, 32, 16, 112}
    : RwLayout{24, 32, 36, 16, 8, 64};

struct MemoryRange
{
    uintptr_t begin;
    uintptr_t end;
};

const std::vector<MemoryRange>& GetReadableRanges()
{
    static std::vector<MemoryRange> ranges;
    static bool loaded = false;

    if (loaded) {
        return ranges;
    }

    loaded = true;
    FILE* maps = std::fopen("/proc/self/maps", "r");
    if (!maps) {
        return ranges;
    }

    char line[256];
    while (std::fgets(line, sizeof(line), maps)) {
        unsigned long long beginRaw = 0;
        unsigned long long endRaw = 0;
        char perms[5] = {};
        if (std::sscanf(line, "%llx-%llx %4s", &beginRaw, &endRaw, perms) != 3) {
            continue;
        }
        const uintptr_t begin = static_cast<uintptr_t>(beginRaw);
        const uintptr_t end = static_cast<uintptr_t>(endRaw);
        if (perms[0] != 'r') {
            continue;
        }
        if (begin >= end) {
            continue;
        }
        ranges.push_back({begin, end});
    }

    std::fclose(maps);
    return ranges;
}

bool IsReadableRange(uintptr_t address, size_t bytes)
{
    if (address < 0x10000 || bytes == 0) {
        return false;
    }

    if (address > static_cast<uintptr_t>(-1) - bytes) {
        return false;
    }

    const uintptr_t end = address + bytes;
    const std::vector<MemoryRange>& ranges = GetReadableRanges();
    for (const MemoryRange& range : ranges) {
        if (address >= range.begin && end <= range.end) {
            return true;
        }
    }
    return false;
}

template <typename T>
bool ReadMemoryValue(uintptr_t address, T& outValue)
{
    if (!IsReadableRange(address, sizeof(T))) {
        return false;
    }
    outValue = *reinterpret_cast<const T*>(address);
    return true;
}

std::string Trim(const std::string& value)
{
    size_t begin = 0;
    while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin]))) {
        ++begin;
    }

    size_t end = value.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(begin, end - begin);
}

std::string ExtractCommandToken(std::string value)
{
    value = Trim(value);
    if (value.empty()) {
        return {};
    }

    if (value[0] == '/') {
        value.erase(0, 1);
    }

    const size_t split = value.find_first_of(" \t\r\n");
    if (split != std::string::npos) {
        value.resize(split);
    }

    for (char& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

std::string ExtractCommandFromBitStream(const RakNet::BitStream* bitStream)
{
    if (!bitStream) {
        return {};
    }

    const unsigned char* data = bitStream->GetData();
    const unsigned int bytes = bitStream->GetNumberOfBytesUsed();
    if (!data || bytes == 0) {
        return {};
    }

    if (bytes >= 2) {
        const uint8_t len8 = data[0];
        if (len8 > 0 && static_cast<unsigned int>(len8) + 1 <= bytes) {
            return std::string(reinterpret_cast<const char*>(data + 1), len8);
        }

        const uint16_t len16 = static_cast<uint16_t>(data[0]) |
                               (static_cast<uint16_t>(data[1]) << 8);
        if (len16 > 0 && static_cast<unsigned int>(len16) + 2 <= bytes) {
            return std::string(reinterpret_cast<const char*>(data + 2), len16);
        }
    }

    std::string fallback;
    fallback.reserve(bytes);
    for (unsigned int i = 0; i < bytes; ++i) {
        const unsigned char ch = data[i];
        if (ch == '\0') {
            break;
        }
        if (std::isprint(ch)) {
            fallback.push_back(static_cast<char>(ch));
        }
    }
    return fallback;
}

std::string ExtractTextureName(const char* rawName)
{
    if (!rawName || !IsReadableRange(reinterpret_cast<uintptr_t>(rawName), 32)) {
        return {};
    }

    std::string result;
    result.reserve(32);
    for (int i = 0; i < 32; ++i) {
        const unsigned char ch = static_cast<unsigned char>(rawName[i]);
        if (ch == '\0') {
            break;
        }

        const bool isAllowed =
            std::isalnum(ch) || ch == '_' || ch == '-' || ch == '.';
        if (!isAllowed) {
            return {};
        }
        result.push_back(static_cast<char>(ch));
    }

    return result;
}

void CollectTexturesFromAtomic(uintptr_t atomic, std::unordered_set<std::string>& textures)
{
    if (!atomic) {
        return;
    }

    uintptr_t geometry = 0;
    if (!ReadMemoryValue(atomic + kRwLayout.atomicGeometryOffset, geometry) || !geometry) {
        return;
    }

    int32_t materialCount = 0;
    if (!ReadMemoryValue(geometry + kRwLayout.geometryMaterialCountOffset, materialCount)) {
        return;
    }

    if (materialCount <= 0 || materialCount > kMaxMaterialsPerGeometry) {
        return;
    }

    uintptr_t materials = 0;
    if (!ReadMemoryValue(geometry + kRwLayout.geometryMaterialListOffset, materials) || !materials) {
        return;
    }

    for (int32_t i = 0; i < materialCount; ++i) {
        uintptr_t material = 0;
        if (!ReadMemoryValue(materials + static_cast<size_t>(i) * sizeof(uintptr_t), material) || !material) {
            continue;
        }

        uintptr_t texture = 0;
        if (!ReadMemoryValue(material, texture) || !texture) {
            continue;
        }

        const char* namePtr = reinterpret_cast<const char*>(texture + kRwLayout.textureNameOffset);
        std::string texName = ExtractTextureName(namePtr);
        if (!texName.empty()) {
            textures.insert(texName);
        }
    }
}

void CollectTexturesFromRwObject(uintptr_t rwObject, std::unordered_set<std::string>& textures)
{
    if (!rwObject) {
        return;
    }

    uint8_t rwType = 0;
    if (!ReadMemoryValue(rwObject, rwType)) {
        return;
    }
    if (rwType == 1) {
        CollectTexturesFromAtomic(rwObject, textures);
        return;
    }

    if (rwType != 2) {
        return;
    }

    const uintptr_t listHead = rwObject + kRwLayout.clumpAtomicListOffset;
    uintptr_t link = 0;
    if (!ReadMemoryValue(listHead, link)) {
        return;
    }
    int guard = 0;
    while (link && link != listHead && guard < kMaxAtomicInClump) {
        if (link < kRwLayout.atomicInClumpLinkOffset) {
            break;
        }

        const uintptr_t atomic = link - kRwLayout.atomicInClumpLinkOffset;
        CollectTexturesFromAtomic(atomic, textures);

        if (!ReadMemoryValue(link, link)) {
            break;
        }
        ++guard;
    }
}

std::vector<std::string> CollectObjectTextureNames()
{
    std::unordered_set<std::string> uniqueTextures;

    const uintptr_t poolAddr = CGameAPI::GetBase(xorstr("CNetGame::m_pObjectPool"));
    if (!poolAddr) {
        return {};
    }

    uintptr_t objectPool = 0;
    if (!ReadMemoryValue(poolAddr, objectPool) || !objectPool) {
        return {};
    }

    for (int objectId = 0; objectId < kMaxObjectsInPool; ++objectId) {
        uintptr_t objectPtr = 0;
        if (!ReadMemoryValue(
                objectPool + static_cast<size_t>(objectId) * sizeof(uintptr_t),
                objectPtr) ||
            !objectPtr) {
            continue;
        }

        uintptr_t rwObject = 0;
        if (!ReadMemoryValue(objectPtr + offsetof(CEntity, m_rwObject), rwObject) || !rwObject) {
            continue;
        }

        CollectTexturesFromRwObject(rwObject, uniqueTextures);
    }

    std::vector<std::string> result(uniqueTextures.begin(), uniqueTextures.end());
    std::sort(result.begin(), result.end());
    return result;
}

void PrintObjectTextureNamesToChat()
{
    const std::vector<std::string> textures = CollectObjectTextureNames();
    if (textures.empty()) {
        CChat::AddDebugMessage(xorstr("[DEBUG] Object textures not found."));
        return;
    }

    CChat::AddDebugMessage(xorstr("[DEBUG] Found textures: %d"), static_cast<int>(textures.size()));
    const size_t maxPrint = std::min(textures.size(), static_cast<size_t>(kMaxTexturesToPrint));
    for (size_t i = 0; i < maxPrint; ++i) {
        CChat::AddDebugMessage(xorstr("[DEBUG] %s"), textures[i].c_str());
    }

    if (textures.size() > maxPrint) {
        CChat::AddDebugMessage(
            xorstr("[DEBUG] ... and %d more"),
            static_cast<int>(textures.size() - maxPrint));
    }
}
}

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

    std::string text = formatted;
    const std::string from = "BLACK RUSSIA";
    const std::string to = " BUT MOBILE";
    size_t pos = 0;
    while((pos = text.find(from, pos)) != std::string::npos) {
        text.replace(pos, from.length(), to);
        pos += to.length();
    }

    return orig_CChat__AddDebugMessage("%s", text.c_str());
}

bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    return pRakClient->Connect(185.207.214.14, 4345, 1, 5, 5);
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

    if ((uniqueID == BR_RPC_ServerCommand || sampRpcId == RPC_ServerCommand) && bitStream) {
        const std::string commandToken = ExtractCommandToken(ExtractCommandFromBitStream(bitStream));
        if (commandToken == "debug") {
            PrintObjectTextureNamesToChat();
            return false;
        }
    }
    
    if(sampRpcId != -1) {
        if(sampRpcId == RPC_RequestClass && g_bInitGameProcess) {
            return false;
        }
        
        return pRakClient->RPC(&sampRpcId, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
    }
    
    return false;
}

static uint8_t g_LastTurnLightState = 0;
static uint64_t g_LastBlinkTime = 0;
static bool g_bLightsOn = false;
static bool g_bForceUpdate = false;

uint64_t GetTickCountMS() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}
// поворотники под доработку
void ProcessTurnSignals(uint64_t veh, BRInCarSyncData& data) {
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

bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel );
bool hook_RakClient__Send( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel )
{
    RakNet::BitStream bsCopy(bitStream->GetData(), bitStream->GetNumberOfBytesUsed() + 1, false);
    uint8_t pktId;
    bsCopy.Read(pktId);
    if(pktId == BR_ID_USER_INTERFACE_SYNC) {
        uint16_t guiId;
        uint32_t jsonLen;
        bsCopy.Read(guiId);
        bsCopy.Read(jsonLen);
        if(jsonLen > 0) {
            const uint32_t kMaxJsonSize = 1u << 20;
            if (jsonLen > kMaxJsonSize) {
                CNetGame::SendJsonData(guiId, "", 0);
                return false;
            }

            char* buff = new char[jsonLen + 1];
            if (!bsCopy.Read(buff, jsonLen)) {
                delete[] buff;
                return false;
            }
            buff[jsonLen] = '\0';


            if(guiId == 10) {
                size_t utfCap = static_cast<size_t>(jsonLen) * 3 + 1;
                char* buffGUI = new char[utfCap];
                cp1251_to_utf8(buffGUI, buff, jsonLen);

                const char* dialogJson = nullptr;
                if (nlohmann::json::accept(buff)) {
                    dialogJson = buff;
                } else if (nlohmann::json::accept(buffGUI)) {
                    dialogJson = buffGUI;
                }

                if (dialogJson) {
                    nlohmann::json jsonObj = nlohmann::json::parse(dialogJson);
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
                delete[] buffGUI;
            } else {
                // For pawn-json payloads preserve original bytes to avoid UTF-8/CP1251 double conversion.
                CNetGame::SendJsonData(guiId, buff, jsonLen);
            }
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
				///snprintf(debugMsg, sizeof(debugMsg), "Vehicle %d health: %.1f -> %.1f", data.VehicleID, lastHealth[data.VehicleID], data.fCarHealth);
				///CChat::AddDebugMessage(debugMsg);
				lastHealth[data.VehicleID] = data.fCarHealth;
			}
    	}
		uintptr_t vehPoolAddr = CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool"));
		if (vehPoolAddr) {
			uint64_t vehPool = *(uint64_t *)(vehPoolAddr);
			if (vehPool && data.VehicleID < 2000) {
				uint64_t veh = *(uint64_t *)(vehPool + 8ull * data.VehicleID);
			}
		}
    
    RakNet::BitStream bsSend;
    bsSend.WriteBits(&pktVehicleSync, 8);
    ConvertBRInCarSyncToSampSync(&bsSend, data);
    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}
	if(pktId == BR_ID_PASSENGER_SYNC) {
		uint8_t pktPassengerSync = ID_PASSENGER_SYNC;
		BRPassengerSyncData data;
		bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16);
		uint8_t seatAndDriveBy = 0;
		bsCopy.ReadBits((unsigned char *)&seatAndDriveBy, 8);
		data.byteSeatFlags = (seatAndDriveBy & 0x7F);
		data.byteDriveBy = ((seatAndDriveBy >> 7) & 0x01);
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
