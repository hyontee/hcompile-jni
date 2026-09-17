#include "CServerManager.h"
#include "obfuscate/str_obfuscate.hpp"

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
    CServerInstance::create(OBFUSCATE("188.127.241.74"), 1, 16, 2142, false),  // 1
	CServerInstance::create(OBFUSCATE("188.127.241.74"), 1, 16, 2142, false)  // 2
};