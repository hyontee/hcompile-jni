#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.74", 1, 16, 2142, false), // 1
	CServerInstance::create("188.127.241.74", 1, 16, 2142, false) // 2
};
