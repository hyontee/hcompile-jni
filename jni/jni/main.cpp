#include "main.h"
#include "CSettingsLoader.h"
#include "jniutil.h"

#include <android/log.h>
#include <arpa/inet.h>
#include <array>
#include <cstddef>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <inttypes.h>
#include <memory>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <utility>

#include "vendor/raknet/DS_RangeList.h"
#include "vendor/raknet/InternalPacket.h"
#include "vendor/raknet/RPCMap.h"

uintptr_t g_libGTASA = 0;
uintptr_t g_libSAMP = 0;
uintptr_t g_libBR = 0;
const char* g_pAPKPackage = nullptr;

jobject appContext = nullptr;
JavaVM* mVm = nullptr;
JNIEnv* mEnv = nullptr;

extern "C" bool Pizd_SendCustomRpc(uint8_t rpcId, const void* data, uint16_t dataSizeBytes);

namespace {

constexpr const char* kLogTag = "TEST-JNI";
constexpr const char* kRedirectIp = "188.127.241.8";
constexpr const char* kSpecialSourceIp = "46.174.53.161";
constexpr uint16_t kSpecialSourcePort = 7777;
constexpr uint16_t kDefaultRedirectPort = 1371;
constexpr uint16_t kSpecialRedirectPort = 1371;
constexpr uint8_t kIdCustomRpc = 251;
constexpr uint16_t kClientHelloGuiId = 65001;
constexpr const char* kClientHelloPayload = "USUPOV_GUARD_HELLO_v1";
constexpr uint8_t kClientGuardRpcId = 97U;
constexpr uint32_t kClientGuardActChallenge = 90U;
constexpr uint32_t kClientGuardActResponse = 91U;
constexpr uint32_t kClientGuardServerMagic = 0x55535550U;
constexpr uint32_t kClientGuardClientMagic = 0x434C4E54U;
constexpr uint32_t kOfflineFallbackDelayMs = 8000U;
constexpr uint16_t kOfflinePlayerId = 0;
constexpr uint16_t kOfflineVehicleId = 1;
constexpr int kOfflinePlayerSkin = 122;
constexpr int kOfflineVehicleModel = 2582;
constexpr float kOfflinePlayerPosX = 434.035888f;
constexpr float kOfflinePlayerPosY = 242.063186f;
constexpr float kOfflinePlayerPosZ = 11.955309f;
constexpr float kOfflinePlayerAngle = 335.599761f;
constexpr float kOfflineVehiclePosX = 438.720428f;
constexpr float kOfflineVehiclePosY = 243.237564f;
constexpr float kOfflineVehiclePosZ = 11.961235f;
constexpr float kOfflineVehicleAngle = 343.115844f;
constexpr bool kVerboseNetLogs = false;
constexpr bool kVerboseRpcSendLogs = false;
constexpr bool kVerboseRpcQueueLogs = false;

constexpr uintptr_t kCNetGameCtorOffset = 0x7706C;
constexpr uintptr_t kWorldVehicleAddRpcOffset = 0x07BD28;
constexpr uintptr_t kVehiclePoolGlobalOffset = 0x205338;
constexpr uintptr_t kVehicleAddFuncOffset = 0x08BB18;

constexpr uintptr_t kRpcStatProcessOffset = 0x30B38;
constexpr uintptr_t kIncomingHeadOffset = 0x206C58;
constexpr uintptr_t kIncomingTailOffset = 0x206C64;
constexpr uintptr_t kOutgoingHeadOffset = 0x206C70;
constexpr uintptr_t kOutgoingTailOffset = 0x206C7C;
constexpr uintptr_t kBRSettingsJsonFnOffset = 0x18ED08;
constexpr uintptr_t kBRSettingsSendFnOffset = 0x08CC5E;
constexpr uintptr_t kBRSettingsManagerOffset = 0x7FB444;

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, kLogTag, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, kLogTag, __VA_ARGS__)

inline bool IsSpecialSourceEndpoint(in_addr_t ip, uint16_t port) {
    return ip == inet_addr(kSpecialSourceIp) && port == kSpecialSourcePort;
}

inline bool IsRedirectEndpoint(in_addr_t ip) {
    return ip == inet_addr(kRedirectIp);
}

inline bool IsManagedRedirectPort(uint16_t port) {
    return port == kDefaultRedirectPort || port == kSpecialRedirectPort;
}

inline uint16_t ResolveRedirectPort(in_addr_t originalIp, uint16_t originalPort) {
    if (IsSpecialSourceEndpoint(originalIp, originalPort)) {
        return kSpecialRedirectPort;
    }
    if (IsManagedRedirectPort(originalPort)) {
        return originalPort;
    }
    return kDefaultRedirectPort;
}

struct PendingConnectEndpoint {
    bool valid = false;
    in_addr_t originalIp = 0;
    uint16_t originalPort = 0;
    uint16_t redirectPort = 0;
};

PendingConnectEndpoint g_PendingConnectEndpoint;
pthread_mutex_t g_PendingConnectMutex = PTHREAD_MUTEX_INITIALIZER;

struct OfflineFallbackState {
    bool armed = false;
    bool activated = false;
    bool pendingMainThread = false;
    uint32_t deadlineMs = 0;
    uint32_t token = 0;
};

OfflineFallbackState g_OfflineFallbackState;
pthread_mutex_t g_OfflineFallbackMutex = PTHREAD_MUTEX_INITIALIZER;

void SetPendingConnectEndpoint(in_addr_t originalIp, uint16_t originalPort, uint16_t redirectPort) {
    pthread_mutex_lock(&g_PendingConnectMutex);
    g_PendingConnectEndpoint.valid = true;
    g_PendingConnectEndpoint.originalIp = originalIp;
    g_PendingConnectEndpoint.originalPort = originalPort;
    g_PendingConnectEndpoint.redirectPort = redirectPort;
    pthread_mutex_unlock(&g_PendingConnectMutex);
}

bool ConsumePendingConnectEndpoint(uint16_t redirectPort, in_addr_t* originalIp, uint16_t* originalPort) {
    if (!originalIp || !originalPort) {
        return false;
    }

    bool matched = false;
    pthread_mutex_lock(&g_PendingConnectMutex);
    if (g_PendingConnectEndpoint.valid && g_PendingConnectEndpoint.redirectPort == redirectPort) {
        *originalIp = g_PendingConnectEndpoint.originalIp;
        *originalPort = g_PendingConnectEndpoint.originalPort;
        g_PendingConnectEndpoint.valid = false;
        matched = true;
    }
    pthread_mutex_unlock(&g_PendingConnectMutex);
    return matched;
}

using CNetGameCtorFn = void (*)(char*, const char*, int, const char*, int);
CNetGameCtorFn g_OrigCNetGameCtor = nullptr;

struct WorldRpcParameters {
    unsigned char* input;
    unsigned int numberOfBitsOfData;
};

struct Vector3 {
    float X;
    float Y;
    float Z;
};

using VehicleId = uint16_t;

struct NewVehiclePacket {
    VehicleId VehicleID;
    int iVehicleType;
    Vector3 vecPos;
    float fRotation;
    uint8_t aColor1;
    uint8_t aColor2;
    float fHealth;
    uint8_t byteInterior;
    uint32_t dwDoorDamageStatus;
    uint32_t dwPanelDamageStatus;
    uint8_t byteLightDamageStatus;
    uint8_t byteTireDamageStatus;
    uint8_t byteAddSiren;
    uint8_t byteModSlots[14];
    uint8_t bytePaintjob;
    int cColor1;
    int cColor2;
};

#pragma pack(push, 1)
struct ScrSetSpawnInfoRpcData {
    uint8_t team;
    int32_t skin;
    float posX;
    float posY;
    float posZ;
    float rotation;
    uint8_t unknown;
    int32_t weapon1;
    int32_t ammo1;
    int32_t weapon2;
    int32_t ammo2;
    int32_t weapon3;
    int32_t ammo3;
};

struct ScrSetPlayerPosRpcData {
    float posX;
    float posY;
    float posZ;
};

struct ScrSetPlayerSkinRpcData {
    int32_t playerId;
    int32_t skinId;
};

struct ScrPutPlayerInVehicleRpcData {
    uint16_t vehicleId;
    uint8_t seatId;
};
#pragma pack(pop)

static_assert(sizeof(ScrSetSpawnInfoRpcData) == 46, "Unexpected ScrSetSpawnInfoRpcData size");
static_assert(sizeof(ScrSetPlayerPosRpcData) == 12, "Unexpected ScrSetPlayerPosRpcData size");
static_assert(sizeof(ScrSetPlayerSkinRpcData) == 8, "Unexpected ScrSetPlayerSkinRpcData size");
static_assert(sizeof(ScrPutPlayerInVehicleRpcData) == 3, "Unexpected ScrPutPlayerInVehicleRpcData size");

class BitReader {
public:
    BitReader(const unsigned char* data, unsigned int numberOfBits)
        : m_Data(data), m_NumberOfBits(numberOfBits), m_ReadOffset(0) {}

    bool ReadBits(unsigned char* output, int numberOfBitsToRead, bool alignBitsToRight) {
        if (!output || numberOfBitsToRead <= 0) {
            return false;
        }
        if (m_ReadOffset + (unsigned int)numberOfBitsToRead > m_NumberOfBits) {
            return false;
        }

        int offset = 0;
        memset(output, 0, (size_t)((numberOfBitsToRead + 7) >> 3));
        int readOffsetMod8 = (int)(m_ReadOffset & 7);

        while (numberOfBitsToRead > 0) {
            output[offset] |= m_Data[m_ReadOffset >> 3] << readOffsetMod8;
            if (readOffsetMod8 > 0 && numberOfBitsToRead > 8 - readOffsetMod8) {
                output[offset] |= m_Data[(m_ReadOffset >> 3) + 1] >> (8 - readOffsetMod8);
            }

            numberOfBitsToRead -= 8;
            if (numberOfBitsToRead < 0) {
                if (alignBitsToRight) {
                    output[offset] >>= -numberOfBitsToRead;
                }
                m_ReadOffset += (unsigned int)(8 + numberOfBitsToRead);
            } else {
                m_ReadOffset += 8;
            }

            ++offset;
        }

        return true;
    }

    template <typename T>
    bool Read(T& value) {
        return ReadBits(reinterpret_cast<unsigned char*>(&value), (int)(sizeof(T) * 8), true);
    }

private:
    const unsigned char* m_Data;
    unsigned int m_NumberOfBits;
    unsigned int m_ReadOffset;
};

using WorldVehicleAddRpcFn = void (*)(WorldRpcParameters*);
WorldVehicleAddRpcFn g_OrigWorldVehicleAddRpc = nullptr;
using AddVehicleFn = uintptr_t (*)(uintptr_t, NewVehiclePacket*);
AddVehicleFn g_AddVehicleFn = nullptr;

using RpcStatProcessFn = int (*)();
RpcStatProcessFn g_OrigRpcStatProcess = nullptr;

