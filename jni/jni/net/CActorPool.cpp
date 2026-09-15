#include "../main.h"
#include "../CSettings.h"
#include "../game/game.h"
#include "../gui/gui.h"
#include "netgame.h"

extern CSettings* pSettings;
extern CGUI* pGUI;

CActorPool::CActorPool() 
{
	for (uint16_t actorId = 0; actorId < MAX_ACTORS; actorId++) 
	{
		m_bActorSlotState[actorId] = false;
		m_pActors[actorId] = nullptr;
	}
	m_CountActors.clear();
}

CActorPool::~CActorPool() 
{
	for (uint16_t actorId = 0; actorId < MAX_ACTORS; actorId++)
	{
		Delete(actorId);
	}
	m_CountActors.clear();
}

bool CActorPool::Spawn(uint16_t actorId, int iSkin, VECTOR vecPos, float fRotation, float fHealth, float bInvulnerable) 
{
	if (!IsValidActorId(actorId))
		return false;

	if (m_pActors[actorId] != nullptr)
		Delete(actorId);

	m_pActors[actorId] = new CActorPed((uint16_t)iSkin, vecPos, fRotation, fHealth, bInvulnerable);

	if (m_pActors[actorId]) 
	{
		m_bActorSlotState[actorId] = true;
		m_CountActors.push_back(actorId);
		return true;
	}

	return false;
}

bool CActorPool::Delete(uint16_t actorId) 
{	
	if (!IsValidActorId(actorId))
		return false;

	m_bActorSlotState[actorId] = false;

	if (m_pActors[actorId]) 
	{
		delete m_pActors[actorId];
		m_pActors[actorId] = nullptr;
		m_CountActors.remove(actorId);
	}

	return true;
}

PLAYERID CActorPool::FindRemoteActorIDFromGtaPtr(PED_TYPE * pActor)
{
	for (int i: m_CountActors)
	{
		if(true == m_bActorSlotState[i])
		{
			PED_TYPE *pTestActor = m_pActors[i]->GetGtaActor();
			if ((pTestActor != NULL) && (pActor == pTestActor))
				return i;
		}
	}

	return INVALID_PLAYER_ID;
}

void CActorPool::Process()
{
	for (int i: m_CountActors)
	{
		if (m_bActorSlotState[i])
		{
			m_pActors[i]->Process();
		}
	}
}

void CActorPool::Render()
{
	for (int i: m_CountActors)
	{
		if (m_bActorSlotState[i])
		{
			CActorPed* pPed = this->GetAt(i);
			if (pPed)
			{
				if (pPed->IsAdded() && !pPed->IsDead())
				{
					VECTOR vecPos;
					vecPos.X = 0.0f;
					vecPos.Y = 0.0f;
					vecPos.Z = 0.0f;
					pPed->GetBonePosition(8, &vecPos);
					float fDist = pPed->GetDistanceFromCamera();
					this->Draw(&vecPos, fDist, pPed->GetFloatingTagText(), pPed->GetFloatingTagTextColor(), pPed->GetHealth(), pPed->GetArmour(), pPed->HasNeedDraw());
				}

				if (pPed->IsDead() && !pPed->HasDeadState())
				{
					pPed->SetDead();
					//SendSmartActorDeathPacket(i);
					pPed->SetDeadState(true);
				}
			}
		}
	}
}

