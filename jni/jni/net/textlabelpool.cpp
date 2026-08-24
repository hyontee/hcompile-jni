#include "../main.h"
#include "../gui/gui.h"
#include "../game/game.h"
#include "../net/netgame.h"

extern CNetGame *pNetGame;
extern CGame *pGame;
extern CGUI *pGUI;

extern int showHud;

void CText3DLabelsPool::Free()
{
	for (int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		Delete(x);
	}
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
	if (GetAt(labelID))
	{
		Delete(labelID);
	}
	auto pTextLabel = list[labelID] = new TEXT_LABELS;
	if (pTextLabel)
	{
		//pTextLabel->text = text;
		cp1251_to_utf8(pTextLabel->text, text);
		cp1251_to_utf8(pTextLabel->textWithoutColors, text);
		FilterColors(pTextLabel->textWithoutColors);
		pTextLabel->color = color;
		pTextLabel->pos.x = posX;
		pTextLabel->pos.y = posY;
		pTextLabel->pos.z = posZ;
		pTextLabel->drawDistance = drawDistance;
		pTextLabel->useLineOfSight = useLOS;
		pTextLabel->attachedToPlayerID = attachedToPlayerID;
		pTextLabel->attachedToVehicleID = attachedToVehicleID;

		pTextLabel->m_fTrueX = -1;
		if (attachedToVehicleID != INVALID_VEHICLE_ID || attachedToPlayerID != INVALID_PLAYER_ID)
		{
			pTextLabel->offsetCoords.x = posX;
			pTextLabel->offsetCoords.y = posY;
			pTextLabel->offsetCoords.z = posZ;
		}
	}
}

void CText3DLabelsPool::Delete(int labelID)
{
	if (GetAt(labelID))
	{
		delete list[labelID];
		list.erase(labelID);
	}
}

void CText3DLabelsPool::AttachToPlayer(int labelID, PLAYERID playerID, CVector pos)
{
	if (GetAt(labelID))
	{
		//tempPlayerID = playerID;
		list[labelID]->attachedToPlayerID = playerID;
		list[labelID]->pos = pos;
		list[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::AttachToVehicle(int labelID, VEHICLEID vehicleID, CVector pos)
{
	if (GetAt(labelID))
	{
		list[labelID]->attachedToVehicleID = vehicleID;
		list[labelID]->pos = pos;
		list[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::Update3DLabel(int labelID, uint32_t color, char* text)
{
	if (GetAt(labelID))
	{
		list[labelID]->color = color;
		//m_pTextLabels[labelID]->text = text;
		cp1251_to_utf8(list[labelID]->text, text);
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);
void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szWithColors = nullptr)
{
	if (pNetGame)
	{
		if (CPlayerPool::GetLocalPlayer())
		{
			CLocalPlayer* pPlayer = CPlayerPool::GetLocalPlayer();
			if (pPlayer->GetPlayerPed())
			{
				if (pPlayer->GetPlayerPed()->GetActionTrigger() == ACTION_DEATH || pPlayer->GetPlayerPed()->IsDead())
				{
					return;
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
	if (!showHud) return;
	
	int hitEntity = 0;
	for(auto & pair : list) {
		//D3DXVECTOR3 textPos;
		CVector textPos;
		if ( pair.second->attachedToPlayerID != INVALID_PLAYER_ID)
		{
			if (pair.second->attachedToPlayerID == CPlayerPool::GetLocalPlayerID())
				continue;

			if (CPlayerPool::GetSpawnedPlayer(pair.second->attachedToPlayerID))
			{
				if(!CPlayerPool::GetSpawnedPlayer(pair.second->attachedToPlayerID))
					continue;

				CPlayerPed* pPlayerPed = CPlayerPool::GetSpawnedPlayer(pair.second->attachedToPlayerID)->GetPlayerPed();
				if(!pPlayerPed)
					continue;

				if (!pPlayerPed->IsAdded())
					continue;

				CVector matPlayer;
				pPlayerPed->GetBonePosition(8, &matPlayer);

				textPos.x = matPlayer.x + pair.second->offsetCoords.x;
				textPos.y = matPlayer.y + pair.second->offsetCoords.y;
				textPos.z = matPlayer.z + 0.23 + pair.second->offsetCoords.z;
			}
		}
		if(pair.second->attachedToVehicleID != INVALID_VEHICLE_ID)
		{
			if (CVehiclePool::GetAt(pair.second->attachedToVehicleID))
			{
				CVehicle* pVehicle = CVehiclePool::GetAt(pair.second->attachedToVehicleID);
				if(!pVehicle)
					continue;

				if (!pVehicle->IsAdded())
					continue;

				RwMatrix matVehicle;
				pVehicle->GetMatrix(&matVehicle);

				textPos.x = matVehicle.pos.x + pair.second->offsetCoords.x;
				textPos.y = matVehicle.pos.y + pair.second->offsetCoords.y;
				textPos.z = matVehicle.pos.z + pair.second->offsetCoords.z;
			}
		}
		else if(pair.second->attachedToVehicleID == INVALID_VEHICLE_ID && pair.second->attachedToPlayerID == INVALID_PLAYER_ID)
		{
			textPos.x = pair.second->pos.x;
			textPos.y = pair.second->pos.y;
			textPos.z = pair.second->pos.z;
		}
		if (pair.second->useLineOfSight)
		{
			RwMatrix mat;
			CVector playerPosition;

			CAMERA_AIM *pCam = GameGetInternalAim();
			CPlayerPool::GetLocalPlayer()->GetPlayerPed()->GetMatrix(&mat);

			playerPosition.x = mat.pos.x;
			playerPosition.y = mat.pos.y;
			playerPosition.z = mat.pos.z;

			if (pair.second->useLineOfSight)
				hitEntity = ScriptCommand(&get_line_of_sight,
				playerPosition.x, playerPosition.y, playerPosition.z,
				pCam->pos1x, pCam->pos1y, pCam->pos1z,
				1, 0, 0, 0, 0);
		}
		pair.second->pos.x = textPos.x;
		pair.second->pos.y = textPos.y;
		pair.second->pos.z = textPos.z;
		if (!pair.second->useLineOfSight || hitEntity)
		{
			CPlayerPed* pPlayerPed = CPlayerPool::GetLocalPlayer()->GetPlayerPed();
			if(!pPlayerPed)
				continue;

			if(!pPlayerPed->IsAdded())
				continue;

			if (pPlayerPed->GetDistanceFromPoint(pair.second->pos.x, pair.second->pos.y, pair.second->pos.z) <= pair.second->drawDistance)
			{
				CVector Out;

				// CSprite::CalcScreenCoors
				(( void (*)(CVector*, CVector*, float*, float*, bool, bool))(g_libGTASA+0x54EEC0+1))(&textPos, &Out, 0, 0, 0, 0);
				if(Out.z < 1.0f) continue;
				ImVec2 pos = ImVec2(Out.x, Out.y);
				// removed piece
				Render3DLabel(pos, pair.second->text, pair.second->color );
			}
		}
	}
}
