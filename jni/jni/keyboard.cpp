#include "main.h"

#include "keyboardhistory.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"
#include "game/game.h"
#include "keyboard.h"
#include "vendor/imgui/fontawesome.h"
#include "vendor/imgui/fonts.h"
#include "net/netgame.h"
#include "java_systems/CHUD.h"
#include "java_systems/CAdminRecon.h"
#include "java_systems/casino/CBaccarat.h"
#include "java_systems/CSpeedometr.h"
#include "gui/gui.h"

extern CGUI *pGUI;
extern CGame *pGame;

std::vector<kbKey> CKeyBoard::m_Rows[3][4];
std::string CKeyBoard::m_sInput;
int CKeyBoard::dop_butt;
bool CKeyBoard::m_bEnable;
ImVec2 CKeyBoard::m_Size;
ImVec2 CKeyBoard::m_Pos;
float CKeyBoard::m_fKeySizeY;
float CKeyBoard::m_fKeySizeX;
float CKeyBoard::m_fFontSize;

bool CKeyBoard::m_iPushedKeyUp;
bool CKeyBoard::m_iPushedKeyDown;

ImFont *CKeyBoard::fonticon;
ImFont *CKeyBoard::chatfuncfont;

int CKeyBoard::m_iLayout;
int CKeyBoard::m_iCase;
int CKeyBoard::m_iPushedKey;
int CKeyBoard::chatinputposx;


char CKeyBoard::m_utf8Input[MAX_INPUT_LEN * 3 + 0xF];

CKeyBoardHistory *CKeyBoard::m_pkHistory;

DataStructures::SingleProducerConsumer<std::string> CKeyBoard::bufferedStrings;

void CKeyBoard::init()
{
	Log("Initalizing KeyBoard..");

	ImGuiIO &io = ImGui::GetIO();
	m_Size = ImVec2(io.DisplaySize.x, io.DisplaySize.y * 0.55);
	m_Pos = ImVec2(0, io.DisplaySize.y * (1 - 0.55));
	m_fFontSize = pGUI->ScaleY(70.0f);
	m_fKeySizeY = m_Size.y / 5;
	m_fKeySizeX = m_Size.x / 10;

	io.Fonts->AddFontDefault();
	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f;
	static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
	fonticon = io.Fonts->AddFontFromMemoryTTF(iconfont, sizeof(iconfont), 20.0f, &config, icon_ranges);

	//chatfuncfont = pGUI->LoadFont("visby-round-cf-extra-bold.ttf", 22);

	Log("Size: %f, %f. Pos: %f, %f", m_Size.x, m_Size.y, m_Pos.x, m_Pos.y);
	Log("font size: %f. Key's height: %f", m_fFontSize, m_fKeySizeY);

	m_bEnable = false;
	m_iLayout = LAYOUT_ENG;
	m_iCase = LOWER_CASE;
	m_iPushedKey = -1;
	chatinputposx = 0;

	dop_butt = -1;

	m_utf8Input[0] = '\0';

	m_pkHistory = new CKeyBoardHistory();

	InitENG();
	InitRU();
	InitNUM();
}

void CKeyBoard::Render()
{
	if (!m_bEnable)
		return;

	ImGuiIO &io = ImGui::GetIO();
	ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 6, ImGui::GetFontSize() * 3.5);

	// background
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(m_Pos.x, m_Pos.y + m_fKeySizeY), ImVec2(m_Size.x, io.DisplaySize.y), 0xB0000000);

	// background-triangleup
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(m_fKeySizeX * 7.6, m_Pos.y), ImVec2(m_fKeySizeX * 8.6, m_Pos.y + m_fKeySizeY), 0xB0000000);

	// background-triangledown
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(m_fKeySizeX * 8.8, m_Pos.y), ImVec2(m_fKeySizeX * 9.8, m_Pos.y + m_fKeySizeY), 0xB0000000);

	// triangleup
	ImGui::GetForegroundDrawList()->AddTriangleFilled(
				ImVec2(m_fKeySizeX * 7.6 + 70, m_Pos.y + 100),
				ImVec2(m_fKeySizeX * 7.6 + 115, m_Pos.y + 30),
				ImVec2(m_fKeySizeX * 7.6 + 160, m_Pos.y + 100),
				0xFF8A8886);

	// triangledown
	ImGui::GetForegroundDrawList()->AddTriangleFilled(
				ImVec2(m_fKeySizeX * 8.8 + 70, m_Pos.y + 30),
				ImVec2(m_fKeySizeX * 8.8 + 115, m_Pos.y + 100),
				ImVec2(m_fKeySizeX * 8.8 + 160, m_Pos.y + 30),
				0xFF8A8886);

	// background-inputbar
	//ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY + 50 - 40), ImVec2(m_fChatPosX + m_fChatSizeX + 45, m_fChatPosY + m_fChatSizeY + 125 - 40), 0xB0000000, 10);

	// background-settings
	//ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY + 135 - 40), ImVec2(m_fChatPosX + 70, m_fChatPosY + m_fChatSizeY + 205 - 40), 0xB0000000, 10);

	//ImGui::GetOverlayDrawList()->AddText(fonticon, ImGui::GetFontSize(),
										// ImVec2(m_fChatPosX + 16, m_fChatPosY + m_fChatSizeY + 135 + 27 - 40), 0xFFFFFFFF, ICON_FA_COG);

	// background-me
//	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(m_fChatPosX + 80, m_fChatPosY + m_fChatSizeY + 135 - 40), ImVec2(m_fChatPosX + 200, m_fChatPosY + m_fChatSizeY + 205 - 40), 0xB0000000, 10);
//
//	ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize - pGUI->ScaleY(35.0f),
//										 ImVec2(m_fChatPosX + 80 + 23, m_fChatPosY + m_fChatSizeY + 135 + 17.5 - 40), sME ? 0xFFFFFFFF : 0xFF90908E, "/me");
//
//	// background-try
//	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(m_fChatPosX + 210, m_fChatPosY + m_fChatSizeY + 135 - 40), ImVec2(m_fChatPosX + 330, m_fChatPosY + m_fChatSizeY + 205 - 40), 0xB0000000, 10);
//
//	ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize - pGUI->ScaleY(35.0f),
//										 ImVec2(m_fChatPosX + 210 + 23, m_fChatPosY + m_fChatSizeY + 135 + 17.5 - 40), sTRY ? 0xFFFFFFFF : 0xFF90908E, "/try");
//
//	// background-do
//	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(m_fChatPosX + 340, m_fChatPosY + m_fChatSizeY + 135 - 40), ImVec2(m_fChatPosX + 460, m_fChatPosY + m_fChatSizeY + 205 - 40), 0xB0000000, 10);
//
//	ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize - pGUI->ScaleY(35.0f),
//										 ImVec2(m_fChatPosX + 340 + 26, m_fChatPosY + m_fChatSizeY + 135 + 17.5 - 40), sDO ? 0xFFFFFFFF : 0xFF90908E, "/do");
//
//	// background-gov
//	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(m_fChatPosX + 470, m_fChatPosY + m_fChatSizeY + 135 - 40), ImVec2(m_fChatPosX + 640, m_fChatPosY + m_fChatSizeY + 205 - 40), 0xB0000000, 10);
//
//	ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize - pGUI->ScaleY(35.0f),
									//	 ImVec2(m_fChatPosX + 470 + 17, m_fChatPosY + m_fChatSizeY + 135 + 17.5 - 40), sGOV ? 0xFFFFFFFF : 0xFF90908E, "/gnews");


	// input string
