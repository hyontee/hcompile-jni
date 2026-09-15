#include "../main.h"

#include "gui.h"
#include "buttons.h"

#include "../graphics/CInterface.h"

#include "../game/game.h"
#include "../game/crosshair.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../CSettings.h"
#include "..//scoreboard.h"
#include "../KillList.h"
#include "../CDebugInfo.h"
#include "../GButton.h"
#include "../util/CJavaWrapper.h"

extern CScoreBoard* pScoreBoard;
extern CChatWindow *pChatWindow;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern KillList *pKillList;
extern CCrossHair *pCrossHair;
extern CGame* pGame;
extern CGButton *pGButton;
extern CButtons *m_pButtons;
extern CActorPool *pActorPool;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

/*
	Все координаты GUI-элементов задаются
	относительно разрешения 1920x1080
*/
#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080

CGUI::CGUI()
{
	Log(OBFUSCATE("Initializing GUI.."));

	m_bMouseDown = 0;
	m_vTouchPos = ImVec2(-1, -1);
	m_bNextClear = false;
	m_bNeedClearMousePos = false;

	// setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplRenderWare_Init();

	// scale
	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;
	// font Size
	m_fFontSize = ScaleY( pSettings->GetReadOnly().fFontSize );

	Log(OBFUSCATE("GUI | Scale factor: %f, %f Font size: %f"), m_vecScale.x, m_vecScale.y, m_fFontSize);

	// setup style
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(25.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();

    bLabelBackground = pSettings->GetReadOnly().bLabelBg;
    timestamp = pSettings->GetReadOnly().szTimeStamp;
	bShowDebugLabels = false;
	
	// load fonts
	char path[0xFF];
	sprintf(path, OBFUSCATE("%sSAMP/fonts/%s"), g_pszStorage, pSettings->GetReadOnly().szFont);
	// cp1251 ranges
	static const ImWchar ranges[] =
	{
		0x0020, 0x0080,
		0x00A0, 0x00C0,
		0x0400, 0x0460,
		0x0490, 0x04A0,
		0x2010, 0x2040,
		0x20A0, 0x20B0,
		0x2110, 0x2130,
		0
	};

	Log(OBFUSCATE("GUI | Loading font: %s"), pSettings->GetReadOnly().szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	Log(OBFUSCATE("GUI | ImFont pointer = 0x%X"), m_pFont);

	m_pFontGTAWeap = LoadFont(OBFUSCATE("gtaweap3.ttf"), 0);
	style.WindowRounding = 0.0f;

	CRadarRect::LoadTextures();

	m_bKeysStatus = false;
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}

extern uint8_t m_SelectedServer;

#include "..//CServerManager.h"
bool g_IsVoiceServer()
{
	if (m_SelectedServer != ServerCrime && m_SelectedServer != ServerGoldRussia && m_SelectedServer != ServerBoost) return true;
	/*else
	{
		for (int i = 0; i < MAX_SERVERS; i++)
		{
			if (!strcmp(pNetGame->m_szHostOrIp, g_sEncryptedAddresses[i].decrypt()) && (pNetGame->m_iPort == g_sEncryptedAddresses[i].getPort()))
			{
				if (g_sEncryptedAddresses[i].isVoice())
				{
					return true;
				}
			}
		}
	}
	return false;*/
	return true;
}

extern float g_fMicrophoneButtonPosX;
extern float g_fMicrophoneButtonPosY;

extern uint32_t g_uiLastTickVoice;

void CGUI::PreProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	io.MousePos = m_vTouchPos;
	io.MouseDown[0] = m_bMouseDown;

	if (!m_bNeedClearMousePos && m_bNextClear)
		m_bNextClear = false;

	if (m_bNeedClearMousePos && m_bNextClear)
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}

void CGUI::PostProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (m_bNeedClearMousePos && io.MouseDown[0])
		return;

	if (m_bNeedClearMousePos && !io.MouseDown[0])
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}

void* ButtonPressed(void* g_button)
{
    for (int i = 0; i < 50; i++)
    {   
    	usleep(5000);
    	*(bool*)g_button = true;
    }
	pthread_exit(0);
	return (void*)0;
}
pthread_t threadBut;

