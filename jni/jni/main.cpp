#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <unistd.h> // system api
#include <sys/mman.h> // sysm()
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen

static bool testbuild = true; 

#include "main.h"
#include "util/patch.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "settings.h"
#include "util/CClientJava.h"
#include "scoreboard.h"
#include "CAudioStream.h"
#include "util/armhook.h"
#include "game/ccheckfilehash.h"
#include "str_obfuscator_no_template.hpp"

#include "voice/CVoiceChatClient.h"
#include "crashlytics.h"

#include "br/CHUD.h"

#include "util/CClientJava.h"
#include "vendor/bass/bass.h"
#include "gui/CFontRenderer.h"
#include "util/CJavaWrapper.h"
#include "cryptors/INITSAMP_result.h"
#include "debug.h"

uintptr_t g_libGTASA = 0;
uintptr_t g_libSTUFF = 0;
uintptr_t* mapthiz = 0;
const char* g_pszStorage = nullptr;

#include "clientlogic.h"

#include "br/CNetwork.h"
#include "br/CLocalisation.h"

#include "gtare/CSkyBox.h"
#include "gtare/CRQ_Commands.h"
#include "CAudioStreamManager.h"

const cryptor::string_encryptor encLib = cryptor::create("libsamp.so", 11);
void CrashLog(const char* fmt, ...);
CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr; 
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;
CVoiceChatClient* pVoice = nullptr;
CSnapShotHelper* pSnapShotHelper = nullptr;
CScoreBoard* pScoreBoard = nullptr;
CAudioStream* pAudioStream = nullptr;
CAudioStreamManager* pAudioStreamManager = nullptr;
CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
CSettings *pSettings = nullptr;
CClientLogic *pClientLogic = nullptr;

void InitHookStuff();
void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();

void PrintBuildInfo()
{
	Log(OBFUSCATE("Initializing zalupa: 0"));
}

#ifdef GAME_EDITION_CR
extern uint16_t g_usLastProcessedModelIndexAutomobile;
extern int g_iLastProcessedModelIndexAutoEnt;
#endif

extern int g_iLastProcessedSkinCollision;
extern int g_iLastProcessedEntityCollision;
extern int g_iLastRenderedObject;
extern char g_bufRenderQueueCommand[200];
char g_iLastBlock[512];
extern uintptr_t g_dwRenderQueueOffset;


void PrintBuildCrashInfo()
{
	CrashLog(OBFUSCATE("Версия клиента: %s"), _VERSION);

	CrashLog(OBFUSCATE("Последний использованный объект: %d"), g_iLastRenderedObject);

                  if(g_iLastProcessedEntityCollision == 228) 
	                  CrashLog(OBFUSCATE("Последнее использованное авто: 0"));
	else
                                  	CrashLog(OBFUSCATE("Последнее использованное авто: %d"), g_iLastProcessedEntityCollision);

                  if(g_iLastProcessedSkinCollision == 228) 
	                  CrashLog(OBFUSCATE("Последний использованный скин: 0"));
	else
                                    CrashLog(OBFUSCATE("Последний использованный скин: %d"), g_iLastProcessedSkinCollision);
    CrashLog("Last texture: %s", g_iLastBlock);
    CrashLog("Last rendered object: %d", g_iLastRenderedObject);
}

void InitSAMP(JNIEnv* pEnv, jobject thiz/*, jstring storage*/);
extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initSAMP(JNIEnv* pEnv, jobject thiz/*, jstring storage*/)
	{
		InitSAMP(pEnv, thiz/*, storage*/);
	}
}

void InitBASSFuncs();
void InitSAMP(JNIEnv* pEnv, jobject thiz/*, jstring storage*/)
{
	PROTECT_CODE_INITSAMP;

	PrintBuildInfo();

	InitBASSFuncs();
	BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);      
                
	// g_pszStorage = (const char*)(SA_ADDR(0x63C4B8));
	// g_pszStorage = OBFUSCATE("/storage/emulated/0/KirillVovk/");
	g_pszStorage = OBFUSCATE("/storage/emulated/0/BlackRussia/");
	// g_pszStorage = OBFUSCATE("/storage/emulated/0/Android/data/blackrussia.online/files/");

	if(!g_pszStorage)
                  {
		Log(OBFUSCATE("Error: storage path not found."));
		std::terminate();
	}

	pSettings = new CSettings();

