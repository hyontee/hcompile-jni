// -- -- -- -- -- -- -- 
// BLACK RUSSIA v1.2.1 by Skarf
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"LAIRD STUDIO | RED",
	"LAIRD STUDIO | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.74", 1, 16, 4119, false),
	CSetServer::create("188.127.241.74", 1, 16, 4119, false)	
};