//rick was here
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BLACK MOSCOW | RED",
	"BLACK RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("51.75.232.64", 1, 16, 1125, false),
	CSetServer::create("51.75.232.64", 1, 16, 1125, false)	
};