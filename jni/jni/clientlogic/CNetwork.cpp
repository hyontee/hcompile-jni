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
	CSetServer::create("185.189.255.97", 1, 16, 2990, false),
	CSetServer::create("185.189.255.97", 1, 16, 2990, false)	
};