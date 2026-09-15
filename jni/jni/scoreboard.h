#pragma once

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "..//vendor/raknet/SingleProducerConsumer.h"

#include <iostream>

typedef struct _PLAYER_SCORE_INFO
{
    uint32_t dwID;
    char *szName;
    int iScore;
    int32_t dwPing;
    uint32_t dwColor;
    int iState;
} PLAYER_SCORE_INFO;

class CScoreBoard
{
public:
    int m_iOffset;
    bool m_bSorted;
    bool m_bToggle;

    CScoreBoard();
    ~CScoreBoard();

    void Draw();
	bool GetToggle();
    void Toggle();

    bool OnTouchEvent(int type, bool multi, float x, float y);

private:
	// display size
    float m_fDisplaySizeX;
    float m_fDisplaySizeY;
	// window size
    float m_fWindowSizeX;
    float m_fWindowSizeY;
	// scoreboard size
	float m_fScoreBoardSizeX;
	float m_fScoreBoardSizeY;
	// scroll
    float m_fScrollY;
    int m_iLastEndPlayer;

    // player scores
    PLAYER_SCORE_INFO *m_pPlayers;
    uint16_t m_pPlayerCount;
	
	uint32_t m_tickPressedKey;

    int m_iSelectedItem;
    int m_iLastSelectedItem;
	
    void ProcessUpdating();
};