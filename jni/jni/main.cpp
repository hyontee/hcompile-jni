#include "main.h"

#include "game/game.h"
#include "game/crosshair.h"
#include "game/materialtext.h"
#include "game/RW/RenderWare.h"

#include "graphics/CSkyBox.h"
#include "graphics/CInterface.h"

#include "net/netgame.h"

#include "gui/gui.h"
#include "gui/CFontRenderer.h"
#include "gui/buttons.h"

#include "chatwindow.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "CSettings.h"
#include "CClientInfo.h"
#include "scoreboard.h"
#include "CAudioStream.h"
#include "crashlytics.h"
#include "CCheckFileHash.h"
#include "str_obfuscator_no_template.hpp"
#include "CDebugInfo.h"
#include "CServerManager.h"
#include "CLocalisation.h"
#include "KillList.h"
#include "CClientInfo.h"
#include "GButton.h"

#include "util/armhook.h"
#include "util/CStackTrace.h"
#include "util/CJavaWrapper.h"
#include "util/interfaces/CLoading.h"
#include "util/interfaces/CNoty.h"

#include "voice/CVoiceChatClient.h"

#include "vendor/bass/bass.h"

#include "cryptors/INITSAMP_result.h"

uintptr_t g_libGTASA = 0;

const char* g_pszStorage = nullptr;
const cryptor::string_encryptor encLib = cryptor::create(OBFUSCATE("libsamp.so"), 11);

CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr;
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;
CVoiceChatClient* pVoice = nullptr;
CSnapShotHelper* pSnapShotHelper = nullptr;
CScoreBoard* pScoreBoard = nullptr;
CAudioStream* pAudioStream = nullptr;
CMaterialText *pMaterialText = nullptr;
CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
CSettings *pSettings = nullptr;
KillList *pKillList = nullptr;
CCrossHair *pCrossHair = nullptr;
CGButton *pGButton = nullptr;
CButtons *m_pButtons = nullptr;
 
char* CMain::BaseStorage = nullptr;
char* CMain::RootStorage = nullptr;
char* CMain::CustomStorage = nullptr;
char* CMain::cryptStorage = nullptr;



bool CollisionPatchesUsed;
bool m_bChatStarted = false;
uint8_t m_PortVoice;
uint8_t m_SelectedServer;
uint32_t col_all1;
uint32_t col_all2;
uint32_t col_all3;




void ProcessCheckForKeyboard();
void InitHookStuff();
int InitBASSFuncs();
void InitRenderWareFunctions();
void InitSAMP(JNIEnv* pEnv, jobject thiz);
void InstallSpecialHooks();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();

extern int g_iLastModelLoaded;
extern char g_iLastModelStatus[128];
extern char g_iLastModelRequest[128];
extern char g_iLastModelRequestLoad[128];

#ifdef GAME_EDITION_CR
	extern uint16_t g_usLastProcessedModelIndexAutomobile;
	extern int g_iLastProcessedModelIndexAutoEnt;
#endif

extern int g_iLastProcessedSkinCollision;
extern int g_iLastProcessedEntityCollision;
extern int g_iLastRenderedObject;
extern char g_iLastBlock[512];

#define PRINT_BUILD_CRASH_INFO \
	CrashLog(OBFUSCATE("Build time: %s %s"), __TIME__, __DATE__); \
	CrashLog(OBFUSCATE("IsModelLoaded: %d (%s)"), g_iLastModelLoaded, g_iLastModelStatus); \
	CrashLog(OBFUSCATE("RequestModel: %s"), g_iLastModelRequest); \
	CrashLog(OBFUSCATE("LoadRequestedModels: %s"), g_iLastModelRequestLoad); \
	CrashLog(OBFUSCATE("Last rendered object: %d"), g_iLastRenderedObject); \
	CrashLog(OBFUSCATE("Last texture: %s"), g_iLastBlock); \
	CrashLog(OBFUSCATE("Last processed auto and entity: %d %d"), g_usLastProcessedModelIndexAutomobile, g_iLastProcessedModelIndexAutoEnt);\
	CrashLog(OBFUSCATE("Last processed skin and entity: %d %d"), g_iLastProcessedSkinCollision, g_iLastProcessedEntityCollision);

