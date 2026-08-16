#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BLACK MOSCOW | RED",
	"BLACK RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.165.140.93", 1, 16, 2122, false),
	CSetServer::create("188.165.140.93", 1, 16, 2122, false)	
};