RakClientInterface* g_RakClient = nullptr;
bool g_RpcVtableHooksInstalled = false;
bool g_IncomingRpcBridgeInitialized = false;
int g_RpcIds[256] = {};

using RpcRawFn = bool (*)(RakClientInterface*,
                          int*,
                          const char*,
                          unsigned int,
                          PacketPriority,
                          PacketReliability,
                          char,
                          bool,
                          NetworkID,
                          RakNet::BitStream*);
using RpcBitStreamFn = bool (*)(RakClientInterface*,
                                int*,
                                RakNet::BitStream*,
                                PacketPriority,
                                PacketReliability,
                                char,
                                bool,
                                NetworkID,
                                RakNet::BitStream*);

RpcRawFn g_OrigRpcRaw = nullptr;
RpcBitStreamFn g_OrigRpcBitStream = nullptr;
using SendToFn = ssize_t (*)(int, const void*, size_t, int, const struct sockaddr*, socklen_t);
using RecvFromFn = ssize_t (*)(int, void*, size_t, int, struct sockaddr*, socklen_t*);
using RecvFromChkFn = ssize_t (*)(int, void*, size_t, size_t, int, struct sockaddr*, socklen_t*);
using RecvMsgFn = ssize_t (*)(int, struct msghdr*, int);
using ConnectFn = int (*)(int, const struct sockaddr*, socklen_t);
using EglSwapBuffersFn = EGLBoolean (*)(EGLDisplay, EGLSurface);
SendToFn g_OrigSendTo = nullptr;
RecvFromFn g_OrigRecvFrom = nullptr;
RecvFromChkFn g_OrigRecvFromChk = nullptr;
RecvMsgFn g_OrigRecvMsg = nullptr;
ConnectFn g_OrigConnect = nullptr;
EglSwapBuffersFn g_OrigEglSwapBuffers = nullptr;

constexpr size_t kRpcRawVtableIndex = 25;
constexpr size_t kRpcBitStreamVtableIndex = 26;

using BrSettingsJsonFn = uintptr_t (*)(uintptr_t*, char*);
using BrSettingsSendFn = uintptr_t (*)(uintptr_t*, int, uintptr_t*);

RakClient* GetRakClientObject(RakClientInterface* rakClient) {
    if (!rakClient) {
        return nullptr;
    }

    static const ptrdiff_t kRakClientInterfaceOffset =
        reinterpret_cast<char*>(static_cast<RakClientInterface*>(reinterpret_cast<RakClient*>(0x1000))) -
        reinterpret_cast<char*>(reinterpret_cast<RakClient*>(0x1000));

    return reinterpret_cast<RakClient*>(reinterpret_cast<char*>(rakClient) - kRakClientInterfaceOffset);
}

struct EndpointMapEntry {
    int sockfd = -1;
    in_addr_t originalIp = 0;
    uint16_t originalPort = 0;
    bool used = false;
};

EndpointMapEntry g_EndpointMap[128];
pthread_mutex_t g_EndpointMapMutex = PTHREAD_MUTEX_INITIALIZER;

inline size_t BitsToBytes(unsigned int bits) {
    return (size_t)((bits + 7U) >> 3U);
}

bool IsExecutableAddress(uintptr_t addr) {
    char mapsPath[64] = {0};
    snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/maps", getpid());

    FILE* fp = fopen(mapsPath, "r");
    if (!fp) {
        return false;
    }

    char line[512] = {0};
    bool ok = false;
    while (fgets(line, sizeof(line), fp)) {
        unsigned long start = 0;
        unsigned long end = 0;
        char perms[8] = {0};
        if (sscanf(line, "%lx-%lx %7s", &start, &end, perms) != 3) {
            continue;
        }
        if (addr >= (uintptr_t)start && addr < (uintptr_t)end) {
            ok = (strchr(perms, 'x') != nullptr);
            break;
        }
    }

    fclose(fp);
    return ok;
}

bool LooksLikeCode(uintptr_t addr) {
    if (!IsExecutableAddress(addr)) {
        return false;
    }

    uint16_t op = 0;
    memcpy(&op, reinterpret_cast<const void*>(addr), sizeof(op));
    if (op == 0x0000 || op == 0xFFFF) {
        return false;
    }
    return true;
}

bool IsPlausiblePtr(uintptr_t p) {
    return p > 0x10000U && p < 0xF0000000U;
}

bool MakeWritable(void* addr) {
    if (!addr) {
        return false;
    }
    const long pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize <= 0) {
        return false;
    }
    const uintptr_t ptr = (uintptr_t)addr;
    const uintptr_t pageStart = ptr & ~((uintptr_t)pageSize - 1U);
    return mprotect((void*)pageStart, (size_t)pageSize, PROT_READ | PROT_WRITE | PROT_EXEC) == 0;
}

bool InstallInlineHookSafe(const char* name, uintptr_t offset, void* hook, void** orig) {
    if (!g_libSAMP) {
        LOGE("%s skipped: libsamp base is 0", name);
        return false;
    }
    if (!offset) {
        LOGE("%s skipped: offset is 0", name);
        return false;
    }
    if (!hook || !orig) {
        LOGE("%s skipped: bad hook/orig ptr", name);
        return false;
    }

    const uintptr_t target = g_libSAMP + offset;
    if (!LooksLikeCode(target)) {
        LOGE("%s warning: target check failed, trying hook anyway (libsamp+0x%X -> 0x%08X)",
             name,
             (unsigned)offset,
             (unsigned)target);
    }

    const uint32_t hookTarget = (uint32_t)(target + CRYPT_MASK + 1U);
    enum ele7en_status regStatus =
        registerInlineHook(hookTarget, (uint32_t)(uintptr_t)hook, (uint32_t**)orig);
    if (regStatus != ELE7EN_OK) {
        LOGE("%s registerInlineHook failed: %d (target=0x%08X)", name, (int)regStatus, (unsigned)hookTarget);
        return false;
    }

    enum ele7en_status hookStatus = inlineHook(hookTarget);
    if (hookStatus != ELE7EN_OK) {
        LOGE("%s inlineHook failed: %d (target=0x%08X)", name, (int)hookStatus, (unsigned)hookTarget);
        return false;
    }

    LOGI("%s hook installed (libsamp+0x%X, orig=%p)", name, (unsigned)offset, *orig);
    return true;
}

bool InstallSymbolHookSafe(const char* symbolName, void* hookFn, void** origFn) {
    if (!symbolName || !hookFn || !origFn) {
        return false;
    }

    void* sym = dlsym(RTLD_DEFAULT, symbolName);
    if (!sym) {
        LOGE("dlsym(%s) failed", symbolName);
        return false;
    }

    const uint32_t target = (uint32_t)(uintptr_t)sym;
    const uint32_t hookedTarget = target + CRYPT_MASK;
    if (registerInlineHook(hookedTarget, (uint32_t)(uintptr_t)hookFn, (uint32_t**)origFn) != ELE7EN_OK) {
        LOGE("registerInlineHook(%s) failed", symbolName);
        return false;
    }
    if (inlineHook(hookedTarget) != ELE7EN_OK) {
        LOGE("inlineHook(%s) failed", symbolName);
        return false;
    }

    LOGI("%s hook installed (target=0x%08X, orig=%p)", symbolName, target, *origFn);
    return true;
}

struct ParsedRpcInfo {
    bool isRpc = false;
    bool isCustomRpc = false;
    bool hasPacketId = false;
    uint8_t packetId = 0;
    uint8_t rpcId = 0;
    unsigned int payloadBits = 0;
    unsigned int payloadBytes = 0;
    unsigned int rpcIdByteOffsetBits = 0;
    bool hasRpcIdByteOffset = false;
    bool hasRpcUserPayload = false;
    unsigned int rpcUserBits = 0;
    unsigned int rpcUserBytes = 0;
    unsigned char rpcUserHead[16] = {0};
    size_t rpcUserHeadBytes = 0;
};

bool TryReadInternalPacketRpc(RakNet::BitStream& bs, ParsedRpcInfo* out) {
    if (!out) {
        return false;
    }

    *out = ParsedRpcInfo {};

    if (bs.GetNumberOfUnreadBits() < (int)(sizeof(MessageNumberType) * 8 + 4 + 1)) {
        return false;
    }

    MessageNumberType messageNumber = 0;
    if (!bs.Read(messageNumber)) {
        return false;
    }

    unsigned char reliability = 0;
    if (!bs.ReadBits(&reliability, 4)) {
        return false;
    }

    if (reliability == UNRELIABLE_SEQUENCED || reliability == RELIABLE_SEQUENCED || reliability == RELIABLE_ORDERED) {
        unsigned char orderingChannel = 0;
        OrderingIndexType orderingIndex = 0;
        if (!bs.ReadBits(&orderingChannel, 5)) {
            return false;
        }
        if (!bs.Read(orderingIndex)) {
            return false;
        }
    }

    bool isSplitPacket = false;
    if (!bs.Read(isSplitPacket)) {
        return false;
    }
    if (isSplitPacket) {
        SplitPacketIdType splitPacketId = 0;
        SplitPacketIndexType splitPacketIndex = 0;
        SplitPacketIndexType splitPacketCount = 0;
        if (!bs.Read(splitPacketId)) {
            return false;
        }
        if (!bs.ReadCompressed(splitPacketIndex)) {
            return false;
        }
        if (!bs.ReadCompressed(splitPacketCount)) {
            return false;
        }
    }

    unsigned short payloadBits = 0;
    if (!bs.ReadCompressed(payloadBits)) {
        return false;
    }

    const size_t payloadBytes = BitsToBytes((unsigned int)payloadBits);
    out->payloadBits = (unsigned int)payloadBits;
    out->payloadBytes = (unsigned int)payloadBytes;
    if (payloadBytes == 0) {
        return true;
    }
    if (bs.GetNumberOfUnreadBits() < (int)(payloadBytes * 8U)) {
        return false;
    }

    const unsigned int readOffsetBeforePayload = (unsigned int)bs.GetReadOffset();
    const unsigned int payloadBitStart = (readOffsetBeforePayload + 7U) & ~7U;

    unsigned char payload[64] = {0};
    size_t capturedBytes = payloadBytes;
    if (capturedBytes > sizeof(payload)) {
        capturedBytes = sizeof(payload);
    }
    if (!bs.ReadAlignedBytes(payload, (int)capturedBytes)) {
        return false;
    }
    if (payloadBytes > capturedBytes) {
        bs.IgnoreBits((int)((payloadBytes - capturedBytes) * 8U));
    }

    if (capturedBytes < 2U) {
        return true;
    }

    uint8_t packetId = payload[0];
    size_t rpcBase = 0;
    if (packetId == ID_TIMESTAMP) {
        const size_t tsOffset = sizeof(uint8_t) + sizeof(RakNetTime);
        if (capturedBytes <= tsOffset) {
            return true;
        }
        rpcBase = tsOffset;
        packetId = payload[rpcBase];
    }

    if (capturedBytes <= rpcBase + 1U) {
        return true;
    }

    out->hasPacketId = true;
    out->packetId = packetId;

    if (packetId == ID_RPC || packetId == kIdCustomRpc) {
        out->isRpc = true;
        out->isCustomRpc = (packetId == kIdCustomRpc);
        out->rpcId = payload[rpcBase + 1U];
        out->hasRpcIdByteOffset = true;
        out->rpcIdByteOffsetBits = payloadBitStart + (unsigned int)((rpcBase + 1U) * 8U);

        if (rpcBase == 0 && capturedBytes == payloadBytes) {
            RakNet::BitStream rpcBs(payload, (int)capturedBytes, false);
            unsigned char rpcPacketId = 0;
            unsigned char rpcUniqueId = 0;
            unsigned int rpcUserBits = 0;
            if (rpcBs.Read(rpcPacketId) &&
                rpcBs.Read(rpcUniqueId) &&
                rpcBs.ReadCompressed(rpcUserBits)) {
                out->hasRpcUserPayload = true;
                out->rpcUserBits = rpcUserBits;
                out->rpcUserBytes = (unsigned int)BitsToBytes(rpcUserBits);
                out->rpcUserHeadBytes = out->rpcUserBytes;
                if (out->rpcUserHeadBytes > sizeof(out->rpcUserHead)) {
                    out->rpcUserHeadBytes = sizeof(out->rpcUserHead);
                }
                if (out->rpcUserHeadBytes > 0 &&
                    rpcBs.GetNumberOfUnreadBits() >= (int)(out->rpcUserHeadBytes * 8U)) {
                    rpcBs.ReadBits(out->rpcUserHead, (int)(out->rpcUserHeadBytes * 8U), false);
                } else {
                    out->rpcUserHeadBytes = 0;
                }
            }
        }
    }

    return true;
}

