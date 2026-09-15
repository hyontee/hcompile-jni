#include "main.h"
#include "scoreboard.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
#include "CSettings.h"

#include "../util/interfaces/CTab.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

extern CNetGame* pNetGame;
extern CGame* pGame;
extern CGUI* pGUI;
extern CSettings* pSettings;

ImGuiWindowFlags fScoreBoardFlags =
ImGuiWindowFlags_NoMove |
ImGuiWindowFlags_NoResize |
ImGuiWindowFlags_NoSavedSettings |
ImGuiWindowFlags_NoCollapse;

CScoreBoard::CScoreBoard()
{
    m_iOffset = 0;
    m_bSorted = false;
    m_bToggle = false;
    m_iLastEndPlayer = 0;
    m_fScrollY = 0.0f;

    m_fScoreBoardSizeX = pGUI->ScaleX(pSettings->GetReadOnly().fScoreBoardSizeX);
    m_fScoreBoardSizeY = pGUI->ScaleY(pSettings->GetReadOnly().fScoreBoardSizeY);

    m_pPlayers = 0;
    m_pPlayerCount = 0;
}

CScoreBoard::~CScoreBoard() {}

void SwapPlayerInfo(PLAYER_SCORE_INFO* psi1, PLAYER_SCORE_INFO* psi2)
{
    PLAYER_SCORE_INFO plrinf;
    memcpy(&plrinf, psi1, sizeof(PLAYER_SCORE_INFO));
    memcpy(psi1, psi2, sizeof(PLAYER_SCORE_INFO));
    memcpy(psi2, &plrinf, sizeof(PLAYER_SCORE_INFO));
}