#define PRINT_BASE_ADDRESSES \
	CrashLog(OBFUSCATE("libGTASA base address: 0x%X"), g_libGTASA); \
	CrashLog(OBFUSCATE("libsamp base address: 0x%X"), FindLibrary(OBFUSCATE("libsamp.so"))); \
	CrashLog(OBFUSCATE("libc base address: 0x%X"), FindLibrary(OBFUSCATE("libc.so")));

#define PRINT_CRASH_STATES(context) \
	CrashLog(OBFUSCATE("register states:")); \
	CrashLog(OBFUSCATE("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X"), (context)->uc_mcontext.arm_r0, (context)->uc_mcontext.arm_r1, (context)->uc_mcontext.arm_r2, (context)->uc_mcontext.arm_r3); \
	CrashLog(OBFUSCATE("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x"), (context)->uc_mcontext.arm_r4, (context)->uc_mcontext.arm_r5, (context)->uc_mcontext.arm_r6, (context)->uc_mcontext.arm_r7); \
	CrashLog(OBFUSCATE("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x"), (context)->uc_mcontext.arm_r8, (context)->uc_mcontext.arm_r9, (context)->uc_mcontext.arm_r10, (context)->uc_mcontext.arm_fp); \
	CrashLog(OBFUSCATE("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x"), (context)->uc_mcontext.arm_ip, (context)->uc_mcontext.arm_sp, (context)->uc_mcontext.arm_lr, (context)->uc_mcontext.arm_pc);

#define PRINT_BACKTRACE(context) \
	CrashLog(OBFUSCATE("backtrace:")); \
	CrashLog(OBFUSCATE("1: libGTASA.so + 0x%X"), (context)->uc_mcontext.arm_pc - g_libGTASA); \
	CrashLog(OBFUSCATE("2: libGTASA.so + 0x%X"), (context)->uc_mcontext.arm_lr - g_libGTASA); \
																					\
	CrashLog(OBFUSCATE("1: libsamp.so + 0x%X"), (context)->uc_mcontext.arm_pc - FindLibrary(OBFUSCATE("libsamp.so"))); \
	CrashLog(OBFUSCATE("2: libsamp.so + 0x%X"), (context)->uc_mcontext.arm_lr - FindLibrary(OBFUSCATE("libsamp.so"))); \
																						\
	CrashLog(OBFUSCATE("1: libc.so + 0x%X"), (context)->uc_mcontext.arm_pc - FindLibrary(OBFUSCATE("libc.so"))); \
	CrashLog(OBFUSCATE("2: libc.so + 0x%X"), (context)->uc_mcontext.arm_lr - FindLibrary(OBFUSCATE("libc.so"))); \
																							\
	PrintSymbols((void*)((context)->uc_mcontext.arm_pc), (void*)((context)->uc_mcontext.arm_lr)); \
																							\
	CStackTrace::printBacktrace(context);

extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initSAMP(JNIEnv* pEnv, jobject thiz)
	{
		InitSAMP(pEnv, thiz);
	}
}

int loaded_libBass = 0;
bool DumpLibraries(std::vector<std::string>& buff);
void InitSAMP(JNIEnv* pEnv, jobject thiz)
{
	PROTECT_CODE_INITSAMP;
	
	Log(OBFUSCATE("Initializing SAMP.."));

	loaded_libBass = InitBASSFuncs();
	if (loaded_libBass) BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, nullptr); // ������������� ��������� ������

	if(!g_pszStorage)
	{
		Log(OBFUSCATE("Error: storage path not found!"));
		std::terminate();
	}

	Log("Build time: %s %s", __TIME__, __DATE__);
	Log("Storage: %s", g_pszStorage);

	pSettings = new CSettings();

	g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);
	g_pJavaWrapper->SetUseFullScreen(pSettings->GetReadOnly().iCutout);

	CWeaponsOutFit::SetEnabled(pSettings->GetReadOnly().iOutfitGuns);
	CRadarRect::SetEnabled(pSettings->GetReadOnly().iRadarRect);
	CGame::SetEnabledPCMoney(pSettings->GetReadOnly().iPCMoney);
	CDebugInfo::SetDrawFPS(pSettings->GetReadOnly().iFPSCounter);
	CInfoBarText::SetEnabled(pSettings->GetReadOnly().iHPArmourText);
	CSnow::SetCurrentSnow(pSettings->GetReadOnly().iSnow);

	CLocalisation::Initialise("ru.lc");
	CWeaponsOutFit::ParseDatFile();

	firebase::crashlytics::SetUserId(pSettings->GetReadOnly().szNickName);

	if(!CCheckFileHash::IsFilesValid())
	{
		CClientInfo::bSAMPModified = false;
		return;
	}
}

