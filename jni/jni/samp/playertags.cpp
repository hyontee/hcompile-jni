#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "playertags.h"
#include "CSettings.h"
#include "gui/gui.h"
#include <string>
#include <cstring> // strrchr
#include <algorithm>

// === БЕЙДЖИ (LVL/ADM) ===
#include "java_systems/badges.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;

// Универсальный рисователь «бейджа-таблички»
static inline void DrawBadge(const ImVec2& posLeftTop, const char* text,
                             ImU32 bg, ImU32 fg, float rounding,
                             float padX, float padY)
{
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImVec2 ts = ImGui::CalcTextSize(text);
    ImVec2 min = ImVec2(posLeftTop.x, posLeftTop.y);
    ImVec2 max = ImVec2(posLeftTop.x + ts.x + padX * 2.0f,
                        posLeftTop.y + ts.y + padY * 2.0f);

    dl->AddRectFilled(min, max, bg, rounding);
    dl->AddRect(min, max, IM_COL32(255,255,255,30), rounding);
    ImVec2 textPos = ImVec2(min.x + padX, min.y + padY);
    dl->AddText(textPos, fg, text);
}

CPlayerTags::CPlayerTags()
{
    m_pAfk_icon = CUtil::LoadTextureFromDB("samp", "afk_icon");

    HealthBarBDRColor = ImColor( 0x00, 0x00, 0x00, 0xFF );

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        m_bChatBubbleStatus[i] = 0;
        m_pSzText[i] = nullptr;
        m_pSzTextWithoutColors[i] = nullptr;
        m_iVoiceTime[i] = 0;
        m_iLastVoiceTimeUpdated[i] = 0;
    }
}

CPlayerTags::~CPlayerTags() {}

void CPlayerTags::Render()
{
    CVector VecPos;
    int dwHitEntity;
    char szNickBuf[50];

    if(pNetGame && pNetGame->m_bShowPlayerTags)
    {
        CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

        for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {

            // Сброс кэша — ТОЛЬКО когда слот реально пуст/неактивен
            if (pPlayerPool->m_pPlayers[playerId] == nullptr) {
                Badges_ResetFor(playerId);
                continue;
            }

            CRemotePlayer *pPlayer = pPlayerPool->GetAt(playerId);
            if (!pPlayer || !pPlayer->IsActive() || !pPlayer->m_bShowNameTag) {
                Badges_ResetFor(playerId);
                continue;
            }

            CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();

            // Chat bubble — без вмешательства в бейдж-кэш
            if (m_bChatBubbleStatus[playerId]) {
                if (!pPlayerPed) {
                    ResetChatBubble(playerId);
                    // НЕ сбрасываем бейджи здесь
                    continue;
                }
                if (pPlayerPed->GetDistanceFromCamera() <= m_fDistance[playerId]) {
                    if (!pPlayerPed->IsAdded()) continue;
                    VecPos.x = 0.0f; VecPos.y = 0.0f; VecPos.z = 0.0f;
                    pPlayerPed->GetBonePosition(8, &VecPos);
                    DrawChatBubble(playerId, &VecPos, pPlayerPed->GetDistanceFromCamera());
                }
                if (GetTickCount() - m_dwStartTime[playerId] >= m_dwTime[playerId]) {
                    ResetChatBubble(playerId);
                }
            }

            // Дальше — НИГДЕ не делаем Badges_ResetFor(...), только continue
            if (!pPlayerPed) continue;
            if (pPlayerPed->GetDistanceFromCamera() > pNetGame->m_fNameTagDrawDistance) continue;
            if (!pPlayerPed->IsAdded()) continue;

            VecPos.x = 0.0f; VecPos.y = 0.0f; VecPos.z = 0.0f;
            pPlayerPed->GetBonePosition(8, &VecPos);

            CAMERA_AIM *pCam = GameGetInternalAim();
            dwHitEntity = 0;

            if (pNetGame->m_bNameTagLOS) {
                dwHitEntity = ScriptCommand(&get_line_of_sight,
                                            VecPos.x, VecPos.y, VecPos.z,
                                            pCam->pos1x, pCam->pos1y, pCam->pos1z,
                                            1, 0, 0, 1, 0);
            }

            if (!pNetGame->m_bNameTagLOS || dwHitEntity) {
                sprintf(szNickBuf, "%s (%d)",
                        pPlayerPool->GetPlayerName(playerId),
                        playerId);

                Draw(&VecPos, szNickBuf,
                     pPlayer->GetPlayerColor(),
                     pPlayerPed->GetDistanceFromCamera(),
                     pPlayer->m_fCurrentHealth,
                     pPlayer->m_fCurrentArmor,
                     pPlayer->IsAFK(), 0, pPlayer->m_bKeyboardOpened);
            }
        }
    }
}