//                ARTEFACTS
//	if (!*(uintptr_t *)(g_libGTASA + 0x61B298) && !((int (*)(const char *))(g_libGTASA + 0x179A20))("glAlphaFuncQCOM") )
//  	{
//   	                  CHook::NOP(g_libGTASA + 0x1A6164, 4);
//    	                  CHook::WriteMemory(g_libGTASA + 0x1A6164, "\x70\x47", 2);
//  	}

	CWeaponsOutFit::SetEnabled(0);
	CRadarRect::SetEnabled(0);
	CGame::SetEnabledPCMoney(pSettings->GetReadOnly().iPCMoney);
	CDebugInfo::SetDrawFPS(pSettings->GetReadOnly().iFPSCounter);
	CInfoBarText::SetEnabled(0);
	CSnow::SetCurrentSnow(pSettings->GetReadOnly().iSnow);

	g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);
	g_pJavaWrapper->SetUseFullScreen(pSettings->GetReadOnly().iCutout);

	CLocalisation::Initialise(OBFUSCATE("ru.lc"));

	firebase::crashlytics::SetUserId(pSettings->GetReadOnly().szNickName);

	CWeaponsOutFit::ParseDatFile();

	if(!CCheckFileHash::IsFilesValid())
	{
		CClientInfo::bSAMPModified = false;
		return;
	}
}

extern CSnapShotHelper* pSnapShotHelper;
void ProcessCheckForKeyboard();
void GTASAInit();
void InitInMenu()
{
                  GTASAInit();

	pGame = new CGame();
	pGame->InitInMenu();

	pGUI = new CGUI();
	pKeyBoard = new CKeyBoard();
	pChatWindow = new CChatWindow();
	pPlayerTags = new CPlayerTags();
	pDialogWindow = new CDialogWindow();
	pScoreBoard = new CScoreBoard();
	pSnapShotHelper = new CSnapShotHelper();
	pAudioStreamManager = new CAudioStreamManager();

	ProcessCheckForKeyboard();

	CFontRenderer::Initialise();

	CHUD::Initialise();
}

bool unique_library_handler(const char* library)
{
	Dl_info info;
	if (dladdr(__builtin_return_address(0), &info) != 0)
	{
		void* current_library_addr = dlopen(info.dli_fname, RTLD_NOW);
		if (!current_library_addr)
			return false;

		if (dlopen(library, RTLD_NOW) != current_library_addr)
			return false;
	}
	return true;
}

void ProcessCheckForKeyboard()
{
	if (pSettings->GetReadOnly().iAndroidKeyboard)
	{
		pKeyBoard->EnableNewKeyboard();
	}
	else
	{
		pKeyBoard->EnableOldKeyboard();
	}
}

void ObfuscatedForceExit3()
{
       /* todo */	
}

int g_iServer = 0;
#include "net/CUDPSocket.h"
void InitInGame()
{
	static bool bGameInited = false;
	static bool bNetworkInited = false;
	if (!bGameInited)
	{
		pGame->InitInGame();
		pGame->SetMaxStats();
                                    pGame->SetWorldWeather(10); // сетаем погоду
		pGame->DisplayWidgets(false); // показываем кнопки
		pGame->ToggleRadar(false); // скрываем радар


        *(uint8_t*)(g_libGTASA+0x7165E8) = 0;
		g_pJavaWrapper->UpdateSplash(111); // обновляем прогресс загрузки
       /* pNetGame = new CNetGame(
                g_sEncryptedAddresses[0].decrypt(),
                g_sEncryptedAddresses[0].getPort(),
                pSettings->GetReadOnly().szNickName,
                pSettings->GetReadOnly().szPassword);*/
        //pSettings->GetWrite().last_server = 0;

        g_pJavaWrapper->HideGPS();
        g_pJavaWrapper->ShowRadar();
        g_pJavaWrapper->HideZona();
        g_pJavaWrapper->HideX2();

		CPlayerPed *pPlayer = pGame->FindPlayerPed();
	                  CCamera *pCamera = pGame->GetCamera();
                                    pCamera->SetPosition(431.8335, 247.1207, 11.9394, 0.0f, 0.0f, 0.0f);
		pCamera->LookAtPoint(431.8335, 247.1207, 11.9394, 2);

		//pClientLogic = new CClientLogic();
		bGameInited = true;

		return;
	}

	if (!bNetworkInited)
	{
		bNetworkInited = true;
		return;
	}
}