//	if (IsHidden())
//	{
//		char _utf8DialogInputBuffer[100 * 3 + 1];
//		strcpy(_utf8DialogInputBuffer, m_utf8Input);
//
//		for (int i = 0; i < strlen(_utf8DialogInputBuffer); i++)
//		{
//			if (_utf8DialogInputBuffer[i] == '\0')
//				break;
//			_utf8DialogInputBuffer[i] = '*';
//		}
//		ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize - pGUI->ScaleY(30.0f), ImVec2(m_fChatPosX + m_Size.x * 0.02 - 25, m_fChatPosY + m_fChatSizeY + 50 - 40 + m_Pos.y * 0.05 - 5), 0xFFFFFFFF, _utf8DialogInputBuffer);
//	}
//	else
//		ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize - pGUI->ScaleY(30.0f), ImVec2(m_fChatPosX + m_Size.x * 0.02 - 25 + chatinputposx, m_fChatPosY + m_fChatSizeY + 50 - 40 + m_Pos.y * 0.05 - 5), 0xFFFFFFFF, m_utf8Input);

	// dividing line
	ImGui::GetForegroundDrawList()->AddLine(
		ImVec2(m_Pos.x, m_Pos.y + m_fKeySizeY),
		ImVec2(m_Size.x, m_Pos.y + m_fKeySizeY), 0xFF3291F5);

	float fKeySizeY = m_fKeySizeY;

	for (int i = 0; i < 4; i++)
	{
		for (auto key : m_Rows[m_iLayout][i])
		{
			if (key.id == m_iPushedKey && key.type != KEY_SPACE)
				ImGui::GetForegroundDrawList()->AddRectFilled(
					key.pos,
					ImVec2(key.pos.x + key.width, key.pos.y + fKeySizeY),
					0xFF3291F5);

			if (m_iPushedKeyUp)
			{
				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(m_fKeySizeX * 7.6, m_Pos.y), ImVec2(m_fKeySizeX * 8.6, m_Pos.y + m_fKeySizeY), 0xFF3291F5);
				ImGui::GetForegroundDrawList()->AddTriangleFilled(
				ImVec2(m_fKeySizeX * 7.6 + 70, m_Pos.y + 100),
				ImVec2(m_fKeySizeX * 7.6 + 115, m_Pos.y + 30),
				ImVec2(m_fKeySizeX * 7.6 + 160, m_Pos.y + 100),
				0xFF8A8886);
			}
			else if (m_iPushedKeyDown)
			{
				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(m_fKeySizeX * 8.8, m_Pos.y), ImVec2(m_fKeySizeX * 9.8, m_Pos.y + m_fKeySizeY), 0xFF3291F5);
				ImGui::GetForegroundDrawList()->AddTriangleFilled(
				ImVec2(m_fKeySizeX * 8.8 + 70, m_Pos.y + 30),
				ImVec2(m_fKeySizeX * 8.8 + 115, m_Pos.y + 100),
				ImVec2(m_fKeySizeX * 8.8 + 160, m_Pos.y + 30),
				0xFF8A8886);
			}

			switch (key.type)
			{
			case KEY_DEFAULT:
				ImGui::GetForegroundDrawList()->AddText(pGUI->GetFont(), m_fFontSize, key.symPos, 0xFFFFFFFF, key.name[m_iCase]);
				break;

			case KEY_SHIFT:
				ImGui::GetForegroundDrawList()->AddTriangleFilled(
					ImVec2(key.pos.x + key.width * 0.37, key.pos.y + fKeySizeY * 0.50),
					ImVec2(key.pos.x + key.width * 0.50, key.pos.y + fKeySizeY * 0.25),
					ImVec2(key.pos.x + key.width * 0.63, key.pos.y + fKeySizeY * 0.50),
					m_iCase == LOWER_CASE ? 0xFF8A8886 : 0xFF3291F5);
				ImGui::GetForegroundDrawList()->AddRectFilled(
					ImVec2(key.pos.x + key.width * 0.44, key.pos.y + fKeySizeY * 0.5 - 1),
					ImVec2(key.pos.x + key.width * 0.56, key.pos.y + fKeySizeY * 0.68),
					m_iCase == LOWER_CASE ? 0xFF8A8886 : 0xFF3291F5);
				break;

			case KEY_BACKSPACE:
				static ImVec2 points[5];
				points[0] = ImVec2(key.pos.x + key.width * 0.35, key.pos.y + fKeySizeY * 0.5);
				points[1] = ImVec2(key.pos.x + key.width * 0.45, key.pos.y + fKeySizeY * 0.25);
				points[2] = ImVec2(key.pos.x + key.width * 0.65, key.pos.y + fKeySizeY * 0.25);
				points[3] = ImVec2(key.pos.x + key.width * 0.65, key.pos.y + fKeySizeY * 0.65);
				points[4] = ImVec2(key.pos.x + key.width * 0.45, key.pos.y + fKeySizeY * 0.65);
				ImGui::GetForegroundDrawList()->AddConvexPolyFilled(points, 5, 0xFF8A8886);
				break;

			case KEY_SWITCH:
				ImGui::GetForegroundDrawList()->AddText(pGUI->GetFont(), m_fFontSize, key.symPos, 0xFFFFFFFF, "lang");
				break;

			case KEY_SPACE:
				ImGui::GetForegroundDrawList()->AddRectFilled(
					ImVec2(key.pos.x + key.width * 0.07, key.pos.y + fKeySizeY * 0.3),
					ImVec2(key.pos.x + key.width * 0.93, key.pos.y + fKeySizeY * 0.7),
					key.id == m_iPushedKey ? 0xFF3291F5 : 0xFF8A8886);
				break;

			case KEY_SEND:
				ImGui::GetForegroundDrawList()->AddTriangleFilled(
					ImVec2(key.pos.x + key.width * 0.3, key.pos.y + fKeySizeY * 0.25),
					ImVec2(key.pos.x + key.width * 0.3, key.pos.y + fKeySizeY * 0.75),
					ImVec2(key.pos.x + key.width * 0.7, key.pos.y + fKeySizeY * 0.50),
					0xFF8A8886);
				break;

			case KEY_UP:
				ImGui::GetForegroundDrawList()->AddTriangleFilled(
					ImVec2(io.DisplaySize.x - (vecButSize.x * 3) + 50, io.DisplaySize.y / 2 - vecButSize.y * 1.635 + 100),
					ImVec2(io.DisplaySize.x - (vecButSize.x * 3) + 95, io.DisplaySize.y / 2 - vecButSize.y * 1.635 + 30),
					ImVec2(io.DisplaySize.x - (vecButSize.x * 3) + 140, io.DisplaySize.y / 2 - vecButSize.y * 1.635 + 100),
					0xFF8A8886);
				break;

			case KEY_DOWN:
				ImGui::GetForegroundDrawList()->AddTriangleFilled(
					ImVec2(io.DisplaySize.x - (vecButSize.x * 1.5) + 50, io.DisplaySize.y / 2 - vecButSize.y * 1.635 + 30),
					ImVec2(io.DisplaySize.x - (vecButSize.x * 1.5) + 95, io.DisplaySize.y / 2 - vecButSize.y * 1.635 + 100),
					ImVec2(io.DisplaySize.x - (vecButSize.x * 1.5) + 140, io.DisplaySize.y / 2 - vecButSize.y * 1.635 + 30),
					0xFF8A8886);
				break;
			}
		}
	}
}

