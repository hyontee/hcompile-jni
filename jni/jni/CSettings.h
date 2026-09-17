#pragma once

#include "main.h"

#define MAX_SETTINGS_STRING	0x7F

struct stSettings
{
	// client
	char szNickName[MAX_PLAYER_NAME+1];
	char szPassword[MAX_SETTINGS_STRING+1];

	// gui
	char szFont[40];
	float fFontSize;
	int iFontOutline;

	float fHealthBarWidth;
	float fHealthBarHeight;
	int iFPS;
	float fButtonCameraCycleX;
	float fButtonCameraCycleY;
	float fButtonCameraCycleSize;
	int iAndroidKeyboard;
	int iCutout;
	int iFPSCounter;
	int iRadarRect;
	int iDialog;
	int iHud;
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
	void LoadSettings(const char* szNickName);
private:
	stSettings m_Settings;
};