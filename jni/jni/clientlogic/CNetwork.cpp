// -- -- -- -- -- -- -- 
// BLACK RUSSIA v1.2.1 by Merixton
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"TEST KESHA | RED",
	"TEST KESHA | GREEN"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.74", 1, 16, 2524, false),
	CSetServer::create("188.127.241.74", 1, 16, 2524, false)	
};