void CActorPool::Draw(VECTOR* vec, float fDist, char* szInfo, uint32_t dwColor, float fHealth, float fArmour, bool draw)
{
	if(fDist > 60.0f || !szInfo) return;

	RwV3d TagPos;

	TagPos.x = vec->X;
	TagPos.y = vec->Y;
	TagPos.z = vec->Z;
	TagPos.z += 0.25f + (fDist * 0.0475f);

	RwV3d Out;

	// CSprite::CalcScreenCoors
	(( void (*)(RwV3d const&, RwV3d*, float*, float*, bool, bool))(SA_ADDR(0x54EEC0 + 1)))(TagPos, &Out, 0, 0, 0, 0);

	if(Out.z < 1.0f)
		return;

	char tempBuff[2048+1]{'\0'};
	cp1251_to_utf8(tempBuff, szInfo);

	ImVec2 pos = ImVec2(Out.x, Out.y);
	pos.x -= ImGui::CalcTextSize(tempBuff).x/2;

	pGUI->RenderText(pos, __builtin_bswap32(dwColor | (0x000000FF)), true, tempBuff);

	if (!draw) return;

	// Health Bar
	if(fHealth <= 0.0f) return;

	// îêðóãëÿåì
	// Out.X = (float)((int)Out.X);
	// Out.Y = (float)((int)Out.Y);

	ImVec2 HealthBarBDR1;
	ImVec2 HealthBarBDR2;
	ImVec2 HealthBarBG1;
	ImVec2 HealthBarBG2;
	ImVec2 HealthBar1;
	ImVec2 HealthBar2;

	ImColor HealthBarColor;
	ImColor HealthBarBGColor;
	ImColor HealthBarBDRColor;

	HealthBarBDRColor = ImColor( 0x00, 0x00, 0x00, 0xFF );

	HealthBarColor = ImColor( 0xB9, 0x22, 0x28, 0xFF );
	HealthBarBGColor = ImColor( 0x4B, 0x0B, 0x14, 0xFF );

	float fWidth = pGUI->ScaleX( pSettings->GetReadOnly().fHealthBarWidth );
	float fHeight = pGUI->ScaleY( pSettings->GetReadOnly().fHealthBarHeight );
	float fOutline = (float)pSettings->GetReadOnly().iFontOutline;

	// top left
	HealthBarBDR1.x = Out.x - ((fWidth/2) + fOutline);
	HealthBarBDR1.y = Out.y + (pGUI->GetFontSize()*1.2f);//35.0f;
	// bottom right
	HealthBarBDR2.x = Out.x + ((fWidth/2) + fOutline);
	HealthBarBDR2.y = Out.y + (pGUI->GetFontSize()*1.2f) + fHeight;//48.0f;

	// top left
	HealthBarBG1.x = HealthBarBDR1.x + fOutline;//Out.X - 40.0f;
	HealthBarBG1.y = HealthBarBDR1.y + fOutline;//Out.Y + 37.0f;
	// bottom right
	HealthBarBG2.x = HealthBarBDR2.x - fOutline;//Out.X + 40.0f;
	HealthBarBG2.y = HealthBarBDR2.y - fOutline;//Out.Y + 46.0f;

	// top left
	HealthBar1.x = HealthBarBG1.x;//Out.X - 40.0f;
	HealthBar1.y = HealthBarBG1.y;//Out.Y + 37.0f;
	// bottom right
	HealthBar2.y = HealthBarBG2.y;//Out.Y + 46.0f;

	if (fHealth > 100.0f)
		fHealth = 100.0f;

	fHealth *= fWidth/100.0f;
	fHealth -= (fWidth/2);
	HealthBar2.x = Out.x + fHealth;

	if(fArmour > 0.0f)
	{
		HealthBarBDR1.y += 13.0f;
		HealthBarBDR2.y += 13.0f;
		HealthBarBG1.y += 13.0f;
		HealthBarBG2.y += 13.0f;
		HealthBar1.y += 13.0f;
		HealthBar2.y += 13.0f;
	}

	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);

	// Armour Bar
	if(fArmour > 0.0f)
	{
		HealthBarBDR1.y -= 13.0f;
		HealthBarBDR2.y -= 13.0f;
		HealthBarBG1.y -= 13.0f;
		HealthBarBG2.y -= 13.0f;
		HealthBar1.y -= 13.0f;
		HealthBar2.y -= 13.0f;

		HealthBarColor = ImColor(200, 200, 200, 255);
		HealthBarBGColor = ImColor(40, 40, 40, 255);

		if(fArmour > 100.0f)
			fArmour = 100.0f;

		fArmour *= fWidth/100.0f;
		fArmour -= (fWidth/2);
		HealthBar2.x = Out.x + fArmour;
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);
	}
}