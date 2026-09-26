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
	CSetServer::create("65.108.99.34", 1, 16, 2115, false),
	CSetServer::create("65.108.99.34", 1, 16, 2115, false)	
};