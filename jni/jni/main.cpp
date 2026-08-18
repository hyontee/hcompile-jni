
#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>

#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "chatwindow.h"
#include "playertags.h"
#include "keyboard.h"
#include "CSettings.h"
#include "java_systems/CHUD.h"
#include "CLoader.h"
#include "str_obfuscator_no_template.hpp"

#include "crashlytics.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

uintptr_t g_libGTASA = 0;
uintptr_t g_libSAMP = 0;
char *g_pszStorage = nullptr;

#include "CServerManager.h"
#include "CLocalisation.h"
#include "java_systems/CHUD.h"
#include "java_systems/CInventory.h"
#include "gui/gui.h"

const cryptor::string_encryptor encLib = cryptor::create("libsamp.so", 11);

void CrashLog(const char *fmt, ...);

bool g_bIsTestMode = false;
int h_int;

CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CPlayerTags *pPlayerTags = nullptr;
CGUI *pGUI = nullptr;

void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();

#ifdef GAME_EDITION_CR
extern uint16_t g_usLastProcessedModelIndexAutomobile;
extern int g_iLastProcessedModelIndexAutoEnt;
#endif

extern int g_iLastProcessedSkinCollision;
extern int g_iLastProcessedEntityCollision;

extern char g_bufRenderQueueCommand[200];
extern uintptr_t g_dwRenderQueueOffset;

void PrintBuildCrashInfo() 
{
    CrashLog("Build times: %s %s", __TIME__, __DATE__);
    CrashLog("Last processed auto and entity: %d %d", g_usLastProcessedModelIndexAutomobile, g_iLastProcessedModelIndexAutoEnt);
    CrashLog("Last processed skin and entity: %d %d", g_iLastProcessedSkinCollision, g_iLastProcessedEntityCollision);
}

#include <sstream>
#include "vendor/bass/bass.h"
#include "gui/CFontRenderer.h"
#include "util/CJavaWrapper.h"
#include "CDebugInfo.h"

void InitSAMP(JNIEnv *pEnv, jobject thiz) {
    Log("WEIKTON GAMES LTD.");
    Log("Initializing SAMP..");

    CLoader::loadSetting();

    g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);

    g_pJavaWrapper->StartSamp(5);
    CLoader::initCrashLytics();

    CWeaponsOutFit::ParseDatFile();
}

void InitInMenu() {
    pGame = new CGame();

    pGame->InitInMenu();

    pGUI = new CGUI();
    CKeyBoard::init();
    pPlayerTags = new CPlayerTags();
    CFontRenderer::Initialise();
}

#include <unistd.h> // system api
#include <sys/mman.h>
#include <cassert> // assert()
#include <dlfcn.h> // dlopen

bool unique_library_handler(const char *library) {
    Dl_info info;
    if (dladdr(__builtin_return_address(0), &info) != 0) {
        void *current_library_addr = dlopen(info.dli_fname, RTLD_NOW);
        if (!current_library_addr)
            return false;

        if (dlopen(library, RTLD_NOW) != current_library_addr)
            return false;
    }
    return true;
}

#include "net/CUDPSocket.h"
#include "CDebugInfo.h"
#include "game/Models/ModelInfo.h"
#include "util/CStackTrace.h"
#include "voice/Plugin.h"

void MainLoop() {
    if (pGame->bIsGameExiting)return;

    static bool bNetworkInited = false;
    if (!CGame::bIsGameInited) {

        pGame->InitInGame();
        pGame->SetMaxStats();

        Plugin::OnPluginLoad();
        Plugin::OnSampLoad();
        LogVoice("[dbg:samp:load] : module loading...");

        for(const auto& loadCallback : Samp::loadCallbacks)
        {
            if(loadCallback != nullptr)
                loadCallback();
        }

        Samp::loadStatus = true;
        LogVoice("[dbg:samp:load] : module loaded");

        CPlayerPed *pPlayer = pGame->FindPlayerPed();
        pPlayer->TogglePlayerControllable(false);
        *(uint8_t *)(g_libGTASA + (VER_x32 ? 0x819D88 : 0x9FF3A8)) = 0;

        CGame::bIsGameInited = true;

        return;
    }

    if (!pNetGame)
    {
        CHUD::Render();
        if(strlen(CSettings::m_Settings.szNickName) > 3) {
            pNetGame = new CNetGame(
                    "188.127.241.74",
                    2775,
                    CSettings::m_Settings.szNickName,
                    CSettings::m_Settings.szPassword);
            bNetworkInited = true;
            Log("InitInGame() end");
        }
        return;
    }

    pNetGame->Process();
}

extern int g_iLastRenderedObject;
char g_iLastBlock[512];

void PrintSymbols(void *pc, void *lr) {
    Dl_info info_pc, info_lr;
    if (dladdr(pc, &info_pc) != 0) {
        CrashLog("PC: %s", info_pc.dli_sname);
    }
    if (dladdr(lr, &info_lr) != 0) {
        CrashLog("LR: %s", info_lr.dli_sname);
    }
}


struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;
struct sigaction act4_old;

void handler(int signum, siginfo_t *info, void* contextPtr)
{
    ucontext* context = (ucontext_t*)contextPtr;

    if (act_old.sa_sigaction)
    {
        act_old.sa_sigaction(signum, info, contextPtr);
    }

    if(info->si_signo == SIGSEGV)
    {
        CrashLog("SIGSEGV | Fault address: 0x%x", info->si_addr);

        PRINT_CRASH_STATES(context);

        CStackTrace::printBacktrace();
    }

    return;
}

