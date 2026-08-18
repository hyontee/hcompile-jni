#include "../main.h"
#include "../gui/gui.h"
#include "../game/common.h"
#include "../game/util.h"
#include "../game/game.h"
#include "../game/playerped.h"
#include "../net/netgame.h"

#include "SpeakerList.h"

#include "PluginConfig.h"
#include "game/CGtaWidgets.h"

extern CGUI *pGUI;
extern CNetGame *pNetGame;

bool SpeakerList::Init() noexcept
{
    LogVoice("[sv:dbg:speakerlist:init] : start init");
    if(SpeakerList::initStatus)
        return false;

    try
    {
        SpeakerList::tSpeakerIcon = CUtil::LoadTextureFromDB("samp", "micro_icon");

    }
    catch (const std::exception& exception)
    {
        LogVoice("[sv:err:speakerlist:init] : failed to create speaker icon");
        SpeakerList::tSpeakerIcon = nullptr;
        return false;
    }

    if(!PluginConfig::IsSpeakerLoaded())
    {
        PluginConfig::SetSpeakerLoaded(true);
        LogVoice("[sv:dbg:speakerlist:init] : SetSpeakerLoaded");
    }

    SpeakerList::initStatus = true;
    LogVoice("[sv:dbg:speakerlist:init] : Init success");
    return true;
}

void SpeakerList::Free() noexcept
{
    if(!SpeakerList::initStatus)
        return;

    SpeakerList::tSpeakerIcon = nullptr;

    SpeakerList::initStatus = false;
}

void SpeakerList::Show() noexcept
{
    SpeakerList::showStatus = true;
}

bool SpeakerList::IsShowed() noexcept
{
    return SpeakerList::showStatus;
}

void SpeakerList::Hide() noexcept
{
    SpeakerList::showStatus = false;
}

void SpeakerList::Render()
{
    //  return;
    //Log("SpeakerList::initStatus: %d, SpeakerList::IsShowed(): %d", SpeakerList::initStatus ,SpeakerList::IsShowed());
    if(!SpeakerList::initStatus)
        return;

    if(!pNetGame) return;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    if(!pPlayerPool) return;

    int curTextLine;
    char szText[64], szText2[64];
    ImVec2 textPos = ImVec2(pGUI->ScaleX(24), pGUI->ScaleY(480));

    for(PLAYERID playerId { 0 }; playerId < MAX_PLAYERS; ++playerId)
    {
        CRemotePlayer* pPlayer = pPlayerPool->GetAt(playerId);

        if(pPlayer && pPlayer->IsActive())
        {
            CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
            if(pPlayerPed)
            {
                if(const auto playerName = pPlayerPool->GetPlayerName(playerId); playerName)
                {
                    if(!SpeakerList::playerStreams[playerId].empty())
                    {
                        for(const auto& playerStream : SpeakerList::playerStreams[playerId])
                        {
                            if(playerStream.second.GetType() == StreamType::LocalStreamAtPlayer)
                            {
                                CVector VecPos;

                                if(!pPlayerPed->IsAdded()) continue;
                                VecPos.x = 0.0f;
                                VecPos.y = 0.0f;
                                VecPos.z = 0.0f;
                                pPlayerPed->GetBonePosition(8, &VecPos);

                                if(pPlayerPed->GetDistanceFromLocalPlayerPed() < 60.0f)
                                    SpeakerList::Draw(&VecPos, pPlayerPed->GetDistanceFromCamera());
                            }
                        }

                        if(curTextLine < 4 && playerName && strlen(playerName) && !SpeakerList::IsShowed())
                        {
                            ImVec2 a = ImVec2(textPos.x, textPos.y);
                            ImVec2 b = ImVec2(textPos.x + pGUI->GetFontSize(), textPos.y + pGUI->GetFontSize());
                            ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)SpeakerList::tSpeakerIcon->raster, a, b);

                            textPos.x = pGUI->GetFontSize() * 2.4f;
                            sprintf(szText, "%s (%d) ", playerName, playerId);
                            pGUI->RenderText(textPos, 0xFFFFFFFF, true, szText);

                            /*for(const auto& streamInfo : SpeakerList::playerStreams[playerId])
                            {
                                if(streamInfo.second.GetColor() == NULL)
                                    continue;

                                textPos.x = ImGui::CalcTextSize(szText).x;
                                sprintf(szText2, "[%s]", streamInfo.second.GetName().c_str());
                                pGUI->RenderText(textPos, streamInfo.second.GetColor(), true, szText2);
                            }*/

                            textPos.x -= ImGui::CalcTextSize(szText).x;
                            textPos.y += pGUI->GetFontSize();

                            curTextLine++;
                        }
                    }
                }
            }
        }
    }
}

void SpeakerList::Draw(CVector* vec, float fDist) {
    CVector TagPos;

    TagPos.x = vec->x;
    TagPos.y = vec->y;
    TagPos.z = vec->z;
    TagPos.z += 0.25f + (fDist * 0.0475f);

    CVector Out;
    // CSprite::CalcScreenCoors
    ((void (*)(CVector *, CVector *, float *, float *, bool, bool)) (g_libGTASA + (VER_x32 ? 0x005C57E8 + 1 : 0x6E9DF8)))(
            &TagPos, &Out, 0, 0, 0, 0);

    if (Out.z < 1.0f)
        return;

    ImVec2 pos = ImVec2(Out.x, Out.y);
    pos.x -= PluginConfig::kDefValSpeakerIconSize / 2;
    pos.y -= pGUI->GetFontSize();

    ImVec2 a = ImVec2(pos.x, pos.y);
    ImVec2 b = ImVec2(pos.x + PluginConfig::kDefValSpeakerIconSize,
                      pos.y + PluginConfig::kDefValSpeakerIconSize);
    ImGui::GetForegroundDrawList()->AddImage((ImTextureID) SpeakerList::tSpeakerIcon->raster, a, b);


}

void SpeakerList::OnSpeakerPlay(const Stream& stream, const uint16_t speaker) noexcept
{
    uint16_t wSpeaker = speaker;
    if(speaker < 0) wSpeaker = 0;
    else if(speaker > MAX_PLAYERS - 1) wSpeaker = MAX_PLAYERS - 1;
    if(speaker != wSpeaker) return;

    SpeakerList::playerStreams[speaker][(Stream*)(&stream)] = stream.GetInfo();
}

void SpeakerList::OnSpeakerStop(const Stream& stream, const uint16_t speaker) noexcept
{
    uint16_t wSpeaker = speaker;
    if(speaker < 0) wSpeaker = 0;
    else if(speaker > MAX_PLAYERS - 1) wSpeaker = MAX_PLAYERS - 1;
    if(speaker != wSpeaker) return;

    SpeakerList::playerStreams[speaker].erase((Stream*)(&stream));
}

std::array<std::unordered_map<Stream*, StreamInfo>, MAX_PLAYERS> SpeakerList::playerStreams;

bool SpeakerList::initStatus { false };
bool SpeakerList::showStatus { false };

RwTexture* SpeakerList::tSpeakerIcon { nullptr };