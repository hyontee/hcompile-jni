#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
#include "keyboard.h"
#include "CHack.h"

#include <stdlib.h>
#include <string.h>

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;

bool CHack::m_bIsActive;
int CHack::step;

CHack::CHack()
{
	m_bIsActive = false;
}

CHack::~CHack()
{

}

void CHack::Show(bool bShow)
{
	m_bIsActive = bShow;
	CHack::Render();
}

void CHack::Clear()
{
	m_bIsActive = false;
	CHack::Render();
}

void CHack::Render()
{
	if(!m_bIsActive) return;

	ImGuiIO &io = ImGui::GetIO();
	ImGui::StyleColorsClassic();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));

	ImGui::Begin("Beta (debug) admin menu", nullptr,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

	if(ImGui::Button("WallHack", ImVec2(125, 50)))
	{
		Show(false);
	}
	ImGui::SetWindowSize(ImVec2(-1, -1));
	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y - size.y)/2)) );
	ImGui::End();

	ImGui::PopStyleVar();
}