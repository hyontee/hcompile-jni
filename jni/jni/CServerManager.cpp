#include "CServerManager.h"
#include "dialog.h"

#include "main.h"

extern CDialogWindow *pDialogWindow;

#if defined SCrime
const char* g_szServerNames[MAX_SERVERS] = 
{
	OBFUSCATE("{ff9729}[1]{ffffff} Crime RP | Server: 01"),
	OBFUSCATE("{ff9729}[2]{ffffff} Crime RP | Server: 02"),
	OBFUSCATE("{ff9729}[3]{ffffff} Crime RP | Server: Test")
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = 
{
	CServerInstance::create("65.108.99.34", 1, 19, 1773, true),
	CServerInstance::create("65.108.99.34", 1, 19, 1773, true),
	CServerInstance::create(NUM_TO_STR_IP(65, 108, 99, 34), 16, 1773, true)
};
#endif

#if defined SGoldRussia
const char* g_szServerNames[MAX_SERVERS] = 
{
	OBFUSCATE("{ff9729}[1]{ffffff} Gold Russia RP | Server: 01"),
	OBFUSCATE("{ff9729}[2]{ffffff} Gold Russia RP | Server: Test")
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = 
{
	CServerInstance::create("play.gold-russia.ru", 1, 20, 7777, false),
	CServerInstance::create(NUM_TO_STR_IP(46, 174, 49, 47), 16, 7823, true)
};
#endif

void CServerInstance::ShowDialog()
{
	pDialogWindow->Clear();

	pDialogWindow->m_wDialogID = 0xFFF7;
	pDialogWindow->m_byteDialogStyle = DIALOG_STYLE_LIST;

	cp1251_to_utf8(pDialogWindow->m_utf8Title, "Выбор сервера");
	cp1251_to_utf8(pDialogWindow->m_utf8Button1, "Выбрать");
	cp1251_to_utf8(pDialogWindow->m_utf8Button2, "");

	char szBuff[400];
	memset(szBuff, 0, sizeof(szBuff));
	for (auto & g_szServerName : g_szServerNames)
	{
		strcat(&szBuff[0], g_szServerName);
		strcat(&szBuff[0], OBFUSCATE("\n"));
	}

	pDialogWindow->SetInfo(szBuff, strlen(szBuff));
	pDialogWindow->Show(true);
}