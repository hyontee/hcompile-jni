#include "main.h"
#include "gui/gui.h"
#include "gui/buttons.h"
#include "game/game.h"
#include "net/netgame.h"
#include "dialog.h"
#include "GButton.h"
#include "scoreboard.h"
#include "keyboard.h"
#include "util/interfaces/CHud.h"

#include "../CSettings.h"

extern CGUI * pGUI;
extern CGame* pGame;
extern CNetGame* pNetGame;
extern CDialogWindow* pDialogWindow;
extern CScoreBoard* pScoreBoard;
extern CKeyBoard* pKeyBoard;
extern CSettings* pSettings;

CGButton::CGButton()
{
	LoadTextureButton();
}

CGButton::~CGButton()
{
}

void CGButton::LoadTextureButton()
{
	p_GButtonTexture = (RwTexture*)LoadTextureFromDB(CMain::GetCustomStorage(), "gtexture");
}

void CGButton::RenderButton()
{
	if(pDialogWindow->m_bIsActive || pScoreBoard->m_bToggle || pKeyBoard->IsOpen() || !pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS)) return;
	
	if(!pNetGame) return;
	if(pNetGame->GetGameState() != GAMESTATE_CONNECTED) return;
	
	ImGuiIO &io = ImGui::GetIO();
	
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
	
	ImGuiStyle style;
	style.FrameBorderSize = ImGui::GetStyle().FrameBorderSize;
	ImGui::GetStyle().FrameBorderSize = 0.0f;
	
	ImGui::Begin("Passenger", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
	
	ImVec2 vecButSize_g = ImVec2(pSettings->GetReadOnly().fButtonEnterPassengerSize, pSettings->GetReadOnly().fButtonEnterPassengerSize);
	
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if(!pPlayerPed->IsInVehicle() && !pPlayerPed->IsAPassenger())
	{
		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
		if(pVehiclePool)
		{
			uint16_t sNearestVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
			if(sNearestVehicleID != INVALID_VEHICLE_ID && sNearestVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(sNearestVehicleID))
			{
				CVehicle *pVehicle = pVehiclePool->GetAt(sNearestVehicleID);
				CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
				if(pPlayerPool)
				{
					CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
					if(pLocalPlayer)
					{
						if(GetDistanceBetween3DPoints(&pGame->FindPlayerPed()->m_pEntity->mat->pos, &pNetGame->GetVehiclePool()->GetAt(sNearestVehicleID)->m_pEntity->mat->pos) < 5.0f)
						{
							if(ImGui::ImageButton((ImTextureID)p_GButtonTexture->raster, vecButSize_g))
							{
								pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
								pLocalPlayer->SendEnterVehicleNotification(sNearestVehicleID, true);
							}
							if (CButtons::m_bIsActive == true && CButtons::m_bIsShow == true)
							{
								CButtons::m_iKey[2] = 1;
								CHud::hudButton(CButtons::m_iKey[0], CButtons::m_iKey[1], CButtons::m_iKey[2], CButtons::m_iKey[3], CButtons::m_iKey[4], CButtons::m_iKey[5], CButtons::m_iKey[6]);
							}
						}
						else
						{
							if (CButtons::m_bIsActive == true && CButtons::m_bIsShow == true)
							{
								CButtons::m_iKey[2] = 0;
								CHud::hudButton(CButtons::m_iKey[0], CButtons::m_iKey[1], CButtons::m_iKey[2], CButtons::m_iKey[3], CButtons::m_iKey[4], CButtons::m_iKey[5], CButtons::m_iKey[6]);
							}
						}
					}
				}
			}
		}
	}
	else if(pPlayerPed->IsInVehicle() && !pPlayerPed->IsAPassenger())
	{
		if (CButtons::m_bIsActive == true && CButtons::m_bIsShow == true)
		{
			CButtons::m_iKey[2] = 1;
			CHud::hudButton(CButtons::m_iKey[0], CButtons::m_iKey[1], CButtons::m_iKey[2], CButtons::m_iKey[3], CButtons::m_iKey[4], CButtons::m_iKey[5], CButtons::m_iKey[6]);
		}
	}
	
	ImGui::SetWindowSize(ImVec2(-1, -1));

	ImVec2 winPos = ImVec2(pSettings->GetReadOnly().fButtonEnterPassengerX, pSettings->GetReadOnly().fButtonEnterPassengerY);

	ImGui::SetWindowPos(winPos);
	ImGui::End();
	
	ImGui::PopStyleColor(3);
	ImGui::GetStyle().FrameBorderSize = style.FrameBorderSize;
}


