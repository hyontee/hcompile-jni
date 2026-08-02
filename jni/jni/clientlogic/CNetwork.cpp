//rick was here
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"VOLK RUSSIA | RED",
	"VOLK RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.74", 1, 16, 1060, false),
	CSetServer::create("188.127.241.74", 1, 16, 1060, false)	
};