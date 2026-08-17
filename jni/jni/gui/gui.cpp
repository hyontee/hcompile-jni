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
#include "../deathmessage.h"

//#include "CHUD.h"

extern CScoreBoard* pScoreBoard;
extern CChatWindow *pChatWindow;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CDeathMessage* pDeathMessage;
extern CCrossHair	*pCrossHair;

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

//#define PRIMARY_COLOR 	ImVec4(1.0 / 255 * 204, 1.0 / 255 * 59, 1.0 / 255 * 202, 1.0f)
//#define SECONDARY_COLOR ImVec4(1.0 / 255 * 204, 1.0 / 255 * 59, 1.0 / 255 * 202, 1.0f)

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
	m_fFontSizeNoScale = pSettings->GetReadOnly().fFontSize;
	m_fFontSize = ScaleY( pSettings->GetReadOnly().fFontSize );

	Log("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);

	// setup style
/*	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::StyleColorsClassic();

	style.ScrollbarSize = ScaleY(45.0f);
	style.WindowPadding = ImVec2(12, 4);
	style.WindowBorderSize = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.FrameBorderSize = 3.0f;
	style.WindowRounding = 2.75f;
	style.FrameRounding = 0.75f;
	style.ChildRounding = 2.75f;

	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]  = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	style.Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.75f);

    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 2.0f);
    
    style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.75f);
	style.Colors[ImGuiCol_ButtonHovered] = (ImVec4)ImColor(0xFF, 0x8C, 0x00, 255);
	style.Colors[ImGuiCol_ButtonActive] = (ImVec4)ImColor(0xFF, 0x8C, 0x00, 255);
	
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.75f);
	style.Colors[ImGuiCol_ScrollbarGrab] = SECONDARY_COLOR;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = SECONDARY_COLOR;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = SECONDARY_COLOR;
	
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	style.Colors[ImGuiCol_HeaderActive] = SECONDARY_COLOR;
	style.Colors[ImGuiCol_Header] = SECONDARY_COLOR;
	style.Colors[ImGuiCol_HeaderHovered] = SECONDARY_COLOR;

	style.Colors[ImGuiCol_Border] = (ImVec4)ImColor(0xFF, 0xA5, 0x00, 255);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.75f);
	style.Colors[ImGuiCol_FrameBgHovered] = PRIMARY_COLOR;
	style.Colors[ImGuiCol_FrameBgActive] = PRIMARY_COLOR;*/

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
	
	m_pFontGTAWeap = LoadFont("gtaweap3.ttf", 0);

	m_pSplashTexture = nullptr;

	m_pSplashTexture = (RwTexture*)LoadTextureFromDB("txd", "splash_icon");

	CRadarRect::LoadTextures();

	m_bKeysStatus = false;
}

ImFont* CGUI::LoadFont(char *font, float fontsize)
{
	ImGuiIO &io = ImGui::GetIO();

	// load fonts
	char path[0xFF];
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, font);
	
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

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}




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

// костыльный костыль :)
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

