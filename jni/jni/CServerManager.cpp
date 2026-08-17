#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"{ff9729}[1]{ffffff} GOMM",
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	// �������
	//CServerInstance::create("141.94.184.106", 1, 20, 1259, false), // 2 dev
	//CServerInstance::create("141.94.184.106", 1, 20, 1259, false), // 1
	CServerInstance::create("188.127.241.74", 1, 20, 2905, false), // 2 dev
	//CServerInstance::create("141.94.184.106", 1, 14, 1259, false) // 1  Айпи СВОЙ IP
	CServerInstance::create("188.127.241.74", 1, 20, 2905, false)
};
