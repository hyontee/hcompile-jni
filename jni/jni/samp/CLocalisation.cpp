#include "CLocalisation.h"

#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/RW/RenderWare.h"
#include "chatwindow.h"
#include "playertags.h"
#include "keyboard.h"
#include "CSettings.h"


char CLocalisation::m_szMessages[E_MSG::MSG_COUNT][MAX_LOCALISATION_LENGTH] = {
	"Соединение к LIVE RUSSIA",
	"Соединено. Подготовка к игре...",
	"Сервер закрыл соеденение, перезайдите",
	"{bbbbbb}Клиент {ff0000}LIVE RUSSIA{bbbbbb} запущен",
	"unused",
	"Вы были заблокированы сервером",
	"Потеряно соеденение с сервером, переподключение...",
	"Проблемы с сетью, переподключение...",
	"Сервер полон. Переподключение к LIVE RUSSIA...",
                  "{FFFF00}| {FFFFFF}Вы находитесь в очереди на подключении к серверу: {FFFF00}1"
};

void CLocalisation::Initialise(const char* szFile)
{
	char buff[MAX_LOCALISATION_LENGTH];

	sprintf(&buff[0], "%sSAMP/%s", g_pszStorage, szFile);

	FILE* pFile = fopen(&buff[0], "r");
	if (!pFile)
	{
		Log("Localisation | Cannot initialise %s", szFile);
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
