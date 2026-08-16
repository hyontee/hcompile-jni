#include "../main.h"
#include "CNetwork.h"
#include <stdint.h>

#include "../util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"FUSS RUSSIA | RED",
	"FUSS RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(OBFUSCATE("5.39.108.52"), 14, 18, 1165, 1), // 0
	CSetServer::create(OBFUSCATE("5.39.108.52"), 14, 18, 1165, 1) // 1
};