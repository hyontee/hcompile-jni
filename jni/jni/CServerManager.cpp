#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Vain Games | 01",
	"Brilliant RP | Cullinan"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("185.189.255.97", 1, 16, 2990, false), // 1
	CServerInstance::create("185.189.255.97", 1, 16, 2990, false) // 2
};