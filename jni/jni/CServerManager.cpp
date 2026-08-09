#include "CServerManager.h"
#include <stdint.h>
#include <cstring>

const char* g_szServerNames[MAX_SERVERS] = {
	"MATRESHKA LUNES | #1",
	"MATRESHKA LUNES | 01"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("54.38.117.76", 1, strlen("54.38.117.76"), 1405, true),
	CServerInstance::create("54.38.117.76", 1, strlen("54.38.117.76"), 1405, true)
};