bool TryRemapTurnlightOutgoingRpc(void* buf, size_t len) {
    if (!buf || len == 0) {
        return false;
    }

    RakNet::BitStream datagram((unsigned char*)buf, (int)len, false);
    bool hasAcks = false;
    if (!datagram.Read(hasAcks)) {
        return false;
    }

    if (hasAcks) {
        DataStructures::RangeList<MessageNumberType> incomingAcks;
        if (!incomingAcks.Deserialize(&datagram)) {
            return false;
        }
    }

    bool mutated = false;
    ParsedRpcInfo parsedRpc {};
    while (datagram.GetNumberOfUnreadBits() > 0) {
        const int before = datagram.GetReadOffset();
        if (!TryReadInternalPacketRpc(datagram, &parsedRpc)) {
            break;
        }

        uint32_t action = 0;
        uint16_t vehicleId = 0;
        uint8_t turnStatus = 0;
        uint8_t action8 = 0;
        bool hasAction32 = false;
        bool hasAction8 = false;
        bool looksLikeTurnlightPayload = false;
        bool looksLikeRadialPayload = false;
        if (parsedRpc.hasRpcUserPayload) {
            if (parsedRpc.rpcUserHeadBytes >= sizeof(action)) {
                memcpy(&action, parsedRpc.rpcUserHead, sizeof(action));
                hasAction32 = true;
                looksLikeRadialPayload = (action == 4U && parsedRpc.rpcUserBytes <= 4U);
            }
            if (parsedRpc.rpcUserHeadBytes >= 1U) {
                action8 = parsedRpc.rpcUserHead[0];
                hasAction8 = true;
                if (!looksLikeRadialPayload) {
                    looksLikeRadialPayload = (action8 == 4U && parsedRpc.rpcUserBytes <= 4U);
                }
            }
            if (parsedRpc.rpcUserHeadBytes >= 7U && hasAction32) {
                memcpy(&vehicleId, parsedRpc.rpcUserHead + 4, sizeof(vehicleId));
                memcpy(&turnStatus, parsedRpc.rpcUserHead + 6, sizeof(turnStatus));
                looksLikeTurnlightPayload =
                    (action == 3U && vehicleId > 0U && turnStatus >= 1U && turnStatus <= 3U);
            }
        }

        if (parsedRpc.isRpc &&
            !parsedRpc.isCustomRpc &&
            (parsedRpc.rpcId == 39 || parsedRpc.rpcId == 98) &&
            (looksLikeTurnlightPayload || looksLikeRadialPayload) &&
            parsedRpc.hasRpcIdByteOffset &&
            (parsedRpc.rpcIdByteOffsetBits % 8U) == 0U) {
            const size_t rpcIdByteOffset = (size_t)(parsedRpc.rpcIdByteOffsetBits / 8U);
            if (rpcIdByteOffset < len) {
                unsigned char* raw = reinterpret_cast<unsigned char*>(buf);
                const uint8_t srcRpcId = raw[rpcIdByteOffset];
                raw[rpcIdByteOffset] = 97;
                mutated = true;
                if (looksLikeTurnlightPayload) {
                    LOGI("Turnlight remap RPC %u->97 userBits=%u userBytes=%u action=%u vehicleId=%u status=%u",
                         (unsigned)srcRpcId,
                         parsedRpc.rpcUserBits,
                         parsedRpc.rpcUserBytes,
                         (unsigned)action,
                         (unsigned)vehicleId,
                         (unsigned)turnStatus);
                } else {
                    const unsigned loggedAction = hasAction32 ? (unsigned)action : (unsigned)(hasAction8 ? action8 : 0U);
                    LOGI("Radial remap RPC %u->97 userBits=%u userBytes=%u action=%u",
                         (unsigned)srcRpcId,
                         parsedRpc.rpcUserBits,
                         parsedRpc.rpcUserBytes,
                         loggedAction);
                }
            }
        }

        if (datagram.GetReadOffset() <= before) {
            break;
        }
    }

    return mutated;
}

const std::array<unsigned char, 256>& GetSampEncryptTable() {
    static const std::array<unsigned char, 256> encryptTable = {{
        0x27, 0x69, 0xFD, 0x87, 0x60, 0x7D, 0x83, 0x02, 0xF2, 0x3F, 0x71, 0x99, 0xA3, 0x7C, 0x1B, 0x9D,
        0x76, 0x30, 0x23, 0x25, 0xC5, 0x82, 0x9B, 0xEB, 0x1E, 0xFA, 0x46, 0x4F, 0x98, 0xC9, 0x37, 0x88,
        0x18, 0xA2, 0x68, 0xD6, 0xD7, 0x22, 0xD1, 0x74, 0x7A, 0x79, 0x2E, 0xD2, 0x6D, 0x48, 0x0F, 0xB1,
        0x62, 0x97, 0xBC, 0x8B, 0x59, 0x7F, 0x29, 0xB6, 0xB9, 0x61, 0xBE, 0xC8, 0xC1, 0xC6, 0x40, 0xEF,
        0x11, 0x6A, 0xA5, 0xC7, 0x3A, 0xF4, 0x4C, 0x13, 0x6C, 0x2B, 0x1C, 0x54, 0x56, 0x55, 0x53, 0xA8,
        0xDC, 0x9C, 0x9A, 0x16, 0xDD, 0xB0, 0xF5, 0x2D, 0xFF, 0xDE, 0x8A, 0x90, 0xFC, 0x95, 0xEC, 0x31,
        0x85, 0xC2, 0x01, 0x06, 0xDB, 0x28, 0xD8, 0xEA, 0xA0, 0xDA, 0x10, 0x0E, 0xF0, 0x2A, 0x6B, 0x21,
        0xF1, 0x86, 0xFB, 0x65, 0xE1, 0x6F, 0xF6, 0x26, 0x33, 0x39, 0xAE, 0xBF, 0xD4, 0xE4, 0xE9, 0x44,
        0x75, 0x3D, 0x63, 0xBD, 0xC0, 0x7B, 0x9E, 0xA6, 0x5C, 0x1F, 0xB2, 0xA4, 0xC4, 0x8D, 0xB3, 0xFE,
        0x8F, 0x19, 0x8C, 0x4D, 0x5E, 0x34, 0xCC, 0xF9, 0xB5, 0xF3, 0xF8, 0xA1, 0x50, 0x04, 0x93, 0x73,
        0xE0, 0xBA, 0xCB, 0x45, 0x35, 0x1A, 0x49, 0x47, 0x6E, 0x2F, 0x51, 0x12, 0xE2, 0x4A, 0x72, 0x05,
        0x66, 0x70, 0xB8, 0xCD, 0x00, 0xE5, 0xBB, 0x24, 0x58, 0xEE, 0xB4, 0x80, 0x81, 0x36, 0xA9, 0x67,
        0x5A, 0x4B, 0xE8, 0xCA, 0xCF, 0x9F, 0xE3, 0xAC, 0xAA, 0x14, 0x5B, 0x5F, 0x0A, 0x3B, 0x77, 0x92,
        0x09, 0x15, 0x4E, 0x94, 0xAD, 0x17, 0x64, 0x52, 0xD3, 0x38, 0x43, 0x0D, 0x0C, 0x07, 0x3C, 0x1D,
        0xAF, 0xED, 0xE7, 0x08, 0xB7, 0x03, 0xE6, 0x8E, 0xAB, 0x91, 0x89, 0x3E, 0x2C, 0x96, 0x42, 0xD9,
        0x78, 0xDF, 0xD0, 0x57, 0x5D, 0x84, 0x41, 0x7E, 0xCE, 0xF7, 0x32, 0xC3, 0xD5, 0x20, 0x0B, 0xA7
    }};
    return encryptTable;
}

const unsigned char* GetSampDecryptTable() {
    static const std::array<unsigned char, 256> decryptTable = []() {
        std::array<unsigned char, 256> table {};
        const std::array<unsigned char, 256>& encryptTable = GetSampEncryptTable();
        for (int i = 0; i < 256; ++i) {
            table[encryptTable[(size_t)i]] = (unsigned char)i;
        }
        return table;
    }();
    return decryptTable.data();
}

bool DecodeRaksampClientDatagram(const void* encryptedBuf,
                                 size_t encryptedLen,
                                 uint16_t port,
                                 std::unique_ptr<unsigned char[]>* outPlainBuf,
                                 size_t* outPlainLen) {
    if (!encryptedBuf || encryptedLen < 2 || !outPlainBuf || !outPlainLen) {
        return false;
    }

    const unsigned char* in = reinterpret_cast<const unsigned char*>(encryptedBuf);
    const unsigned char* decryptTable = GetSampDecryptTable();
    const size_t plainLen = encryptedLen - 1U;

    std::unique_ptr<unsigned char[]> plain(new unsigned char[plainLen]);

    uint8_t unk = 0;
    const uint8_t portMask = (uint8_t)(port ^ 0xCCU);
    uint8_t checksum = 0;

    for (size_t i = 0; i < plainLen; ++i) {
        uint8_t b = in[i + 1U];
        if (unk) {
            b ^= portMask;
        }
        unk ^= 1U;

        plain[i] = decryptTable[b];
        checksum ^= (uint8_t)(plain[i] & 0xAAU);
    }

    if (checksum != in[0]) {
        return false;
    }

    *outPlainLen = plainLen;
    *outPlainBuf = std::move(plain);
    return true;
}

