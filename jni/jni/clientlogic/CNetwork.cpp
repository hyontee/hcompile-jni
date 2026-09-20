// -- -- -- -- -- -- -- 
// BLACK RUSSIA v1.2.1 by Skarf
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"LAIRD STUDIO | RED",
	"LAIRD STUDIO | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("178.130.53.117", 1, 16, 1113, false),
	CSetServer::create("178.130.53.117", 1, 16, 1113, false)	
};