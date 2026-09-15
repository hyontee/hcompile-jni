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
	"gr RP | hf",
	"gr RP | Cullinan"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("80.242.59.112", 1, 16, 6977, true), // указывать тут
	CServerInstance::create("80.242.59.112", 1, 16, 6977, true) // и тут тоже что сверху
};