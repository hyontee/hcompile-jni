// -- -- -- -- -- -- -- 
// by Fozan FriezZ
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"GradiX | Moskow",
	"GradiX | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("51.75.232.70", 1, 16, 1117, false),
	CSetServer::create("51.75.232.70", 1, 16, 1117, false)	
};