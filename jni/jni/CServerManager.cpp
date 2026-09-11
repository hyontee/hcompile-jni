#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"01",
	"Test"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.74:2413", 1, 16, 2413, false), // 1
	CServerInstance::create("147.135.229.229", 1, 16, 1279, false) // 2
};