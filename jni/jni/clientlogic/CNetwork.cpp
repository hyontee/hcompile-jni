// -- -- -- -- -- -- -- 
// by Weikton 
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BLACK MOSCOW | RED",
	"BLACK RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("54.36.82.238", 1, 16, 1177, false),
	CSetServer::create("54.36.82.238", 1, 16, 1177, false)	
};