//Manager
#include "CServerManager.h"
#include "dialog.h"

#include "main.h"
#include "util/CJavaWrapper.h"

extern CDialogWindow *pDialogWindow;

const char* g_szServerNames[MAX_SERVERS] = {
	OBFUSCATE("{ff9729}[1]{ffffff} Stage Mobile BY REIJI"),
	OBFUSCATE("{ff9729}[1]{ffffff} Stage Mobile"),
	OBFUSCATE("{ff9729}[1]{ffffff} Stage Mobile"),
	OBFUSCATE("{ff9729}[1]{ffffff} Stage Mobile"),
	OBFUSCATE("{ff9729}[1]{ffffff} Test1"),
	OBFUSCATE("{ff9729}[1]{ffffff} Test12"),
	OBFUSCATE("{ff9729}[1]{ffffff} Test123"),
	OBFUSCATE("{ff9729}[1]{ffffff} Test1234"),
    OBFUSCATE("{ff9729}[1]{ffffff} Test12345")
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 28, 1312, false), // 0
	CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 28, 1312, false),
	CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 28, 1312, false),
	CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 28, 1312, false),
	CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 30, 1312, false), // 0
	CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 28, 1312, false),
	CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 28, 1312, false),
	CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 28, 1312, false),
    CServerInstance::create(OBFUSCATE("188.127.241.8"), 1, 28, 1312, false)
};