void CKeyBoard::Open()
{
	Close();
	if (m_pkHistory)
	{
		m_pkHistory->m_iCounter = 0;
	}
	m_bEnable = true;

	//g_pJavaWrapper->HideVoice();

	if(pNetGame->m_GreenZoneState) CHUD::toggleGreenZone(false);
	if(CAdminRecon::bIsToggle) CAdminRecon::tempToggle(false);
	if(pGame->isCasinoDiceActive)g_pJavaWrapper->TempToggleCasinoDice(false);
	if(CBaccarat::bIsShow) CBaccarat::tempToggle(false);
	CHUD::toggleServerLogo(false);

	if(CSpeedometr::bIsShow) CSpeedometr::tempToggle(false);

	if (pGame->m_bRaceCheckpointsEnabled)
	{
		CHUD::toggleGps(false);
	}

}

void CKeyBoard::Close()
{
	if(!m_bEnable) return;

	m_bEnable = false;

	m_utf8Input[0] = 0;
	m_iCase = LOWER_CASE;
	m_iPushedKey = -1;

	CHUD::toggleServerLogo(true);
	if (pGame->m_bRaceCheckpointsEnabled)
	{
		CHUD::toggleGps(true);
	}
	if(pGame->isCasinoDiceActive)g_pJavaWrapper->TempToggleCasinoDice(true);
	if(CBaccarat::bIsShow) CBaccarat::tempToggle(true);
	if(CAdminRecon::bIsToggle) CAdminRecon::tempToggle(true);
	if(pNetGame->m_GreenZoneState) CHUD::toggleGreenZone(true);

	CHUD::ToggleChatInput(false);
	//g_pJavaWrapper->ShowVoice();

}
#include "util/CJavaWrapper.h"
#include "chatwindow.h"
#include "CDebugInfo.h"
#include "java_systems/CTab.h"
#include "java_systems/CDuelsGui.h"
#include "util/patch.h"
#include "game/Core/Vector.h"

bool CKeyBoard::OnTouchEvent(int type, bool multi, int x, int y)
{
	static bool bWannaClose = false;

	if (!m_bEnable)
		return true;

	ImGuiIO &io = ImGui::GetIO();

	if (x >= io.DisplaySize.x - ImGui::GetFontSize() * 4 && y >= io.DisplaySize.y / 2 - (ImGui::GetFontSize() * 2.5) * 3 && y <= io.DisplaySize.y / 2) // keys
	{
		return true;
	}


	static bool bWannaCopy = false;
	static uint32_t uiTouchTick = 0;

	m_iPushedKey = -1;
	m_iPushedKeyUp = false;
	m_iPushedKeyDown = false;

	ImVec2 leftCornertriangleup(m_fKeySizeX * 7.6, m_Pos.y);
	ImVec2 rightCornertriangleup(m_fKeySizeX * 8.6, m_Pos.y + m_fKeySizeY);

	if ((type == TOUCH_PUSH || type == TOUCH_MOVE) && x >= leftCornertriangleup.x && y >= leftCornertriangleup.y && x <= rightCornertriangleup.x && y <= rightCornertriangleup.y)
	{
		m_iPushedKeyUp = true;
		if (type == TOUCH_PUSH)
		{
			m_pkHistory->PageUp();
		}
	}

	ImVec2 leftCornertriangledown(m_fKeySizeX * 8.8, m_Pos.y);
	ImVec2 rightCornertriangledown(m_fKeySizeX * 9.8, m_Pos.y + m_fKeySizeY);

	if ((type == TOUCH_PUSH || type == TOUCH_MOVE) && x >= leftCornertriangledown.x && y >= leftCornertriangledown.y && x <= rightCornertriangledown.x && y <= rightCornertriangledown.y)
	{
		m_iPushedKeyDown = true;
		if (type == TOUCH_PUSH)
		{
			m_pkHistory->PageDown();
		}
	}

	kbKey *key = GetKeyFromPos(x, y);
	// Log("CKeyBoard::OnTouchEvent(%d, %d, %d, %d)", type, multi, x, y);
	if (!key)
		return false;

	switch (type)
	{
	case TOUCH_PUSH:
		m_iPushedKey = key->id;
		break;

	case TOUCH_MOVE:
		m_iPushedKey = key->id;
		break;

	case TOUCH_POP:
		HandleInput(*key);
		break;
	}
	delete key;
	return false;
}

void CKeyBoard::HandleInput(kbKey &key)
{
	switch (key.type)
	{
	case KEY_DEFAULT:
	case KEY_SPACE:
		AddCharToInput(key.code[m_iCase]);
		break;

	case KEY_SWITCH:
		m_iLayout++;
		if (m_iLayout >= 3)
			m_iLayout = 0;
		m_iCase = LOWER_CASE;
		break;

	case KEY_BACKSPACE:
		DeleteCharFromInput();
		break;

	case KEY_SHIFT:
		m_iCase ^= 1;
		break;

	case KEY_SEND:
		CKeyBoard::Send();
		CKeyBoard::Close();
		break;
	case KEY_UP:
		m_pkHistory->PageUp();
		break;
	case KEY_DOWN:
		m_pkHistory->PageDown();
		break;
	}
}

void CKeyBoard::AddCharToInput(char sym)
{
	if (m_sInput.length() < MAX_INPUT_LEN && sym)
	{
		m_sInput.push_back(sym);
		cp1251_to_utf8(m_utf8Input, m_sInput.c_str());

		CHUD::SetChatInput(m_sInput.c_str());

	}
}

void CKeyBoard::DeleteCharFromInput()
{
	if (!m_sInput.length())
		return;

	m_sInput.pop_back();

	CHUD::SetChatInput(m_sInput.c_str());

}
extern bool ProcessLocalCommands(const char str[]);

