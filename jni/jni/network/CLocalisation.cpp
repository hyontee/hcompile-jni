#include "CLocalisation.h"
#include "../main.h"
#include "../gui/gui.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../settings.h"
#include "../util/armhook.h"

extern CGUI* pGUI;
extern CChatWindow* pChatWindow;

char CLocalisation::m_szMessages[E_MSG::MSG_COUNT][MAX_LOCALISATION_LENGTH] = {
	"Соединение к BLACK RUSSIA...",
	"Соединено. Подготовка к игре...",
	"Сервер закрыл соеденение, перезайдите...",
	"{ffffff}Клиент {ff0000}BLACK RUSSIA {ffffff}запущен",
	"unused",
	"Вы были заблокированы сервером. Выйдите из игры и подключитесь заново.",
	"Потеряно соеденение с сервером, переподключение через пару секунд...",
	"Потеряно соеденение с сервером, переподключение через пару секунд...",
	"Сервер полон. Переподключение..."
};

void CLocalisation::Initialise(const char* szFile)
{
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
