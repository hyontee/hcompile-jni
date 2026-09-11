//Manager
#include "CServerManager.h"
#include "dialog.h"

#include "main.h"
#include "util/CJavaWrapper.h"

extern CDialogWindow *pDialogWindow;

const char* g_szServerNames[MAX_SERVERS] = {
	OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA"),
	OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA"),
	OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA"),
	OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA"),
	OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA"),
	OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA"),
	OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA"),
	OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA"),
    OBFUSCATE("{ff9729}[1]{ffffff} FIRE RUSSIA")
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 28, 1545, false), // 0
	CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 28, 1545, false),
	CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 28, 1545, false),
	CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 28, 1545, false),
	CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 30, 1545, false), // 0
	CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 28, 1545, false),
	CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 28, 1545, false),
	CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 28, 1545, false),
    CServerInstance::create(OBFUSCATE("80.242.59.112"), 1, 28, 1545, false)
};


