//
// Created by x1y2z on 06.01.2023.
//

#include "Timer.h"
#include "../util/CJavaWrapper.h"
#include "../main.h"
#include "util/armhook.h"
#include "../keyboard.h"
#include "game_sa.h"

uint8_t* gTimerRunning = nullptr;

float CTimer::game_FPS = 0;

bool        CTimer::m_CodePause = false;
bool        CTimer::m_UserPause = false;
float       CTimer::ms_fTimeScale;
uint32_t    CTimer::m_FrameCounter = 0;
uint32_t    CTimer::m_snTimeInMilliseconds = 0;
bool        CTimer::bSlowMotionActive = false;
bool        CTimer::bSkipProcessThisFrame = false;
float       CTimer::ms_fTimeStep = 0;
float       CTimer::ms_fSlowMotionScale = 0;
uint32_t    CTimer::m_snPPPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snTimeInMillisecondsPauseMode;
uint32_t    CTimer::m_snTimeInMillisecondsNonClipped;
uint32_t    CTimer::ms_fTimeStepNonClipped;
uint32_t    CTimer::m_snPreviousTimeInMillisecondsNonClipped;
CTimer::TimerFunction_t CTimer::timerDef;
float       CTimer::ms_fOldTimeStep = 0;
uint32_t    CTimer::gSuspendDepth = 0;
uint8_t     CTimer::gTimerRunning = 0;
uint32_t    CTimer::m_snRenderStartTime = 0;
uint32_t    CTimer::m_snRenderPauseTime = 0;
uint32_t    CTimer::m_snTimerDivider = 0;

extern CKeyBoard* pKeyBoard;

void CTimer::InjectHooks()
{
    Write(g_libGTASA + 0x005D141C, &CTimer::m_CodePause);
    Write(g_libGTASA + 0x005D07E4, &CTimer::m_FrameCounter);
    Write(g_libGTASA + 0x005CF440, &CTimer::game_FPS);
    Write(g_libGTASA + 0x005D0514, &CTimer::bSlowMotionActive);
    Write(g_libGTASA + 0x008C9BA3, &CTimer::m_UserPause);
    Write(g_libGTASA + 0x005CE5E8, &CTimer::ms_fTimeScale);
    Write(g_libGTASA + 0x005CED58, &CTimer::m_snTimeInMilliseconds);
    Write(g_libGTASA + 0x005D1180, &CTimer::bSkipProcessThisFrame);
    Write(g_libGTASA + 0x005CF4E8, &CTimer::ms_fTimeStep);
    Write(g_libGTASA + 0x005CE950, &CTimer::ms_fOldTimeStep);

    Write(g_libGTASA + 0x005D1400, &CTimer::m_snPPPPreviousTimeInMilliseconds);
    Write(g_libGTASA + 0x005CFC20, &CTimer::m_snPPPreviousTimeInMilliseconds);
    Write(g_libGTASA + 0x005D1D1C, &CTimer::m_snPPreviousTimeInMilliseconds);
    Write(g_libGTASA + 0x005CF794, &CTimer::m_snPreviousTimeInMilliseconds);
    Write(g_libGTASA + 0x005CF820, &CTimer::m_snTimeInMillisecondsPauseMode);
    Write(g_libGTASA + 0x005D1DA4, &CTimer::m_snTimeInMillisecondsNonClipped);
    Write(g_libGTASA + 0x005CF3C4, &CTimer::m_snPreviousTimeInMillisecondsNonClipped);
    Write(g_libGTASA + 0x005CE86C, &CTimer::ms_fSlowMotionScale);

    Write(g_libGTASA + 0x005CE1F8, &CTimer::timerDef);
    Write(g_libGTASA + 0x005D195C, &CTimer::ms_fTimeStepNonClipped);

    //005D195C

    CTimer::gTimerRunning = *(uint8_t*)(g_libGTASA + 0x008C9B74);
    CTimer::gSuspendDepth = *(uint32_t*)(g_libGTASA + 0x008C9B70);
    CTimer::m_snRenderStartTime = *(uint32_t*)(g_libGTASA + 0x008C9B78);
    CTimer::m_snRenderPauseTime = *(uint32_t*)(g_libGTASA + 0x008C9B80);
    CTimer::m_snTimerDivider = *(uint32_t*)(g_libGTASA + 0x008C9B9C);

    Redirect(g_libGTASA, 0x003BF784, &CTimer::StartUserPause);
    Redirect(g_libGTASA, 0x003BF7A0, &CTimer::EndUserPause);

    /*Redirect(g_libGTASA, 0x003BF240, &CTimer::Initialise);//CTimer::Initialise(void)	003BF240
    Redirect(g_libGTASA, 0x003BF374, &CTimer::Shutdown);//CTimer::Shutdown(void)	003BF374
    Redirect(g_libGTASA, 0x003BF61C, &CTimer::Suspend);//CTimer::Suspend(void)	003BF61C
    Redirect(g_libGTASA, 0x003BF64C, &CTimer::Resume);//CTimer::Resume(void)	003BF64C
    Redirect(g_libGTASA, 0x003BF38C, &CTimer::UpdateVariables);//CTimer::UpdateVariables(float)	003BF38C
    Redirect(g_libGTASA, 0x003BF48C, &CTimer::Update);//CTimer::Update(void)	003BF48C
    Redirect(g_libGTASA, 0x003BF694, &CTimer::GetCyclesPerMillisecond);// CTimer::GetCyclesPerMillisecond(void)	003BF694
    Redirect(g_libGTASA, 0x003BF6AC, &CTimer::GetCyclesPerFrame);// CTimer::GetCyclesPerFrame(void)	003BF6AC
    Redirect(g_libGTASA, 0x003BF6DC, &CTimer::GetCurrentTimeInCycles);// CTimer::GetCurrentTimeInCycles(void)	003BF6DC
    Redirect(g_libGTASA, 0x003BF784, &CTimer::StartUserPause);
    Redirect(g_libGTASA, 0x003BF7A0, &CTimer::EndUserPause);
    Redirect(g_libGTASA, 0x003BF6F4, &CTimer::Stop);
    Redirect(g_libGTASA, 0x003BF758, &CTimer::GetIsSlowMotionActive);*/
}

