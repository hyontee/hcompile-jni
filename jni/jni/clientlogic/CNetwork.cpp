// -- -- -- -- -- -- -- 
// BLACK RUSSIA v1.2.1 by Laird
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"PRIME MOBILE | МОСКВА",
	"PRIME MOBILE | САНКТ-ПЕТЕРБУРГ"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.8", 1, 16, 1239, false),
	CSetServer::create("188.127.241.8", 1, 16, 1239, false)	
};