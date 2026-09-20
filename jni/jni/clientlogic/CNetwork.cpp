// -- -- -- -- -- -- -- 
// BLACK RUSSIA v1.2.1 by Skarf
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"RED  | RED",
	"test | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("178.130.53.114", 1, 16, 1313, false),
	CSetServer::create("178.130.53.114", 1, 16, 1313, false)	
};