void CScoreBoard::Draw()
{
    ProcessUpdating();

    if(!m_bToggle) return;
    if(!m_pPlayers) return;

    PLAYERID endplayer = m_pPlayerCount;
    /*char szPlayerId[11], szScore[11], szPing[11], szServerAddress[512];
    unsigned char RGBcolors[3];
    char ctitle[128];

    std::string serverUTF8 = pNetGame->m_szHostName;

    cp1251_to_utf8(ctitle, serverUTF8.c_str());
    sprintf(szServerAddress, OBFUSCATE("%s | Игроков: %d"), ctitle, m_pPlayerCount);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 size = ImGui::GetWindowSize();

    ImGui::SetNextWindowSize(ImVec2(m_fScoreBoardSizeX, m_fScoreBoardSizeY), NULL);
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    m_fWindowSizeX = ImGui::GetWindowSize().x;
    m_fWindowSizeY = ImGui::GetWindowSize().y;

    m_fDisplaySizeX = io.DisplaySize.x;
    m_fDisplaySizeY = io.DisplaySize.y;

    ImGui::Begin(szServerAddress, NULL, fScoreBoardFlags);

    ImGui::Columns(5, OBFUSCATE("Scoreboard"));

    ImGui::Text("ID"); ImGui::SetColumnWidth(-1, m_fScoreBoardSizeX / 100 * 10); ImGui::NextColumn();
    ImGui::Text("Никнейм"); ImGui::SetColumnWidth(-1, m_fScoreBoardSizeX / 100 * 35); ImGui::NextColumn();
    ImGui::Text("Уровень"); ImGui::SetColumnWidth(-1, m_fScoreBoardSizeX / 100 * 15); ImGui::NextColumn();
    ImGui::Text("Пинг"); ImGui::SetColumnWidth(-1, m_fScoreBoardSizeX / 100 * 15); ImGui::NextColumn();
    ImGui::Text("Голосовой чат"); ImGui::SetColumnWidth(-1, m_fScoreBoardSizeX / 100 * 20); ImGui::NextColumn();

	ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = (ImVec4)ImColor(119, 4, 4, 255);
	ImGui::GetStyle().Colors[ImGuiCol_Header] = ImGui::GetStyle().Colors[ImGuiCol_HeaderActive];
	ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImGui::GetStyle().Colors[ImGuiCol_HeaderActive];

	ImGui::GetStyle().WindowPadding = ImVec2(15, 15);
    ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().FramePadding = ImVec2(4, 4);
    ImGui::GetStyle().FrameRounding = 15.0f;
    ImGui::GetStyle().ItemSpacing = ImVec2(12, 8);
    ImGui::GetStyle().ItemInnerSpacing = ImVec2(8, 6);
    ImGui::GetStyle().IndentSpacing = 25.0f;
    ImGui::GetStyle().GrabMinSize = 5.0f;
    ImGui::GetStyle().GrabRounding = 1.0f;
	
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 0.90f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);*/
	
	for(uint32_t line = m_iOffset; line < endplayer; line++)
    {
        if (!m_bToggle || !m_pPlayers) break; // crash fix
		
		/*int tmpTabId = 0;
        ImVec2 differentOffsets;
        if(tmpTabId == 0)
        {
            ImGui::PushID(tmpTabId+line);

            std::stringstream ss;
            ss << line+tmpTabId;
            std::string s = ss.str();

            std::string itemid = "##" + s;
            bool is_selected = (m_iSelectedItem == line);

            if(ImGui::Selectable(itemid.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick))
				if (ImGui::IsMouseDoubleClicked(0) && m_iSelectedItem == line) // fix double clicked
				{
					RakNet::BitStream bsSend;
					bsSend.Write(m_pPlayers[line].dwID);
					bsSend.Write(0); // 0 from scoreboard - 1 from ? - 2 from ?
					pNetGame->GetRakClient()->RPC(&RPC_ClickPlayer, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
				   
					if(m_bToggle) m_bToggle = false;
				}

            if(ImGui::IsItemHovered())
                m_iSelectedItem = line;

            if(is_selected) 
                ImGui::SetItemDefaultFocus();

            ImGui::SameLine(); 
        }

        sprintf(szPlayerId, "%d", m_pPlayers[line].dwID);
        sprintf(szScore, "%d", m_pPlayers[line].iScore);
        sprintf(szPing, "%d", m_pPlayers[line].dwPing);
        
        RGBcolors[0] = (m_pPlayers[line].dwColor - 0xFF000000) >> 16;
        RGBcolors[1] = ((m_pPlayers[line].dwColor - 0xFF000000) & 0x00ff00) >> 8;
        RGBcolors[2] = ((m_pPlayers[line].dwColor - 0xFF000000) & 0x0000ff);

        ImGui::TextColored(ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), "%s", szPlayerId);
        ImGui::NextColumn();

        ImGui::TextColored(ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), "%s", m_pPlayers[line].szName);
        ImGui::NextColumn();

        ImGui::TextColored(ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), "%s", szScore);
        ImGui::NextColumn();

        ImGui::TextColored(ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), "%s", szPing);
        ImGui::NextColumn();
		
		
		int m_StatusVoice = pVoice->StatusMutePlayer(line);
		if (m_StatusVoice == 0) ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(2.55, 0.00f, 0.00f, 0.80f);
		else ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.50f, 2.05f, 0.50f, 0.80f);
	
		ImGui::SliderInt("", &m_StatusVoice, 0, 1);
		if (pVoice->StatusMutePlayer(line) != m_StatusVoice)
		{
			if (m_StatusVoice == 0) pVoice->MutePlayer(line);
			else pVoice->UnMutePlayer(line);
		}
		ImGui::NextColumn();*/
    }
	
	/*pGUI->m_imWindow = ImGui::GetCurrentWindow();
    ImGui::Columns(1);
    ImGui::End();*/
}


bool CScoreBoard::GetToggle()
{
	return m_bToggle;
}