void InitInMenu()
{
	Log(OBFUSCATE("InitInMenu"));
	pGame = new CGame();
	pGame->InitInMenu();

	pGUI 				= new CGUI();
	pKeyBoard 			= new CKeyBoard();
	pChatWindow 		= new CChatWindow();
	pPlayerTags 		= new CPlayerTags();
	pDialogWindow 		= new CDialogWindow();
	pScoreBoard 		= new CScoreBoard();
	pSnapShotHelper 	= new CSnapShotHelper();
	pAudioStream 		= new CAudioStream();
    pMaterialText 		= new CMaterialText();
    pKillList 			= new KillList();
	pCrossHair 			= new CCrossHair();
	pGButton 			= new CGButton();
    m_pButtons      	= new CButtons();

	ProcessCheckForKeyboard();

	CFontRenderer::Initialise();
	Log(OBFUSCATE("InitInMenu done!"));
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
	if (pSettings->GetReadOnly().iAndroidKeyboard) pKeyBoard->EnableNewKeyboard(pSettings->GetReadOnly().iAndroidKeyboard);
	else pKeyBoard->EnableOldKeyboard();
}

bool m_bStartedJava = false;
bool m_bStopLoadingScreen = false;
void InitInGame()
{
	static bool bGameInited = false;
	static bool bNetworkInited = false;

	if(!bGameInited)
	{
		if (!unique_library_handler(encLib.decrypt()))
			std::terminate();

		pGame->InitInGame();
		pGame->SetMaxStats();

		switch (m_SelectedServer)
		{
			case ServerTape:
			{
				//pNetGame = new CNetGame(cryptor::create("91.134.254.233", 15).decrypt(), atoi(cryptor::create("7777", 4).decrypt()), pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
				break;
			}
			case ServerJust:
			{
				//pNetGame = new CNetGame(cryptor::create("51.91.91.98", 12).decrypt(), atoi(cryptor::create("7777", 4).decrypt()), pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
				pNetGame = new CNetGame(cryptor::create("188.127.241.74", 13).decrypt(), atoi(cryptor::create("1037", 4).decrypt()), pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
				break;
			}
			case ServerGoldRussia:
			{
				std::vector<std::string> szLibs;
				if (DumpLibraries(szLibs))
				{
					//std::string szFinal;
					for (size_t i = 0; i < szLibs.size(); i++)
					{
						if (!strstr(szLibs[i].c_str(), "libsamp.so") && !strstr(szLibs[i].c_str(), "libSCAnd.so") && !strstr(szLibs[i].c_str(), "libGTASA.so") && !strstr(szLibs[i].c_str(), "libbass.so") &&
							!strstr(szLibs[i].c_str(), "libImmEmulatorJ.so") && !strstr(szLibs[i].c_str(), "libcrashlytics.so") && !strstr(szLibs[i].c_str(), "libcrashlytics-common.so"))
						{
							exit(0);
						}
						//szFinal += szLibs[i];
						//szFinal += ':';
						
						//Log("szLibs[%d] (%d) %s", i, szLibs[i].length(), szLibs[i].c_str());
					}
					//Log("szFinal (%d) %s", szFinal.length(), szFinal.c_str());
				}
			
				if (m_bStartedJava == true)
				{
					m_bStartedJava = false;
					m_bStopLoadingScreen = true;
				}
				
				//CServerInstance::ShowDialog();
				//pNetGame = new CNetGame(cryptor::create("46.174.49.47", 13).decrypt(), atoi(cryptor::create("7807", 4).decrypt()), pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
				break;
			}
			case ServerCrime:
			{
				std::vector<std::string> szLibs;
				if (DumpLibraries(szLibs))
				{
					//std::string szFinal;
					for (size_t i = 0; i < szLibs.size(); i++)
					{
						if (!strstr(szLibs[i].c_str(), "libsamp.so") && !strstr(szLibs[i].c_str(), "libSCAnd.so") && !strstr(szLibs[i].c_str(), "libGTASA.so") && !strstr(szLibs[i].c_str(), "libbass.so") &&
							!strstr(szLibs[i].c_str(), "libImmEmulatorJ.so") && !strstr(szLibs[i].c_str(), "libcrashlytics.so") && !strstr(szLibs[i].c_str(), "libp7zip.so") && 
							!strstr(szLibs[i].c_str(), "libcrashlytics-common.so") && !strstr(szLibs[i].c_str(), "libcrashlytics-handler.so") && !strstr(szLibs[i].c_str(), "libcrashlytics-trampoline.so"))
						{
							Log(OBFUSCATE("Not accept entered (1)"));
							exit(0);
						}
						//szFinal += szLibs[i];
						//szFinal += ':';
						
						//Log("szLibs[%d] (%d) %s", i, szLibs[i].length(), szLibs[i].c_str());
					}
					//Log("szFinal (%d) %s", szFinal.length(), szFinal.c_str());
				}
			
				g_pJavaWrapper->setCheckFiles();
				
				if (m_bStartedJava == true)
				{
					m_bStartedJava = false;
					m_bStopLoadingScreen = true;
				}
			
				//CServerInstance::ShowDialog();
				g_pJavaWrapper->openSelectorServer();
				//pNetGame = new CNetGame(cryptor::create("51.91.91.98", 12).decrypt(), atoi(cryptor::create("7777", 4).decrypt()), pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
				break;
			}
			case ServerBoost:
			{
				//CServerInstance::ShowDialog();
				//pNetGame = new CNetGame(cryptor::create("51.91.91.98", 12).decrypt(), atoi(cryptor::create("7777", 4).decrypt()), pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
				break;
			}
		}

		bGameInited = true;
		return;
	}

	if(!bNetworkInited)
	{
		// HERE IS
		bNetworkInited = true;
		return;
	}
}

void MainLoop()
{
	InitInGame();

	if(pNetGame)
		pNetGame->Process();
	
	if (pAudioStream) 
		pAudioStream->Process();

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
	else
	{
		if (pGame)
		{
			pGame->SetWorldTime(12, 0);
			pGame->DisplayWidgets(false);
		}
	}
}

void PrintSymbols(void* pc, void* lr)
{
	Dl_info info_pc, info_lr;

	if (dladdr(pc, &info_pc) != 0)
		CrashLog(OBFUSCATE("PC: %s"), info_pc.dli_sname);

	if (dladdr(lr, &info_lr) != 0)
		CrashLog(OBFUSCATE("LR: %s"), info_lr.dli_sname);
}

struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;

void handler3(int signum, siginfo_t* info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	if (act3_old.sa_sigaction)
		act3_old.sa_sigaction(signum, info, contextPtr);

	if (info->si_signo == SIGBUS)
	{
		int crashId = (int)rand() % 20000;
		
		Log(OBFUSCATE("Crashed. %d"), crashId);
		CrashLog(" ");

		PRINT_BUILD_CRASH_INFO;

		CrashLog(OBFUSCATE("ID: %d"), crashId);
		CrashLog(OBFUSCATE("SIGBUS | Fault address: 0x%X"), info->si_addr);

		PRINT_BASE_ADDRESSES;
		PRINT_CRASH_STATES(context);
		PRINT_BACKTRACE(context);

		exit(0);
	}
}

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	if (act_old.sa_sigaction)
		act_old.sa_sigaction(signum, info, contextPtr);

	if(info->si_signo == SIGSEGV)
	{
		int crashId = (int)rand() % 20000;

		Log(OBFUSCATE("Crashed. %d"), crashId);
		CrashLog(" ");

		PRINT_BUILD_CRASH_INFO;

		CrashLog(OBFUSCATE("ID: %d"), crashId);
		CrashLog(OBFUSCATE("SIGSEGV | Fault address: 0x%X"), info->si_addr);

		PRINT_BASE_ADDRESSES;
		PRINT_CRASH_STATES(context);
		PRINT_BACKTRACE(context);

		exit(0);
	}
}