void (*CTimer__StartUserPause)();
void CTimer__StartUserPause_hook()
{
	// process pause event
	if (g_pJavaWrapper)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsNewKeyboard())
			{
				pKeyBoard->Close();
			}
		}
		g_pJavaWrapper->SetPauseState(true);
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 1;
}

void (*CTimer__EndUserPause)();
void CTimer__EndUserPause_hook()
{
	// process resume event
	if (g_pJavaWrapper)
	{
		g_pJavaWrapper->SetPauseState(false);
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 0;
}

#include "debug.h"
void MainLoop()
{
	InitInGame();

	if(pNetGame) pNetGame->Process();

	if (pNetGame)
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
				if (pVeh)
				{
					VECTOR vec;
					pVeh->GetMoveSpeedVector(&vec);
					CDebugInfo::ProcessSpeedMode(&vec);
				}
			}
		}
	}

	if(pClientLogic) pClientLogic->ProcessLogic();
}
extern int g_iLastRenderedObject;

void PrintSymbols(void* pc, void* lr)
{
	Dl_info info_pc, info_lr;
	if (dladdr(pc, &info_pc) != 0)
	{
		CrashLog(OBFUSCATE("PC: %s"), info_pc.dli_sname);
	}
	if (dladdr(lr, &info_lr) != 0)
	{
		CrashLog(OBFUSCATE("LR: %s"), info_lr.dli_sname);
	}
}


struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;
void handler3(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act3_old.sa_sigaction)
	{
		act3_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGBUS)
	{
		int crashId = (int)rand() % 20000;
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID: %d", crashId);

		CrashLog("ADDR:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);
		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));
		exit(0);
	}

	return;
}
/*OBFUSCATE("WEIKTON");*/
void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act_old.sa_sigaction)
	{
		act_old.sa_sigaction(signum, info, contextPtr);
	}

	if(info->si_signo == SIGSEGV)
	{

		int crashId = (int)rand() % 20000;
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID(2): %d", crashId);

		CrashLog("ADDR:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));
		exit(0);
	}

	return;
}

void handler2(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act2_old.sa_sigaction)
	{
		act2_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGFPE)
	{

		int crashId = (int)rand() % 20000;
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID(3): %d", crashId);

		CrashLog("ADDR:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

		exit(0);
	}

	return;
}

void handler1(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act1_old.sa_sigaction)
	{
		act1_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGABRT)
	{

		int crashId = (int)rand() % 20000;
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID(5): %d", crashId);

		CrashLog("ADDR:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

		exit(0);
	}

	return;
}

extern "C"
{
	JavaVM* javaVM = NULL;
	JavaVM* alcGetJavaVM(void) {
		return javaVM;
	}
}
extern "C" AL_API jint AL_APIENTRY JNI_OnLoad_alc(JavaVM* vm, void* reserved);

void (*RQ_Command_rqSetAlphaTest)(char**);
void RQ_Command_rqSetAlphaTest_hook(char** a1)
{
	return;
}

#include "new_fps.h"
#include "CLoader.h"

CFPSFix g_fps;

