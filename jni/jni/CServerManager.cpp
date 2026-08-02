#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"SAMP MOBILE",
	"SAMP MOBILE"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("141.94.184.108", 1, 16, 1241, false), // 1
	CServerInstance::create("141.94.184.108", 1, 16, 1241, false) // 2
};