extern uint32_t g_uiSendChatVehicleInfo;
void Render3DLabel(ImVec2 pos, char* utf8string, uint32_t dwColor);
void CGUI::Render()
{
	PreProcessInput();

	ProcessPushedTextdraws();
	if (pChatWindow)
		pChatWindow->ProcessPushedCommands();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	RenderVersion();
	RenderDebugSquare();

	if (pCrossHair)
		pCrossHair->Render();

	if (pKeyBoard)
		pKeyBoard->ProcessInputCommands();

	if (pPlayerTags) pPlayerTags->Render();
	
	if(pNetGame && pNetGame->GetLabelPool())
		pNetGame->GetLabelPool()->Draw();
	
	if (pNetGame && pNetGame->GetActorPool())
		pNetGame->GetActorPool()->Render();

    if(pKillList)
        pKillList->Render();

	if (pChatWindow)
		pChatWindow->Render();

	if (pScoreBoard)
		pScoreBoard->Draw();

	if (pKeyBoard)
		pKeyBoard->Render();

	if (pDialogWindow)
		pDialogWindow->Render();

	if (pNetGame && !pDialogWindow->m_bIsActive && pNetGame->GetGameState() == GAMESTATE_CONNECTED)
	{
		if (m_SelectedServer == ServerCrime || m_SelectedServer == ServerGoldRussia || m_SelectedServer == ServerBoost)
		{
			m_pButtons->Render();
			
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool) pGButton->RenderButton();
		}
		else if (m_SelectedServer == ServerJust)
		{
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool) 
			{
				if (pGButton->p_GButtonTexture) pGButton->RenderButton();
			}
		}
		else
		{
			if (m_SelectedServer == ServerTape)
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				if (pVehiclePool) pGButton->RenderButton();
			}
			
			ImGuiIO& io = ImGui::GetIO();
			ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 3.5, ImGui::GetFontSize() * 2.5);
			ImGui::SetNextWindowPos(ImVec2(2.0f, io.DisplaySize.y / 2.8 - vecButSize.x / 2));
			ImGui::Begin(OBFUSCATE("###keys"), nullptr,
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_AlwaysAutoResize);
				
			if (!pScoreBoard->m_bToggle) 
			{
				if (ImGui::Button(OBFUSCATE("TAB"), vecButSize))
				{
					pScoreBoard->Toggle();
				}
			}
			else
			{
				if (ImGui::Button(OBFUSCATE("TAB"), vecButSize))
				{
					pScoreBoard->Toggle();
				}
			}

			ImGui::SameLine();
			if (ImGui::Button(m_bKeysStatus ? OBFUSCATE("<<") : OBFUSCATE(">>"), vecButSize))
			{
				if (m_bKeysStatus)
					m_bKeysStatus = false;
				else m_bKeysStatus = true;
			}

			ImGui::SameLine();
			if (ImGui::Button(OBFUSCATE("Alt"), vecButSize))
			{
				CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
				if (pPlayerPool)
				{
					CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
					if(pPlayerPed->IsInVehicle()) pthread_create(&threadBut, nullptr, ButtonPressed, (void*)&LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]);
					else pthread_create(&threadBut, nullptr, ButtonPressed, (void*)&LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]);
				}
			}

			ImGui::SameLine();
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool) 
			{
				if (pGButton->p_GButtonTexture) pGButton->RenderButton();
				else
				{
					CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
					if (pPlayerPool)
					{
						CLocalPlayer* pLocalPlayer;
						if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
						{
							VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
							if (ClosetVehicleID != INVALID_VEHICLE_ID && ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
							{
								CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
								if (pVehicle)
								{
									if (pVehicle->GetDistanceFromLocalPlayerPed() < 5.0f)
									{
										if (ImGui::Button(OBFUSCATE("G"), vecButSize))
										{
											if (pNetGame)
											{
												pLocalPlayer = pPlayerPool->GetLocalPlayer();
												if (pLocalPlayer)
													pLocalPlayer->HandlePassengerEntryEx();
											}
										}
										ImGui::SameLine();
									}
								}
							}
						}
					}
				}
			}
			
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
			{
				switch (m_SelectedServer)
				{
					case ServerBoost:
					case ServerCrime:
					{
						if (ImGui::Button(OBFUSCATE("L. Ctrl"), vecButSize))
						LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;
						break;
					}
					case ServerTape:
					{
						if (ImGui::Button(OBFUSCATE("L. Ctrl"), vecButSize))
						LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;
						break;
					}
					case ServerGoldRussia:
					{
						if (ImGui::Button(OBFUSCATE("ENG"), vecButSize))
						LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
						break;
					}
				}
			}

			if (m_bKeysStatus)
			{
				ImGui::SameLine();
				if (ImGui::Button(OBFUSCATE("Y"), vecButSize))
					LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;

				ImGui::SameLine();
				if (ImGui::Button(OBFUSCATE("N"), vecButSize))
					LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;

				ImGui::SameLine();
				if (ImGui::Button(OBFUSCATE("H"), vecButSize))
					LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;
				
				ImGui::SameLine();
				if (ImGui::Button(OBFUSCATE("F"), vecButSize))
					LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
			}

			ImGui::End();
		}
	}
	
	// Debug label
	if (bShowDebugLabels)
	{
		if(pNetGame)
		{
			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			if(pVehiclePool)
			{
				for(VEHICLEID x = 0; x < MAX_VEHICLES; x++)
				{
					CVehicle *pVehicle = pVehiclePool->GetAt(x);
					if(pVehicle)
					{
						if(pVehicle->GetDistanceFromLocalPlayerPed() <= 20.0f)
						{
							MATRIX4X4 matVehicle;
							pVehicle->GetMatrix(&matVehicle);

							RwV3d rwPosition;
							rwPosition.x = matVehicle.pos.X;
							rwPosition.y = matVehicle.pos.Y;
							rwPosition.z = matVehicle.pos.Z;

							RwV3d rwOutResult;

							// CSPrite::CalcScreenCoors(RwV3d const&, RwV3d *, float *, float *, bool, bool) - 0x54EEC0
							((void (*)(RwV3d const&, RwV3d *, float *, float *, bool, bool))(g_libGTASA + 0x54EEC0 + 1))(rwPosition, &rwOutResult, 0, 0, 0, 0);
							if (g_uiSendChatVehicleInfo == 1)
							{
								Log("VehicleID: %d, rwOutResult.z: %f", x, rwOutResult.z);
							}
							if(rwOutResult.z < 1.0f)
								break;

							char szTextLabel[256];
							sprintf(szTextLabel, "[id: %d | model: %d | subtype: %d | Health: %.1f]\nDistance: %.2fm\ncPos: %.3f, %.3f, %.3f\nsPos: %.3f, %.3f, %.3f",
								x, pVehicle->GetModelIndex(), pVehicle->GetVehicleSubtype(), 
								pVehicle->GetHealth(),
								pVehicle->GetDistanceFromLocalPlayerPed(),
								matVehicle.pos.X, matVehicle.pos.Y, matVehicle.pos.Z,
								pVehiclePool->m_vecSpawnPos[x].X, pVehiclePool->m_vecSpawnPos[x].Y, pVehiclePool->m_vecSpawnPos[x].Z 
							);

							ImVec2 vecRealPos = ImVec2(rwOutResult.x, rwOutResult.y);
							Render3DLabel(vecRealPos, szTextLabel, 0x358BD4FF);
						}
					}
				}
				
				if (g_uiSendChatVehicleInfo == 1) g_uiSendChatVehicleInfo = 0;
			}
		}
	}

	if (pNetGame) // voice comment
	{
		CInterface::RenderHud();
		pNetGame->GetVehiclePool()->UpdateSpeed();
	}

	CDebugInfo::Draw();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	PostProcessInput();
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	if(!pKeyBoard->OnTouchEvent(type, multi, x, y))
		return false;

	if (!pScoreBoard->OnTouchEvent(type, multi, x, y))
		return false;

	bool bFalse = true;
	if (pNetGame)
	{
		if (pNetGame->GetTextDrawPool()->OnTouchEvent(type, multi, x, y))
		{
			if (!pChatWindow->OnTouchEvent(type, multi, x, y))
				return false;
		}
		else bFalse = false;
	}

	switch(type)
	{
		case TOUCH_PUSH:
		{
			m_vTouchPos = ImVec2(x, y);
			m_bMouseDown = true;
			m_bNeedClearMousePos = false;
			break;
		}

		case TOUCH_POP:
		{
			CheckTouchZoneForObjects();
			m_bMouseDown = false;
			m_bNeedClearMousePos = true;
			break;
		}

		case TOUCH_MOVE:
		{
			m_bNeedClearMousePos = false;
			m_vTouchPos = ImVec2(x, y);
			
			ScrollDialog(x, y);
            m_iLastPosY = y;
			break;
		}
	}

	if (!bFalse)
		return false;

	return true;
}

