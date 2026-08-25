#include "BRClient.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"Moscow Russia | Red",
	"Moscow Russia | TEST"
                  // 2 = 1 !!! 5.39.108.49:7777
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("141.95.190.146", 1, 16, 1209, false),
	CSetServer/*launch*/::create("141.95.190.146", 1, 16, 1209, false)
				
};
