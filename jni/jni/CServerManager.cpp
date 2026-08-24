#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"1",
	"Brilliant RP | Cullinan"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.74", 1, 16, 4101, false), // 1
	CServerInstance::create("188.127.241.74", 1, 16, 2209, false) // 2
};