bool EncodeRaksampClientDatagram(const unsigned char* plainBuf,
                                 size_t plainLen,
                                 uint16_t port,
                                 unsigned char* outEncryptedBuf,
                                 size_t outEncryptedLen) {
    if (!plainBuf || plainLen == 0 || !outEncryptedBuf || outEncryptedLen != plainLen + 1U) {
        return false;
    }

    uint8_t checksum = 0;
    for (size_t i = 0; i < plainLen; ++i) {
        checksum ^= (uint8_t)(plainBuf[i] & 0xAAU);
    }
    outEncryptedBuf[0] = checksum;

    uint8_t unk = 0;
    const uint8_t portMask = (uint8_t)(port ^ 0xCCU);
    const std::array<unsigned char, 256>& encryptTable = GetSampEncryptTable();
    for (size_t i = 0; i < plainLen; ++i) {
        uint8_t b = encryptTable[plainBuf[i]];
        if (unk) {
            b ^= portMask;
        }
        unk ^= 1U;
        outEncryptedBuf[i + 1U] = b;
    }

    return true;
}

void LogPacketRpcStream(const char* direction, const void* data, size_t len) {
    if (!kVerboseNetLogs) {
        return;
    }
    if (!direction || !data || len == 0) {
        return;
    }

    RakNet::BitStream datagram((unsigned char*)data, (int)len, false);

    bool hasAcks = false;
    if (!datagram.Read(hasAcks)) {
        return;
    }

    if (hasAcks) {
        DataStructures::RangeList<MessageNumberType> incomingAcks;
        if (!incomingAcks.Deserialize(&datagram)) {
            static uint32_t sLastAckFailLog = 0;
            const uint32_t now = GetTickCount();
            if (now - sLastAckFailLog > 1000U) {
                sLastAckFailLog = now;
                const unsigned char* raw = reinterpret_cast<const unsigned char*>(data);
                const size_t shown = len < 12 ? len : 12;
                char hex[12 * 3 + 1];
                unsigned p = 0;
                for (size_t i = 0; i < shown && p + 4 < sizeof(hex); ++i) {
                    p += (unsigned)snprintf(hex + p, sizeof(hex) - p, "%02X ", raw[i]);
                }
                hex[p] = '\0';
                LOGI("RakPeer::parse ack-fail %s len=%u head=%s", direction, (unsigned)len, hex);
            }
            return;
        }
    }

    ParsedRpcInfo parsedRpc {};
    bool printedSomething = false;
    while (datagram.GetNumberOfUnreadBits() > 0) {
        const int before = datagram.GetReadOffset();
        if (!TryReadInternalPacketRpc(datagram, &parsedRpc)) {
            break;
        }

        if (parsedRpc.isRpc) {
            if (parsedRpc.isCustomRpc) {
                LOGI("RakPeer::CustomRPC %s rpcId=%u bits=%u bytes=%u",
                     direction,
                     (unsigned)parsedRpc.rpcId,
                     (unsigned)parsedRpc.payloadBits,
                     (unsigned)parsedRpc.payloadBytes);
            } else {
                LOGI("RakPeer::RPC %s rpcId=%u bits=%u bytes=%u",
                     direction,
                     (unsigned)parsedRpc.rpcId,
                     (unsigned)parsedRpc.payloadBits,
                     (unsigned)parsedRpc.payloadBytes);
                if ((parsedRpc.rpcId == 97 ||
                     parsedRpc.rpcId == 98 ||
                     parsedRpc.rpcId == 117) &&
                    parsedRpc.hasRpcUserPayload &&
                    parsedRpc.rpcUserHeadBytes >= 7) {
                    uint32_t action = 0;
                    uint16_t vehicleId = 0;
                    uint8_t turnStatus = 0;
                    memcpy(&action, parsedRpc.rpcUserHead, sizeof(action));
                    memcpy(&vehicleId, parsedRpc.rpcUserHead + 4, sizeof(vehicleId));
                    memcpy(&turnStatus, parsedRpc.rpcUserHead + 6, sizeof(turnStatus));
                    LOGI("RakPeer::RPC detail %s rpcId=%u action=%u vehicleId=%u turnStatus=%u",
                         direction,
                         (unsigned)parsedRpc.rpcId,
                         (unsigned)action,
                         (unsigned)vehicleId,
                         (unsigned)turnStatus);
                }
            }
            printedSomething = true;
        } else if (parsedRpc.hasPacketId) {
            static uint32_t sLastPacketLogById[256] = {};
            const uint32_t now = GetTickCount();
            const uint8_t pid = parsedRpc.packetId;
            if (now - sLastPacketLogById[pid] > 1000U) {
                sLastPacketLogById[pid] = now;
                LOGI("RakPeer::Packet %s packetId=%u bits=%u bytes=%u",
                     direction,
                     (unsigned)pid,
                     (unsigned)parsedRpc.payloadBits,
                     (unsigned)parsedRpc.payloadBytes);
                printedSomething = true;
            }
        }

        if (datagram.GetReadOffset() <= before) {
            break;
        }
    }

    if (!printedSomething && len == 24U) {
        static uint32_t sLastLen24Log = 0;
        const uint32_t now = GetTickCount();
        if (now - sLastLen24Log > 1000U) {
            sLastLen24Log = now;
            const unsigned char* raw = reinterpret_cast<const unsigned char*>(data);
            char hex[24 * 3 + 1];
            unsigned p = 0;
            for (size_t i = 0; i < 24 && p + 4 < sizeof(hex); ++i) {
                p += (unsigned)snprintf(hex + p, sizeof(hex) - p, "%02X ", raw[i]);
            }
            hex[p] = '\0';
            LOGI("RakPeer::sendto len24 head=%s", hex);
        }
    }
}

bool ReadMem32(uintptr_t addr, uint32_t* out) {
    if (!addr || !out) {
        return false;
    }
    memcpy(out, reinterpret_cast<const void*>(addr), sizeof(uint32_t));
    return true;
}

bool ReadMem8(uintptr_t addr, uint8_t* out) {
    if (!addr || !out) {
        return false;
    }
    memcpy(out, reinterpret_cast<const void*>(addr), sizeof(uint8_t));
    return true;
}

void LogRpcQueueList(const char* tag, uintptr_t head, uintptr_t tail) {
    uintptr_t node = head;
    unsigned iter = 0;
    while (node && node != tail && iter < 512U) {
        ++iter;
        if (!IsPlausiblePtr(node)) {
            LOGI("sub_30B38 %s bad node ptr=0x%08X", tag, (unsigned)node);
            break;
        }

        uint8_t active = 0;
        if (!ReadMem8(node + 16U, &active) || active == 0) {
            break;
        }

        uint32_t packetOrId = 0;
        uint32_t rpcId = 0;
        uint32_t size = 0;
        uint32_t tick = 0;
        uint32_t next = 0;
        if (!ReadMem32(node + 0U, &packetOrId) ||
            !ReadMem32(node + 4U, &rpcId) ||
            !ReadMem32(node + 8U, &size) ||
            !ReadMem32(node + 12U, &tick) ||
            !ReadMem32(node + 17U, &next)) {
            break;
        }

        if ((int32_t)rpcId == -1) {
            LOGI("sub_30B38 %s packetId=%u size=%u tick=%u", tag, packetOrId, size, tick);
        } else {
            LOGI("sub_30B38 %s rpcId=%u size=%u tick=%u", tag, rpcId, size, tick);
        }

        if (next == 0 || !IsPlausiblePtr((uintptr_t)next) || next == (uint32_t)node) {
            break;
        }
        node = (uintptr_t)next;
    }
}

void LogRpcStatQueues() {
    if (!g_libSAMP) {
        return;
    }

    uint32_t inHead = 0;
    uint32_t inTail = 0;
    uint32_t outHead = 0;
    uint32_t outTail = 0;
    if (!ReadMem32(g_libSAMP + kIncomingHeadOffset, &inHead) ||
        !ReadMem32(g_libSAMP + kIncomingTailOffset, &inTail) ||
        !ReadMem32(g_libSAMP + kOutgoingHeadOffset, &outHead) ||
        !ReadMem32(g_libSAMP + kOutgoingTailOffset, &outTail)) {
        return;
    }

    LogRpcQueueList("incoming", (uintptr_t)inHead, (uintptr_t)inTail);
    LogRpcQueueList("outgoing", (uintptr_t)outHead, (uintptr_t)outTail);
}

bool ReadNewVehiclePacket(BitReader& bsData, NewVehiclePacket& outVehicle) {
    memset(&outVehicle, 0, sizeof(outVehicle));

    if (!bsData.Read(outVehicle.VehicleID)) return false;
    if (!bsData.Read(outVehicle.iVehicleType)) return false;
    if (!bsData.Read(outVehicle.vecPos.X)) return false;
    if (!bsData.Read(outVehicle.vecPos.Y)) return false;
    if (!bsData.Read(outVehicle.vecPos.Z)) return false;
    if (!bsData.Read(outVehicle.fRotation)) return false;
    if (!bsData.Read(outVehicle.aColor1)) return false;
    if (!bsData.Read(outVehicle.aColor2)) return false;
    if (!bsData.Read(outVehicle.fHealth)) return false;
    if (!bsData.Read(outVehicle.byteInterior)) return false;
    if (!bsData.Read(outVehicle.dwDoorDamageStatus)) return false;
    if (!bsData.Read(outVehicle.dwPanelDamageStatus)) return false;
    if (!bsData.Read(outVehicle.byteLightDamageStatus)) return false;
    if (!bsData.Read(outVehicle.byteTireDamageStatus)) return false;
    if (!bsData.Read(outVehicle.byteAddSiren)) return false;
    for (int i = 0; i < 14; ++i) {
        if (!bsData.Read(outVehicle.byteModSlots[i])) return false;
    }
    if (!bsData.Read(outVehicle.bytePaintjob)) return false;
    if (!bsData.Read(outVehicle.cColor1)) return false;
    if (!bsData.Read(outVehicle.cColor2)) return false;
    return true;
}

bool EnsureBrLibraryBase();
bool SendGraphicsSettingsToBr();
bool SendClientGuardResponse(uint32_t challenge);
bool SendClientHelloPacket();
void ProcessOfflineFallback();

uint32_t ClientGuard_CalcProof(uint32_t challenge) {
    uint32_t v = challenge ^ 0x53A91F27U;
    v = v * 1664525U + 1013904223U;
    v ^= (v >> 15);
    v = (v << 9) | (v >> 23);
    v ^= 0x7F4A7C15U;
    v += 0x13579BDFU;
    v ^= (v >> 16);
    return v;
}