void CKeyBoard::Send()
{
	if(!m_sInput.empty()) {

		switch (dop_butt) {
			case 0:{
				m_sInput = "/me " + m_sInput;
				break;
			}
			case 1:{
				m_sInput = "/do " + m_sInput;
				break;
			}
			case 2:{
				m_sInput = "/try " + m_sInput;
				break;
			}
		}

		if (m_pkHistory) m_pkHistory->AddStringToHistory(m_sInput);

		if (m_sInput[0] == '/') {
			if (!ProcessLocalCommands(m_sInput.c_str())) {
				pNetGame->SendChatCommand(m_sInput.c_str());
			}
		} else {
			pNetGame->SendChatMessage(m_sInput.c_str());
		}
	}
	CKeyBoard::Flush();
}

kbKey *CKeyBoard::GetKeyFromPos(int x, int y)
{
	int iRow = (y - m_Pos.y) / m_fKeySizeY;

	if (iRow <= 0)
		return nullptr;

	for (auto key : m_Rows[m_iLayout][iRow - 1])
	{
		if (x >= key.pos.x && x <= (key.pos.x + key.width))
		{
			kbKey *pKey = new kbKey;
			memcpy((void *)pKey, (const void *)&key, sizeof(kbKey));
			return pKey;
		}
	}

	Log("UNKNOWN KEY");
	return nullptr;
}

void CKeyBoard::InitENG()
{
	Log(__FUNCTION__);
	ImVec2 curPos;
	std::vector<kbKey> *row = nullptr;
	std::vector<kbKey>::iterator it;

	float defWidth = m_Size.x / 10;
	struct kbKey key;
	key.type = KEY_HISTORY;
	key.id = 0;

	// 1-ый ряд
	row = &m_Rows[LAYOUT_ENG][0];
	curPos = ImVec2(0, m_Pos.y + m_fKeySizeY);

	key.type = KEY_DEFAULT;
	// q/Q
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'q';
	key.code[UPPER_CASE] = 'Q';
	cp1251_to_utf8(key.name[LOWER_CASE], "q");
	cp1251_to_utf8(key.name[UPPER_CASE], "Q");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// w/W
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'w';
	key.code[UPPER_CASE] = 'W';
	cp1251_to_utf8(key.name[LOWER_CASE], "w");
	cp1251_to_utf8(key.name[UPPER_CASE], "W");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// e/E
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'e';
	key.code[UPPER_CASE] = 'E';
	cp1251_to_utf8(key.name[LOWER_CASE], "e");
	cp1251_to_utf8(key.name[UPPER_CASE], "E");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// r/R
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'r';
	key.code[UPPER_CASE] = 'R';
	cp1251_to_utf8(key.name[LOWER_CASE], "r");
	cp1251_to_utf8(key.name[UPPER_CASE], "R");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// t/T
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 't';
	key.code[UPPER_CASE] = 'T';
	cp1251_to_utf8(key.name[LOWER_CASE], "t");
	cp1251_to_utf8(key.name[UPPER_CASE], "T");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// y/Y
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'y';
	key.code[UPPER_CASE] = 'Y';
	cp1251_to_utf8(key.name[LOWER_CASE], "y");
	cp1251_to_utf8(key.name[UPPER_CASE], "Y");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// u/U
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'u';
	key.code[UPPER_CASE] = 'U';
	cp1251_to_utf8(key.name[LOWER_CASE], "u");
	cp1251_to_utf8(key.name[UPPER_CASE], "U");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// i/I
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'i';
	key.code[UPPER_CASE] = 'I';
	cp1251_to_utf8(key.name[LOWER_CASE], "i");
	cp1251_to_utf8(key.name[UPPER_CASE], "I");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// o/O
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'o';
	key.code[UPPER_CASE] = 'O';
	cp1251_to_utf8(key.name[LOWER_CASE], "o");
	cp1251_to_utf8(key.name[UPPER_CASE], "O");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// p/P
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'p';
	key.code[UPPER_CASE] = 'P';
	cp1251_to_utf8(key.name[LOWER_CASE], "p");
	cp1251_to_utf8(key.name[UPPER_CASE], "P");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 2-й ряд
	row = &m_Rows[LAYOUT_ENG][1];
	curPos.x = defWidth * 0.5;
	curPos.y += m_fKeySizeY;

	// a/A
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'a';
	key.code[UPPER_CASE] = 'A';
	cp1251_to_utf8(key.name[LOWER_CASE], "a");
	cp1251_to_utf8(key.name[UPPER_CASE], "A");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// s/S
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 's';
	key.code[UPPER_CASE] = 'S';
	cp1251_to_utf8(key.name[LOWER_CASE], "s");
	cp1251_to_utf8(key.name[UPPER_CASE], "S");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// d/D
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'd';
	key.code[UPPER_CASE] = 'D';
	cp1251_to_utf8(key.name[LOWER_CASE], "d");
	cp1251_to_utf8(key.name[UPPER_CASE], "D");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// f/F
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'f';
	key.code[UPPER_CASE] = 'F';
	cp1251_to_utf8(key.name[LOWER_CASE], "f");
	cp1251_to_utf8(key.name[UPPER_CASE], "F");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// g/G
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'g';
	key.code[UPPER_CASE] = 'G';
	cp1251_to_utf8(key.name[LOWER_CASE], "g");
	cp1251_to_utf8(key.name[UPPER_CASE], "G");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// h/H
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'h';
	key.code[UPPER_CASE] = 'H';
	cp1251_to_utf8(key.name[LOWER_CASE], "h");
	cp1251_to_utf8(key.name[UPPER_CASE], "H");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// j/J
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'j';
	key.code[UPPER_CASE] = 'J';
	cp1251_to_utf8(key.name[LOWER_CASE], "j");
	cp1251_to_utf8(key.name[UPPER_CASE], "J");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// k/K
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'k';
	key.code[UPPER_CASE] = 'K';
	cp1251_to_utf8(key.name[LOWER_CASE], "k");
	cp1251_to_utf8(key.name[UPPER_CASE], "K");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// l/L
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'l';
	key.code[UPPER_CASE] = 'L';
	cp1251_to_utf8(key.name[LOWER_CASE], "l");
	cp1251_to_utf8(key.name[UPPER_CASE], "L");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 3-й ряд
	row = &m_Rows[LAYOUT_ENG][2];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// Shift
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth * 1.5;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SHIFT;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// z/Z
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'z';
	key.code[UPPER_CASE] = 'Z';
	cp1251_to_utf8(key.name[LOWER_CASE], "z");
	cp1251_to_utf8(key.name[UPPER_CASE], "Z");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// x/X
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'x';
	key.code[UPPER_CASE] = 'X';
	cp1251_to_utf8(key.name[LOWER_CASE], "x");
	cp1251_to_utf8(key.name[UPPER_CASE], "X");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// c/C
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'c';
	key.code[UPPER_CASE] = 'C';
	cp1251_to_utf8(key.name[LOWER_CASE], "c");
	cp1251_to_utf8(key.name[UPPER_CASE], "C");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// v/V
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'v';
	key.code[UPPER_CASE] = 'V';
	cp1251_to_utf8(key.name[LOWER_CASE], "v");
	cp1251_to_utf8(key.name[UPPER_CASE], "V");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// b/B
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'b';
	key.code[UPPER_CASE] = 'B';
	cp1251_to_utf8(key.name[LOWER_CASE], "b");
	cp1251_to_utf8(key.name[UPPER_CASE], "B");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// n/N
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'n';
	key.code[UPPER_CASE] = 'N';
	cp1251_to_utf8(key.name[LOWER_CASE], "n");
	cp1251_to_utf8(key.name[UPPER_CASE], "N");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// m/M
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'm';
	key.code[UPPER_CASE] = 'M';
	cp1251_to_utf8(key.name[LOWER_CASE], "m");
	cp1251_to_utf8(key.name[UPPER_CASE], "M");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// delete
	key.pos = curPos;
	key.symPos = ImVec2(0, 0);
	key.width = defWidth * 1.5;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_BACKSPACE;
	key.id++;
	it = row->begin();
	row->insert(it, key);

	// 4-я строка
	row = &m_Rows[LAYOUT_ENG][3];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	key.type = KEY_DEFAULT;

	// slash (/)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '/';
	key.code[UPPER_CASE] = '/';
	cp1251_to_utf8(key.name[LOWER_CASE], "/");
	cp1251_to_utf8(key.name[UPPER_CASE], "/");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// comma (,)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ',';
	key.code[UPPER_CASE] = ',';
	cp1251_to_utf8(key.name[LOWER_CASE], ",");
	cp1251_to_utf8(key.name[UPPER_CASE], ",");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// switch language
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.2, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SWITCH;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// Space
	key.pos = curPos;
	key.symPos = ImVec2(0, 0);
	key.width = defWidth * 4;
	key.code[LOWER_CASE] = ' ';
	key.code[UPPER_CASE] = ' ';
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SPACE;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// ?
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '?';
	key.code[UPPER_CASE] = '?';
	cp1251_to_utf8(key.name[LOWER_CASE], "?");
	cp1251_to_utf8(key.name[UPPER_CASE], "?");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// !
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '!';
	key.code[UPPER_CASE] = '!';
	cp1251_to_utf8(key.name[LOWER_CASE], "!");
	cp1251_to_utf8(key.name[UPPER_CASE], "!");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// Send
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SEND;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x -= key.width;

	//	// UP
	//	key.pos = ImVec2(curPos.x, curPos.y - m_fKeySizeY * 5);
	//	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	//	key.width = defWidth;
	//	key.code[LOWER_CASE] = 0;
	//	key.code[UPPER_CASE] = 0;
	//	key.name[LOWER_CASE][0] = 0;
	//	key.name[UPPER_CASE][0] = 0;
	//	key.type = KEY_UP;
	//	key.id++;
	//	it = row->begin();
	//	row->insert(it, key);
	//	curPos.x += key.width;
	//
	//	// DOWN
	//	key.pos = ImVec2(curPos.x - defWidth * 1.3, curPos.y - m_fKeySizeY * 5);
	//	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	//	key.width = defWidth;
	//	key.code[LOWER_CASE] = 0;
	//	key.code[UPPER_CASE] = 0;
	//	key.name[LOWER_CASE][0] = 0;
	//	key.name[UPPER_CASE][0] = 0;
	//	key.type = KEY_DOWN;
	//	key.id++;
	//	it = row->begin();
	//	row->insert(it, key);

	return;
}

