#include "ChatMessenger.h"
#include "../main.h"
#include "../gui/gui.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../settings.h"
#include "../util/armhook.h"
// t . me / r k k d e v
extern CGUI* pGUI;
extern CChatWindow* pChatWindow;

char CLocalisation::m_szMessages[E_MSG::MSG_COUNT][MAX_LOCALISATION_LENGTH] = {
	"{bfbeba}Соединение",
	"{bfbeba}Соединено. Подготовка к игре...",
	"{bfbeba}Сервер закрыл соеденение, перезайдите",
	"{ffffff}Клиент {ff0000}BLACK RUSSIA {ffffff}запущен",
	"{bfbeba}unused",
	"{bfbeba}Вы были заблокированы сервером",
	"{bfbeba}Потеряно соединение к серверу. Переподключение через 15 секунд...",
	"{bfbeba}Отсутствует соединение. Для решения: попробуйте перезагрузить роутер, либо сменить интернет, либо воспользоваться VPN",
	"{bfbeba}Сервер полон. Повторное соединение..."
};

void CLocalisation::Initialise(const char* szFile)
{
	Log("Initializing loader | by Weikton");
	char buff[MAX_LOCALISATION_LENGTH];

	sprintf(&buff[0], "%sSAMP/%s", g_pszStorage, szFile);

	FILE* pFile = fopen(&buff[0], "r");
	if (!pFile)
	{
		Log("Localisation | Cannot initialise");
		return;
	}

	for (int i = 0; i < E_MSG::MSG_COUNT; i++)
	{
		memset(m_szMessages[i], 0, MAX_LOCALISATION_LENGTH);
	}
	uint32_t counter = 0;
	while (fgets(&buff[0], MAX_LOCALISATION_LENGTH, pFile) != NULL)
	{
		if (counter == E_MSG::MSG_COUNT)
		{
			break;
		}

		memcpy((void*)& m_szMessages[counter][0], (const void*)(&buff[0]), MAX_LOCALISATION_LENGTH);
		counter++;
	}
	fclose(pFile);
	Log("Localisation initialized");
}

char* CLocalisation::GetMessage(E_MSG msg)
{
	return &m_szMessages[msg][0];
}
