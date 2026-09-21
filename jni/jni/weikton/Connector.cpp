#include "Connector.h"
#include <stdint.h>

#include "../blackrussia/Java.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BOSS RUSSIA | RED",
	"BOSS RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {	
	CSetServer::create("188.127.241.74", 1, 16, 3857, true), // указывать тут
	CSetServer::create("188.127.241.74", 1, 16, 3857, true) // и тут тоже что сверху	
	//CSetServer::create("185.189.15.22", 1, 16, 6643, true), // указывать тут
	//CSetServer::create("185.189.15.22", 1, 16, 6643, true) // и тут тоже что сверху		
};