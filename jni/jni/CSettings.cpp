#include "main.h"
#include "CSettings.h"
#include "game/game.h"
#include "vendor/ini/config.h"
#include "game/CAdjustableHudColors.h"
#include "game/CAdjustableHudPosition.h"
#include "game/CAdjustableHudScale.h"
#include "gtasa/JSONParser.h"

static void ClearBackslashN(char* pStr, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (pStr[i] == '\n' || pStr[i] == 13)
        {
            pStr[i] = 0;
        }
    }
}

CSettings::CSettings()
{
    LoadSettings(nullptr);
}

CSettings::~CSettings()
{
}

void CSettings::ToDefaults(int iCategory)
{
    char buff[0x7F];
    sprintf(buff, "%smultiplayer/settings.json", g_pszStorage);

    FILE* pFile = fopen(buff, "w");
    if (pFile)
    {
        fprintf(pFile, "{\"settings\": {\"Font\": \"Arial.ttf\", \"FontSize\": 30.0, \"FontOutline\": 2}}");
        fclose(pFile);
    }

    Save(iCategory);
    LoadSettings(m_Settings.szNickName);
}

void CSettings::Save(int iIgnoreCategory)
{
    char buff[0x7F];
    sprintf(buff, "%smultiplayer/settings.json", g_pszStorage);

    JSONParser parser(buff);

    parser.addString("client.name", m_Settings.szNickName);
    parser.addString("client.password", m_Settings.szPassword);
    parser.addString("settings.Font", m_Settings.szFont);

    parser.addFloat("settings.FontSize", m_Settings.fFontSize);
    parser.addInt("settings.FontOutline", m_Settings.iFontOutline);

    parser.addFloat("settings.HealthBarWidth", m_Settings.fHealthBarWidth);
    parser.addFloat("settings.HealthBarHeight", m_Settings.fHealthBarHeight);

    if (iIgnoreCategory != 2)
    {
        parser.addFloat("settings.CameraCycleSize", m_Settings.fButtonCameraCycleSize);
        parser.addFloat("settings.CameraCycleX", m_Settings.fButtonCameraCycleX);
        parser.addFloat("settings.CameraCycleY", m_Settings.fButtonCameraCycleY);
    }

    parser.addInt("settings.fps", m_Settings.iFPS);

    if (iIgnoreCategory != 1)
    {
        parser.addInt("settings.cutout", m_Settings.iCutout);
        parser.addInt("settings.fpscounter", m_Settings.iFPSCounter);
        parser.addInt("settings.radarrect", m_Settings.iRadarRect);
        parser.addInt("settings.hud", m_Settings.iHud);
    }

    parser.saveToFile();
}

const stSettings& CSettings::GetReadOnly()
{
    return m_Settings;
}

stSettings& CSettings::GetWrite()
{
    return m_Settings;
}

void CSettings::LoadSettings(const char* szNickName)
{
    char tempNick[40];
    if (szNickName)
    {
        strcpy(tempNick, szNickName);
    }

    Log("Loading settings..");

    char buff[0x7F];
    sprintf(buff, "%smultiplayer/settings.json", g_pszStorage);



    JSONParser parser(buff);
    if (!parser.loadFile())
    {
        Log("Cannot load settings, exiting...");
        std::terminate();
        return;
    }

    snprintf(m_Settings.szNickName, sizeof(m_Settings.szNickName), "noname_%d%d", rand() % 1000, rand() % 1000);
    memset(m_Settings.szPassword, 0, sizeof(m_Settings.szPassword));
    snprintf(m_Settings.szFont, sizeof(m_Settings.szFont), "arial.ttf");

    std::string strName = parser.getValue("client.name");
    std::string strPassword = parser.getValue("client.password");
    std::string strFont = parser.getValue("settings.Font");

    const char* szName = strName.c_str();
    const char* szPassword = strPassword.c_str();
    const char* szFontName = strFont.c_str();

    if (szName)
    {
        strcpy(m_Settings.szNickName, szName);
    }
    if (szPassword)
    {
        strcpy(m_Settings.szPassword, szPassword);
    }
    if (szFontName)
    {
        strcpy(m_Settings.szFont, szFontName);
    }

    ClearBackslashN(m_Settings.szNickName, sizeof(m_Settings.szNickName));
    ClearBackslashN(m_Settings.szPassword, sizeof(m_Settings.szPassword));
    ClearBackslashN(m_Settings.szFont, sizeof(m_Settings.szFont));

    if (szNickName)
    {
        strcpy(m_Settings.szNickName, tempNick);
    }

    m_Settings.fFontSize = parser.getValueAsFloat("settings.FontSize", 30.0f);
    m_Settings.iFontOutline = parser.getValueAsInt("settings.FontOutline", 2);

    m_Settings.fHealthBarWidth = parser.getValueAsFloat("settings.HealthBarWidth", 60.0f);
    m_Settings.fHealthBarHeight = parser.getValueAsFloat("settings.HealthBarHeight", 10.0f);

    m_Settings.fButtonCameraCycleX = parser.getValueAsFloat("settings.CameraCycleX", 1810.0f);
    m_Settings.fButtonCameraCycleY = parser.getValueAsFloat("settings.CameraCycleY", 260.0f);
    m_Settings.fButtonCameraCycleSize = parser.getValueAsFloat("settings.CameraCycleSize", 90.0f);

    m_Settings.iFPS = parser.getValueAsInt("settings.fps", 60);

    m_Settings.iCutout = parser.getValueAsInt("settings.cutout", 0);

    m_Settings.iFPSCounter = parser.getValueAsInt("settings.fpscounter", 1);
    m_Settings.iRadarRect = parser.getValueAsInt("settings.radarrect", 0);
    m_Settings.iHud = parser.getValueAsInt("settings.hud", 1);
    m_Settings.iDialog = parser.getValueAsInt("settings.dialog", 1);
}