void handler1(int signum, siginfo_t *info, void* contextPtr)
{
    ucontext* context = (ucontext_t*)contextPtr;

    if (act1_old.sa_sigaction)
    {
        act1_old.sa_sigaction(signum, info, contextPtr);
    }

    if(info->si_signo == SIGABRT)
    {
        CrashLog("SIGABRT | Fault address: 0x%x", info->si_addr);

        PRINT_CRASH_STATES(context);

        CStackTrace::printBacktrace();
    }

    return;
}

void handler2(int signum, siginfo_t *info, void* contextPtr)
{
    ucontext* context = (ucontext_t*)contextPtr;

    if (act2_old.sa_sigaction)
    {
        act2_old.sa_sigaction(signum, info, contextPtr);
    }

    if(info->si_signo == SIGFPE)
    {
        CrashLog("SIGFPE | Fault address: 0x%x", info->si_addr);

        PRINT_CRASH_STATES(context);

        CStackTrace::printBacktrace();
    }

    return;
}

void handler3(int signum, siginfo_t *info, void* contextPtr)
{
    ucontext* context = (ucontext_t*)contextPtr;

    if (act3_old.sa_sigaction)
    {
        act3_old.sa_sigaction(signum, info, contextPtr);
    }

    if(info->si_signo == SIGBUS)
    {
        CrashLog("SIGBUS | Fault address: 0x%x", info->si_addr);

        PRINT_CRASH_STATES(context);

        CStackTrace::printBacktrace();
    }

    return;
}

void handler4(int signum, siginfo_t *info, void* contextPtr)
{
    ucontext* context = (ucontext_t*)contextPtr;

    if (act4_old.sa_sigaction)
    {
        act4_old.sa_sigaction(signum, info, contextPtr);
    }

    if(info->si_signo == SIGILL)
    {
        CrashLog("SIGBUS | Fault address: 0x%x", info->si_addr);

        PRINT_CRASH_STATES(context);

        CStackTrace::printBacktrace();
    }

    return;
}

extern "C"
{
JavaVM *javaVM = nullptr;
JavaVM *alcGetJavaVM(void) {
    return javaVM;
}
}

#include "CFPSFix.h"
#include "util/patch.h"
#include "CLoader.h"
#include "MaterialText.h"
#include "game/multitouch.h"
#include "voice/Header.h"

CFPSFix g_fps;

void (*ANDRunThread)(void *a1);

void ANDRunThread_hook(void *a1) {
    g_fps.PushThread(gettid());

    ANDRunThread(a1);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    g_libGTASA = CUtil::FindLib("libGTASA.so");
    g_libSAMP = CUtil::FindLib("libsamp.so");

    if (g_libGTASA == 0) {
        Log("ERROR: libGTASA.so address not found!");
        return 0;
    }
    Log("libGTASA.so image base address: 0x%X", g_libGTASA);

    CLoader::initJavaClasses(vm);

    CHook::Lib = dlopen("libGTASA.so", 1);

    InstallSpecialHooks();
    InitRenderWareFunctions();
    MultiTouch::initialize();
    ApplyPatches_level0();

    CLoader::loadBassLib();

    struct sigaction act;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &act, &act_old);

    struct sigaction act1;
    act1.sa_sigaction = handler1;
    sigemptyset(&act1.sa_mask);
    act1.sa_flags = SA_SIGINFO;
    sigaction(SIGABRT, &act1, &act1_old);

    struct sigaction act2;
    act2.sa_sigaction = handler2;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = SA_SIGINFO;
    sigaction(SIGFPE, &act2, &act2_old);

    struct sigaction act3;
    act3.sa_sigaction = handler3;
    sigemptyset(&act3.sa_mask);
    act3.sa_flags = SA_SIGINFO;
    sigaction(SIGBUS, &act3, &act3_old);

    struct sigaction act4;
    act4.sa_sigaction = handler4;
    sigemptyset(&act4.sa_mask);
    act4.sa_flags = SA_SIGINFO;
    sigaction(SIGILL, &act4, &act4_old);

    return JNI_VERSION_1_6;
}

void Log(const char *fmt, ...) {
    char buffer[512];

    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    firebase::crashlytics::Log(buffer);
    __android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

    //if(pDebug) pDebug->AddMessage(buffer);
    static FILE *flLog = nullptr;

    if (flLog == nullptr && g_pszStorage != nullptr) {
        sprintf(buffer, "%slog.txt", g_pszStorage);
        flLog = fopen(buffer, "a");
    }

    if (flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);

    return;
}

void CrashLog(const char *fmt, ...) {
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    firebase::crashlytics::Log(buffer);
    __android_log_write(ANDROID_LOG_FATAL, "AXL", buffer);

    static FILE *flLog = nullptr;

    if (flLog == nullptr && g_pszStorage != nullptr) {
        sprintf(buffer, "%scrash_log.log", g_pszStorage);
        flLog = fopen(buffer, "a");
    }

    if (flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);

    return;
}
uint32_t GetTickCount() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
void ApplyFPSPatch(uint8_t fps);

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_core_Samp_initSAMP(JNIEnv *env, jobject thiz, jfloat maxfps) {
    CSettings::maxFps = (int)maxfps;
    ApplyFPSPatch(CSettings::maxFps);
    g_pJavaWrapper = new CJavaWrapper(env, thiz);
}
void LogVoice(const char *fmt, ...)
{
    char buffer[0xFF];
    static FILE* flLog = nullptr;

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%sSAMP/sampvoice/%s", g_pszStorage, SV::kLogFileName);
        flLog = fopen(buffer, "w");
    }

    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);
    __android_log_write(ANDROID_LOG_INFO, "SampVoice", buffer);
    if(flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);
    return;
}

