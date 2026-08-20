#include "../main.h"
#include "gui.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../dialog.h"
#include "../game/vehicle.h"
#include "../keyboard.h"
#include "../settings.h"
#include "../util/util.h"
#include "../game/vehicle.h"

extern CChatWindow *pChatWindow;
extern CVehicle *pVehicle;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CGame* pGame;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;

void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();
bool ImGui_ImplRenderWare_Init();

#define MULT_X	0.00052083333f	// *1920
#define MULT_Y	0.00092592592f 	// *1080

CGUI::CGUI()
{
	Log("GUI LOAD..");

	m_bMouseDown = 0;
	m_vTouchPos = ImVec2(-1, -1);
	m_bNextClear = false;
	m_bNeedClearMousePos = false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplRenderWare_Init();

    //io.DisplaySize.x = 1580 * 1.2;
    //io.DisplaySize.y = 720 * 1.2;

	m_vecScale.x = (io.DisplaySize.x * MULT_X) * 1.2;
	m_vecScale.y = (io.DisplaySize.y * MULT_Y) * 1.2;
    io.DisplayFramebufferScale = m_vecScale;
    //io. = m_vecScale;
    ImGui::GetIO().DisplayFramebufferScale = ImVec2(MULT_X, MULT_Y);

    io.DisplayFramebufferScale.x = 1580 * MULT_X;
    io.DisplayFramebufferScale.y = 720 * MULT_Y;

    m_fFontSize = ScaleY(pSettings->GetReadOnly().fFontSize );

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();

	char path[0xFF];
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, pSettings->GetReadOnly().szFont);
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
	Log("Loading font: %s", pSettings->GetReadOnly().szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	Log("GUI | ImFont pointer = 0x%X", m_pFont);

	style.WindowRounding = 0.0f;

	m_pSplashTexture = nullptr;

	//m_pSplashTexture = (RwTexture*)LoadTextureFromDB("txd", "splash_icon");

	//CRadarRect::LoadTextures();

	m_bKeysStatus = false;
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}
bool g_IsVoiceServer()
{
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

extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendG(JNIEnv* pEnv, jobject thiz)
	{
	                                    CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	                                    CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
		                  if (pVehiclePool)
		                  {
		                  	VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
		 	                  if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
			                  {
			                  	CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
			                  	if (pVehicle)
			                  	{
						CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
						if (pPlayerPool)
						{
							CLocalPlayer* pLocalPlayer;
                                                                                                                              pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
							pLocalPlayer->SendEnterVehicleNotification(ClosetVehicleID, true);
						}
				                   }
			                  }
	                  	}     
	}
                  JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendVoice(JNIEnv* pEnv, jobject thiz)
	{

	}
}

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

	if (pKeyBoard)
	{
		pKeyBoard->ProcessInputCommands();
	}

	
	if(pNetGame && pNetGame->GetLabelPool())
	{
		pNetGame->GetLabelPool()->Draw();
	}

	if (pChatWindow) pChatWindow->Render();
	if(pGame) CGUI::ShowSpeed();
	if (pKeyBoard) pKeyBoard->Render();
	if (pDialogWindow) pDialogWindow->Render();

                  if(pNetGame && !pDialogWindow->m_bIsActive)
	{
                                   /* if (pVoice && g_IsVoiceServer())
		{
			if (pVoice->IsRecording() && GetTickCount() - g_uiLastTickVoice >= 20000)
			{
				char buf[64];
				sprintf(&buf[0], "%d", (int)((30000 - (GetTickCount() - g_uiLastTickVoice)) / 1000) + 1);
				ImVec2 test(ScaleX(pSettings->GetReadOnly().fButtonMicrophoneX + pSettings->GetReadOnly().fButtonMicrophoneSize / 2.0f) - ImGui::CalcTextSize(&buf[0]).x / 2.0f, ScaleY(g_fMicrophoneButtonPosY) - GetFontSize() * 2.6f);
				RenderText(test, 0xFF0000FF, true, &buf[0]);
			}
			ImVec2 centre(ScaleX(35.0f), ScaleY(35.0f));
			if (pVoice->IsDisconnected())
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 10.0f, ImColor(1.0f, 0.0f, 0.0f));
			}
			if (pVoice->GetNetworkState() == VOICECHAT_CONNECTING || pVoice->GetNetworkState() == VOICECHAT_WAIT_CONNECT)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 10.0f, ImColor(1.0f, 1.0f, 0.0f));
			}
			if (pVoice->GetNetworkState() == VOICECHAT_CONNECTED)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 10.0f, ImColor(0.0f, 1.0f, 0.0f));
			}
		}*/

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
							if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger()) {
                                // showG
                            }
						}
					}
				}
			}
		}

	}


	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	PostProcessInput();
}

void CGUI::CoordsRadar(CRect* rect)
{
	rect->left=83.0f; // свои координаты
	rect->bottom=57.0f; // свои координаты
	rect->right=25.0f;  // свои координаты
	rect->top=25.0f; // свои координаты
	//break;
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	if(!pKeyBoard->OnTouchEvent(type, multi, x, y)) return false;

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


void CGUI::RenderPosition()
{
    ImGuiIO& io = ImGui::GetIO();
    MATRIX4X4 matFromPlayer;

    CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
    pLocalPlayerPed->GetMatrix(&matFromPlayer);

    ImVec2 _ImVec2 = ImVec2(ScaleX(10), io.DisplaySize.y - ImGui::GetFontSize() * 0.85);

    char text[128];
    sprintf(text, "\t\t", matFromPlayer.pos.X, matFromPlayer.pos.Y, matFromPlayer.pos.Z);

    RenderText(_ImVec2, ImColor(255, 255, 255, 255), false, text, nullptr);
}

void CGUI::RenderGUI()
{

}

void CGUI::RenderServer()
{
}


void CGUI::RenderVersion()
{
     ImVec2 verpos = ImVec2(ScaleX(10), ScaleY(10));
	RenderText(verpos, ImColor(0xFFFFFFFF), true, "v0"); 
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

void CGUI::ShowSpeed(){
	if (!pGame || !pNetGame || !pGame->FindPlayerPed()->IsInVehicle()) {
		bMeliage =0;
		m_fuel = 0;
		return;
	}
	if (pGame->FindPlayerPed()->IsAPassenger()) {
		bMeliage =0;
		m_fuel = 0;
		return;
	}
                  if(pNetGame->GetGameState() != GAMESTATE_CONNECTED) {
		return;
                  }
	if(pKeyBoard->IsOpen()) {
		return;
	}

	int i_speed = 0;
	bDoor =0;
	bEngine = 0;
	bLights = 0;
	float fHealth = 0;
	CVehicle *pVehicle = nullptr;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
                  VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
                  pVehicle = pVehiclePool->GetAt(id);
    
                  if(pPlayerPed)
                  {
                                    if(pVehicle)
                                    {
                                                VECTOR vecMoveSpeed;
                                                pVehicle->GetMoveSpeedVector(&vecMoveSpeed);
                                                i_speed = sqrt((vecMoveSpeed.X * vecMoveSpeed.X) + (vecMoveSpeed.Y * vecMoveSpeed.Y) + (vecMoveSpeed.Z * vecMoveSpeed.Z)) * 180;
                                                bHealth = pVehicle->GetHealth();
                                                bDoor = pVehicle->GetDoorState();
                                                bEngine = pVehicle->GetEngineState();
                                                bLights = pVehicle->GetLightsState();
                                     }
                  }
}
