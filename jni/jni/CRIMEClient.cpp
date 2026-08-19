// -- -- -- -- -- -- -- 
// Client v3 by westonov
// -- -- -- -- -- -- --
#include "CRIMEClient.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"
// 2 = 1 (времено, пока не добавили сервера)

const char* g_szServerNames[MAX_SERVERS] = {
	"Westonov | Test client",
	"Crime Mobile | RED"
                  // 2 = 1 IP:PORT
				   
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.74", 1, 16, 1825, false),
	CSetServer::create("188.127.241.74", 1, 16, 1825, false)		
};