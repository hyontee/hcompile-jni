#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"{ff9729}[1]{ffffff} OUR RUSSIA",
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("5.39.108.54", 1, 20, 1721, false),
	CServerInstance::create("5.39.108.54", 1, 20, 1721, false)
};