void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1)
{
	g_fps.PushThread(gettid());

	ANDRunThread(a1);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNI_OnLoad_alc(vm, reserved);
	javaVM = vm;

	Log(OBFUSCATE("libsamp.so loaded!"));
	
	g_libGTASA = FindLibrary(OBFUSCATE("libGTASA.so"));
	if(g_libGTASA == 0)
	{
		Log(OBFUSCATE("ERROR: libGTASA.so address not found!"));
		return 0;
	}
	Log("libGTASA.so image base address: 0x%X", g_libGTASA);

    CLoader::LoadClass(vm);

                  // FULL SECURITY ----------------------------------------------------------------------------
//                  Log(OBFUSCATE("Install Security..."));
//	dlopen(OBFUSCATE("/data/data/blackrussia.online/cache/libSTUFF.so"), 1);
//
//	g_libSTUFF = FindLibrary(OBFUSCATE("libSTUFF.so")); 
  //                if(!g_libSTUFF)
      //            {
    //                                Log(OBFUSCATE("Error: security not installed."));
        //                            return 0;
          //        }
//
//	CHook::Write<uintptr_t>(g_libSTUFF + 0x1D5FB4, g_libGTASA);
//	CHook::Write<const char*>(g_libSTUFF + 0x1D5FB8, g_pszStorage);
//	CHook::Write<JavaVM*>(g_libSTUFF + 0x409F74, javaVM);
                  // -----------------------------------------------------------------------------------------------------

	firebase::crashlytics::Initialize();

	uintptr_t libgtasa = FindLibrary("libGTASA.so");
	uintptr_t libsamp = FindLibrary("libsamp.so");
	uintptr_t libc = FindLibrary("libc.so");

	Log("libGTASA.so: 0x%x", libgtasa);
	Log("libsamp.so: 0x%x", libsamp);
	Log("libc.so: 0x%x", libc);

	char str[100];
    firebase::crashlytics::SetCustomKey("build data", __DATE__);
    firebase::crashlytics::SetCustomKey("build time", __TIME__);
    firebase::crashlytics::SetCustomKey("Nick", pSettings->GetReadOnly().szNickName);
	sprintf(str, "0x%x", libgtasa);
	firebase::crashlytics::SetCustomKey("libGTASA.so", str);
	
	sprintf(str, "0x%x", libsamp);
	firebase::crashlytics::SetCustomKey("libsamp.so", str);

	sprintf(str, "0x%x", libc);
	firebase::crashlytics::SetCustomKey("libc.so", str);

	srand(time(0));

	InitHookStuff();

	InitRenderWareFunctions();
	InstallSpecialHooks();
	// increase render memory buffer
	SetUpHook(g_libGTASA + 0x003BF784, (uintptr_t)CTimer__StartUserPause_hook, (uintptr_t*)& CTimer__StartUserPause);
	SetUpHook(g_libGTASA + 0x003BF7A0, (uintptr_t)CTimer__EndUserPause_hook, (uintptr_t*)& CTimer__EndUserPause);

	// yes, just nop-out this fucking shit
	// this should prevent game from crashing when exiting(R*)
	NOP(g_libGTASA + 0x0039844E, 2);
	NOP(g_libGTASA + 0x0039845E, 2);
	NOP(g_libGTASA + 0x0039840A, 2);

	NOP(g_libGTASA + 0x002E1EDC, 2); // get the fuck up this uninitialised shit!
	NOP(g_libGTASA + 0x00398972, 2); // get out fucking roadblocks
	// maybe nop engine terminating ????
	// terminate all stuff when exiting
	// nop shit pause

	ApplyPatches_level0();

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

	return JNI_VERSION_1_4;
}

void Log(const char *fmt, ...)
{	
	char buffer[0xFF];
	static FILE* flLog = nullptr;

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%sloader/blackrussia_client.log", g_pszStorage);
        flLog = fopen(buffer, "a");
    }
	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

    __android_log_write(ANDROID_LOG_INFO, "AXL", buffer);
	firebase::crashlytics::Log(buffer);

	//if(pDebug) pDebug->AddMessage(buffer);

	if(flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

void CrashLog(const char* fmt, ...)
{
    char buffer[0xFF];
    static FILE* flLog = nullptr;

    if (flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%sloader/blackrussia_crash.log", g_pszStorage);
        flLog = fopen(buffer, "a");
    }

    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    __android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

    firebase::crashlytics::Log(buffer);

    if (flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);

    return;
}
uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000+tv.tv_usec/1000);
}
