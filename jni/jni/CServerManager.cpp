#include "CServerManager.h"
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Brilliant RP | Weiktonovka",
	"Brilliant RP | Cullinan"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.74", 1, 16, 5413, true), // указывать тут
	CServerInstance::create("188.127.241.74", 1, 16, 5413, true) // и тут тоже что сверху
};