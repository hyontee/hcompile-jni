// -- -- -- -- -- -- -- 
// by snowy
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"VISKI SLIV INVENTORY REYTIZ | RED",
	"VISKI SLIV INVENTORY REYTIZ | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("178.130.53.117", 1, 16, 1113, false),
	CSetServer::create("178.130.53.117", 1, 16, 1113, false)	
};