void LogRpcPayloadHead(const char* tag, int rpcId, const unsigned char* data, unsigned int bits) {
    if (!data || bits == 0) {
        return;
    }
    const unsigned int bytes = (unsigned int)BitsToBytes(bits);
    const unsigned int shown = bytes < 16U ? bytes : 16U;
    char hex[16 * 3 + 1];
    unsigned int pos = 0;
    for (unsigned int i = 0; i < shown && pos + 4 < sizeof(hex); ++i) {
        pos += (unsigned int)snprintf(hex + pos, sizeof(hex) - pos, "%02X ", data[i]);
    }
    hex[pos] = '\0';
    LOGI("%s rpcId=%d bits=%u bytes=%u head=%s", tag, rpcId, bits, bytes, hex);
}

void OnIncomingRpcInternal(int rpcId, RPCParameters* rpcParams) {
    if (!rpcParams) {
        LOGE("OnIncomingRPC: rpcId=%d params=null", rpcId);
        return;
    }

    if (rpcId == (int)kClientGuardRpcId) {
        if (!rpcParams->input || rpcParams->numberOfBitsOfData < 96U) {
            LOGE("OnIncomingRPC: client guard rpc malformed bits=%u", rpcParams->numberOfBitsOfData);
            return;
        }

        BitReader reader(reinterpret_cast<const unsigned char*>(rpcParams->input), rpcParams->numberOfBitsOfData);
        uint32_t action = 0;
        uint32_t challenge = 0;
        uint32_t serverMagic = 0;
        if (!reader.Read(action)) {
            LOGE("OnIncomingRPC: client guard action read failed");
            return;
        }
        if (action == kClientGuardActChallenge) {
            if (!reader.Read(challenge) || !reader.Read(serverMagic)) {
                LOGE("OnIncomingRPC: client guard payload read failed");
                return;
            }
            if (serverMagic != kClientGuardServerMagic) {
                LOGE("OnIncomingRPC: client guard bad server magic=0x%08X", serverMagic);
                return;
            }
            if (!SendClientGuardResponse(challenge)) {
                LOGE("OnIncomingRPC: client guard response send failed");
            }
            return;
        }

        if (kVerboseNetLogs) {
            LOGI("OnIncomingRPC: client guard rpc ignored action=%u bits=%u", (unsigned)action, rpcParams->numberOfBitsOfData);
        }
        return;
    }

    if (rpcId == 100) {
        if (!SendGraphicsSettingsToBr()) {
            LOGE("OnIncomingRPC: rpcId=100 graphics settings send failed");
        }
        return;
    }

    if (!kVerboseNetLogs) {
        return;
    }

    const unsigned int bits = rpcParams->numberOfBitsOfData;
    const unsigned int bytes = (bits + 7U) / 8U;
    LOGI("OnIncomingRPC: rpcId=%d bits=%u bytes=%u", rpcId, bits, bytes);
}

template <int RpcId>
void IncomingRpcStub(RPCParameters* rpcParams) {
    OnIncomingRpcInternal(RpcId, rpcParams);
}

using RpcHandler = void (*)(RPCParameters*);

template <size_t... Is>
constexpr std::array<RpcHandler, sizeof...(Is)> BuildRpcStubTable(std::index_sequence<Is...>) {
    return {&IncomingRpcStub<(int)Is>...};
}

constexpr auto kRpcStubTable = BuildRpcStubTable(std::make_index_sequence<256>{});

bool EnsureBrLibraryBase() {
    if (g_libBR) {
        return true;
    }

    g_libBR = FindLibrary(OBFUSCATE("libBR.so"));
    if (!g_libBR) {
        g_libBR = FindLibrary(OBFUSCATE("libbr.so"));
    }
    if (!g_libBR) {
        LOGE("BR library not found");
        return false;
    }

    LOGI("BR library base=0x%08X", (unsigned)g_libBR);
    return true;
}

bool SendGraphicsSettingsToBr() {
    CSettingsLoader settings;

    char json[256] = {0};
    snprintf(json,
             sizeof(json),
             "{\"o\":1,\"w\":%d,\"carrefl\":%d,\"d\":%d,\"s\":%d,\"a\":%d,\"l\":%d,\"z\":%d}",
             settings.water,
             settings.carrefl,
             settings.d,
             settings.sky,
             settings.aa,
             settings.lowerdd,
             settings.lowercars);

    RakNet::BitStream payload;
    const uint16_t guiId = 19;
    payload.Write(guiId);
    payload.Write(json, (int)strlen(json) + 1);

    const bool ok = Pizd_SendCustomRpc(kIdCustomRpc,
                                       payload.GetData(),
                                       (uint16_t)payload.GetNumberOfBytesUsed());
    LOGI("graphics settings rpc251 sent: ok=%d json=%s", ok ? 1 : 0, json);
    return ok;
}

bool SendClientGuardResponse(uint32_t challenge) {
    RakNet::BitStream payload;
    const uint32_t proof = ClientGuard_CalcProof(challenge);
    payload.Write(kClientGuardActResponse);
    payload.Write(challenge);
    payload.Write(proof);
    payload.Write(kClientGuardClientMagic);

    SendClientHelloPacket();

    const bool ok = Pizd_SendCustomRpc(kClientGuardRpcId,
                                       payload.GetData(),
                                       (uint16_t)payload.GetNumberOfBytesUsed());
    LOGI("client guard rpc97 response sent: ok=%d challenge=%u proof=0x%08X", ok ? 1 : 0, challenge, proof);
    return ok;
}

