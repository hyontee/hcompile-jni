#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"{ff9729}[1]{ffffff} OUR RUSSIA",
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	//        
	//CServerInstance::create("5.39.108.50", 1, 20, 4669, false), // 2 dev
	//CServerInstance::create("5.39.108.50", 1, 20, 4669, false), // 1
	CServerInstance::create("188.127.241.74", 1, 20, 2291, false), // 2 dev
	//CServerInstance::create("5.39.108.50", 1, 14, 2291, false) // 1  Айпи СВОЙ IP
	CServerInstance::create("188.127.241.74", 1, 20, 2291, false)
};
