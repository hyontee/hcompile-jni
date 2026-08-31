#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"ASTANA",
	"TEST"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("178.130.53.123", 1, 16, 1209, false), // 1
	CServerInstance::create("178.130.53.123", 1, 16, 1209, false) // 2
};