void CKeyBoard::InitRU()
{
	Log(__FUNCTION__);
	ImVec2 curPos;
	std::vector<kbKey> *row = nullptr;
	std::vector<kbKey>::iterator it;
	float defWidth = m_Size.x / 11;

	struct kbKey key;
	key.type = KEY_DEFAULT;
	key.id = 0;

	// 1-ый ряд
	row = &m_Rows[LAYOUT_RUS][0];
	curPos = ImVec2(0, m_Pos.y + m_fKeySizeY);

	// й/Й
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'й';
	key.code[UPPER_CASE] = 'Й';
	cp1251_to_utf8(key.name[LOWER_CASE], "й");
	cp1251_to_utf8(key.name[UPPER_CASE], "Й");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ц/Ц
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ц';
	key.code[UPPER_CASE] = 'Ц';
	cp1251_to_utf8(key.name[LOWER_CASE], "ц");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ц");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// у/У
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'у';
	key.code[UPPER_CASE] = 'У';
	cp1251_to_utf8(key.name[LOWER_CASE], "у");
	cp1251_to_utf8(key.name[UPPER_CASE], "У");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// к/К
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'к';
	key.code[UPPER_CASE] = 'К';
	cp1251_to_utf8(key.name[LOWER_CASE], "к");
	cp1251_to_utf8(key.name[UPPER_CASE], "К");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// е/Е
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'е';
	key.code[UPPER_CASE] = 'Е';
	cp1251_to_utf8(key.name[LOWER_CASE], "е");
	cp1251_to_utf8(key.name[UPPER_CASE], "Е");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// н/Н
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'н';
	key.code[UPPER_CASE] = 'Н';
	cp1251_to_utf8(key.name[LOWER_CASE], "н");
	cp1251_to_utf8(key.name[UPPER_CASE], "Н");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// г/Г
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'г';
	key.code[UPPER_CASE] = 'Г';
	cp1251_to_utf8(key.name[LOWER_CASE], "г");
	cp1251_to_utf8(key.name[UPPER_CASE], "Г");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ш/Ш
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ш';
	key.code[UPPER_CASE] = 'Ш';
	cp1251_to_utf8(key.name[LOWER_CASE], "ш");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ш");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// щ/Щ
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'щ';
	key.code[UPPER_CASE] = 'Щ';
	cp1251_to_utf8(key.name[LOWER_CASE], "щ");
	cp1251_to_utf8(key.name[UPPER_CASE], "Щ");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// з/З
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'з';
	key.code[UPPER_CASE] = 'З';
	cp1251_to_utf8(key.name[LOWER_CASE], "з");
	cp1251_to_utf8(key.name[UPPER_CASE], "З");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// х/Х
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'х';
	key.code[UPPER_CASE] = 'Х';
	cp1251_to_utf8(key.name[LOWER_CASE], "х");
	cp1251_to_utf8(key.name[UPPER_CASE], "Х");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 2-й ряд
	row = &m_Rows[LAYOUT_RUS][1];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// ф/Ф
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ф';
	key.code[UPPER_CASE] = 'Ф';
	cp1251_to_utf8(key.name[LOWER_CASE], "ф");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ф");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ы/Ы
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ы';
	key.code[UPPER_CASE] = 'Ы';
	cp1251_to_utf8(key.name[LOWER_CASE], "ы");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ы");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// в/В
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'в';
	key.code[UPPER_CASE] = 'В';
	cp1251_to_utf8(key.name[LOWER_CASE], "в");
	cp1251_to_utf8(key.name[UPPER_CASE], "В");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// а/А
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'а';
	key.code[UPPER_CASE] = 'А';
	cp1251_to_utf8(key.name[LOWER_CASE], "а");
	cp1251_to_utf8(key.name[UPPER_CASE], "А");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// п/П
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'п';
	key.code[UPPER_CASE] = 'П';
	cp1251_to_utf8(key.name[LOWER_CASE], "п");
	cp1251_to_utf8(key.name[UPPER_CASE], "П");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// р/Р
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'р';
	key.code[UPPER_CASE] = 'Р';
	cp1251_to_utf8(key.name[LOWER_CASE], "р");
	cp1251_to_utf8(key.name[UPPER_CASE], "Р");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// о/О
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'о';
	key.code[UPPER_CASE] = 'О';
	cp1251_to_utf8(key.name[LOWER_CASE], "о");
	cp1251_to_utf8(key.name[UPPER_CASE], "О");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// л/Л
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'л';
	key.code[UPPER_CASE] = 'Л';
	cp1251_to_utf8(key.name[LOWER_CASE], "л");
	cp1251_to_utf8(key.name[UPPER_CASE], "Л");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// д/Д
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'д';
	key.code[UPPER_CASE] = 'Д';
	cp1251_to_utf8(key.name[LOWER_CASE], "д");
	cp1251_to_utf8(key.name[UPPER_CASE], "Д");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ж/Ж
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ж';
	key.code[UPPER_CASE] = 'Ж';
	cp1251_to_utf8(key.name[LOWER_CASE], "ж");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ж");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// э/Э
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'э';
	key.code[UPPER_CASE] = 'Э';
	cp1251_to_utf8(key.name[LOWER_CASE], "э");
	cp1251_to_utf8(key.name[UPPER_CASE], "Э");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 3-й ряд
	row = &m_Rows[LAYOUT_RUS][2];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// Shift
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SHIFT;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// я/Я
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'я';
	key.code[UPPER_CASE] = 'Я';
	cp1251_to_utf8(key.name[LOWER_CASE], "я");
	cp1251_to_utf8(key.name[UPPER_CASE], "Я");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ч/Ч
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ч';
	key.code[UPPER_CASE] = 'Ч';
	cp1251_to_utf8(key.name[LOWER_CASE], "ч");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ч");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// с/С
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'с';
	key.code[UPPER_CASE] = 'С';
	cp1251_to_utf8(key.name[LOWER_CASE], "с");
	cp1251_to_utf8(key.name[UPPER_CASE], "С");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// м/М
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'м';
	key.code[UPPER_CASE] = 'М';
	cp1251_to_utf8(key.name[LOWER_CASE], "м");
	cp1251_to_utf8(key.name[UPPER_CASE], "М");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// и/И
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'и';
	key.code[UPPER_CASE] = 'И';
	cp1251_to_utf8(key.name[LOWER_CASE], "и");
	cp1251_to_utf8(key.name[UPPER_CASE], "И");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// т/Т
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'т';
	key.code[UPPER_CASE] = 'Т';
	cp1251_to_utf8(key.name[LOWER_CASE], "т");
	cp1251_to_utf8(key.name[UPPER_CASE], "Т");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ь/Ь
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ь';
	key.code[UPPER_CASE] = 'Ь';
	cp1251_to_utf8(key.name[LOWER_CASE], "ь");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ь");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// б/Б
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'б';
	key.code[UPPER_CASE] = 'Б';
	cp1251_to_utf8(key.name[LOWER_CASE], "б");
	cp1251_to_utf8(key.name[UPPER_CASE], "Б");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ю/Ю
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ю';
	key.code[UPPER_CASE] = 'Ю';
	cp1251_to_utf8(key.name[LOWER_CASE], "ю");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ю");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// backspace
	key.pos = curPos;
	key.symPos = ImVec2(0, 0);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.id++;
	key.type = KEY_BACKSPACE;
	it = row->begin();
	row->insert(it, key);

	// 4-я строка
	row = &m_Rows[LAYOUT_RUS][3];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	key.type = KEY_DEFAULT;
	defWidth = m_Size.x / 10;

	// slash (/)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '/';
	key.code[UPPER_CASE] = '/';
	cp1251_to_utf8(key.name[LOWER_CASE], "/");
	cp1251_to_utf8(key.name[UPPER_CASE], "/");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// comma (,)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ',';
	key.code[UPPER_CASE] = ',';
	cp1251_to_utf8(key.name[LOWER_CASE], ",");
	cp1251_to_utf8(key.name[UPPER_CASE], ",");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// switch language
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.2, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SWITCH;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// Space
	key.pos = curPos;
	key.symPos = ImVec2(0, 0);
	key.width = defWidth * 4;
	key.code[LOWER_CASE] = ' ';
	key.code[UPPER_CASE] = ' ';
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SPACE;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// ?
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '?';
	key.code[UPPER_CASE] = '?';
	cp1251_to_utf8(key.name[LOWER_CASE], "?");
	cp1251_to_utf8(key.name[UPPER_CASE], "?");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// !
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '!';
	key.code[UPPER_CASE] = '!';
	cp1251_to_utf8(key.name[LOWER_CASE], "!");
	cp1251_to_utf8(key.name[UPPER_CASE], "!");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// Send
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SEND;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x -= key.width;

	//	// UP
	//	key.pos = ImVec2(curPos.x, curPos.y - m_fKeySizeY * 5);
	//	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	//	key.width = defWidth;
	//	key.code[LOWER_CASE] = 0;
	//	key.code[UPPER_CASE] = 0;
	//	key.name[LOWER_CASE][0] = 0;
	//	key.name[UPPER_CASE][0] = 0;
	//	key.type = KEY_UP;
	//	key.id++;
	//	it = row->begin();
	//	row->insert(it, key);
	//	curPos.x += key.width;
	//
	//	// DOWN
	//	key.pos = ImVec2(curPos.x - defWidth * 1.3, curPos.y - m_fKeySizeY * 5);
	//	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	//	key.width = defWidth;
	//	key.code[LOWER_CASE] = 0;
	//	key.code[UPPER_CASE] = 0;
	//	key.name[LOWER_CASE][0] = 0;
	//	key.name[UPPER_CASE][0] = 0;
	//	key.type = KEY_DOWN;
	//	key.id++;
	//	it = row->begin();
	//	row->insert(it, key);

	return;
}

