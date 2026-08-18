#include "../main.h"
#include "../game/common.h"
#include "../game/util.h"

#include "MicroIcon.h"

#include "PluginConfig.h"

bool MicroIcon::Init() noexcept
{
    LogVoice("[sv:dbg:microicon:init] : start init");
    if(MicroIcon::initStatus)
        return false;

    try
    {
        MicroIcon::tPassiveIcon = CUtil::LoadTextureFromDB("mobile", "voice_btn");
        MicroIcon::tActiveIcon = CUtil::LoadTextureFromDB("mobile", "voice_active_btn");
        MicroIcon::tMutedIcon = CUtil::LoadTextureFromDB("mobile", "voice_mute_btn");
    }
    catch(const std::exception& exception)
    {
        LogVoice("[sv:err:microicon:init] : failed to create icons");
        MicroIcon::tPassiveIcon = nullptr;
        MicroIcon::tActiveIcon = nullptr;
        MicroIcon::tMutedIcon = nullptr;
        return false;
    }

    if(!PluginConfig::IsMicroLoaded())
    {
        PluginConfig::SetMicroLoaded(true);
    }

    MicroIcon::initStatus = true;

    return true;
}

void MicroIcon::Free() noexcept
{
    if(!MicroIcon::initStatus)
        return;

    MicroIcon::tPassiveIcon = nullptr;
    MicroIcon::tActiveIcon = nullptr;
    MicroIcon::tMutedIcon = nullptr;

    MicroIcon::initStatus = false;
}

void MicroIcon::Show() noexcept
{
    MicroIcon::hasShowed = true;
    MicroIcon::showStatus = true;
}

bool MicroIcon::IsShowed() noexcept
{
    return MicroIcon::showStatus;
}

void MicroIcon::Hide() noexcept
{
    MicroIcon::showStatus = false;
}

bool MicroIcon::initStatus { false };
bool MicroIcon::showStatus { false };
bool MicroIcon::hasShowed { false };

RwTexture* MicroIcon::tPassiveIcon { nullptr };
RwTexture* MicroIcon::tActiveIcon { nullptr };
RwTexture* MicroIcon::tMutedIcon { nullptr };
