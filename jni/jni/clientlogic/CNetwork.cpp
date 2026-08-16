#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BLACK MOSCOW | RED",
	"BLACK RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(NUM_TO_STR_IP(54, 38, 117, 79), 12, 1497, false),
	CSetServer::create(NUM_TO_STR_IP(54, 38, 117, 79), 12, 1497, false)
};