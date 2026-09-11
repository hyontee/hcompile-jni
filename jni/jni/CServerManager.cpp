#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"01",
	"Test"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("80.242.59.112", 1, 16, 2489, false), // 1
	CServerInstance::create("147.135.229.229", 1, 16, 1279, false) // 2
};