void CScoreBoard::Toggle()
{
	if ((GetTickCount() - m_tickPressedKey) < 500)
    {
        return;
    }
	
	m_tickPressedKey = GetTickCount();
	
    m_bToggle = !m_bToggle;
    if (m_bToggle)
    {
        // Freeze player
		if (pGame)
		{
			pGame->FindPlayerPed()->TogglePlayerControllableWithoutLock(false);
			*(uint8_t*)(g_libGTASA+0x8EF36B) = 0;
		}
		
		pGUI->m_imWindow = nullptr;

        // Get player list
        pNetGame->UpdatePlayerScoresAndPings();

        CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
        PLAYERID playercount = pPlayerPool->GetCount() + 1;
        m_pPlayerCount = playercount;

        if (m_iOffset > (playercount - 20)) m_iOffset = (playercount - 20);
        if (m_iOffset < 0) m_iOffset = 0;

        m_pPlayers = (PLAYER_SCORE_INFO*)malloc(playercount * sizeof(PLAYER_SCORE_INFO));
        memset(m_pPlayers, 0, playercount * sizeof(PLAYER_SCORE_INFO));
        m_pPlayers[0].dwID = pPlayerPool->GetLocalPlayerID();
        m_pPlayers[0].szName = pPlayerPool->GetLocalPlayerName();
        m_pPlayers[0].iScore = pPlayerPool->GetLocalPlayerScore();
        m_pPlayers[0].dwPing = pPlayerPool->GetLocalPlayerPing();
        m_pPlayers[0].dwColor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsARGB();
        PLAYERID i = 1;
		
		CTab::ClearTAB();
		CTab::SetTABStats(m_pPlayers[0].dwID, m_pPlayers[0].szName, m_pPlayers[0].iScore, m_pPlayers[0].dwPing);
        for (int x = 0; x < MAX_PLAYERS; x++)
        {
            if (!pPlayerPool->GetSlotState(x)) continue;
            m_pPlayers[i].dwID = x;
            m_pPlayers[i].szName = pPlayerPool->GetPlayerName(x);
            m_pPlayers[i].iScore = pPlayerPool->GetRemotePlayerScore(x);
            m_pPlayers[i].dwPing = pPlayerPool->GetRemotePlayerPing(x);
            m_pPlayers[i].dwColor = pPlayerPool->GetAt(x)->GetPlayerColorAsARGB();
            m_pPlayers[i].iState = (int)pPlayerPool->GetAt(x)->GetState();
			
			CTab::SetTABStats(m_pPlayers[i].dwID, m_pPlayers[i].szName, m_pPlayers[i].iScore, m_pPlayers[i].dwPing);

            i++;
        }
		CTab::ShowTAB();

        if (m_bSorted)
        {
            for (i = 0; i < playercount - 1; i++)
            {
                for (PLAYERID j = 0; j < playercount - 1 - i; j++)
                {
                    if (m_pPlayers[j + 1].iScore > m_pPlayers[j].iScore)
                    {
                        SwapPlayerInfo(&m_pPlayers[j], &m_pPlayers[j + 1]);
                    }
                }
            }
        }
    }
    else
    {
		ImGui::GetStyle().WindowPadding = ImVec2(8, 8);
		ImGui::GetStyle().WindowRounding = 7.0f;
		ImGui::GetStyle().FramePadding = ImVec2(4, 3);
		ImGui::GetStyle().FrameRounding = 0.0f;
		ImGui::GetStyle().ItemSpacing = ImVec2(8, 4);
		ImGui::GetStyle().ItemInnerSpacing = ImVec2(4, 4);
		ImGui::GetStyle().IndentSpacing = 21.0f;
		ImGui::GetStyle().GrabMinSize = 10.0f;
		ImGui::GetStyle().GrabRounding = 0.0f;
		
		ImGui::StyleColorsDark();
		CTab::HideTAB();
		
        // Unfreeze player
		pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllableWithoutLock(true);
        *(uint8_t*)(g_libGTASA+0x8EF36B) = m_bToggle;

        if (m_pPlayers)
        {
            memset(m_pPlayers, 0, m_pPlayerCount * sizeof(PLAYER_SCORE_INFO));
            free(m_pPlayers);
        }
        m_pPlayers = 0;
    }
}

bool CScoreBoard::OnTouchEvent(int type, bool multi, float x, float y)
{
    if (!m_bToggle) return true;

    static bool bWannaCloseTab = false;

    switch (type)
    {
    case TOUCH_PUSH:
        if (
            x < (m_fDisplaySizeX * 0.5f - m_fWindowSizeX) || x >(m_fDisplaySizeX * 0.5f + m_fWindowSizeX) ||
            y < (m_fDisplaySizeY * 0.5f - m_fWindowSizeY) || y >(m_fDisplaySizeY * 0.5f + m_fWindowSizeY)
            )
        {
            bWannaCloseTab = true;
        }
        break;
    case TOUCH_POP:
        if (
            bWannaCloseTab &&
            x < (m_fDisplaySizeX * 0.5f - m_fWindowSizeX) || x >(m_fDisplaySizeX * 0.5f + m_fWindowSizeX) ||
            y < (m_fDisplaySizeY * 0.5f - m_fWindowSizeY) || y >(m_fDisplaySizeY * 0.5f + m_fWindowSizeY)
            )
        {
            //Toggle();
            bWannaCloseTab = false;
        }
        break;
    }
    return true;
}

static uint32_t m_tickProcessingUpdate = GetTickCount();
void CScoreBoard::ProcessUpdating()
{
    if (pNetGame)
    {
        if ((GetTickCount() - m_tickProcessingUpdate) >= 5000)
        {
            m_tickProcessingUpdate = GetTickCount();
            pNetGame->UpdatePlayerScoresAndPings();
        }
    }
}
