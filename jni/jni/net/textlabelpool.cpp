#include "../main.h"
#include "../gui/gui.h"
#include "../game/game.h"
#include "../net/netgame.h"

extern CNetGame *pNetGame;
extern CGame *pGame;
extern CGUI *pGUI;

CText3DLabelsPool::CText3DLabelsPool()
{
	for(int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		m_pTextLabels[x] = nullptr;
		m_bSlotState[x] = false;

		m_pTexture[x] = nullptr;
		SetShowBackground(x, false);
		SetShowIcon(x, false);
	}
	m_TextLabelCount.clear();
}

CText3DLabelsPool::~CText3DLabelsPool()
{
	for (int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		if (m_pTextLabels[x])
		{
			m_pTextLabels[x]->text[0] = '\0';
			delete m_pTextLabels[x];
			m_pTextLabels[x] = nullptr;
		}

		m_pTexture[x] = nullptr;
		SetShowBackground(x, false);
		SetShowIcon(x, false);
	}
	m_TextLabelCount.clear();
}

void FilterColors(char* szStr)
{
	if(!szStr) return;

	char szNonColored[2048+1];
	int iNonColoredMsgLen = 0;

	for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
	{
		if(pos+7 < strlen(szStr))
		{
			if(szStr[pos] == '{' && szStr[pos+7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColored[iNonColoredMsgLen] = szStr[pos];
		iNonColoredMsgLen++;
	}

	szNonColored[iNonColoredMsgLen] = 0;
	strcpy(szStr, szNonColored);
}

void CText3DLabelsPool::CreateTextLabel(int labelID, char* text, uint32_t color,
	float posX, float posY, float posZ, float drawDistance, bool useLOS, PLAYERID attachedToPlayerID, VEHICLEID attachedToVehicleID)
{
	if (m_pTextLabels[labelID])
	{
		delete m_pTextLabels[labelID];
		m_pTextLabels[labelID] = nullptr;
		
		m_bSlotState[labelID] = false;
	}
	TEXT_LABELS* pTextLabel = new TEXT_LABELS;
	if (pTextLabel)
	{
		//pTextLabel->text = text;
		cp1251_to_utf8(pTextLabel->text, text);
		cp1251_to_utf8(pTextLabel->textWithoutColors, text);
		FilterColors(pTextLabel->textWithoutColors);
		pTextLabel->color = color;
		pTextLabel->pos.X = posX;
		pTextLabel->pos.Y = posY;
		pTextLabel->pos.Z = posZ;
		pTextLabel->drawDistance = drawDistance;
		pTextLabel->useLineOfSight = useLOS;
		pTextLabel->attachedToPlayerID = attachedToPlayerID;
		pTextLabel->attachedToVehicleID = attachedToVehicleID;

		pTextLabel->m_fTrueX = -1;
		if (attachedToVehicleID != INVALID_VEHICLE_ID || attachedToPlayerID != INVALID_PLAYER_ID)
		{
			pTextLabel->offsetCoords.X = posX;
			pTextLabel->offsetCoords.Y = posY;
			pTextLabel->offsetCoords.Z = posZ;
		}
		
		m_pTextLabels[labelID] = pTextLabel;
		m_bSlotState[labelID] = true;
		
		m_TextLabelCount.push_back(labelID);
	}
}

void CText3DLabelsPool::Delete(int labelID)
{
	if (m_pTextLabels[labelID])
	{
		m_pTextLabels[labelID]->attachedToPlayerID = INVALID_PLAYER_ID;
		m_pTextLabels[labelID]->attachedToVehicleID = INVALID_VEHICLE_ID;
		delete m_pTextLabels[labelID];
		m_pTextLabels[labelID] = nullptr;
		m_bSlotState[labelID] = false;
		
		SetShowBackground(labelID, false);
		SetShowIcon(labelID, false);
		
		if (m_pTexture[labelID] != nullptr)
		{
			RwTextureDestroy((RwTexture*)m_pTexture[labelID]);
			m_pTexture[labelID] = nullptr;
		}
		
		m_TextLabelCount.remove(labelID);
	}
}

void CText3DLabelsPool::AttachToPlayer(int labelID, PLAYERID playerID, VECTOR pos)
{
	if (m_bSlotState[labelID] == true)
	{
		//tempPlayerID = playerID;
		m_pTextLabels[labelID]->attachedToPlayerID = playerID;
		m_pTextLabels[labelID]->pos = pos;
		m_pTextLabels[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::AttachToVehicle(int labelID, VEHICLEID vehicleID, VECTOR pos)
{
	if (m_bSlotState[labelID] == true)
	{
		m_pTextLabels[labelID]->attachedToVehicleID = vehicleID;
		m_pTextLabels[labelID]->pos = pos;
		m_pTextLabels[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::Update3DLabel(int labelID, uint32_t color, char* text)
{
	if (m_bSlotState[labelID] == true)
	{
		m_pTextLabels[labelID]->color = color;
		//m_pTextLabels[labelID]->text = text;
		cp1251_to_utf8(m_pTextLabels[labelID]->text, text);
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);
void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szWithColors = nullptr)
{
	if (pNetGame)
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
				if (pPlayer->GetPlayerPed())
				{
					if (pPlayer->GetPlayerPed()->GetActionTrigger() == ACTION_DEATH || pPlayer->GetPlayerPed()->IsDead())
					{
						return;
					}
				}
			}
		}
	}
	
	char tempStr[4096];

	ImVec2 vecPos = pos;

	strcpy(tempStr, szStr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				pGUI->RenderText(vecPos, col, true, textStart, textCur);
				vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
			{
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				col = textColor;
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			// Print accumulated text an go to next line
			pGUI->RenderText(vecPos, col, true, textStart, textCur);
			vecPos.x = pos.x;
			vecPos.y += pGUI->GetFontSize();
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
	{
		pGUI->RenderText(vecPos, col, true, textStart, textCur);
		vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
	}
	else
		vecPos.y += pGUI->GetFontSize();
}

ImVec2 CalcTextSizeWithoutTags(char* szStr);

void Render3DLabel(ImVec2 pos, char* utf8string, uint32_t dwColor)
{
	uint16_t linesCount = 0;
	std::string strUtf8 = utf8string;
	int size = strUtf8.length();
	std::string color;

	if (pGUI->bLabelBackground)
	{	
		ImVec2 backgroundSize = CalcTextSizeWithoutTags(utf8string);
		backgroundSize.y += pGUI->GetFontSize() * 2;
		backgroundSize.x += pGUI->GetFontSize() * 2;

		ImVec2 backgroundPos1 = ImVec2(pos.x - (backgroundSize.x / 2), pos.y - (backgroundSize.y / 2));
		ImVec2 backgroundPos2 = ImVec2(backgroundPos1.x + backgroundSize.x, backgroundPos1.y + backgroundSize.y);

		backgroundPos1.y -= pGUI->GetFontSize() * 1.5f;
		ImGui::GetBackgroundDrawList()->AddRectFilled(backgroundPos1, backgroundPos2, ImColor(0.06f, 0.05f, 0.07f, 0.70f), 7.0f);
	}

	ALL:

	for(uint32_t i = 0; i < size; i++)
	{
		if(i+7 < strUtf8.length())
		{
			if(strUtf8[i] == '{' && strUtf8[i+7] == '}' )
			{
				color = strUtf8.substr(i, 7+1);
			}
		}
		if(strUtf8[i] == '\n')
		{
			linesCount++;
			if(i+1 < strUtf8.length() && !color.empty())
			{
				strUtf8.insert(i+1 , color);
				size += color.length();
				color.clear();
			}
		}
		if(strUtf8[i] == '\t')
		{
			strUtf8.replace(i, 1, " ");
		}
	}
	pos.y += pGUI->GetFontSize()*(linesCount / 2);
	if(linesCount)
	{
		uint16_t curLine = 0;
		uint16_t curIt = 0;
		for(uint32_t i = 0; i < strUtf8.length(); i++)
		{
			if(strUtf8[i] == '\n')
			{
				if(strUtf8[curIt] == '\n' )
				{
					curIt++;
				}
				ImVec2 _pos = pos;
				_pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, i-curIt).c_str()).x / 2;
				_pos.y -= ( pGUI->GetFontSize()*(linesCount - curLine) );
				TextWithColors( _pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, i-curIt).c_str() );
				curIt = i;
				curLine++;
			}
		}
		if(strUtf8[curIt] == '\n')
		{
			curIt++;
		}
		if(strUtf8[curIt] != '\0')
		{
			ImVec2 _pos = pos;
			_pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, strUtf8.size()-curIt).c_str()).x / 2;
			_pos.y -= ( pGUI->GetFontSize()*(linesCount - curLine) );
			TextWithColors( _pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, strUtf8.length()-curIt).c_str() );
		}
	}
	else
	{
		pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.c_str()).x / 2;
		TextWithColors( pos, __builtin_bswap32(dwColor), (char*)strUtf8.c_str() );
	}
}

void CText3DLabelsPool::Draw()
{
	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_TEXTLABELS)) return;
	
	TEXT_LABELS *m_pTextLabel;
	int hitEntity = 0;
	for (int x: m_TextLabelCount)
	{
		if (x == MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2) continue;
		if (m_bSlotState[x])
		{
			CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			//D3DXVECTOR3 textPos;
			VECTOR textPos;
			if(pPlayerPool)
			{
				m_pTextLabel = m_pTextLabels[x];
				if (m_pTextLabel->attachedToPlayerID != INVALID_PLAYER_ID)
				{
					if (m_pTextLabel->attachedToPlayerID == pPlayerPool->GetLocalPlayerID())
						continue;

					if (pPlayerPool->GetSlotState(m_pTextLabel->attachedToPlayerID) == true)
					{
						if(!pPlayerPool->GetAt(m_pTextLabel->attachedToPlayerID))
							continue;
						
						CPlayerPed* pPlayerPed = pPlayerPool->GetAt(m_pTextLabel->attachedToPlayerID)->GetPlayerPed();
						if(!pPlayerPed)
							continue;
						
						if (!pPlayerPed->IsAdded())
							continue;

						VECTOR matPlayer;
						pPlayerPed->GetBonePosition(8, &matPlayer);

						textPos.X = matPlayer.X + m_pTextLabel->offsetCoords.X;
						textPos.Y = matPlayer.Y + m_pTextLabel->offsetCoords.Y;
						textPos.Z = matPlayer.Z + 0.23 + m_pTextLabel->offsetCoords.Z;
					}
				}
			}
			if(pVehiclePool && m_pTextLabel->attachedToVehicleID != INVALID_VEHICLE_ID)
			{
				if (pVehiclePool->GetSlotState(m_pTextLabel->attachedToVehicleID) == true)
				{
					CVehicle* pVehicle = pVehiclePool->GetAt(m_pTextLabel->attachedToVehicleID);
					if(!pVehicle)
						continue;
					
					if (!pVehicle->IsAdded())
						continue;
					
					MATRIX4X4 matVehicle;
					pVehicle->GetMatrix(&matVehicle);

					textPos.X = matVehicle.pos.X + m_pTextLabel->offsetCoords.X;
					textPos.Y = matVehicle.pos.Y + m_pTextLabel->offsetCoords.Y;
					textPos.Z = matVehicle.pos.Z + m_pTextLabel->offsetCoords.Z;
				}
			}
			else if(m_pTextLabel->attachedToVehicleID == INVALID_VEHICLE_ID && m_pTextLabel->attachedToPlayerID == INVALID_PLAYER_ID)
			{
				textPos.X = m_pTextLabel->pos.X;
				textPos.Y = m_pTextLabel->pos.Y;
				textPos.Z = m_pTextLabel->pos.Z;
			}
			if (m_pTextLabel->useLineOfSight)
			{
				MATRIX4X4 mat;
				VECTOR playerPosition;

				CAMERA_AIM *pCam = GameGetInternalAim();
				pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetMatrix(&mat);

				playerPosition.X = mat.pos.X;
				playerPosition.Y = mat.pos.Y;
				playerPosition.Z = mat.pos.Z;

				if (m_pTextLabel->useLineOfSight) hitEntity = 1;
					/*hitEntity = ScriptCommand(&get_line_of_sight,
					playerPosition.X, playerPosition.Y, playerPosition.Z,
					pCam->pos1x, pCam->pos1y, pCam->pos1z,
					1, 0, 0, 0, 0);*/
			}
			m_pTextLabel->pos.X = textPos.X;
			m_pTextLabel->pos.Y = textPos.Y;
			m_pTextLabel->pos.Z = textPos.Z;
			if (!m_pTextLabel->useLineOfSight || hitEntity)
			{
				CPlayerPed* pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
				if(!pPlayerPed)
					continue;
				
				if(!pPlayerPed->IsAdded())
					continue;
				
				if (pPlayerPed->GetDistanceFromPoint(m_pTextLabel->pos.X, m_pTextLabel->pos.Y, m_pTextLabel->pos.Z) <= m_pTextLabel->drawDistance)
				{
					VECTOR Out;

					// CSprite::CalcScreenCoors
					(( void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(g_libGTASA+0x54EEC0+1))(&textPos, &Out, 0, 0, 0, 0);
					if(Out.Z < 1.0f) continue;
					ImVec2 pos = ImVec2(Out.X, Out.Y);
					
					// removed piece
					if (IsShowBackground(x))
					{
						ImVec2 backgroundSize = CalcTextSizeWithoutTags(m_pTextLabel->text);
						backgroundSize.y += pGUI->GetFontSize() * 2;
						backgroundSize.x += pGUI->GetFontSize() * 2;
						
						ImVec2 backgroundPos1 = ImVec2(pos.x - (backgroundSize.x / 2), pos.y - (backgroundSize.y / 2));
						ImVec2 backgroundPos2 = ImVec2(backgroundPos1.x + backgroundSize.x, backgroundPos1.y + backgroundSize.y);

						backgroundPos1.y -= pGUI->GetFontSize() * 1.5f;
						ImGui::GetBackgroundDrawList()->AddRectFilled(backgroundPos1, backgroundPos2, ImColor(0.06f, 0.05f, 0.07f, 0.70f), 7.0f);
					}
					if (IsShowIcon(x))
					{
						const char* iconTexture = GetIconTexture(x);
						if (iconTexture != nullptr)
						{
							if (m_pTexture[x] == nullptr)
							{
								m_pTexture[x] = (RwTexture*)LoadTextureFromDB(m_szDatabase[x], iconTexture);
							}
							
							if(m_pTexture[x])
							{
								ImVec2 iconPos = ImVec2(pos.x, pos.y);
								iconPos.x -= pGUI->GetFontSize();
								iconPos.y -= pGUI->GetFontSize() * 4.25f;
								ImVec2 iconSize = ImVec2(iconPos.x + pGUI->GetFontSize() * 2, iconPos.y + pGUI->GetFontSize() * 2);
								ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pTexture[x]->raster, iconPos, iconSize);
							}
						}
					}
					Render3DLabel(pos, m_pTextLabel->text, m_pTextLabel->color);
				}
			}
		}
	}
}
