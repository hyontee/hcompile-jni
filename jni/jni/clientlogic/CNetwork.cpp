// -- -- -- -- -- -- -- 
// BLACK RUSSIA v1.2.1 by Merixton
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"Grim Russia | RED",
	"Grim Russia | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("5.39.108.50", 1, 16, 1377, false),
	CSetServer::create("5.39.108.50", 1, 16, 1377, false)	
};