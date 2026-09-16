// -- -- -- -- -- -- -- 
// BLACK RUSSIA v1.3
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BLACK RUSSIA | MOSCOW",
	"BLACK RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.74", 1, 16, 1972, false),
	CSetServer::create("188.127.241.74", 1, 16, 1972, false)	
};