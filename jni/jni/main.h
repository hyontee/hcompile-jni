#pragma once

#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unistd.h>
#include <unordered_map>
#include "game/common.h"


#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)

#define SAMP_VERSION	"ukraine-mobile"
#define PORT_VERSION	"0.69"

#define MAX_PLAYERS		1004
#define MAX_VEHICLES	2000
#define MAX_PLAYER_NAME	24
#define MAX_MATERIALS_PER_MODEL 16

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969
#include "vendor/raknet/SAMP/samp_netencr.h"
#include "vendor/raknet/SAMP/SAMPRPC.h"
#include "game/bass.h"
#include "vendor/bass/bass_fx.h"
#include <vendor/opus/opus.h>
#include "obfuscate_v2/str_obfuscate.hpp"
#include "util/util.h"

extern uintptr_t g_libGTASA;
extern uintptr_t g_libSAMP;
extern void* hGTASA;
extern char* g_pszStorage;

void Log(const char *fmt, ...);
void LogVoice(const char *fmt, ...);

#ifdef _CDEBUG
#define DLOG(str, ...) \
	Log(str, __VA_ARGS__)
#else
#define DLOG(str, ...)
#endif

uint32_t GetTickCount();