//GetOSWPerformanceTime(void)	003BF200
uint64_t GetOSWPerformanceTime() {
    return CallFunction<uint64_t>(g_libGTASA+0x003BF200+1);
}

// 0x5617E0
void CTimer::Initialise()
{
    m_UserPause = false;
    m_CodePause = false;
    bSlowMotionActive = false;
    bSkipProcessThisFrame = false;

    m_snTimeInMilliseconds = 0;
    m_snTimeInMillisecondsPauseMode = 1;
    m_snTimeInMillisecondsNonClipped = 1;
    m_snPreviousTimeInMilliseconds = 0;
    m_snPPreviousTimeInMilliseconds = 0;
    m_snPPPreviousTimeInMilliseconds = 0;
    m_snPPPPreviousTimeInMilliseconds = 0;
    m_snPreviousTimeInMillisecondsNonClipped = 0;

    gSuspendDepth = 0;

    m_FrameCounter = 0;
    gTimerRunning = false;
    game_FPS = 0.0f;

    ms_fTimeScale = 1.0f;
    ms_fSlowMotionScale = -1.0f; // unused
    ms_fTimeStep = 1.0f;
    ms_fOldTimeStep = 1.0f;

    TimerFunction_t timerFunc;
    timerFunc = GetOSWPerformanceTime;
    m_snTimerDivider = 1000;

    timerDef = timerFunc;
    m_snRenderStartTime = GetOSWPerformanceTime();
}

// 0x5618C0
void CTimer::Shutdown() {
    gTimerRunning = false;
}

// 0x5619D0
void CTimer::Suspend()
{
    if (gTimerRunning)
    {
        if (++gSuspendDepth <= 1)
            m_snRenderPauseTime = timerDef();
    }
}

// 0x561A00
void CTimer::Resume()
{
    if (gTimerRunning)
    {
        if (!--gSuspendDepth) {
            m_snRenderStartTime += timerDef() - m_snRenderPauseTime;
        }
    }
}