void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szStrWithoutColors = nullptr);
void FilterColors(char* szStr);

void CPlayerTags::AddChatBubble(PLAYERID playerId, char* szText, uint32_t dwColor, float fDistance, uint32_t dwTime)
{
    if (m_bChatBubbleStatus[playerId])
    {
        ResetChatBubble(playerId);
        m_dwColors[playerId] = dwColor;
        m_fDistance[playerId] = fDistance;
        m_dwTime[playerId] = dwTime;
        m_dwStartTime[playerId] = GetTickCount();
        m_bChatBubbleStatus[playerId] = 1;
        m_fTrueX[playerId] = -1.0f;
        cp1251_to_utf8(m_pSzText[playerId], szText);
        cp1251_to_utf8(m_pSzTextWithoutColors[playerId], szText);
        FilterColors(m_pSzTextWithoutColors[playerId]);
        const char* pText = m_pSzTextWithoutColors[playerId];
        m_iOffset[playerId] = 0;
        while (*pText)
        {
            if (*pText == '\n') m_iOffset[playerId]++;
            pText++;
        }
        return;
    }
    m_dwColors[playerId] = dwColor;
    m_fDistance[playerId] = fDistance;
    m_dwTime[playerId] = dwTime;
    m_dwStartTime[playerId] = GetTickCount();
    m_bChatBubbleStatus[playerId] = 1;
    m_fTrueX[playerId] = -1.0f;
    m_pSzText[playerId] = new char[1024];
    m_pSzTextWithoutColors[playerId] = new char[1024];
    cp1251_to_utf8(m_pSzText[playerId], szText);
    cp1251_to_utf8(m_pSzTextWithoutColors[playerId], szText);
    FilterColors(m_pSzTextWithoutColors[playerId]);
    const char* pText = m_pSzTextWithoutColors[playerId];
    m_iOffset[playerId] = 0;
    while (*pText)
    {
        if (*pText == '\n') m_iOffset[playerId]++;
        pText++;
    }
}

void CPlayerTags::ResetChatBubble(PLAYERID playerId)
{
    if (m_bChatBubbleStatus[playerId])
    {
        m_dwTime[playerId] = 0;
    }
    m_bChatBubbleStatus[playerId] = 0;
}

void CPlayerTags::DrawChatBubble(PLAYERID playerId, CVector* vec, float fDistance)
{
    CVector TagPos;

    TagPos.x = vec->x;
    TagPos.y = vec->y;
    TagPos.z = vec->z + 0.45f + (fDistance * 0.0675f) + ((float)m_iOffset[playerId] * pGUI->ScaleY(0.35f));

    CVector Out;
    ((void (*)(CVector*, CVector*, float*, float*, bool, bool))(g_libGTASA + 0x005C57E8 + 1))(&TagPos, &Out, nullptr, nullptr, false, false);

    if (Out.z < 1.0f) return;

    ImVec2 pos = ImVec2(Out.x, Out.y);

    if (m_fTrueX[playerId] < 0)
    {
        char* curBegin = m_pSzTextWithoutColors[playerId];
        char* curPos = m_pSzTextWithoutColors[playerId];
        while (*curPos != '\0')
        {
            if (*curPos == '\n')
            {
                float width = ImGui::CalcTextSize(curBegin, (char*)(curPos - 1)).x;
                if (width > m_fTrueX[playerId]) m_fTrueX[playerId] = width;
                curBegin = curPos + 1;
            }
            curPos++;
        }
        if (m_fTrueX[playerId] < 0)
            m_fTrueX[playerId] = ImGui::CalcTextSize(m_pSzTextWithoutColors[playerId]).x;
    }

    pos.x -= (m_fTrueX[playerId] / 2);
    TextWithColors(pos, __builtin_bswap32(m_dwColors[playerId]), m_pSzText[playerId]);
}

