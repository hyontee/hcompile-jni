#include "../main.h"
#include "gui.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../CSettings.h"
#include "..//scoreboard.h"


extern CScoreBoard* pScoreBoard;
extern CChatWindow *pChatWindow;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;

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
	Log("Initializing GUI..");

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

	Log("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);

	// setup style
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();

	// load fonts
	char path[0xFF];
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, pSettings->GetReadOnly().szFont);
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
	Log("GUI | Loading font: %s", pSettings->GetReadOnly().szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	Log("GUI | ImFont pointer = 0x%X", m_pFont);

	style.WindowRounding = 0.0f;

	m_pSplashTexture = nullptr;

	m_pSplashTexture = (RwTexture*)LoadTextureFromDB("txd", "splash_icon");

	CRadarRect::LoadTextures();

	m_bKeysStatus = false;
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}
#include "..//CServerManager.h"
bool g_IsVoiceServer()
{
	return true;
}

extern float g_fMicrophoneButtonPosX;
extern float g_fMicrophoneButtonPosY;
extern uint32_t g_uiLastTickVoice;
#include "..//voice/CVoiceChatClient.h"
extern CVoiceChatClient* pVoice;

void CGUI::PreProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	io.MousePos = m_vTouchPos;
	io.MouseDown[0] = m_bMouseDown;
	if (!m_bNeedClearMousePos && m_bNextClear)
	{
		m_bNextClear = false;
	}
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
	{
		return;
	}

	if (m_bNeedClearMousePos && !io.MouseDown[0])
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}
#include "..//CDebugInfo.h"
extern CGame* pGame;
void CGUI::Render()
{
	PreProcessInput();

	ProcessPushedTextdraws();
	if (pChatWindow)
	{
		pChatWindow->ProcessPushedCommands();
	}

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	RenderVersion();
	//RenderRakNetStatistics();

	if (pKeyBoard)
	{
		pKeyBoard->ProcessInputCommands();
	}

	if (pPlayerTags) pPlayerTags->Render();
	
	if(pNetGame && pNetGame->GetLabelPool())
	{
		pNetGame->GetLabelPool()->Draw();
	}

	if (pChatWindow) pChatWindow->Render();
	if (pScoreBoard) pScoreBoard->Draw();
	if (pKeyBoard) pKeyBoard->Render();
	if (pDialogWindow) pDialogWindow->Render();

	if (pNetGame && !pDialogWindow->m_bIsActive && pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 3.5, ImGui::GetFontSize() * 2.5);
		ImGui::SetNextWindowPos(ImVec2(2.0f, io.DisplaySize.y / 2.8 - vecButSize.x / 2));
		ImGui::Begin("###keys", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::GetStyle().Colors[ImGuiCol_Button] = (ImVec4)ImColor(47, 103, 177, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = (ImVec4)ImColor(47, 103, 177, 255);
		if (ImGui::Button(m_bKeysStatus ? "<<" : ">>", vecButSize))
		{
			if (m_bKeysStatus)
				m_bKeysStatus = false;
			else
				m_bKeysStatus = true;
		}


		ImGui::SameLine();
		
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		if (pVehiclePool)
		{
			VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
			if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
			{
				CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
				if (pVehicle)
				{
					if (pVehicle->GetDistanceFromLocalPlayerPed() < 5.0f)
					{
						CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
						if (pPlayerPool)
						{
							CLocalPlayer* pLocalPlayer;
							if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
							{
								if (ImGui::Button("G", vecButSize))
							{
								CPlayerPool *pPlayerPool;
								CLocalPlayer *pLocalPlayer;

								if(pNetGame)
								{
									pPlayerPool = pNetGame->GetPlayerPool();
									if(pPlayerPool)
									{
										pLocalPlayer = pPlayerPool->GetLocalPlayer();
										if(pLocalPlayer)
										{
											pLocalPlayer->HandlePassengerEntryEx();
										}
									}
								}
							}
							ImGui::SameLine();
						}
					}
				}
			}
		}
	}	
		if (m_bKeysStatus)
		{
			ImGui::SameLine();
			if (ImGui::Button("ALT", vecButSize))
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				CLocalPlayer* pLocalPlayer;
				if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
					LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
				else
					LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
			}
		}

		ImGui::SameLine();
		if(!pScoreBoard->m_bToggle) {
			if (ImGui::Button("TAB", vecButSize))
			{
				pScoreBoard->Toggle();
			}
		}
		else {
			if (ImGui::Button("X", vecButSize))
			{
				pScoreBoard->Toggle();
			}
		}

		ImGui::SameLine();
			if (ImGui::Button("Y", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
			ImGui::SameLine();
			if (ImGui::Button("N", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
			ImGui::SameLine();
			if (ImGui::Button("F", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;

		}
		ImGui::End();
	}

	if (pNetGame)
	{
		if (pVoice && g_IsVoiceServer())
		{
			if (pVoice->IsRecording() && GetTickCount() - g_uiLastTickVoice >= 20000)
			{
				char buf[64];
				sprintf(&buf[0], "%d", (int)((30000 - (GetTickCount() - g_uiLastTickVoice)) / 1000) + 1);
				ImVec2 test(ScaleX(pSettings->GetReadOnly().fButtonMicrophoneX + pSettings->GetReadOnly().fButtonMicrophoneSize / 2.0f) - ImGui::CalcTextSize(&buf[0]).x / 2.0f, ScaleY(g_fMicrophoneButtonPosY) - GetFontSize() * 2.6f);
				//RenderText(test, 0xFF0000FF, true, &buf[0]);
			}
			ImVec2 centre(ScaleX(35.0f), ScaleY(35.0f));
			if (pVoice->IsDisconnected())
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(1.0f, 0.0f, 0.0f));
			}
			if (pVoice->GetNetworkState() == VOICECHAT_CONNECTING || pVoice->GetNetworkState() == VOICECHAT_WAIT_CONNECT)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(1.0f, 1.0f, 0.0f));
			}
			if (pVoice->GetNetworkState() == VOICECHAT_CONNECTED)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(0.0f, 1.0f, 0.0f));
			}
		}
	}

	CDebugInfo::Draw();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	PostProcessInput();
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	if(!pKeyBoard->OnTouchEvent(type, multi, x, y)) return false;

	if (!pScoreBoard->OnTouchEvent(type, multi, x, y)) return false;

	bool bFalse = true;
	if (pNetGame)
	{
		if (pNetGame->GetTextDrawPool()->OnTouchEvent(type, multi, x, y))
		{
			if (!pChatWindow->OnTouchEvent(type, multi, x, y)) return false;
		}
		else
		{
			bFalse = false;
		}
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
			m_bMouseDown = false;
			m_bNeedClearMousePos = true;
			break;
		}

		case TOUCH_MOVE:
		{
			m_bNeedClearMousePos = false;
			m_vTouchPos = ImVec2(x, y);
			break;
		}
	}
	if (!bFalse)
	{
		return false;
	}
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

void CGUI::RenderRakNetStatistics()
{
		//StatisticsToString(rss, message, 0);

		/*ImGui::GetOverlayDrawList()->AddText(
			ImVec2(ScaleX(10), ScaleY(400)),
			ImColor(IM_COL32_BLACK), message);*/
}

extern uint32_t g_uiBorderedText;
void CGUI::RenderTextForChatWindow(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	ImColor colOutline = ImColor(IM_COL32_BLACK);
	ImColor colDef = ImColor(col);
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
			{
				co.Value.w = colOutline.Value.w;
			}
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
			if (m_pSplashTexture)
			{
				ImColor co(1.0f, 1.0f, 1.0f, 0.4f);
				ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pSplashTexture->raster, posCur, b, ImVec2(0, 0), ImVec2(1, 1), co);
			}
			else
			{
				ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
				ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
			}
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}