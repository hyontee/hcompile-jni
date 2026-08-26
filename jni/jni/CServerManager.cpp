#include "CServerManager.h"
#include "server_config.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	SAMP_SERVER_NAME_0,
	SAMP_SERVER_NAME_1
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create(SAMP_SERVER_HOST, 1, sizeof(SAMP_SERVER_HOST) - 1, SAMP_SERVER_PORT, false),
	CServerInstance::create(SAMP_SERVER_HOST, 1, sizeof(SAMP_SERVER_HOST) - 1, SAMP_SERVER_PORT, false)
};