void CGUI::RenderVersion()
{
	return;

	ImGui::GetOverlayDrawList()->AddText(
		ImVec2(ScaleX(10), ScaleY(10)), 
		ImColor(IM_COL32_BLACK), PORT_VERSION);
}

void CGUI::ProcessPushedTextdraws()
{
	BUFFERED_COMMAND_TEXTDRAW* pCmd = nullptr;
	while (pCmd = m_BufferedCommandTextdraws.ReadLock())
	{
		RakNet::BitStream bs;
		bs.Write(pCmd->textdrawId);
		pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
		m_BufferedCommandTextdraws.ReadUnlock();
	}
}

void CGUI::ScrollDialog(float x, float y)
{
    if (m_imWindow != nullptr)
    {
        if (m_iLastPosY > y)
            ImGui::SetWindowScrollY(m_imWindow, m_imWindow->Scroll.y + ImGui::GetFontSize() / 2);

        if (m_iLastPosY < y)
            ImGui::SetWindowScrollY(m_imWindow, m_imWindow->Scroll.y - ImGui::GetFontSize() / 2);
    }
}

extern uint32_t g_uiBorderedText;
void CGUI::RenderTextForChatWindow(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	auto colOutline = ImColor(IM_COL32_BLACK);
	auto colDef = ImColor(col);

	colOutline.Value.w = colDef.Value.w;

	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
			if (colOutline.Value.w <= 0.4)
				co.Value.w = colOutline.Value.w;

			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());
			ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CGUI::PushToBufferedQueueTextDrawPressed(uint16_t textdrawId)
{
	BUFFERED_COMMAND_TEXTDRAW* pCmd = m_BufferedCommandTextdraws.WriteLock();

	pCmd->textdrawId = textdrawId;

	m_BufferedCommandTextdraws.WriteUnlock();
}

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());
			ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
			ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CGUI::RenderTextWithSize(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->GetReadOnly().iFontOutline;

    if (bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
}

ImFont* CGUI::LoadFont(char *font, float fontsize)
{
    ImGuiIO &io = ImGui::GetIO();

    // load fonts
    char path[0xFF];
    sprintf(path, OBFUSCATE("%sSAMP/fonts/%s"), g_pszStorage, font);

    // ranges
    static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x04FF, // Russia
		0x0E00, 0x0E7F, // Thai
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0xF020, 0xF0FF, // Half-width characters
		0
	};

    ImFont* pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
    return pFont;
}

