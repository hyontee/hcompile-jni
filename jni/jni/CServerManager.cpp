#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"ASTANA",
	"TEST"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.8", 1, 16, 1544, false), // 1
	CServerInstance::create("188.127.241.8", 1, 16, 1544, false) // 2
};