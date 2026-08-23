#pragma once

#include <cstdint>
#include <string>

#include "vendor/RakNet/RakClient.h"
#include "vendor/RakNet/RakNetworkFactory.h"
#include "vendor/RakNet/BitStream.h"
#include "vendor/RakNet/NetworkTypes.h"
#include "vendor/nlohmann/json.hpp"

#include "plugin/netgame.h"
#include "plugin/netrpc.h"
#include "plugin/common.h"
#include "plugin.h"

extern void (*orig_CJSONTransport__OnDialogRPCIncoming)(uintptr_t thiz, int dialogid, int style, const char* header, const char* content, const char* btn1, const char* btn2);
void hook_CJSONTransport__OnDialogRPCIncoming(uintptr_t thiz, int dialogid, int style, const char* header, const char* content, const char* btn1, const char* btn2);

extern void (*orig_CNetTextDrawPool__SetServerLogo)(uintptr_t thiz, std::string url);
void hook_CNetTextDrawPool__SetServerLogo(uintptr_t thiz, std::string url);

// Receive packets
extern void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork();

// zamena
extern bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer); 
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);