void CGUI::RenderTextDeathMessage(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size, ImFont *font, bool bOutlineUseTextColor)
{
    int iOffset = bOutlineUseTextColor ? 1 : pSettings->GetReadOnly().iFontOutline;
    if(bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, col, text_begin, text_end);
}

void CGUI::AddText(ImFont* font, ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->GetReadOnly().iFontOutline;

    if (bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
}

void CGUI::RenderTags(ImVec2& posCur, ImU32 col, ImU32 col2, int outlineType, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	if (outlineType)
	{
		if (outlineType == 1)
		{
			ImVec2 a(posCur.x - 10.0f, posCur.y - 5.0f);
			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x + 10.0f, posCur.y + GetFontSize() + 5.0f);
			ImGui::GetBackgroundDrawList()->AddRectFilled(a, b, col2, 7.0f);
		}
		if (outlineType == 2)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y -= iOffset;
			
			ImVec2 a(posCur.x - 10.0f, posCur.y - 5.0f);
			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x + 10.0f, posCur.y + GetFontSize() + 5.0f);
			ImGui::GetBackgroundDrawList()->AddRect(a, b, col2, 7.0f, ImDrawCornerFlags_All, 3.0f);
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CalcScreenCoordinates(VECTOR& pos, ImVec2& out)
{
	VECTOR Out;
	((void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(SA_ADDR(0x54EEC0 + 1)))(&pos, &Out, 0, 0, 0, 0);
	out = ImVec2(Out.X - 200.0f, Out.Y - 250.0f);
}

bool isMouseHoveringSquare(const ImVec2& squarePos, float squareSize, const ImVec2& mousePos) {
	return mousePos.x >= squarePos.x && mousePos.x <= squarePos.x + squareSize &&
		   mousePos.y >= squarePos.y && mousePos.y <= squarePos.y + squareSize;
}

void RenderCircularProgressBar(ImDrawList* drawList, ImVec2 center, float size, int progressCounter, int r = 255, int g = 255, int b = 255) 
{
    float progressPercentage = static_cast<float>(progressCounter) / 100.0f;

    float radius = size * 0.5f;

    ImVec2 circleCenter = ImVec2(center.x + radius, center.y + radius);

    float startAngle = -IM_PI * 0.5f; 
    float endAngle = 2 * M_PI; 

    drawList->PathArcTo(circleCenter, radius, startAngle, endAngle, 40);
    drawList->PathStroke(IM_COL32(40, 40, 40, 170), false, 17.0f); 

    endAngle = startAngle + progressPercentage * IM_PI * 2.0f;
    drawList->PathArcTo(circleCenter, radius, startAngle, endAngle, 40 * 2);
    drawList->PathStroke(IM_COL32(r, g, b, 255), false, 16.0f); 
}

#include "../util/interfaces/CNoty.h"
extern bool showNotyBig;
extern bool showNotyBigPlayer;
ENTITY_TYPE* targetedEntity = nullptr;
bool bCustomTargeted = false;
extern int bBlockCWidgetRegionLookUpdate;
void CGUI::RenderDebugSquare()
{
	if (!pNetGame) return;
    ImGuiIO& io = ImGui::GetIO();

    static uint32_t lastRenderTime = GetTickCount();
    static int touchPushCustomCounter = 0;
    static bool bCustomGreenReached = false;
    CPlayerPed* pLocalPlayerPed = pGame->FindPlayerPed();

    if(bCustomTargeted && targetedEntity) 
    {
        if(pLocalPlayerPed->m_pEntity == targetedEntity) {
            return;
        }

        if (targetedEntity->mat) 
        {
            ImDrawList* drawList = ImGui::GetBackgroundDrawList();
            VECTOR TagPos = targetedEntity->mat->pos;

            CPlayerPed* pRemotePed = pNetGame->GetPlayerPool()->FindRemotePlayerPedFromGtaPtr((PED_TYPE*)targetedEntity);
            if(pRemotePed && pRemotePed->IsAdded())
			{
                if(pRemotePed != pLocalPlayerPed)
				{
                    pRemotePed->GetBonePosition(8, &TagPos);
                }
            }

            ImVec2 center;
            const float touchZoneSize = 450.0f;
            const float size = 90.0f;
            CalcScreenCoordinates(TagPos, center);

            if (isMouseHoveringSquare(center, touchZoneSize, ImGui::GetMousePos())) 
            {
                if(io.MouseDown[0]) 
                {
                    uint32_t currentTime = GetTickCount();
                    uint32_t elapsedTime = currentTime - lastRenderTime;
                    uint32_t updateTime = 15;
                    int maxProgress = 100;
                    if (elapsedTime >= updateTime && touchPushCustomCounter < maxProgress)
					{
                        int progressIncrement = (elapsedTime / updateTime) + 1;
                        touchPushCustomCounter += progressIncrement;
                        if (touchPushCustomCounter > maxProgress)
						{
                            touchPushCustomCounter = maxProgress; 
                        }
                        RenderCircularProgressBar(drawList, ImVec2(center.x + 150.0f, center.y - 80.0f), size, touchPushCustomCounter, 255, 255, 255);
                        lastRenderTime = currentTime;
                    }
					else
					{
                    	if(touchPushCustomCounter == maxProgress)
						{
                        	RenderCircularProgressBar(drawList, ImVec2(center.x + 150.0f, center.y - 80.0f), size, touchPushCustomCounter, 0, 255, 0);
                        	bCustomGreenReached = true;
                    	}
						else
						{
                    		RenderCircularProgressBar(drawList, ImVec2(center.x + 150.0f, center.y - 80.0f), size, touchPushCustomCounter, 255, 255, 255);
                    	}	
                    }
                }
				else
				{
                    if(bCustomGreenReached)
					{
                        //pChatWindow->AddDebugMessage("Calling radial for world entity");
                        bCustomGreenReached = false;
						
						RakNet::BitStream bsParams;
						bsParams.Write((uint8_t)ID_CUSTOM_RPC);
						bsParams.Write((uint32_t)RPC_TouchZone);

						if (GetEntityType(targetedEntity) == eEntityType::ENTITY_TYPE_PED)
						{
							if (showNotyBigPlayer && !pSettings->GetReadOnly().iButtonPlayer)
							{
								pSettings->GetWrite().iButtonPlayer = 1;
								pSettings->Save();
								showNotyBigPlayer = false;
								CNoty::hideNotyBig();
							}
							
							bsParams.Write((int32_t)eEntityType::ENTITY_TYPE_PED);
							bsParams.Write((int32_t)pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)targetedEntity));
						}
						else if (GetEntityType(targetedEntity) == eEntityType::ENTITY_TYPE_VEHICLE)
						{
							if (showNotyBig && !pSettings->GetReadOnly().iButtonVehicle)
							{
								pSettings->GetWrite().iButtonVehicle = 1;
								pSettings->Save();
								showNotyBig = false;
								CNoty::hideNotyBig();
							}
							
							bsParams.Write((int32_t)eEntityType::ENTITY_TYPE_VEHICLE);
							bsParams.Write((int32_t)pNetGame->GetVehiclePool()->FindIDFromGtaPtr((VEHICLE_TYPE*)targetedEntity));
						}
						pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
                    }
                    if (touchPushCustomCounter > 0)
					{
                        RenderCircularProgressBar(drawList, ImVec2(center.x + 150.0f, center.y - 80.0f), size, 0);
                        touchPushCustomCounter = 0;
                        lastRenderTime = GetTickCount();
                    }
                }
            }
			else
			{
            	touchPushCustomCounter = 0;
            	lastRenderTime = GetTickCount();
            }
        }
    }
}

