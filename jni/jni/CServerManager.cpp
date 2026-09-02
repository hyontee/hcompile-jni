#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	CServerInstance::create(OBFUSCATE("94.23.168.153"), 1, 16, 2240, false),
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create(OBFUSCATE("77.105.146.144"), 1, 16, 2240, false)
};