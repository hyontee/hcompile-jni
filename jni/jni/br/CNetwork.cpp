#include "../main.h"
#include "CNetwork.h"
#include <stdint.h>

#include "../util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"LIGHT RUSSIA | RED",
	"LIGHT RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(OBFUSCATE("178.130.53.114"), 14, 18, 1313, 1), // 0
	CSetServer::create(OBFUSCATE("178.130.53.114"), 14, 18, 1313, 1) // 1
};