void handler2(int signum, siginfo_t* info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	if (act2_old.sa_sigaction)
		act2_old.sa_sigaction(signum, info, contextPtr);

	if (info->si_signo == SIGFPE)
	{
		int crashId = (int)rand() % 20000;

		Log(OBFUSCATE("Crashed. %d"), crashId);
		CrashLog(" ");

		PRINT_BUILD_CRASH_INFO;

		CrashLog(OBFUSCATE("ID: %d"), crashId);
		CrashLog(OBFUSCATE("SIGFPE | Fault address: 0x%X"), info->si_addr);

		PRINT_BASE_ADDRESSES;
		PRINT_CRASH_STATES(context);
		PRINT_BACKTRACE(context);

		exit(0);
	}
}

void handler1(int signum, siginfo_t* info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	if (act1_old.sa_sigaction)
		act1_old.sa_sigaction(signum, info, contextPtr);

	if (info->si_signo == SIGABRT)
	{
		int crashId = (int)rand() % 20000;

		Log(OBFUSCATE("Crashed. %d"), crashId);
		CrashLog(" ");

		PRINT_BUILD_CRASH_INFO;

		CrashLog(OBFUSCATE("ID: %d"), crashId);
		CrashLog(OBFUSCATE("SIGABRT | Fault address: 0x%X"), info->si_addr);

		PRINT_BASE_ADDRESSES;
		PRINT_CRASH_STATES(context);
		PRINT_BACKTRACE(context);

		exit(0);
	}
}

