// -- -- -- -- -- -- -- 
// OBR BY M3MORY
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"OLD RUSSIA | RED",
	"OLD RUSSIA | ТЕСТОВЫЙ"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.74", 1, 16, 3852, true),
	CSetServer::create("188.127.241.74", 1, 16, 3852, true)	
};