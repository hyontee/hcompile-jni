// -- -- -- -- -- -- -- 
// by weikton x reytiz
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"VOLK RUSSIA | #1",
	"VOLK RUSSIA | #2"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.8", 1, 16, 1291, false),
	CSetServer::create("188.127.241.8", 1, 16, 1380, false)	
};