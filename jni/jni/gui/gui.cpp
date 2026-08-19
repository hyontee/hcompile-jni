#include "../main.h"
#include "gui.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../settings.h"
#include "..//scoreboard.h"
#include "../util/CJavaWrapper.h"
#include "../util/util.h"
#include "../game/vehicle.h"
#include "../interface/hud.h"

extern CScoreBoard* pScoreBoard;
extern CChatWindow *pChatWindow;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CJavaWrapper *g_pJavaWrapper;

void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();
bool ImGui_ImplRenderWare_Init();

#define MULT_X	0.00052083333f	// *1920
#define MULT_Y	0.00092592592f 	// *1080

bool CGUI::debug = false;
float CGUI::screen_xx = 5.0f;
float CGUI::screen_yy = 5.0f;

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

	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;
	
	m_fFontSize = ScaleY( pSettings->GetReadOnly().fFontSize );

	Log("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();

	char path[0xFF];
	sprintf(path, "%sSPACE/fonts/%s", g_pszStorage, pSettings->GetReadOnly().szFont);
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
	//Log("GUI | Loading font: %s", pSettings->GetReadOnly().szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	//Log("GUI | ImFont pointer = 0x%X", m_pFont);

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
#include "..//BRClient.h"
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

void CGUI::CoordsRadar(CRect* rect)
{
        rect->left=83.0f; // свои координаты
        rect->bottom=57.0f; // свои координаты
        rect->right=25.0f;  // свои координаты
        rect->top=25.0f; // свои координаты
        //break;
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
#include "..//debug.h"
extern CGame* pGame;

void CGUI::Render()
{
	PreProcessInput();

//                  RenderGUI();

	ProcessPushedTextdraws();
	if (pChatWindow)
	{
		pChatWindow->ProcessPushedCommands();
	}

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();	

	RenderVersion();
    RenderMap(screen_xx, screen_yy);
	if(debug) {
		RenderPosition();
	}


	if (pKeyBoard)
	{
		pKeyBoard->ProcessInputCommands();
	}

	if (pPlayerTags) pPlayerTags->Render();
	
	if(pNetGame && pNetGame->GetLabelPool())
	{
		pNetGame->GetLabelPool()->Draw();
	}

	Hud::Create();

	if (pChatWindow) pChatWindow->Render();
	if(pGame) CGUI::ShowSpeed();
	if (pScoreBoard) pScoreBoard->Draw();
	if (pKeyBoard) pKeyBoard->Render();
	if (pDialogWindow) pDialogWindow->Render();
	
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
//ImGui::Text("Text");

// OBFUSCATE
#include "obfuscate.h"

#define LOADERW 800
#define LOADERH 600
uint32_t CGUI::uiStreamedPeds = 0;
uint32_t CGUI::uiStreamedVehicles = 0;
uint32_t CGUI::uiStreamedObject = 0;
void CGUI::RenderPosition()
{
    ImGuiIO& io = ImGui::GetIO();
    MATRIX4X4 matFromPlayer;

    CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
    pLocalPlayerPed->GetMatrix(&matFromPlayer);

    ImVec2 _ImVec2 = ImVec2(ScaleX(0), io.DisplaySize.y - ImGui::GetFontSize() * 1.05);

    char text[128];
    char text2[128];
	char text3[228];
	char text4[128];

	uint32_t msUsed = *(uint32_t*)(g_libGTASA + 0x0067067C); // gtasa1
	uint32_t msAvailable = *(uint32_t*)(g_libGTASA + 0x005DE734);
	float percentUsed = ((float)msUsed/(float)msAvailable)*100;

    const char *buildString = "Native Build: " __DATE__ " " __TIME__ " Client by EDGAR 3.0 Version: 2.02 RELEASE NEW LAUNCHER";
    //const char *posString = "\t\tPosition > X: %.4f - Y: %.4f - Z: %.4f" , matFromPlayer.pos.X, matFromPlayer.pos.Y, matFromPlayer.pos.Z;
    sprintf(text, "%s", buildString);
    sprintf(text2, "Position > X: %.4f\n - Y: %.4f - Z: %.4f", matFromPlayer.pos.X, matFromPlayer.pos.Y, matFromPlayer.pos.Z);
	sprintf(text3, "Streamed Peds: %d\nStreamed Vehicles: %d\nStreamed Objects: %d", uiStreamedPeds, uiStreamedVehicles, uiStreamedObject);
	sprintf(text4, "Memory: %.1f/%.1f (%.1f %%)", (float)msUsed/ (1024*1024), (float)msAvailable / (1024*1024), percentUsed);

    RenderText(_ImVec2, ImColor(255, 255, 255, 255), true, text, nullptr);// TEST VERSION
    /*ImGui::Text("By EDGAR 3.0");
    ImGui::Text("Координаты\n %s", text2);
	ImGui::Text("%s", text3);
    ImGui::Text("%s", text4);*/

	char szStr[256];
	char szStrPr[256];
	char szStrMem[256];
	char szStrPos[256];
	ImVec2 pos;

	snprintf(&szStrMem[0], 256, OBFUSCATE("Memory: %.1f/%.1f (%.1f %%)"),
			 (float)msUsed/ (1024*1024),
			 (float)msAvailable / (1024*1024),
			 percentUsed
	);
	pos = ImVec2(ScaleX(40.0f), ScaleY(1080.0f - GetFontSize() * 10));

	RenderText(pos, (ImU32)0xFFFFFFFF, true, &szStrMem[0]);

	snprintf(&szStrPos[0], 256, OBFUSCATE("Position: %.4f, %.4f, %.4f"), matFromPlayer.pos.X,
			 matFromPlayer.pos.Y, matFromPlayer.pos.Z);
	pos = ImVec2(ScaleX(40.0f), ScaleY(1080.0f - GetFontSize() * 8));
	RenderText(pos, (ImU32) 0xFFFFFFFF, true, &szStrPos[0]);

	snprintf(&szStr[0], 256, OBFUSCATE("Streamed Peds: %d, Streamed Vehicles: %d, Streamed Objects: %d"),
			 uiStreamedPeds, uiStreamedVehicles, uiStreamedObject);
	pos = ImVec2(ScaleX(40.0f), ScaleY(1080.0f - GetFontSize() * 12));
	RenderText(pos, (ImU32) 0xFFFFFFFF, true, &szStr[0]);

	snprintf(&szStrPr[0], 256, "Version: 2.02 RELEASE NEW LAUNCHER");
	pos = ImVec2(ScaleX(40.0f), ScaleY(1080.0f - GetFontSize() * 6));
	RenderText(pos, (ImU32)0xFFFFFFFF, true, &szStrPr[0]);

}

void CGUI::RenderMap(float screen_x, float screen_y)
{
    /*Log("[CGUI] OnPlayerClickMap: %f, %f", screen_x, screen_y);
    Log("бляооооооооо");
    float screen_width = 1920.0; // ширина экрана
    float screen_height = 1080.0; // высота экрана
    CSprite2d* map = new CSprite2d();
    map->m_pRwTexture = (RwTexture*)LoadTextureFromDB("radar", "radar_bg");
    //radar->m_pRwTexture = CHUD::hud_radar;

    CRGBA color;
    color.A = 255;
    color.R = 255;
    color.G = 255;
    color.B = 255;

    map->Draw(screen_x / screen_width, screen_y / screen_height, 100, 100, color);*/

}

void CGUI::RenderGUI()
{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 3.8, ImGui::GetFontSize() * 2.3);
		ImGui::SetNextWindowPos(ImVec2(2.0f, io.DisplaySize.y / 2.0 - vecButSize.x / 2));
		ImGui::Begin("###keys", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button(m_bKeysStatus ? "<" : ">", vecButSize))
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
								/*if (ImGui::Button("G", vecButSize))
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
								}*/
							}
							else
								if (pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
								{
									if (ImGui::Button("2", vecButSize))
									{
										LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;
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
			if (ImGui::Button("Y", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
			ImGui::SameLine();
			if (ImGui::Button("N", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
			ImGui::SameLine();
			if (ImGui::Button("H", vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;

		}
		ImGui::End();
	//}
                 /* else { ImVec2 verpos = ImVec2(ScaleX(10), ScaleY(10));
	RenderText(verpos, ImColor(0xFFFFFFFF), true, "BLACK RUSSIA | v29.9"); }*/
}

void CGUI::RenderServer()
{
	if (pNetGame)
	{
		if (pVoice && g_IsVoiceServer())
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
}


void CGUI::RenderVersion()
{
	return;

	/*ImGui::GetOverlayDrawList()->AddText(
		ImVec2(ScaleX(10), ScaleY(10)), 
		ImColor(IM_COL32_BLACK), PORT_VERSION);*/
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
		g_pJavaWrapper->HideSpeed();
		bMeliage =0;
		m_fuel = 0;
		return;
	}
	if (pGame->FindPlayerPed()->IsAPassenger()) {
		g_pJavaWrapper->HideSpeed();
		bMeliage =0;
		m_fuel = 0;
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
	g_pJavaWrapper->ShowSpeed();
	g_pJavaWrapper->UpdateSpeedInfo(i_speed, m_fuel, bHealth, bMeliage, bEngine, bLights, 0, bDoor);
}
