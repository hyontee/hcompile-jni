#pragma once

#define MAX_SETTINGS_STRING	0x7F

struct stSettings
{
	// client
	char hostname[255];

	// gui
	int port;
};

class CSettings
{
public:
	CSettings();
	~CSettings();

	void ToDefaults(int iCategory);

	void Save(int iIgnoreCategory = 0);

	const stSettings& GetReadOnly();
	stSettings& GetWrite();
	void LoadSettings(const char* szNickName, int iChatLines = 8);
private:
	stSettings m_Settings;
};