extern "C"
{
	JavaVM* javaVM = NULL;
	JavaVM* alcGetJavaVM(void) {
		return javaVM;
	}
}

extern "C" AL_API jint AL_APIENTRY JNI_OnLoad_alc(JavaVM* vm, void* reserved);

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNI_OnLoad_alc(vm, reserved);
	javaVM = vm;
	
	m_SelectedServer = ServerCrime;
	switch (m_SelectedServer)
	{
		case ServerTape:
		{
			CMain::SetRootStorage("/data/data/ru.stepdev.tapemobile/");
			CMain::SetBaseStorage("/storage/emulated/0/Tape/");
			CMain::SetCustomStorage("gui");
			CMain::SetCryptStorage("ru.stepdev.crimemobile");
			
			g_pszStorage = CMain::GetBaseStorage();
			
			CollisionPatchesUsed = true;
			m_bChatStarted = false;
			
			col_all1 = 0x45;
			col_all2 = 0x82;
			col_all3 = 0xA1;
			break;
		}
		case ServerGoldRussia:
		{
			CMain::SetRootStorage("/data/data/ru.game.goldrussia/");
			CMain::SetBaseStorage("/storage/emulated/0/Gold/");
			CMain::SetCustomStorage("gui");
			CMain::SetCryptStorage("ru.game.goldrussia");
			
			g_pszStorage = CMain::GetBaseStorage();
			
			CollisionPatchesUsed = true;
			m_bChatStarted = false;
			
			col_all1 = 0xFF;
			col_all2 = 0xD7;
			col_all3 = 0x00;
			break;
		}
		case ServerJust:
		{
			CMain::SetRootStorage("/data/data/com.just.mobile/");
			CMain::SetBaseStorage("/storage/emulated/0/JUST/");
			CMain::SetCustomStorage("gui");
			CMain::SetCryptStorage("ru.stepdev.crimemobile");
			
			//g_pszStorage = CMain::GetBaseStorage();
			g_pszStorage = (const char*)(SA_ADDR(0x63C4B8));
			
			CollisionPatchesUsed = true;
			m_bChatStarted = false;
			
			col_all1 = 0xFF;
			col_all2 = 0xD7;
			col_all3 = 0x00;
			break;
		}
		case ServerCrime:
		{
			//CMain::SetRootStorage("/data/data/com.crimelauncher/");
			CMain::SetRootStorage("/data/data/ru.stepdev.crimemobile/");
			//CMain::SetBaseStorage("/storage/emulated/0/CrimeRp/");
			CMain::SetBaseStorage("/storage/emulated/0/CrimeRp/");
			CMain::SetCustomStorage("samp");
			CMain::SetCryptStorage("ru.stepdev.crimemobile");
			
			g_pszStorage = CMain::GetBaseStorage();
			//g_pszStorage = (const char*)(SA_ADDR(0x63C4B8));
			
			CollisionPatchesUsed = false;
			m_bChatStarted = false;
			
			m_PortVoice = 2;
			
			col_all1 = 0xFF;
			col_all2 = 0xD7;
			col_all3 = 0x00;
			break;
		}
		case ServerBoost:
		{
			//CMain::SetRootStorage("/data/data/com.crimelauncher/");
			CMain::SetRootStorage("/data/data/com.rockstargames.boost/");
			CMain::SetBaseStorage("/storage/emulated/0/Boost/");
			CMain::SetCustomStorage("samp");
			CMain::SetCryptStorage("ru.stepdev.crimemobile");
			
			g_pszStorage = CMain::GetBaseStorage();
			//g_pszStorage = (const char*)(SA_ADDR(0x63C4B8));
			
			CollisionPatchesUsed = true;
			m_bChatStarted = false;
			
			m_PortVoice = 2;
			
			col_all1 = 0x45;
			col_all2 = 0x82;
			col_all3 = 0xA1;
			break;
		}
	}

	Log(OBFUSCATE("SAMP library loaded! Build time: " __DATE__ " " __TIME__));

	//g_libGTASA = FindLibrary(OBFUSCATE("libGTASA.so"));
	auto dlllll = dlopen("libGTASA.so", RTLD_NOLOAD);
    uintptr_t ttt = (uintptr_t) dlsym(dlllll, "_ZN17CVehicleModelInfo21ms_vehicleColourTableE");

    g_libGTASA = ttt - 0x00890108;
	if(g_libGTASA == 0)
	{
		Log(OBFUSCATE("ERROR: libGTASA.so address not found!"));
		return 0;
	}

	Log(OBFUSCATE("libGTASA.so image base address: 0x%X"), g_libGTASA);

	firebase::crashlytics::Initialize();

	uintptr_t libgtasa = FindLibrary(OBFUSCATE("libGTASA.so"));
	uintptr_t libsamp = FindLibrary(OBFUSCATE("libsamp.so"));
	uintptr_t libc = FindLibrary(OBFUSCATE("libc.so"));

	Log(OBFUSCATE("libGTASA.so: 0x%x"), libgtasa);
	Log(OBFUSCATE("libsamp.so: 0x%x"), libsamp);
	Log(OBFUSCATE("libc.so: 0x%x"), libc);

	char str[100];

	sprintf(str, "0x%x", libgtasa);
	firebase::crashlytics::SetCustomKey(OBFUSCATE("libGTASA.so"), str);
	
	sprintf(str, "0x%x", libsamp);
	firebase::crashlytics::SetCustomKey(OBFUSCATE("libsamp.so"), str);

	sprintf(str, "0x%x", libc);
	firebase::crashlytics::SetCustomKey(OBFUSCATE("libc.so"), str);

	srand(time(0));

	InitHookStuff();

	InitRenderWareFunctions();
	InstallSpecialHooks();

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

	return JNI_VERSION_1_6;
}

void Log(const char *fmt, ...)
{	
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, OBFUSCATE("%sSAMP/samp_log.txt"), g_pszStorage);
		flLog = fopen(buffer, OBFUSCATE("w"));
	}
	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	firebase::crashlytics::Log(buffer);

	if(flLog == nullptr) return;
	fprintf(flLog, OBFUSCATE("%s\n"), buffer);
	fflush(flLog);

	__android_log_write(ANDROID_LOG_INFO, OBFUSCATE("AXL"), buffer);
}

void CrashLog(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if (flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, OBFUSCATE("%sSAMP/crash_log.log"), g_pszStorage);
		flLog = fopen(buffer, OBFUSCATE("a"));
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, OBFUSCATE("AXL"), buffer);

	firebase::crashlytics::Log(buffer);

	if (flLog == nullptr) return;
	fprintf(flLog, OBFUSCATE("%s\n"), buffer);
	fflush(flLog);
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);

	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
