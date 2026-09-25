#include "config.h"
#include "settings.h"
#include <cstring>   // Для strncpy(), strlen()
#include <cstdio>    // Для fopen(), fwrite(), fclose(), fprintf()
#include <cstdlib>   // Для remove(), abort()

// Функция очистки символа переноса строки ('\n', '\r')
static void ClearBackslashN(char* pStr, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (pStr[i] == '\n' || pStr[i] == '\r') {
            pStr[i] = 0;
        }
    }
}

CSettings::CSettings() {
    LoadSettings("", 8); // Передача дефолтных аргументов
}

CSettings::~CSettings() {}

// Установка настроек по умолчанию и сохранение их в файл
void CSettings::ToDefaults(int iCategory) {
    const char *filePath = "/storage/emulated/0/bruniv.ini";
    
    FILE* pFile = fopen(filePath, "w");
    if (pFile == NULL) {
        fprintf(stderr, "Ошибка открытия файла '%s'\n", filePath);
		return;
        //abort(); // Вызов стандартной функции завершения процесса
    }

    fwrite("[gui]\n", 1, strlen("[gui]\n"), pFile);
    fclose(pFile);

    Save(iCategory);
    LoadSettings("", 8); // Обновляем загрузку настроек
}

// Сохранение текущих настроек в INI-файл
void CSettings::Save(int iIgnoreCategory) {
    const char *filePath = "/storage/emulated/0/Android/data/com.br.top/bruniv.ini";
    //remove(filePath); // Удаляем существующий файл

    ini_table_s* config = ini_table_create();

    // Создаем записи в таблице инициализации
    ini_table_create_entry(config, "client", "host", m_Settings.hostname);
    ini_table_create_entry_as_int(config, "client", "port", m_Settings.port);

    // Запись таблицы в файл
    ini_table_write_to_file(config, filePath);
    ini_table_destroy(config);
}

// Доступ к неизменяемым настройкам
const stSettings& CSettings::GetReadOnly() {
    return m_Settings;
}

// Доступ к изменяемым настройкам
stSettings& CSettings::GetWrite() {
    return m_Settings;
}

void CSettings::LoadSettings(const char* szNickName, int iChatLines) {
    // Чтение имени хоста
	ini_table_s* config = ini_table_create();
	char buff[0x7F];
	sprintf(buff, "%sbruniv.ini", "/storage/emulated/0/Android/data/com.br.top/");

	if (!ini_table_read_from_file(config, buff))
	{
		return;
	}
    const char* hostname = ini_table_get_entry(config, "client", "host");
    if (hostname != nullptr) {
        strncpy(m_Settings.hostname, hostname, sizeof(m_Settings.hostname)-1);
        m_Settings.hostname[sizeof(m_Settings.hostname)-1] = '\0';
    } else {
        snprintf(m_Settings.hostname, sizeof(m_Settings.hostname), "localhost"); // По умолчанию ставим localhost
    }

    // Очищаем строку от ненужных символов
    ClearBackslashN(m_Settings.hostname, sizeof(m_Settings.hostname));
	
	ini_table_s* config1 = ini_table_create();
	sprintf(buff, "%sbruniv.ini", "/storage/emulated/0/Android/data/com.br.top/");

	if (!ini_table_read_from_file(config1, buff))
	{
		return;
	}

    // Чтение номера порта
	m_Settings.port = ini_table_get_entry_as_int(config1, "client", "port", -1); // Задали разумное значение по умолчанию
    

    // Сообщим о результатах загрузки

    ini_table_destroy(config);
}