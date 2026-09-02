#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"FAMILY GAMES | 01",
	"FAMILY RUSSIA  | Москва"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("ип", 1, 16, порт, false), // 1
	CServerInstance::create("ип", 1, 16, порт, false) // 2
};