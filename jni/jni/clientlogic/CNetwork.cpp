// -- -- -- -- -- -- -- 
// by flerenk
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"GAYRENK | RED",
	"FLERENK | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.8", 1, 16, 1311, false),
	CSetServer::create("188.127.241.8", 1, 16, 1311, false)	
};