bool SendClientHelloPacket() {
    if (!g_RakClient || !g_RakClient->IsConnected()) {
        LOGE("client hello packet252 skipped: RakClient unavailable or not connected");
        return false;
    }

    RakNet::BitStream packet;
    const char* payload = kClientHelloPayload;
    const uint32_t payloadSize = (uint32_t)strlen(payload);

    packet.Write((uint8_t)252U);
    packet.Write((uint16_t)kClientHelloGuiId);
    packet.Write(payloadSize);
    packet.WriteAlignedBytes(reinterpret_cast<const unsigned char*>(payload), (int)payloadSize);

    const bool ok = g_RakClient->Send(&packet, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
    LOGI("client hello packet252 sent: ok=%d guiid=%u bytes=%u",
         ok ? 1 : 0,
         (unsigned)kClientHelloGuiId,
         (unsigned)packet.GetNumberOfBytesUsed());
    return ok;
}

void RegisterMissingRpcHandlers(RakClientInterface* rakClient) {
    if (!rakClient) {
        return;
    }

    for (int i = 0; i < 256; ++i) {
        g_RpcIds[i] = i;
        rakClient->RegisterAsRemoteProcedureCall(&g_RpcIds[i], kRpcStubTable[(size_t)i]);
    }
}

bool InvokeLocalRpcByIndexBits(int rpcId, const void* data, unsigned int bitLength, const char* label) {
    if (!g_RakClient) {
        LOGE("offline fallback: %s skipped, RakClient is null", label);
        return false;
    }

    RakClient* rakClientObject = GetRakClientObject(g_RakClient);
    if (!rakClientObject) {
        LOGE("offline fallback: %s skipped, RakClient object is null", label);
        return false;
    }

    RPCMap* rpcMap = rakClientObject->GetRPCMap(UNASSIGNED_PLAYER_ID);
    if (!rpcMap) {
        LOGE("offline fallback: %s skipped, RPC map is null", label);
        return false;
    }

    RPCNode* node = rpcMap->GetNodeFromIndex((RPCIndex)rpcId);
    if (!node || !node->staticFunctionPointer || node->isPointerToMember) {
        LOGE("offline fallback: %s skipped, rpcId=%d handler missing", label, rpcId);
        return false;
    }

    const bool isStubHandler =
        rpcId >= 0 &&
        rpcId < (int)kRpcStubTable.size() &&
        node->staticFunctionPointer == kRpcStubTable[(size_t)rpcId];

    const size_t dataSizeBytes = BitsToBytes(bitLength);
    std::unique_ptr<unsigned char[]> payloadCopy;
    RPCParameters rpcParams {};
    if (data && bitLength > 0U) {
        payloadCopy.reset(new unsigned char[dataSizeBytes]);
        memcpy(payloadCopy.get(), data, dataSizeBytes);
        rpcParams.input = payloadCopy.get();
        rpcParams.numberOfBitsOfData = bitLength;
    }
    rpcParams.sender = UNASSIGNED_PLAYER_ID;
    rpcParams.recipient = static_cast<RakPeerInterface*>(rakClientObject);
    rpcParams.replyToSender = nullptr;

    node->staticFunctionPointer(&rpcParams);
    LOGI("offline fallback: invoked %s rpcId=%d bytes=%u handler=%p stub=%d",
         label,
         rpcId,
         (unsigned)dataSizeBytes,
         (void*)node->staticFunctionPointer,
         isStubHandler ? 1 : 0);
    return true;
}

bool InvokeLocalRpcByIndex(int rpcId, const void* data, size_t dataSizeBytes, const char* label) {
    return InvokeLocalRpcByIndexBits(rpcId,
                                     data,
                                     (unsigned int)dataSizeBytes * 8U,
                                     label);
}

bool SpawnOfflineFallbackVehicle() {
    NewVehiclePacket vehicle {};
    vehicle.VehicleID = kOfflineVehicleId;
    vehicle.iVehicleType = kOfflineVehicleModel;
    vehicle.vecPos.X = kOfflineVehiclePosX;
    vehicle.vecPos.Y = kOfflineVehiclePosY;
    vehicle.vecPos.Z = kOfflineVehiclePosZ;
    vehicle.fRotation = kOfflineVehicleAngle;
    vehicle.fHealth = 1000.0f;
    vehicle.byteInterior = 0;
    if (InvokeLocalRpcByIndex(RPC_WorldVehicleAdd, &vehicle, sizeof(vehicle), "WorldVehicleAdd")) {
        return true;
    }

    if (!g_AddVehicleFn) {
        LOGE("offline fallback: direct vehicle spawn failed, addVehicle ptr is null");
        return false;
    }

    uintptr_t holder = *(uintptr_t*)(g_libSAMP + kVehiclePoolGlobalOffset);
    if (!IsPlausiblePtr(holder)) {
        LOGE("offline fallback: direct vehicle spawn failed, invalid holder ptr=0x%08X", (unsigned)holder);
        return false;
    }

    uintptr_t vehiclePool = *(uintptr_t*)(holder + 8);
    if (!IsPlausiblePtr(vehiclePool)) {
        LOGE("offline fallback: direct vehicle spawn failed, invalid vehiclePool ptr=0x%08X", (unsigned)vehiclePool);
        return false;
    }

    g_AddVehicleFn(vehiclePool, &vehicle);
    LOGI("offline fallback: direct vehicle spawn ok id=%u model=%d",
         (unsigned)vehicle.VehicleID,
         vehicle.iVehicleType);
    return true;
}

bool InvokeOfflineInitGame() {
    RakNet::BitStream payload;

    payload.Write(true);            
    payload.Write(true);            
    payload.Write(true);            
    payload.Write(false);            
    payload.Write(20000.0f);         
    payload.Write(false);           
    payload.Write(70.0f);           
    payload.Write(false);             
    payload.Write(true);           
    payload.Write(true);           
    payload.Write((int32_t)1);      
    payload.Write((uint16_t)kOfflinePlayerId);
    payload.Write(true);             
    payload.Write((int32_t)1);       
    payload.Write((uint8_t)12);      
    payload.Write((uint8_t)1);       
    payload.Write(0.008f);            
    payload.Write(false);            
    payload.Write((int32_t)0);     
    payload.Write(false);         
    payload.Write((int32_t)40);     
    payload.Write((int32_t)40); 
    payload.Write((int32_t)40);       
    payload.Write((int32_t)2);      
    payload.Write((int32_t)1);   

    const char kOfflineHostName[] = "Offline roam";
    const uint8_t hostNameLen = (uint8_t)(sizeof(kOfflineHostName) - 1U);
    payload.Write(hostNameLen);
    payload.Write(kOfflineHostName, (int)hostNameLen);

    for (int i = 0; i < 212; ++i) {
        payload.Write((uint8_t)1);
    }

    payload.Write((uint32_t)0);   
    return InvokeLocalRpcByIndexBits(RPC_InitGame,
                                     payload.GetData(),
                                     (unsigned int)payload.GetNumberOfBitsUsed(),
                                     "InitGame");
}

void RunOfflineFallback() {
    LOGI("offline fallback: starting local free roam mode");

    uint32_t spectatingDisabled = 0;
    uint8_t enabled = 1;
    float zeroFloat = 0.0f;
    float health = 100.0f;
    uint8_t interior = 0;

    ScrSetSpawnInfoRpcData spawnInfo {};
    spawnInfo.team = 0;
    spawnInfo.skin = kOfflinePlayerSkin;
    spawnInfo.posX = kOfflinePlayerPosX;
    spawnInfo.posY = kOfflinePlayerPosY;
    spawnInfo.posZ = kOfflinePlayerPosZ;
    spawnInfo.rotation = kOfflinePlayerAngle;
    spawnInfo.unknown = 0;

    ScrSetPlayerPosRpcData playerPos {};
    playerPos.posX = kOfflinePlayerPosX;
    playerPos.posY = kOfflinePlayerPosY;
    playerPos.posZ = kOfflinePlayerPosZ;

    ScrSetPlayerSkinRpcData skin {};
    skin.playerId = (int32_t)kOfflinePlayerId;
    skin.skinId = kOfflinePlayerSkin;

    ScrPutPlayerInVehicleRpcData enterVehicle {};
    enterVehicle.vehicleId = kOfflineVehicleId;
    enterVehicle.seatId = 0;

    InvokeOfflineInitGame();
    InvokeLocalRpcByIndex(RPC_ScrTogglePlayerSpectating,
                          &spectatingDisabled,
                          sizeof(spectatingDisabled),
                          "ScrTogglePlayerSpectating");
    InvokeLocalRpcByIndex(RPC_ScrSetInterior, &interior, sizeof(interior), "ScrSetInterior");
    InvokeLocalRpcByIndex(RPC_ScrSetSpawnInfo, &spawnInfo, sizeof(spawnInfo), "ScrSetSpawnInfo");
    InvokeLocalRpcByIndex(RPC_Spawn, nullptr, 0, "Spawn");

    usleep(400000);

    InvokeLocalRpcByIndex(RPC_ScrSetPlayerSkin, &skin, sizeof(skin), "ScrSetPlayerSkin");
    InvokeLocalRpcByIndex(RPC_ScrSetPlayerPos, &playerPos, sizeof(playerPos), "ScrSetPlayerPos");
    InvokeLocalRpcByIndex(RPC_ScrSetPlayerFacingAngle, &spawnInfo.rotation, sizeof(spawnInfo.rotation), "ScrSetPlayerFacingAngle");
    InvokeLocalRpcByIndex(RPC_ScrSetPlayerHealth, &health, sizeof(health), "ScrSetPlayerHealth");
    InvokeLocalRpcByIndex(RPC_ScrSetPlayerArmour, &zeroFloat, sizeof(zeroFloat), "ScrSetPlayerArmour");
    InvokeLocalRpcByIndex(RPC_ScrTogglePlayerControllable, &enabled, sizeof(enabled), "ScrTogglePlayerControllable");
    InvokeLocalRpcByIndex(RPC_ScrSetCameraBehindPlayer, nullptr, 0, "ScrSetCameraBehindPlayer");

    SpawnOfflineFallbackVehicle();
    usleep(250000);

    InvokeLocalRpcByIndex(RPC_ScrPutPlayerInVehicle, &enterVehicle, sizeof(enterVehicle), "ScrPutPlayerInVehicle");
    InvokeLocalRpcByIndex(RPC_ScrSetCameraBehindPlayer, nullptr, 0, "ScrSetCameraBehindPlayer");
}

void ArmOfflineFallback() {
    pthread_t thread {};
    uint32_t token = 0;

    pthread_mutex_lock(&g_OfflineFallbackMutex);
    g_OfflineFallbackState.armed = true;
    g_OfflineFallbackState.activated = false;
    g_OfflineFallbackState.pendingMainThread = false;
    g_OfflineFallbackState.deadlineMs = GetTickCount() + kOfflineFallbackDelayMs;
    token = ++g_OfflineFallbackState.token;
    pthread_mutex_unlock(&g_OfflineFallbackMutex);

    if (pthread_create(&thread, nullptr, [](void* param) -> void* {
            const uint32_t localToken = (uint32_t)(uintptr_t)param;
            usleep(kOfflineFallbackDelayMs * 1000U);

            bool marked = false;
            pthread_mutex_lock(&g_OfflineFallbackMutex);
            if (g_OfflineFallbackState.armed &&
                !g_OfflineFallbackState.activated &&
                g_OfflineFallbackState.token == localToken &&
                g_RakClient &&
                !g_RakClient->IsConnected()) {
                g_OfflineFallbackState.pendingMainThread = true;
                marked = true;
            }
            pthread_mutex_unlock(&g_OfflineFallbackMutex);

            if (marked) {
                LOGI("offline fallback: timer requested main-thread execution token=%u", localToken);
            }
            return nullptr;
        }, (void*)(uintptr_t)token) == 0) {
        pthread_detach(thread);
        LOGI("offline fallback: armed deadline=%u token=%u", g_OfflineFallbackState.deadlineMs, token);
    } else {
        LOGE("offline fallback: failed to create timer thread");
    }
}

void ProcessOfflineFallback() {
    bool shouldRun = false;
    const uint32_t now = GetTickCount();

    pthread_mutex_lock(&g_OfflineFallbackMutex);
    if (g_OfflineFallbackState.armed && g_RakClient) {
        if (g_RakClient->IsConnected()) {
            g_OfflineFallbackState.armed = false;
            g_OfflineFallbackState.pendingMainThread = false;
        } else if (g_OfflineFallbackState.pendingMainThread) {
            g_OfflineFallbackState.armed = false;
            g_OfflineFallbackState.activated = true;
            g_OfflineFallbackState.pendingMainThread = false;
            shouldRun = true;
        } else if (!g_OfflineFallbackState.activated &&
                   now >= g_OfflineFallbackState.deadlineMs) {
            g_OfflineFallbackState.armed = false;
            g_OfflineFallbackState.activated = true;
            shouldRun = true;
        }
    }
    pthread_mutex_unlock(&g_OfflineFallbackMutex);

    if (shouldRun) {
        LOGI("offline fallback: main-thread trigger");
        RunOfflineFallback();
    }
}

void RememberOriginalEndpoint(int sockfd, in_addr_t ip, uint16_t port) {
    if (sockfd < 0) {
        return;
    }
    pthread_mutex_lock(&g_EndpointMapMutex);
    for (int i = 0; i < (int)(sizeof(g_EndpointMap) / sizeof(g_EndpointMap[0])); ++i) {
        if (g_EndpointMap[i].used && g_EndpointMap[i].sockfd == sockfd) {
            g_EndpointMap[i].originalIp = ip;
            g_EndpointMap[i].originalPort = port;
            pthread_mutex_unlock(&g_EndpointMapMutex);
            return;
        }
    }
    for (int i = 0; i < (int)(sizeof(g_EndpointMap) / sizeof(g_EndpointMap[0])); ++i) {
        if (!g_EndpointMap[i].used) {
            g_EndpointMap[i].used = true;
            g_EndpointMap[i].sockfd = sockfd;
            g_EndpointMap[i].originalIp = ip;
            g_EndpointMap[i].originalPort = port;
            pthread_mutex_unlock(&g_EndpointMapMutex);
            return;
        }
    }
    pthread_mutex_unlock(&g_EndpointMapMutex);
}

bool GetOriginalEndpoint(int sockfd, in_addr_t* ip, uint16_t* port) {
    if (sockfd < 0 || !ip || !port) {
        return false;
    }
    bool found = false;
    pthread_mutex_lock(&g_EndpointMapMutex);
    for (int i = 0; i < (int)(sizeof(g_EndpointMap) / sizeof(g_EndpointMap[0])); ++i) {
        if (g_EndpointMap[i].used && g_EndpointMap[i].sockfd == sockfd) {
            *ip = g_EndpointMap[i].originalIp;
            *port = g_EndpointMap[i].originalPort;
            found = true;
            break;
        }
    }
    pthread_mutex_unlock(&g_EndpointMapMutex);
    return found;
}

void RemapSourceEndpointIfNeeded(int sockfd, struct sockaddr* srcAddr, socklen_t addrlen) {
    if (!srcAddr || addrlen < (socklen_t)sizeof(sockaddr_in) || srcAddr->sa_family != AF_INET) {
        return;
    }
    sockaddr_in* src = reinterpret_cast<sockaddr_in*>(srcAddr);
    const in_addr_t redirectIp = inet_addr(kRedirectIp);
    const uint16_t currentPort = ntohs(src->sin_port);
    in_addr_t origIp = 0;
    uint16_t origPort = 0;
    if (GetOriginalEndpoint(sockfd, &origIp, &origPort) &&
        src->sin_addr.s_addr == redirectIp &&
        currentPort == ResolveRedirectPort(origIp, origPort)) {
        src->sin_addr.s_addr = origIp;
        src->sin_port = htons(origPort);
    }
}

ssize_t HookedSendTo(int sockfd,
                     const void* buf,
                     size_t len,
                     int flags,
                     const struct sockaddr* destAddr,
                     socklen_t addrlen) {
    if (!g_OrigSendTo) {
        return -1;
    }

    const bool hasAddr = (destAddr && addrlen >= (socklen_t)sizeof(sockaddr_in) && destAddr->sa_family == AF_INET);
    in_addr_t curIp = 0;
    uint16_t curPort = 0;
    if (hasAddr) {
        const sockaddr_in* in = reinterpret_cast<const sockaddr_in*>(destAddr);
        curIp = in->sin_addr.s_addr;
        curPort = ntohs(in->sin_port);
    }
    sockaddr_in redirectedDest {};
    const struct sockaddr* finalDestAddr = destAddr;
    socklen_t finalAddrLen = addrlen;
    if (hasAddr && IsManagedRedirectPort(curPort)) {
        const in_addr_t redirectIp = inet_addr(kRedirectIp);
        const uint16_t targetPort = ResolveRedirectPort(curIp, curPort);
        if (curIp != redirectIp) {
            redirectedDest = *reinterpret_cast<const sockaddr_in*>(destAddr);
            redirectedDest.sin_addr.s_addr = redirectIp;
            redirectedDest.sin_port = htons(targetPort);
            finalDestAddr = reinterpret_cast<const sockaddr*>(&redirectedDest);
            finalAddrLen = (socklen_t)sizeof(redirectedDest);
            RememberOriginalEndpoint(sockfd, curIp, curPort);
        }
    }

    const void* sendBuf = buf;
    std::unique_ptr<unsigned char[]> remapBuf;
    std::unique_ptr<unsigned char[]> logBuf;
    size_t logLen = len;
    if (buf && len > 0) {
        bool loggedAsPlain = false;
        if (len > 1U) {
            uint16_t decodePort = kDefaultRedirectPort;
            if (hasAddr && (IsManagedRedirectPort(curPort) || IsRedirectEndpoint(curIp))) {
                decodePort = ResolveRedirectPort(curIp, curPort);
            } else {
                in_addr_t originalIp = 0;
                uint16_t originalPort = 0;
                if (GetOriginalEndpoint(sockfd, &originalIp, &originalPort)) {
                    decodePort = ResolveRedirectPort(originalIp, originalPort);
                }
            }
            std::unique_ptr<unsigned char[]> plainBuf;
            size_t plainLen = 0;
            if (DecodeRaksampClientDatagram(buf, len, decodePort, &plainBuf, &plainLen)) {
                if (TryRemapTurnlightOutgoingRpc(plainBuf.get(), plainLen)) {
                    remapBuf.reset(new unsigned char[len]);
                    if (EncodeRaksampClientDatagram(plainBuf.get(), plainLen, decodePort, remapBuf.get(), len)) {
                        sendBuf = remapBuf.get();
                    } else {
                        remapBuf.reset();
                    }
                }

                logBuf = std::move(plainBuf);
                logLen = plainLen;
                loggedAsPlain = true;
            }
        }

        if (!loggedAsPlain) {
            logBuf.reset(new unsigned char[len]);
            memcpy(logBuf.get(), sendBuf, len);
            logLen = len;
        }
    }

    if (!kVerboseNetLogs) {
        return g_OrigSendTo(sockfd, sendBuf, len, flags, finalDestAddr, finalAddrLen);
    }

    struct SendLogEntry {
        bool used;
        bool hasAddr;
        in_addr_t ip;
        uint16_t port;
        size_t len;
        uint32_t lastLogTick;
        uint32_t suppressed;
    };

    static SendLogEntry sSendLogEntries[32] = {};
    constexpr uint32_t kSendLogWindowMs = 1000U;

    const uint32_t now = GetTickCount();
    auto sameKey = [&](const SendLogEntry& e) {
        return e.hasAddr == hasAddr && e.ip == curIp && e.port == curPort && e.len == len;
    };
    auto logSuppressed = [&](const SendLogEntry& e) {
        if (e.suppressed == 0) {
            return;
        }
        if (e.hasAddr) {
            in_addr tmpAddr {};
            tmpAddr.s_addr = e.ip;
            LOGI("sendto raw suppressed repeats=%u dst=%s:%u len=%u",
                 e.suppressed,
                 inet_ntoa(tmpAddr),
                 (unsigned)e.port,
                 (unsigned)e.len);
        } else {
            LOGI("sendto raw suppressed repeats=%u len=%u", e.suppressed, (unsigned)e.len);
        }
    };
    auto logCurrent = [&]() {
        if (hasAddr) {
            in_addr tmpAddr {};
            tmpAddr.s_addr = curIp;
            LOGI("sendto raw dst=%s:%u len=%u", inet_ntoa(tmpAddr), (unsigned)curPort, (unsigned)len);
        } else {
            LOGI("sendto raw len=%u", (unsigned)len);
        }
    };

    int slot = -1;
    int freeSlot = -1;
    int oldestSlot = 0;
    uint32_t oldestTick = UINT32_MAX;
    for (int i = 0; i < (int)(sizeof(sSendLogEntries) / sizeof(sSendLogEntries[0])); ++i) {
        const SendLogEntry& e = sSendLogEntries[i];
        if (!e.used) {
            if (freeSlot < 0) {
                freeSlot = i;
            }
            continue;
        }
        if (sameKey(e)) {
            slot = i;
            break;
        }
        if (e.lastLogTick < oldestTick) {
            oldestTick = e.lastLogTick;
            oldestSlot = i;
        }
    }

    if (slot < 0) {
        slot = (freeSlot >= 0) ? freeSlot : oldestSlot;
        if (sSendLogEntries[slot].used) {
            logSuppressed(sSendLogEntries[slot]);
        }
        sSendLogEntries[slot] = {true, hasAddr, curIp, curPort, len, 0U, 0U};
    }

    SendLogEntry& entry = sSendLogEntries[slot];
    if (entry.lastLogTick != 0U && (now - entry.lastLogTick) < kSendLogWindowMs) {
        ++entry.suppressed;
    } else {
        logSuppressed(entry);
        entry.suppressed = 0;
        logCurrent();
        entry.lastLogTick = now;
    }

    if (logBuf && logLen > 0) {
        LogPacketRpcStream("sendto", logBuf.get(), logLen);
    }
    return g_OrigSendTo(sockfd, sendBuf, len, flags, finalDestAddr, finalAddrLen);
}

int HookedConnect(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    if (!g_OrigConnect) {
        return -1;
    }
    if (!addr || addrlen < (socklen_t)sizeof(sockaddr_in) || addr->sa_family != AF_INET) {
        return g_OrigConnect(sockfd, addr, addrlen);
    }

    const sockaddr_in* in = reinterpret_cast<const sockaddr_in*>(addr);
    const uint16_t port = ntohs(in->sin_port);
    const in_addr_t redirectIp = inet_addr(kRedirectIp);
    if (in->sin_addr.s_addr == redirectIp) {
        in_addr_t pendingOriginalIp = 0;
        uint16_t pendingOriginalPort = 0;
        if (ConsumePendingConnectEndpoint(port, &pendingOriginalIp, &pendingOriginalPort)) {
            RememberOriginalEndpoint(sockfd, pendingOriginalIp, pendingOriginalPort);
        }
        return g_OrigConnect(sockfd, addr, addrlen);
    }

    if (IsManagedRedirectPort(port)) {
        const uint16_t targetPort = ResolveRedirectPort(in->sin_addr.s_addr, port);
        sockaddr_in out = *in;
        out.sin_addr.s_addr = redirectIp;
        out.sin_port = htons(targetPort);
        RememberOriginalEndpoint(sockfd, in->sin_addr.s_addr, port);
        LOGI("connect redirect %s:%u -> %s:%u",
             inet_ntoa(in->sin_addr),
             (unsigned)port,
             kRedirectIp,
             (unsigned)targetPort);
        return g_OrigConnect(sockfd, reinterpret_cast<const sockaddr*>(&out), sizeof(out));
    }

    return g_OrigConnect(sockfd, addr, addrlen);
}

ssize_t HookedRecvFrom(int sockfd,
                       void* buf,
                       size_t len,
                       int flags,
                       struct sockaddr* srcAddr,
                       socklen_t* addrlen) {
    if (!g_OrigRecvFrom) {
        return -1;
    }
    const ssize_t rc = g_OrigRecvFrom(sockfd, buf, len, flags, srcAddr, addrlen);
    if (rc > 0 && buf) {
        LogPacketRpcStream("recvfrom", buf, (size_t)rc);
        if (srcAddr && addrlen) {
            RemapSourceEndpointIfNeeded(sockfd, srcAddr, *addrlen);
        }
    }
    return rc;
}

ssize_t HookedRecvFromChk(int sockfd,
                          void* buf,
                          size_t len,
                          size_t bufSize,
                          int flags,
                          struct sockaddr* srcAddr,
                          socklen_t* addrlen) {
    if (!g_OrigRecvFromChk) {
        return -1;
    }
    const ssize_t rc = g_OrigRecvFromChk(sockfd, buf, len, bufSize, flags, srcAddr, addrlen);
    if (rc > 0 && buf) {
        LogPacketRpcStream("recvfrom_chk", buf, (size_t)rc);
        if (srcAddr && addrlen) {
            RemapSourceEndpointIfNeeded(sockfd, srcAddr, *addrlen);
        }
    }
    return rc;
}

ssize_t HookedRecvMsg(int sockfd, struct msghdr* msg, int flags) {
    if (!g_OrigRecvMsg) {
        return -1;
    }
    const ssize_t rc = g_OrigRecvMsg(sockfd, msg, flags);
    if (rc > 0 && msg && msg->msg_iov && msg->msg_iovlen > 0 && msg->msg_iov[0].iov_base) {
        size_t firstLen = msg->msg_iov[0].iov_len;
        if ((size_t)rc < firstLen) {
            firstLen = (size_t)rc;
        }
        LogPacketRpcStream("recvmsg", msg->msg_iov[0].iov_base, firstLen);
        if (msg->msg_name) {
            RemapSourceEndpointIfNeeded(sockfd,
                                        reinterpret_cast<sockaddr*>(msg->msg_name),
                                        (socklen_t)msg->msg_namelen);
        }
    }
    return rc;
}

EGLBoolean HookedEglSwapBuffers(EGLDisplay display, EGLSurface surface) {
    ProcessOfflineFallback();
    if (!g_OrigEglSwapBuffers) {
        return EGL_FALSE;
    }
    return g_OrigEglSwapBuffers(display, surface);
}

bool HookedRpcRaw(RakClientInterface* self,
                  int* uniqueID,
                  const char* data,
                  unsigned int bitLength,
                  PacketPriority priority,
                  PacketReliability reliability,
                  char orderingChannel,
                  bool shiftTimestamp,
                  NetworkID networkID,
                  RakNet::BitStream* replyFromTarget) {
    if (kVerboseRpcSendLogs) {
        const int rpcId = uniqueID ? *uniqueID : -1;
        LOGI("RakPeer::RPC send(raw) rpcId=%d bits=%u bytes=%u", rpcId, bitLength, (unsigned)BitsToBytes(bitLength));
        LogRpcPayloadHead("RakPeer::RPC send(raw)", rpcId, (const unsigned char*)data, bitLength);
    }

    if (!g_OrigRpcRaw) {
        return false;
    }
    return g_OrigRpcRaw(self,
                        uniqueID,
                        data,
                        bitLength,
                        priority,
                        reliability,
                        orderingChannel,
                        shiftTimestamp,
                        networkID,
                        replyFromTarget);
}

bool HookedRpcBitStream(RakClientInterface* self,
                        int* uniqueID,
                        RakNet::BitStream* bitStream,
                        PacketPriority priority,
                        PacketReliability reliability,
                        char orderingChannel,
                        bool shiftTimestamp,
                        NetworkID networkID,
                        RakNet::BitStream* replyFromTarget) {
    if (kVerboseRpcSendLogs) {
        const int rpcId = uniqueID ? *uniqueID : -1;
        unsigned int bits = 0;
        const unsigned char* data = nullptr;
        if (bitStream) {
            bits = (unsigned int)bitStream->GetNumberOfBitsUsed();
            data = bitStream->GetData();
        }
        LOGI("RakPeer::RPC send(bs) rpcId=%d bits=%u bytes=%u", rpcId, bits, (unsigned)BitsToBytes(bits));
        LogRpcPayloadHead("RakPeer::RPC send(bs)", rpcId, data, bits);
    }

    if (!g_OrigRpcBitStream) {
        return false;
    }
    return g_OrigRpcBitStream(self,
                              uniqueID,
                              bitStream,
                              priority,
                              reliability,
                              orderingChannel,
                              shiftTimestamp,
                              networkID,
                              replyFromTarget);
}

bool InstallRpcVtableHooks(RakClientInterface* rakClient) {
    if (!rakClient || g_RpcVtableHooksInstalled) {
        return g_RpcVtableHooksInstalled;
    }

    void*** vtableOwner = reinterpret_cast<void***>(rakClient);
    if (!vtableOwner || !*vtableOwner) {
        return false;
    }

    void** vtable = *vtableOwner;
    LOGI("RPC vtable probe: [24]=%p [25]=%p [26]=%p [27]=%p",
         vtable[24], vtable[25], vtable[26], vtable[27]);

    void** rawSlot = &vtable[kRpcRawVtableIndex];
    void** bsSlot = &vtable[kRpcBitStreamVtableIndex];
    if (!MakeWritable(rawSlot) || !MakeWritable(bsSlot)) {
        LOGE("RPC vtable hook failed: mprotect");
        return false;
    }

    g_OrigRpcRaw = reinterpret_cast<RpcRawFn>(*rawSlot);
    g_OrigRpcBitStream = reinterpret_cast<RpcBitStreamFn>(*bsSlot);
    *rawSlot = reinterpret_cast<void*>(&HookedRpcRaw);
    *bsSlot = reinterpret_cast<void*>(&HookedRpcBitStream);

    g_RpcVtableHooksInstalled = true;
    LOGI("RPC vtable hooks installed: raw=%p bs=%p", (void*)g_OrigRpcRaw, (void*)g_OrigRpcBitStream);
    return true;
}

void TryInitRakClientFromCNetGame(void* cnetGameThis) {
    if (!cnetGameThis) {
        return;
    }

    auto rakClient = *reinterpret_cast<RakClientInterface**>(cnetGameThis);
    if (!rakClient) {
        return;
    }

    if (rakClient != g_RakClient) {
        g_RakClient = rakClient;
        LOGI("RakClient captured: %p", g_RakClient);
    }

    InstallRpcVtableHooks(g_RakClient);
    if (!g_IncomingRpcBridgeInitialized) {
        RegisterMissingRpcHandlers(g_RakClient);
        g_IncomingRpcBridgeInitialized = true;
        LOGI("OnIncomingRPC bridge initialized");
    }

    if (g_RakClient->IsConnected()) {
        SendClientHelloPacket();
    }
}

void HookedCNetGameCtor(char* self, const char* ip, int port, const char* nick, int unk) {
    const in_addr_t originalIp = ip ? inet_addr(ip) : 0;
    const uint16_t originalPort = (port > 0 && port <= 65535) ? (uint16_t)port : kDefaultRedirectPort;
    const uint16_t redirectPort = ResolveRedirectPort(originalIp, originalPort);
    LOGI("CNetGame::CNetGame redirect %s:%d -> %s:%d",
         ip ? ip : "(null)",
         port,
         kRedirectIp,
         (int)redirectPort);

    if (ip) {
        SetPendingConnectEndpoint(originalIp, originalPort, redirectPort);
    }

    if (g_OrigCNetGameCtor) {
        g_OrigCNetGameCtor(self, kRedirectIp, (int)redirectPort, nick, unk);
    }

    TryInitRakClientFromCNetGame(self);
    ArmOfflineFallback();
}

void HookedWorldVehicleAddRpc(WorldRpcParameters* rpcParams) {
    if (!rpcParams || !rpcParams->input || rpcParams->numberOfBitsOfData == 0) {
        if (g_OrigWorldVehicleAddRpc) {
            g_OrigWorldVehicleAddRpc(rpcParams);
        }
        return;
    }

    BitReader reader(rpcParams->input, rpcParams->numberOfBitsOfData);
    NewVehiclePacket newVehicle {};
    if (!ReadNewVehiclePacket(reader, newVehicle)) {
        LOGE("WorldVehicleAdd fix: read failed (bits=%u)", rpcParams->numberOfBitsOfData);
        return;
    }

    if (newVehicle.VehicleID == 0xFFFF || newVehicle.VehicleID > 3100) {
        LOGE("WorldVehicleAdd fix: drop invalid vehicle id=%u", (unsigned)newVehicle.VehicleID);
        return;
    }
    if (newVehicle.iVehicleType < 400) {
        LOGE("WorldVehicleAdd fix: drop invalid model=%d", newVehicle.iVehicleType);
        return;
    }
    if (!g_AddVehicleFn) {
        LOGE("WorldVehicleAdd fix: addVehicle ptr is null");
        return;
    }

    uintptr_t holder = *(uintptr_t*)(g_libSAMP + kVehiclePoolGlobalOffset);
    if (!IsPlausiblePtr(holder)) {
        LOGE("WorldVehicleAdd fix: invalid holder ptr=0x%08X", (unsigned)holder);
        return;
    }

    uintptr_t vehiclePool = *(uintptr_t*)(holder + 8);
    if (!IsPlausiblePtr(vehiclePool)) {
        LOGE("WorldVehicleAdd fix: invalid vehiclePool ptr=0x%08X", (unsigned)vehiclePool);
        return;
    }

    g_AddVehicleFn(vehiclePool, &newVehicle);
    LOGI("WorldVehicleAdd fix: spawn vehicle id=%u model=%d", (unsigned)newVehicle.VehicleID, newVehicle.iVehicleType);
}

int HookedRpcStatProcess() {
    ProcessOfflineFallback();

    if (kVerboseRpcQueueLogs) {
        static unsigned calls = 0;
        ++calls;
        if ((calls % 500U) == 1U) {
            LOGI("sub_30B38 heartbeat=%u", calls);
        }

        LogRpcStatQueues();
    }
    if (!g_OrigRpcStatProcess) {
        return 0;
    }
    return g_OrigRpcStatProcess();
}

void InitSampSafe() {
    if (!g_libSAMP) {
        LOGE("InitSampSafe skipped: libsamp not found");
        return;
    }

    InstallInlineHookSafe("CNetGame::CNetGame",
                          kCNetGameCtorOffset,
                          (void*)&HookedCNetGameCtor,
                          (void**)&g_OrigCNetGameCtor);

    const uintptr_t addVehicleAddr = g_libSAMP + kVehicleAddFuncOffset;
    g_AddVehicleFn = reinterpret_cast<AddVehicleFn>(addVehicleAddr + 1U);

    InstallInlineHookSafe("WorldVehicleAddRPC",
                          kWorldVehicleAddRpcOffset,
                          (void*)&HookedWorldVehicleAddRpc,
                          (void**)&g_OrigWorldVehicleAddRpc);

    InstallInlineHookSafe("sub_30B38",
                          kRpcStatProcessOffset,
                          (void*)&HookedRpcStatProcess,
                          (void**)&g_OrigRpcStatProcess);

    InstallSymbolHookSafe("sendto", (void*)&HookedSendTo, (void**)&g_OrigSendTo);
    InstallSymbolHookSafe("connect", (void*)&HookedConnect, (void**)&g_OrigConnect);
    InstallSymbolHookSafe("recvfrom", (void*)&HookedRecvFrom, (void**)&g_OrigRecvFrom);
    InstallSymbolHookSafe("__recvfrom_chk", (void*)&HookedRecvFromChk, (void**)&g_OrigRecvFromChk);
    InstallSymbolHookSafe("recvmsg", (void*)&HookedRecvMsg, (void**)&g_OrigRecvMsg);
    InstallSymbolHookSafe("eglSwapBuffers", (void*)&HookedEglSwapBuffers, (void**)&g_OrigEglSwapBuffers);
}

}