void CKeyBoard::InitNUM()
{
	Log(__FUNCTION__);
	ImVec2 curPos;
	std::vector<kbKey> *row = nullptr;
	std::vector<kbKey>::iterator it;
	float defWidth = m_Size.x / 10;

	struct kbKey key;
	key.type = KEY_DEFAULT;
	key.id = 0;

	// 1-ый ряд
	row = &m_Rows[LAYOUT_NUM][0];
	curPos = ImVec2(0, m_Pos.y + m_fKeySizeY);

	// 1
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '1';
	key.code[UPPER_CASE] = '1';
	cp1251_to_utf8(key.name[LOWER_CASE], "1");
	cp1251_to_utf8(key.name[UPPER_CASE], "1");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 2
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '2';
	key.code[UPPER_CASE] = '2';
	cp1251_to_utf8(key.name[LOWER_CASE], "2");
	cp1251_to_utf8(key.name[UPPER_CASE], "2");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 3
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '3';
	key.code[UPPER_CASE] = '3';
	cp1251_to_utf8(key.name[LOWER_CASE], "3");
	cp1251_to_utf8(key.name[UPPER_CASE], "3");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 4
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '4';
	key.code[UPPER_CASE] = '4';
	cp1251_to_utf8(key.name[LOWER_CASE], "4");
	cp1251_to_utf8(key.name[UPPER_CASE], "4");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 5
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '5';
	key.code[UPPER_CASE] = '5';
	cp1251_to_utf8(key.name[LOWER_CASE], "5");
	cp1251_to_utf8(key.name[UPPER_CASE], "5");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 6
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '6';
	key.code[UPPER_CASE] = '6';
	cp1251_to_utf8(key.name[LOWER_CASE], "6");
	cp1251_to_utf8(key.name[UPPER_CASE], "6");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 7
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '7';
	key.code[UPPER_CASE] = '7';
	cp1251_to_utf8(key.name[LOWER_CASE], "7");
	cp1251_to_utf8(key.name[UPPER_CASE], "7");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 8
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '8';
	key.code[UPPER_CASE] = '8';
	cp1251_to_utf8(key.name[LOWER_CASE], "8");
	cp1251_to_utf8(key.name[UPPER_CASE], "8");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 9
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '9';
	key.code[UPPER_CASE] = '9';
	cp1251_to_utf8(key.name[LOWER_CASE], "9");
	cp1251_to_utf8(key.name[UPPER_CASE], "9");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 0
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '0';
	key.code[UPPER_CASE] = '0';
	cp1251_to_utf8(key.name[LOWER_CASE], "0");
	cp1251_to_utf8(key.name[UPPER_CASE], "0");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 2-й ряд
	row = &m_Rows[LAYOUT_NUM][1];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// @
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '@';
	key.code[UPPER_CASE] = '@';
	cp1251_to_utf8(key.name[LOWER_CASE], "@");
	cp1251_to_utf8(key.name[UPPER_CASE], "@");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// #
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '#';
	key.code[UPPER_CASE] = '#';
	cp1251_to_utf8(key.name[LOWER_CASE], "#");
	cp1251_to_utf8(key.name[UPPER_CASE], "#");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// $
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '$';
	key.code[UPPER_CASE] = '$';
	cp1251_to_utf8(key.name[LOWER_CASE], "$");
	cp1251_to_utf8(key.name[UPPER_CASE], "$");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// %
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '%';
	key.code[UPPER_CASE] = '%';
	cp1251_to_utf8(key.name[LOWER_CASE], "%");
	cp1251_to_utf8(key.name[UPPER_CASE], "%");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// "
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '"';
	key.code[UPPER_CASE] = '"';
	cp1251_to_utf8(key.name[LOWER_CASE], "\"");
	cp1251_to_utf8(key.name[UPPER_CASE], "\"");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// *
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '*';
	key.code[UPPER_CASE] = '*';
	cp1251_to_utf8(key.name[LOWER_CASE], "*");
	cp1251_to_utf8(key.name[UPPER_CASE], "*");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// (
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '(';
	key.code[UPPER_CASE] = '(';
	cp1251_to_utf8(key.name[LOWER_CASE], "(");
	cp1251_to_utf8(key.name[UPPER_CASE], "(");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// )
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ')';
	key.code[UPPER_CASE] = ')';
	cp1251_to_utf8(key.name[LOWER_CASE], ")");
	cp1251_to_utf8(key.name[UPPER_CASE], ")");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// -
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '-';
	key.code[UPPER_CASE] = '-';
	cp1251_to_utf8(key.name[LOWER_CASE], "-");
	cp1251_to_utf8(key.name[UPPER_CASE], "-");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// _
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '_';
	key.code[UPPER_CASE] = '_';
	cp1251_to_utf8(key.name[LOWER_CASE], "_");
	cp1251_to_utf8(key.name[UPPER_CASE], "_");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// 3-й ряд
	row = &m_Rows[LAYOUT_NUM][2];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// .
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '.';
	key.code[UPPER_CASE] = '.';
	cp1251_to_utf8(key.name[LOWER_CASE], ".");
	cp1251_to_utf8(key.name[UPPER_CASE], ".");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// :
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ':';
	key.code[UPPER_CASE] = ':';
	cp1251_to_utf8(key.name[LOWER_CASE], ":");
	cp1251_to_utf8(key.name[UPPER_CASE], ":");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ;
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ';';
	key.code[UPPER_CASE] = ';';
	cp1251_to_utf8(key.name[LOWER_CASE], ";");
	cp1251_to_utf8(key.name[UPPER_CASE], ";");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// +
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '+';
	key.code[UPPER_CASE] = '+';
	cp1251_to_utf8(key.name[LOWER_CASE], "+");
	cp1251_to_utf8(key.name[UPPER_CASE], "+");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// =
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '=';
	key.code[UPPER_CASE] = '=';
	cp1251_to_utf8(key.name[LOWER_CASE], "=");
	cp1251_to_utf8(key.name[UPPER_CASE], "=");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// <
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '<';
	key.code[UPPER_CASE] = '<';
	cp1251_to_utf8(key.name[LOWER_CASE], "<");
	cp1251_to_utf8(key.name[UPPER_CASE], "<");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// >
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '>';
	key.code[UPPER_CASE] = '>';
	cp1251_to_utf8(key.name[LOWER_CASE], ">");
	cp1251_to_utf8(key.name[UPPER_CASE], ">");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// [
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '[';
	key.code[UPPER_CASE] = '[';
	cp1251_to_utf8(key.name[LOWER_CASE], "[");
	cp1251_to_utf8(key.name[UPPER_CASE], "[");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// ]
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ']';
	key.code[UPPER_CASE] = ']';
	cp1251_to_utf8(key.name[LOWER_CASE], "]");
	cp1251_to_utf8(key.name[UPPER_CASE], "]");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// delete
	key.pos = curPos;
	key.symPos = ImVec2(0, 0);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_BACKSPACE;
	key.id++;
	it = row->begin();
	row->insert(it, key);

	// 4-я строка
	row = &m_Rows[LAYOUT_NUM][3];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	key.type = KEY_DEFAULT;

	// slash (/)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '/';
	key.code[UPPER_CASE] = '/';
	cp1251_to_utf8(key.name[LOWER_CASE], "/");
	cp1251_to_utf8(key.name[UPPER_CASE], "/");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// comma (,)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ',';
	key.code[UPPER_CASE] = ',';
	cp1251_to_utf8(key.name[LOWER_CASE], ",");
	cp1251_to_utf8(key.name[UPPER_CASE], ",");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// switch language
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.2, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SWITCH;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// Space
	key.pos = curPos;
	key.symPos = ImVec2(0, 0);
	key.width = defWidth * 4;
	key.code[LOWER_CASE] = ' ';
	key.code[UPPER_CASE] = ' ';
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SPACE;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// ?
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '?';
	key.code[UPPER_CASE] = '?';
	cp1251_to_utf8(key.name[LOWER_CASE], "?");
	cp1251_to_utf8(key.name[UPPER_CASE], "?");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// !
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '!';
	key.code[UPPER_CASE] = '!';
	cp1251_to_utf8(key.name[LOWER_CASE], "!");
	cp1251_to_utf8(key.name[UPPER_CASE], "!");
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x += key.width;

	// Send
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SEND;
	key.id++;
	it = row->begin();
	row->insert(it, key);
	curPos.x -= key.width;

	//	// UP
	//	key.pos = ImVec2(curPos.x, curPos.y - m_fKeySizeY * 5);
	//	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	//	key.width = defWidth;
	//	key.code[LOWER_CASE] = 0;
	//	key.code[UPPER_CASE] = 0;
	//	key.name[LOWER_CASE][0] = 0;
	//	key.name[UPPER_CASE][0] = 0;
	//	key.type = KEY_UP;
	//	key.id++;
	//	it = row->begin();
	//	row->insert(it, key);
	//	curPos.x += key.width;
	//
	//	// DOWN
	//	key.pos = ImVec2(curPos.x - defWidth * 1.3, curPos.y - m_fKeySizeY * 5);
	//	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	//	key.width = defWidth;
	//	key.code[LOWER_CASE] = 0;
	//	key.code[UPPER_CASE] = 0;
	//	key.name[LOWER_CASE][0] = 0;
	//	key.name[UPPER_CASE][0] = 0;
	//	key.type = KEY_DOWN;
	//	key.id++;
	//	it = row->begin();
	//	row->insert(it, key);
	return;
}

