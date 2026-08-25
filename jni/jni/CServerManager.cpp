#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"1 сервер",
	"Тестовый"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	// world drift
	CServerInstance::create("188.127.241.74", 1, 11, 3243, true), // 1
	CServerInstance::create("51.91.91.91", 1, 11, 7777, false) // 2

};