extern "C" bool Pizd_SendCustomRpc(uint8_t rpcId, const void* data, uint16_t dataSizeBytes) {
    if (!g_RakClient) {
        LOGE("Pizd_SendCustomRpc failed: RakClient is null");
        return false;
    }

    int rpc = (int)rpcId;
    const char* payload = reinterpret_cast<const char*>(data);
    const unsigned int bitLength = (unsigned int)dataSizeBytes * 8U;
    const bool ok = g_RakClient->RPC(&rpc,
                                     payload,
                                     bitLength,
                                     HIGH_PRIORITY,
                                     RELIABLE_ORDERED,
                                     0,
                                     false,
                                     UNASSIGNED_NETWORK_ID,
                                     nullptr);
    LOGI("Pizd_SendCustomRpc rpcId=%u bytes=%u result=%d",
         (unsigned)rpcId,
         (unsigned)dataSizeBytes,
         ok ? 1 : 0);
    return ok;
}

extern "C" bool Pizd_SendTurnlightRpc(uint16_t vehicleId, uint8_t turnStatus) {
    struct TurnlightPayload {
        uint32_t action;
        uint16_t vehicle;
        uint8_t status;
    } payload {};
    payload.action = 3;
    payload.vehicle = vehicleId;
    payload.status = turnStatus;
    return Pizd_SendCustomRpc(97, &payload, (uint16_t)sizeof(payload));
}

