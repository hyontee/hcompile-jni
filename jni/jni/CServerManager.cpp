#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"{ff9729}[1]{ffffff} Country Rp",
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	// �������
	//CServerInstance::create("51.77.238.67", 1, 20, 7777, false), // 2 dev
	//CServerInstance::create("s1.aries-rp.ru", 1, 20, 7777, false), // 1
	CServerInstance::create("188.165.138.225", 1, 20, 1341, false), // 2 dev
	//CServerInstance::create("ip.ugsamp.com", 1, 14, 7777, false) // 1
	CServerInstance::create("188.165.138.225", 1, 20, 1341, false)
};