#include "..//CDebugInfo.h"
extern CGame* pGame;
void CGUI::Render()
{
	//PreProcessInput();

	//ProcessPushedTextdraws();
	if (pChatWindow)
	{
		pChatWindow->ProcessPushedCommands();
	}

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	RenderVersion();
	//RenderRakNetStatistics();
	if(pCrossHair) pCrossHair->Render(); // fix crosshair game
	//CHUD::Render();

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
    CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if(pDeathMessage)   pDeathMessage->Render();
	
	if (pNetGame && !pDialogWindow->m_bIsActive && pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 3.5, ImGui::GetFontSize() * 2.8);
		ImGui::SetNextWindowPos(ImVec2(2.0f, io.DisplaySize.y / 2.4 - vecButSize.x / 2));
		ImGui::Begin("###keys", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button(m_bKeysStatus ? "<<" : ">>", vecButSize))
		{
			if (m_bKeysStatus)
				m_bKeysStatus = false;
			else
				m_bKeysStatus = true;
		}


		ImGui::SameLine();
	if (ImGui::Button("Alt", vecButSize))
	{      
		if(pPlayerPed->IsInVehicle()) pthread_create(&threadBut, nullptr, ButtonPressed, (void*)&LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]);
		else pthread_create(&threadBut, nullptr, ButtonPressed, (void*)&LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]);

	}
                ImGui::SameLine();
        if(!pScoreBoard->m_bToggle) {
   if (ImGui::Button("TAB", vecButSize))
   {
    pScoreBoard->Toggle();
   }
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
					if (pVehicle->GetDistanceFromLocalPlayerPed() < 10.0f)
					{
						CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
						if (pPlayerPool)
						{
							CLocalPlayer* pLocalPlayer;
							if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
							{
								if (ImGui::Button("G", vecButSize))
								{
									if (pNetGame)
									{
										if (pPlayerPool)
										{
											pLocalPlayer = pPlayerPool->GetLocalPlayer();
											if (pLocalPlayer)
											{
												pLocalPlayer->HandlePassengerEntryEx();
											}
										}
									}
								}
							}
							else
								if (pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
								{
									if (ImGui::Button("CTRL", vecButSize))
									{
										pthread_create(&threadBut, 0, ButtonPressed, &LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION]);
									}
								}
							ImGui::SameLine();
						}
					}
				}
			}
		}
		if (m_bKeysStatus)
		{
			ImGui::SameLine();
			if (ImGui::Button("Y", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
			ImGui::SameLine();
			if (ImGui::Button("N", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
			ImGui::SameLine();
			if (ImGui::Button("F", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
			ImGui::SameLine();
			if (ImGui::Button("H", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;
		/*	ImGui::SameLine();
			if (ImGui::Button("NUM2", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION] = true;
*/
		}
		ImGui::End();
	}

	CDebugInfo::Draw();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	PostProcessInput();
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();

	if(!pKeyBoard->OnTouchEvent(type, multi, x, y)) return false;

	if (!pScoreBoard->OnTouchEvent(type, multi, x, y)) return false;

	if (pNetGame)
	{
		if (pNetGame->GetTextDrawPool()->OnTouchEvent(type, multi, x, y))
		{
			
		}
	}

	if (!pChatWindow->OnTouchEvent(type, multi, x, y)) return false;

    switch(type)
    {
        case TOUCH_PUSH:
            io.MousePos = ImVec2(x, y);
            io.MouseDown[0] = true;
            break;

        case TOUCH_POP:
            io.MouseDown[0] = false;
            m_bNeedClearMousePos = true;
            break;

        case TOUCH_MOVE:
            io.MousePos = ImVec2(x, y);

            ScrollDialog(x, y);

            m_iLastPosY = y;
            break;
    }

    return true;
}
void SetWindowScrollY(ImGuiWindow* window, float new_scroll_y)
{
    window->DC.CursorMaxPos.y += window->Scroll.y; // SizeContents is generally computed based on CursorMaxPos which is affected by scroll position, so we need to apply our change to it.
    window->Scroll.y = new_scroll_y;
    window->DC.CursorMaxPos.y -= window->Scroll.y;
}


void CGUI::ScrollDialog(float x, float y)
{
    if (m_imWindow != nullptr)
    {
        if (m_iLastPosY > y)
            SetWindowScrollY(m_imWindow, m_imWindow->Scroll.y + ImGui::GetFontSize() / 2);

        if (m_iLastPosY < y)
            SetWindowScrollY(m_imWindow, m_imWindow->Scroll.y - ImGui::GetFontSize() / 2);
    }
}

#include "../str_obfuscator_no_template.hpp"
void CGUI::RenderVersion()
{

	ImGui::GetBackgroundDrawList()->AddText(
		ImVec2(ScaleX(10), ScaleY(10)), 
		ImColor(IM_COL32_BLACK), cryptor::create("", 17).decrypt());
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

		/*ImGui::GetBackgroundDrawList()->AddText(
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

void CGUI::RenderTextForMaterialText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size, ImFont *font, bool bOutlineUseTextColor)
{
	int iOffset = bOutlineUseTextColor ? 1 : pSettings->Get().iFontOutline;
	if(bOutline)
	{
		// left
		posCur.x -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right
		posCur.x += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}

	ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, col, text_begin, text_end);
}

void CGUI::RenderTextDeathMessage(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size, ImFont *font, bool bOutlineUseTextColor)
{
	int iOffset = bOutlineUseTextColor ? 1 : pSettings->Get().iFontOutline;
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

void CGUI::RenderTextWithSize(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->Get().iFontOutline;

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