void CGUI::CheckTouchZoneForObjects() 
{
	if(!pNetGame) return;
	ImGuiIO& io = ImGui::GetIO();

	uint32_t ms_nNoOfVisibleEntities = *(uint32_t *)(SA_ADDR(0x8C162C));
	if (!ms_nNoOfVisibleEntities) {
		return;
	}

	uintptr_t* ms_aVisibleEntityPtrs = (uintptr_t*)(SA_ADDR(0x8C0680));

	float fLeastDistance = 10000.0f;
	float fThisDistance = 0.0f;
	ENTITY_TYPE* ClosetSoFar = NULL;

	CPlayerPed* pLocalPlayerPed = pGame->FindPlayerPed();
	VECTOR vecPos;
	MATRIX4X4 matPed;
	pLocalPlayerPed->GetMatrix(&matPed);
	vecPos = matPed.pos;
	const float size = 450.0f;

	for (std::int32_t i = 0; i < ms_nNoOfVisibleEntities; ++i) 
	{
		if(!ms_aVisibleEntityPtrs[i]) {
			continue;
		}

		ENTITY_TYPE* entity = (ENTITY_TYPE*)ms_aVisibleEntityPtrs[i];
		if(!entity->mat) {
			continue;
		}

		if(pLocalPlayerPed->m_pEntity == entity) {
			continue;
		}

        if (/*GetEntityType(entity) == eEntityType::ENTITY_TYPE_OBJECT || 
        	GetEntityType(entity) == eEntityType::ENTITY_TYPE_DUMMY ||*/
        	GetEntityType(entity) == eEntityType::ENTITY_TYPE_PED && pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)entity) != INVALID_PLAYER_ID ||
        	GetEntityType(entity) == eEntityType::ENTITY_TYPE_VEHICLE)
        {
        	fThisDistance = GetDistanceBetween3DPoints(&vecPos, &entity->mat->pos);

        	if(fThisDistance < fLeastDistance)
			{
				fLeastDistance = fThisDistance;
				ClosetSoFar = entity;

				VECTOR TagPos = entity->mat->pos;

				CPlayerPed* pRemotePed = pNetGame->GetPlayerPool()->FindRemotePlayerPedFromGtaPtr((PED_TYPE*)entity);
				if(pRemotePed && pRemotePed->IsAdded())
				{
					if(pRemotePed != pLocalPlayerPed)
					{
						pRemotePed->GetBonePosition(8, &TagPos);
					}
				}
				
				ImVec2 center;
				CalcScreenCoordinates(TagPos, center);

				if (isMouseHoveringSquare(center, size, ImGui::GetMousePos()) && fThisDistance < 4.0f)
				{
					targetedEntity = ClosetSoFar;
					bCustomTargeted = true;
					bBlockCWidgetRegionLookUpdate = 1;
				}
				else
				{
					bBlockCWidgetRegionLookUpdate = 0;
					bCustomTargeted = false;
					targetedEntity = nullptr;
				}
			}
        }
	}
}