JNIEnv* getEnv() {
    JNIEnv* env = nullptr;
    if (!mVm) {
        return nullptr;
    }

    int getEnvStat = mVm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
        if (mVm->AttachCurrentThread(&env, nullptr) != 0) {
            return nullptr;
        }
    } else if (getEnvStat == JNI_EVERSION || getEnvStat == JNI_ERR) {
        return nullptr;
    }

    return env;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    (void)reserved;
    mVm = vm;
    mEnv = getEnv();
    if (mEnv) {
        appContext = GetGlobalActivity(mEnv);
    }

    g_libGTASA = ARMHook::getLibraryAddress(OBFUSCATE("libGTASA.so"));
    g_libSAMP = FindLibrary(OBFUSCATE("libsamp.so"));
    g_libBR = FindLibrary(OBFUSCATE("libBR.so"));
    if (!g_libBR) {
        g_libBR = FindLibrary(OBFUSCATE("libbr.so"));
    }

    LOGI("JNI_OnLoad: libGTASA=0x%08X libsamp=0x%08X libBR=0x%08X",
         (unsigned)g_libGTASA,
         (unsigned)g_libSAMP,
         (unsigned)g_libBR);
    InitSampSafe();
    return JNI_VERSION_1_6;
}

uint32_t GetTickCount() {
    struct timeval tv {};
    gettimeofday(&tv, nullptr);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
