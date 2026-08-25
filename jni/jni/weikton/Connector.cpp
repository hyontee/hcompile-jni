#include "Connector.h"
#include <stdint.h>

#include "../blackrussia/Java.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BLACK RUSSIA | RED",
	"BLACK RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {	
	CSetServer::create("141.95.190.146", 1, 16, 1209, true), // указывать тут
	CSetServer::create("141.95.190.146", 1, 16, 1209, true) // и тут тоже что сверху	
	//CSetServer::create("141.95.190.146", 1, 16, 1209, true), // указывать тут
	//CSetServer::create("141.95.190.146", 1, 16, 1209, true) // и тут тоже что сверху		
};