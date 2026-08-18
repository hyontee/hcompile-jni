#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.74", 1, 20, 2775, false),	// основа
                  CServerInstance::create("188.127.241.74", 1, 20, 2775, false),	// release
	CServerInstance::create("188.127.241.74", 1, 20, 2775, false)
};

