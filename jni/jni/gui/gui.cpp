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
#include "../util/CJavaWrapper.h"
#include "../util/util.h"
#include "../game/vehicle.h"
#include "..//game/sprite2d.h"
#include "../gtasa/HUD.h"

extern CScoreBoard* pScoreBoard;
extern CChatWindow *pChatWindow;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CJavaWrapper *g_pJavaWrapper;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

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
	sprintf(path, "%smultiplayer/fonts/%s", g_pszStorage, pSettings->GetReadOnly().szFont);
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

    m_pNewTexture = nullptr;
    m_pNewTexture = (RwTexture*)LoadTextureFromDB("mobile", "WidgetGetIn");

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

void CGUI::SetHealth(float fhpcar){
   bHealth = static_cast<int>(fhpcar);
}

int CGUI::GetHealth(){
	return 1;//static_cast<int>(pVehicle->GetHealth());
}

void CGUI::SetDoor(int door){
	bDoor = door;
}

void CGUI::SetEngine(int engine){
	bEngine = engine;
}

void CGUI::SetLights(int lights){
	bLights = lights;
}

void CGUI::SetMeliage(float meliage){
	bMeliage = static_cast<int>(meliage);
}

void CGUI::SetEat(float eate){
	eat = static_cast<int>(eate);
}

int CGUI::GetEat(){
	return eat;
}

void CGUI::SetFuel(float fuel){
   m_fuel = static_cast<int>(fuel);
}

#include "..//CDebugInfo.h"
extern CGame* pGame;
extern CText3DLabelsPool* pText3DLabelsPool;
CText3DLabelsPool* pText3DLabelsPool = nullptr;

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

    if (pPlayerTags) pPlayerTags->Render(); // afk

    if(pNetGame && pNetGame->GetLabelPool())
    {
        pNetGame->GetLabelPool()->Draw();
    }

    //if (pChatWindow) pChatWindow->Render();
    //if (pScoreBoard) pScoreBoard->Draw();
    if (pKeyBoard) pKeyBoard->Render();
    if (pDialogWindow) pDialogWindow->Render();
    if (pNetGame && !pDialogWindow->m_bIsActive)
    {
        CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
        if (pVehiclePool)
        {
            VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
            if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
            {
                CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
                if (pVehicle)
                {
                    if (pVehicle->GetDistanceFromLocalPlayerPed() < 5.0f && !pKeyBoard->IsOpen())
                    {
                        CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
                        if (pPlayerPool)
                        {
                            CLocalPlayer* pLocalPlayer;
                            if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
                            {
                                g_pJavaWrapper->ShowG();
                            }
                            else g_pJavaWrapper->HideG();
                        }
                    }
                    else g_pJavaWrapper->HideG();
                }
            }
        }
    }

    // debug
    if (pGame->m_bDl_enabled) {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        ImGui::PopStyleColor(5);

        const ImVec4 resizeGripColor = ImVec4(0.0f, 0.5f, 1.0f, 0.2f);
        style.Colors[ImGuiCol_ResizeGrip] = resizeGripColor;
        style.Colors[ImGuiCol_ResizeGripHovered] = resizeGripColor;
        style.Colors[ImGuiCol_ResizeGripActive] = resizeGripColor;

        style.ScrollbarSize = 30.0f;
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
        ImGui::Begin("Debug", nullptr);

        uintptr_t ms_memoryAvailable = *(uint32_t*)(g_libGTASA + 0x5DE734);
        uintptr_t ms_memoryUsed = *(uint32_t*)(g_libGTASA + 0x67067C);
        uintptr_t storedTexels = (double)*(unsigned int*)(g_libGTASA + 0x61B8C0);
        uintptr_t GetMaxStorage = ((int (*)(void))(g_libGTASA + 0x1BD8CD))();

        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        if (pGame->FindPlayerPed() && pGame->FindPlayerPed()->m_pEntity && pGame->FindPlayerPed()->m_pEntity->mat) {
            ImGui::Text("Pos: %.6f, %.6f, %.6f",
                        pGame->FindPlayerPed()->m_pEntity->mat->pos.X,
                        pGame->FindPlayerPed()->m_pEntity->mat->pos.Y,
                        pGame->FindPlayerPed()->m_pEntity->mat->pos.Z);
        }

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Separator();
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Streaming memory: %d/%d MB", ms_memoryUsed >> 20, ms_memoryAvailable >> 20);
        ImGui::Text("Texture memory: %d/%d MB", (unsigned int)(storedTexels * 3.3) >> 20, (unsigned int)((double)GetMaxStorage * 3.3) >> 20);
        ImGui::Text("Models requested: %d", *(uint32_t*)(g_libGTASA + 0x670680));

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Separator();
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        bool checkboxState = pGame->m_bGui_enabled;
        float originalBorderSize = style.FrameBorderSize;
        style.FrameBorderSize = 0.5f;

        ImVec4 checkboxBorderColor = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.5f, 1.0f, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_Border, checkboxBorderColor);
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(11.0f, 11.0f));
        if (ImGui::Checkbox("  Vehicle Info", &checkboxState)) {
            pGame->m_bGui_enabled = checkboxState;
            if (checkboxState) {
                pText3DLabelsPool->DrawVehiclesInfo();

                /*uint32_t ms_nNoOfVisibleEntities = *(uint32_t*)(g_libGTASA + 0x8C162C);
                uintptr_t* ms_aVisibleEntityPtrs = (uintptr_t*)(g_libGTASA + 0x8C0680);

                if (ms_nNoOfVisibleEntities) {
                    do {
                        uintptr_t m_pEntityPointer = ms_aVisibleEntityPtrs[ms_nNoOfVisibleEntities];
                        if (m_pEntityPointer) {
                            pText3DLabelsPool->DrawModelsInfo(m_pEntityPointer);
                        }
                        --ms_nNoOfVisibleEntities;
                    } while (ms_nNoOfVisibleEntities);
                }*/
            }
        }
        ImGui::PopStyleVar();

        ImGui::PopStyleColor(4);

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Separator();
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        ImGui::Text("t.me/psychobye");

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::End();
        //ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
        style.FrameBorderSize = originalBorderSize;
    }

    if(pGame->FindPlayerPed()->IsInVehicle() && !pGame->FindPlayerPed()->IsAPassenger())
    {
        g_pJavaWrapper->ShowSpeed();
    }
    else
    {
        g_pJavaWrapper->HideSpeed();
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

	//if (!pScoreBoard->OnTouchEvent(type, multi, x, y)) return false;

	/*bool bFalse = true;
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
	}*/

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
	/*if (!bFalse)
	{
		return false;
	}*/
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