void CKeyBoard::Flush()
{
	if (!m_sInput.length())
		return;

	m_sInput.clear();
	CHUD::SetChatInput(m_sInput.c_str());
	memset(m_utf8Input, 0, sizeof(m_utf8Input) - 1);
}

extern void ApplyFPSPatch(uint8_t fps);
bool ProcessLocalCommands(const char str[])
{
	if (strstr(str, "/fontsize "))
	{
		float size = 0;
		if (sscanf(str, "%*s%f", &size) == -1)
		{
			CChatWindow::AddDebugMessage("Используйте: /fontsize [scale]");
			return true;
		}
		size = 27 * size;
		CHUD::ChangeChatTextSize( ceil(size) );
		return true;
	}

	if (strcmp(str, "/q") == 0)
	{
		pGame->exitGame();
		return true;
	}
	if (strstr(str, "/save "))
	{
		std::string msg;
		if (sscanf(str, "%*s%s", &msg) == -1)
		{
			CChatWindow::AddDebugMessage("Используйте: /fontsize [scale]");
			return true;
		}

		CPlayerPed *pPlayer = pGame->FindPlayerPed();
		FILE *fileOut;
		uint32_t dwVehicleID;
		float fZAngle = 0.0f;

		char ff[0xFF];
		sprintf(ff, "%sSAMP/savedposition.txt", g_pszStorage);
		fileOut = fopen(ff, "a");
		if(!fileOut)
			CChatWindow::AddDebugMessage("Cant open savedposition.txt");

		if(pPlayer->IsInVehicle())
		{
			//incar savepos
			CVehicleGta *pVehicle = pPlayer->GetGtaVehicle();
			VEHICLEID m_dwGTAId = GamePool_Vehicle_GetIndex(pVehicle);
			ScriptCommand(&get_car_z_angle, m_dwGTAId, &fZAngle);
			fprintf(fileOut, "%s = %.3f, %.3f, %.3f, %.3f\n",
					msg.c_str(),
					pVehicle->mat->pos.x,
					pVehicle->mat->pos.y,
					pVehicle->mat->pos.z,
					fZAngle
			);
			CChatWindow::AddInfoMessage("-> InCar position saved.");
		}
		else
		{
			//onfoot savepos
			CPedGta *pActor = pPlayer->GetGtaActor();
			ScriptCommand(&get_actor_z_angle, pPlayer->m_dwGTAId, &fZAngle);
			fprintf(fileOut, "%s = %.3f, %.3f, %.3f, %.3f\n",
					msg.c_str(),
					pActor->mat->pos.x,
					pActor->mat->pos.y,
					pActor->mat->pos.z,
					fZAngle
			);
			CChatWindow::AddInfoMessage("-> OnFoot position saved.");
		}

		fclose(fileOut);
		return true;
	}
	if (strcmp(str, "/cameditgui") == 0)
	{
		CHUD::bIsCamEditGui = !CHUD::bIsCamEditGui;
		CHUD::toggleAll( CHUD::bIsCamEditGui );
		return true;
	}

	if (strcmp(str, "/tab") == 0)
	{
		CTab::toggle();
		return true;
	}
	if (strcmp(str, "/testtun") == 0)
	{
		auto pVehicle = pGame->FindPlayerPed()->GetCurrentVehicle();

		pVehicle->AddVehicleUpgrade(1014);
		return true;
	}

	if (strstr(str, "/fpslimit "))
	{
		int fps = 0;
		if (sscanf(str, "%*s%d", &fps) == -1)
		{
			CChatWindow::AddDebugMessage("Используйте: /fpslimit [fps]");
			return true;
		}
		ApplyFPSPatch(fps);

		CChatWindow::AddInfoMessage("-> Новый лимит FPS: %d", fps);

		CSettings::m_Settings.iFPS = fps;
        CSettings::save();
		return true;
	}
	if (strcmp(str, "/dl") == 0)
	{
		pGame->m_bDl_enabled = !pGame->m_bDl_enabled;
		return true;
	}

	if (strcmp(str, "/settings") == 0)
	{
		g_pJavaWrapper->ShowClientSettings();
		return true;
	}

	if (strcmp(str, "/fpsinfo") == 0)
	{
		CDebugInfo::SetDrawFPS(!CDebugInfo::m_uiDrawDebug);
		return true;
	}

	return false;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CriminalHud_SendChatButton(JNIEnv *env, jobject thiz, jint button_id) {
	CKeyBoard::dop_butt = button_id;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_criminal_moscow_gui_CriminalHud_isAndroidKeyboard(JNIEnv *env, jobject thiz) {
    return CSettings::Get().iAndroidKeyboard;
}
