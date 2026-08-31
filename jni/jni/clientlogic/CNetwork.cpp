// -- -- -- -- -- -- -- 
// by skarf
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"SKARF | RED",
	"SKARF | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.74", 1, 16, 4384, false),
	CSetServer::create("188.127.241.74", 1, 16, 4384, false)	
};