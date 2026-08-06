#include "CServerManager.h"
#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Name RP | Test",
	"Name RP | 01"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("80.242.59.112", 1, 13, 2449, true),
	CServerInstance::create("80.242.59.112", 1, 13, 2449, true)
};