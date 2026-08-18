//
// Created by plaka on 24.03.2023.
//

#include <cstdint>
#include <algorithm>
#include "Timer.h"
#include "oswrapper/oswrapper.h"
#include "util/patch.h"
#include "java_systems/CSpeedometr.h"
#include "util/CJavaWrapper.h"
#include "keyboard.h"

float CTimer::game_FPS = 0;

bool CTimer::m_CodePause = false;
bool CTimer::m_UserPause = false;
float CTimer::ms_fTimeScale = 0;

uint32_t    CTimer::m_FrameCounter = 0;
uint32_t    CTimer::m_snTimeInMilliseconds = 0;
bool        CTimer::bSkipProcessThisFrame = false;
float       CTimer::ms_fTimeStep = 0;
uint32_t    CTimer::m_snPPPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snTimeInMillisecondsNonClipped;
uint32_t    CTimer::m_snPreviousTimeInMillisecondsNonClipped;

void CTimer::InjectHooks()
{
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006794BC : 0x8509A0), &CTimer::m_CodePause);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067892C : 0x84F280), &CTimer::m_FrameCounter);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677674 : 0x84CD28), &CTimer::game_FPS);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006776A8 : 0x84CD90), &CTimer::m_UserPause);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067689C : 0x84B1A8), &CTimer::ms_fTimeScale);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676FF0 : 0x84C030), &CTimer::m_snTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679248 : 0x8504B8), &CTimer::bSkipProcessThisFrame);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067770C : 0x84CE50), &CTimer::ms_fTimeStep);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067949C : 0x850960), &CTimer::m_snPPPPreviousTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677DF0 : 0x84DC10), &CTimer::m_snPPPreviousTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679D3C : 0x851A90), &CTimer::m_snPPreviousTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006779A8 : 0x84D388), &CTimer::m_snPreviousTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679DBC : 0x851B90), &CTimer::m_snTimeInMillisecondsNonClipped);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006775F8 : 0x84CC30), &CTimer::m_snPreviousTimeInMillisecondsNonClipped);

    CHook::HookRedirect("_ZN6CTimer14StartUserPauseEv", CTimer::StartUserPause);

    CHook::HookRedirect("_ZN6CTimer12EndUserPauseEv", CTimer::EndUserPause);
}


uint64_t GetMillisecondTime() {

}

// 0x5617E0
void CTimer::Initialise()
{

}

// 0x5618C0
void CTimer::Shutdown() {

}

// 0x5619D0
void CTimer::Suspend()
{

}

// 0x561A00
void CTimer::Resume()
{

}

// 0x561AA0
void CTimer::Stop()
{

}

// 0x561AF0
void CTimer::StartUserPause()
{
    if (g_pJavaWrapper)
    {
        CKeyBoard::Close();
        g_pJavaWrapper->SetPauseState(true);
        CSpeedometr::tempToggle(false);
    }
    m_UserPause = true;
}

// 0x561B00
void CTimer::EndUserPause()
{
    // process resume event
    if (g_pJavaWrapper)
    {
        g_pJavaWrapper->SetPauseState(false);
        CSpeedometr::tempToggle(true);
    }
    m_UserPause = false;
}

// 0x561A40
uint32_t CTimer::GetCyclesPerMillisecond()
{
    return CHook::CallFunction<uint32_t>(g_libGTASA + 0x0042100C + 1);
}

// cycles per ms * 20
// 0x561A50
uint32_t CTimer::GetCyclesPerFrame()
{

}

uint64_t CTimer::GetCurrentTimeInCycles()
{
    return CHook::CallFunction<uint64_t>(g_libGTASA + 0x00421040 + 1);
}

// 0x561AD0
bool CTimer::GetIsSlowMotionActive()
{

}

// 0x5618D0
void CTimer::UpdateVariables(float timeElapsed)
{

}

// 0x561B10
void CTimer::Update()
{

}
