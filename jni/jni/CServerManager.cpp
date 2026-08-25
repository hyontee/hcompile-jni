#include "CServerManager.h"
#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"LIVE RUSSIA BONUS | MOSCOW",
	"LIVE RUSSIA BONUS | TEST"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("141.95.190.146", 1, 20, 1209, false),
	CServerInstance::create("141.95.190.146", 1, 20, 1209, false)
};
