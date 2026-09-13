#include "CServerManager.h"
#include <stdint.h>
#include <cstring>

const char* g_szServerNames[MAX_SERVERS] = {
	"TEST SERVER",
	"TEST SERVER"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("80.242.59.112", 1, strlen("80.242.59.112"), 6977, true),
	CServerInstance::create("80.242.59.112", 1, strlen("80.242.59.112"), 6977, true)
};