#include "CServerManager.h"
#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Name RP | Test",
	"Name RP | 01"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.8", 1, 13, 1312, true),
	CServerInstance::create("188.127.241.8", 1, 13, 1312, true)
};