void CPlayerTags::Draw(CVector* vec, char* szName, uint32_t dwColor,
                       float fDist, float fHealth, float fArmour, bool bAfk, bool bVoice, bool bKeyboard)
{
    if (!pGame->IsToggledHUDElement(HUD_ELEMENT_TAGS)) return;

    RwV3d TagPos;
    TagPos.x = vec->x;
    TagPos.y = vec->y;
    // подняли весь стек (ник/бар/бейджи) чуть выше
    TagPos.z = vec->z + 0.40f + (fDist * 0.0475f);

    CVector Out;
    (( void (*)(RwV3d*, RwV3d*, float*, float*, bool, bool))(g_libGTASA+0x005C57E8+1))(&TagPos, &Out, nullptr, nullptr, false, false);
    if(Out.z < 1.0f) return;

    // ========== НИК и извлечение ID ==========
    char tempBuff[300];
    cp1251_to_utf8(tempBuff, szName);

    const char* full = tempBuff;
    const char* lpar = strrchr(full, '(');
    const char* rpar = lpar ? strrchr(full, ')') : nullptr;

    bool hasIdBrackets = (lpar && rpar && rpar > lpar && rpar[1] == '\0');

    std::string nickStr;
    std::string idDigits;
    if (hasIdBrackets) {
        nickStr.assign(full, lpar - full);
        if (!nickStr.empty() && nickStr.back() == ' ')
            nickStr.pop_back();
        idDigits.assign(lpar + 1, rpar - (lpar + 1));
    } else {
        nickStr = full;
    }

    ImVec2 nickSize = ImGui::CalcTextSize(nickStr.c_str());
    ImVec2 nickPos  = ImVec2(Out.x - nickSize.x * 0.5f, Out.y);

    // базовый цвет ника (с сервера)
    ImU32 textColor = __builtin_bswap32(dwColor | 0x000000FF);

    // заранее посчитаем pid (для админ-проверки и бейджей)
    int pid = (!idDigits.empty() ? std::atoi(idDigits.c_str()) : -1);

    // если админ — подсвечиваем ник (золото). Хочешь голубой? смени строку ниже на IM_COL32(0,191,255,255)
    if (pid >= 0 && pid < MAX_PLAYERS && Badges_IsAdmin((PLAYERID)pid)) {
        textColor = IM_COL32(255, 99, 71, 255);  // золотой #FFD700
        // textColor = IM_COL32(0, 191, 255, 255); // <- голубой #00BFFF, если так нравится больше
    }

    pGUI->RenderText(nickPos, textColor, true, nickStr.c_str());

    // ========== ADM рядом с ником ==========
    bool isAdm = (pid >= 0 && pid < MAX_PLAYERS) ? Badges_IsAdmin((PLAYERID)pid) : false;
    if (isAdm) {
        const char* admText = "ADM";
        ImVec2 admPos  = ImVec2(nickPos.x + nickSize.x + pGUI->ScaleX(6.0f), nickPos.y);
        DrawBadge(admPos, admText,
                  IM_COL32(255, 80, 80, 230),
                  IM_COL32(0,   0,  0, 255),
                  pGUI->ScaleY(3.0f),
                  pGUI->ScaleX(5.0f), pGUI->ScaleY(2.0f));
    }

    // ===== ПОЛОСКИ HP / ARMOUR =====
    if(fHealth < 0.0f) return;

    // цвета HP
    HealthBarColor   = ImColor(0xF0, 0x5C, 0x5C, 0xFF); // светло-красный
    HealthBarBGColor = ImColor(0x55, 0x15, 0x15, 0xFF); // фон

    // размеры: ЕЩЁ ДЛИННЕЕ и тонкий
    float fWidth   = pGUI->ScaleX(93.0f);  // длиннее (было 72.0f)
    float fHeight  = pGUI->ScaleY(10.0f);  // тонкий
    float fOutline = (float)CSettings::m_Settings.iFontOutline;

    // отступы (компактно, чтобы всё было выше)
    float gapBelowNick    = pGUI->ScaleY(2.0f);  // ник → HP
    float gapBetweenBars  = pGUI->ScaleY(2.0f);  // HP → броня
    float gapBarsToBadges = pGUI->ScaleY(4.0f);  // низ полос → бейджи

    // позиция HP под ником
    HealthBarBDR1.x = Out.x - ((fWidth/2) + fOutline);
    HealthBarBDR1.y = Out.y + (pGUI->GetFontSize()*1.0f) + gapBelowNick;
    HealthBarBDR2.x = Out.x + ((fWidth/2) + fOutline);
    HealthBarBDR2.y = HealthBarBDR1.y + fHeight + fOutline;

    HealthBarBG1.x = HealthBarBDR1.x + fOutline;
    HealthBarBG1.y = HealthBarBDR1.y + fOutline;
    HealthBarBG2.x = HealthBarBDR2.x - fOutline;
    HealthBarBG2.y = HealthBarBDR2.y - fOutline;

    HealthBar1.x = HealthBarBG1.x;
    HealthBar1.y = HealthBarBG1.y;
    HealthBar2.y = HealthBarBG2.y;

    if (fHealth > 100.0f) fHealth = 100.0f;
    float fHealthPixels = fHealth * (fWidth/100.0f) - (fWidth/2);
    HealthBar2.x = Out.x + fHealthPixels;

    // рисуем HP
    ImGui::GetForegroundDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
    ImGui::GetForegroundDrawList()->AddRectFilled(HealthBar1,   HealthBar2,   HealthBarColor);

    // броня — ниже HP, если есть
    float lastBarBottomY = HealthBarBDR2.y;
    if(fArmour > 0.0f)
    {
        ImColor ArmourBarColor   = ImColor(200, 200, 200, 255);
        ImColor ArmourBarBGColor = ImColor(40, 40, 40, 255);

        ImVec2 aBDR1, aBDR2, aBG1, aBG2, aF1, aF2;
        aBDR1.x = HealthBarBDR1.x;
        aBDR1.y = lastBarBottomY + gapBetweenBars;
        aBDR2.x = HealthBarBDR2.x;
        aBDR2.y = aBDR1.y + fHeight + fOutline;

        aBG1.x = aBDR1.x + fOutline;
        aBG1.y = aBDR1.y + fOutline;
        aBG2.x = aBDR2.x - fOutline;
        aBG2.y = aBDR2.y - fOutline;

        aF1.x = aBG1.x; aF1.y = aBG1.y; aF2.y = aBG2.y;

        if(fArmour > 100.0f) fArmour = 100.0f;
        float fArmourPixels = fArmour * (fWidth/100.0f) - (fWidth/2);
        aF2.x = Out.x + fArmourPixels;

        ImGui::GetForegroundDrawList()->AddRectFilled(aBG1, aBG2, ArmourBarBGColor);
        ImGui::GetForegroundDrawList()->AddRectFilled(aF1,  aF2,  ArmourBarColor);

        lastBarBottomY = aBDR2.y;
    }

    // ===== БЕЙДЖИ ID / LVL — ПОД полосами =====
    if (!idDigits.empty())
    {
        int level = 0;
        if (pid >= 0 && pid < MAX_PLAYERS) {
            level = Badges_GetLevel((PLAYERID)pid);
            if (level == 0 && !g_BadgePending[pid]) Badges_Request((PLAYERID)pid);
            Badges_MaybeRefresh((PLAYERID)pid, 5000);
        }

        char idText[16];  snprintf(idText,  sizeof(idText),  "%s",    idDigits.c_str());
        char lvlText[16]; snprintf(lvlText, sizeof(lvlText), "%d LVL", level);

        float padX   = pGUI->ScaleX(6.0f);
        float padY   = pGUI->ScaleY(3.0f);
        float round  = pGUI->ScaleY(3.0f);
        float gapBadges = pGUI->ScaleX(4.0f);

        ImVec2 idSize   = ImGui::CalcTextSize(idText);
        ImVec2 lvlSize  = ImGui::CalcTextSize(lvlText);

        float idBadgeW  = idSize.x  + padX * 2.0f;
        float lvlBadgeW = lvlSize.x + padX * 2.0f;

        float badgesTotalW = idBadgeW + gapBadges + lvlBadgeW;
        float badgesStartX = Out.x - badgesTotalW * 0.5f;

        // ряд бейджей — сразу под последней полосой
        float badgesTopYBase = lastBarBottomY + gapBarsToBadges;

        ImVec2 idPosLT  = ImVec2(badgesStartX,                 badgesTopYBase);
        ImVec2 lvlPosLT = ImVec2(badgesStartX + idBadgeW + gapBadges, badgesTopYBase);

        DrawBadge(idPosLT,  idText,
                  IM_COL32(  0,   0,   0, 220),
                  IM_COL32(255, 255, 255, 255),
                  round, padX, padY);

        DrawBadge(lvlPosLT, lvlText,
                  IM_COL32(255, 204,   0, 230),
                  IM_COL32(  0,   0,   0, 255),
                  round, padX, padY);
    }

    // AFK Icon у левого края HP
    if(bAfk)
    {
        ImVec2 a = ImVec2(HealthBarBDR1.x - (pGUI->GetFontSize()*1.4f), HealthBarBDR1.y);
        ImVec2 b = ImVec2(a.x + (pGUI->GetFontSize()*1.3f), a.y + (pGUI->GetFontSize()*1.3f));
        ImGui::GetForegroundDrawList()->AddImage((ImTextureID)m_pAfk_icon->raster, a, b);
    }
}
