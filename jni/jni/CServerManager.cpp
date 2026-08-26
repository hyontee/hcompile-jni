#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Vain Games | 01",
	"Brilliant RP | Cullinan"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.74", 1, 16, 2771, false), // 1
	CServerInstance::create("188.127.241.74", 1, 16, 2771, false) // 2
};