// 0x561AA0
void CTimer::Stop()
{
    CTimer::m_snPPPPreviousTimeInMilliseconds = CTimer::m_snTimeInMilliseconds;
    CTimer::m_snPPPreviousTimeInMilliseconds = CTimer::m_snTimeInMilliseconds;
    CTimer::m_snPPreviousTimeInMilliseconds = CTimer::m_snTimeInMilliseconds;
    CTimer::m_snPreviousTimeInMilliseconds = CTimer::m_snTimeInMilliseconds;
    gTimerRunning = 0;

    CTimer::m_snPreviousTimeInMillisecondsNonClipped = CTimer::m_snTimeInMillisecondsNonClipped;
}

// 0x561AF0
void CTimer::StartUserPause()
{
    if (g_pJavaWrapper) {
        if (pKeyBoard) {
            if (pKeyBoard->IsNewKeyboard())
                pKeyBoard->Close();
        }

        g_pJavaWrapper->ToggleRender(false);
    }
    m_UserPause = true;
}

// 0x561B00
void CTimer::EndUserPause()
{
    // process resume event
    if (g_pJavaWrapper) {
        g_pJavaWrapper->ToggleRender(true);
    }

    m_UserPause = false;
}

// 0x561A40
uint32_t CTimer::GetCyclesPerMillisecond()
{
    return m_snTimerDivider;
}

// cycles per ms * 20
// 0x561A50
uint32_t CTimer::GetCyclesPerFrame()
{
    return (uint32)((float)m_snTimerDivider * 20.0f);
}

uint64_t CTimer::GetCurrentTimeInCycles()
{
    return GetOSWPerformanceTime() - m_snRenderStartTime;
}

// 0x561AD0
bool CTimer::GetIsSlowMotionActive()
{
    return CTimer::ms_fTimeScale < 1.0;
}
#include <cmath>

// 0x5618D0
void CTimer::UpdateVariables(float timeElapsed)
{
    const float frameDelta = (float)timeElapsed / (float)m_snTimerDivider;
    m_snTimeInMillisecondsNonClipped += (uint32)frameDelta;
    ms_fTimeStepNonClipped = frameDelta / TIMESTEP_LEN_IN_MS;

    m_snTimeInMilliseconds += (uint32)std::min<float>(frameDelta, 300.0f);

    if (!m_UserPause && !m_CodePause && !*(uint8_t*)(g_libGTASA+0x005CE7B0)) {
        // Make it be something at least, to avoid division by 0
        ms_fTimeStepNonClipped = std::max((float)ms_fTimeStepNonClipped, 0.01f);
    }

    ms_fOldTimeStep = ms_fTimeStep;
    SetTimeStep((float)std::clamp((float)ms_fTimeStepNonClipped, 0.00001f, 3.0f));
}

// 0x561B10
void CTimer::Update()
{
    if (!timerDef)
        return;

    gTimerRunning = true;
    game_FPS = float(1000.0f / float(m_snTimeInMillisecondsNonClipped - m_snPreviousTimeInMillisecondsNonClipped));

    // Update history
    m_snPPPPreviousTimeInMilliseconds = m_snPPPreviousTimeInMilliseconds;
    m_snPPPreviousTimeInMilliseconds = m_snPPreviousTimeInMilliseconds;
    m_snPPreviousTimeInMilliseconds = m_snPreviousTimeInMilliseconds;
    m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;

    m_snPreviousTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped;

    const uint64 nRenderTimeBefore = m_snRenderStartTime;
    m_snRenderStartTime = timerDef();
    auto fTimeDelta = float(m_snRenderStartTime - nRenderTimeBefore);
    if (!GetIsPaused())
        fTimeDelta *= ms_fTimeScale;

    m_snTimeInMillisecondsPauseMode += (uint32)(fTimeDelta / float(m_snTimerDivider));
    if (GetIsPaused())
        fTimeDelta = 0.0f;

    UpdateVariables(fTimeDelta);
    m_FrameCounter++;
}