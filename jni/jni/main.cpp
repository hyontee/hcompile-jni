//
// Created by gen1z on 02/03/2024.
//

#include <string.h>
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include "main.h"
#include "util/armhook.h"
#include "scripting.h"
#include "util/patch.h"
#include "util/CJavaWrapper.h"

uintptr_t g_libBR;
uintptr_t g_libGTASA;
uintptr_t* g_CNetGame;
uintptr_t g_iGameState;

void LOG(const char* fmt, ...){
    char buffer[10000];
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    __android_log_write(ANDROID_LOG_INFO, OBFUSCATE("BR-NARKOTA"), buffer);
}

void LOGE(const char* fmt, ...){
    char buffer[10000];
    static const char* g_pszStorage = OBFUSCATE("/storage/emulated/0/Android/data/blackrussia.online/files/crash_log.log");
    static FILE* flLog = nullptr;

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%s", g_pszStorage);
        flLog = fopen(buffer, "a");
    }
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    __android_log_write(ANDROID_LOG_INFO, OBFUSCATE("BR-CRASH-LOG"), buffer);

    if(g_pJavaWrapper)
        g_pJavaWrapper->CrashLog(buffer);

    if(flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);
}

void LOGBR(const char* fmt, ...){
    char buffer[10000];
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    __android_log_write(ANDROID_LOG_INFO, OBFUSCATE("BR-ORIG"), buffer);
}


void LogToFile(char* tag, const char *fmt, ...)
{
    char buffer[10000];
    static FILE* flLog = nullptr;
    static const char* g_pszStorage = OBFUSCATE("/storage/emulated/0/BR/SAMP/shader_log.txt");

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%s", g_pszStorage);
        flLog = fopen(buffer, "a");
    }
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);
    if(flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);

    __android_log_write(ANDROID_LOG_INFO, tag, buffer);

    return;
}

uintptr_t* (*sub)(uintptr_t *, char*, int, char*, char*);
uintptr_t* sub_hook(uintptr_t * a1, char* a2, int a3, char* a4, char* a5){
    LOG(OBFUSCATE("3"));
    LOG(OBFUSCATE("IP: %s"), a2);
    LOG(OBFUSCATE("PORT: %i"), a3);
    LOG(OBFUSCATE("SERV PASS: %s"), a5);
    uintptr_t* i = a1;

    sub(a1,a2,a3,a4,a5);

    return i;
}

int sub_77A08(uintptr_t *a1);
void (*CNetGame__UpdateNetowrk)(uintptr_t *a1);
void CNetGame__UpdateNetowrk_hook(uintptr_t *a1)
{
    *(RakClientInterface**)(a1) = CNetGame::Instance()->m_rakClientInterface;
    sub_77A08(a1);
    return;
}

unsigned int GetTickCount();
uintptr_t* (*CNetGame__CNetGame)(uintptr_t *a1, const char *a2, int a3, const char *a4, int a5);
uintptr_t* CNetGame__CNetGame_hook(uintptr_t *a1, const char *a2, int a3, const char *a4, int a5)
{
    CNetGame* pNetGame = CNetGame::Instance();
    pNetGame->DbgConnect((char*)a4);

    uintptr_t * i =CNetGame__CNetGame(a1, a2, a3 ,a4, a5); 
    return i;
}

void (*CNetGame__Process)(uintptr_t *a1);
void CNetGame__Process_hook(uintptr_t *a1)
{
    *a1 = *(uintptr_t*)CNetGame::Instance()->m_rakClientInterface;

    CNetGame__Process(a1);
    return;
}

int (*sub_2BA38)();
int sub_2BA38_hook()
{
    return GetTickCount();
}

extern void InitGame(RPCParameters *rpcParams);
int (*sub_7BB04)(uintptr_t*);
int sub_7BB04_hook(uintptr_t* a1)
{
    return 0;
}

uintptr_t* (*sub_9F48A)(uintptr_t*);
uintptr_t* sub_9F48A_hook(uintptr_t* a1)
{

    return a1;
}

void (*sub_2A548)();
void sub_2A548_hook()
{
    ((uintptr_t(*)())(g_libBR + 0x2A2F8 + 1))();
    if ( CNetGame::Instance()->m_szNickName )
        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x78124 + 1))(*(uintptr_t *)(g_libBR + 0x205338));
    return;
}

void (*sub_7DE34)(uintptr_t*);
void sub_7DE34_hook(uintptr_t* a1)
{
    return;
}

void (*sub_883D0)(uintptr_t*);
void sub_883D0_hook(uintptr_t* a1)
{
    return;
}

uintptr_t* (*sub_A8660)(uintptr_t*);
uintptr_t* sub_A8660_hook(uintptr_t* a1)
{
    a1 = (uintptr_t*)CNetGame::Instance()->m_rakClientInterface;
    return a1;
}

void (*Render2dStuff)();
void Render2dStuff_hook(){

    static bool once = false;

    if (!once){

        CHook::InlineHook(g_libBR, 0x077A08, (uintptr_t)CNetGame__UpdateNetowrk_hook, (uintptr_t*)&CNetGame__UpdateNetowrk);
        CHook::InlineHook(g_libBR, 0x07706C, (uintptr_t)CNetGame__CNetGame_hook, (uintptr_t*)&CNetGame__CNetGame);
        CHook::InlineHook(g_libBR, 0x07DE34, (uintptr_t)sub_7DE34_hook, (uintptr_t*)&sub_7DE34);
        CHook::InlineHook(g_libBR, 0x0883D0, (uintptr_t)sub_883D0_hook, (uintptr_t*)&sub_883D0);

        once = true;
    }
    return Render2dStuff();
}

int (*compileShadersBR)(int a1, const char* a2, const char* a3, int a4);
int compileShadersBR_hook(int a1, const char* a2, const char* a3, int a4){
    LogToFile("BR-SHADERS", "%s\n%s", a2, a3);
    return compileShadersBR(a1, a2, a3, a4);
}

extern "C"
{
JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initNARKOTA(JNIEnv* pEnv, jobject thiz)
{
    static bool once = false;
    if(!once){
        LOG(OBFUSCATE("1"));
        g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);
        once = true;
    }
}
}

void (*nullsub_2)(const char *a1);
void nullsub_2_hook(const char *a1)
{
    LOGBR(a1);
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
        LOGE(OBFUSCATE("Crashed - 1. %d"), crashId);
        LOGE(OBFUSCATE(" "));
        LOGE(OBFUSCATE("ID: %d"), crashId);
        LOGE(OBFUSCATE("SIGBUS | Fault address: 0x%X"), info->si_addr);
        LOGE(OBFUSCATE("libGTASA base address: 0x%X"), g_libGTASA);
        LOGE(OBFUSCATE("libsamp base address: 0x%X"), FindLibrary("libsamp.so"));
        LOGE(OBFUSCATE("libc base address: 0x%X"), FindLibrary("libc.so"));
        LOGE(OBFUSCATE("register states:"));
        LOGE(OBFUSCATE("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X"),
            context->uc_mcontext.arm_r0,
            context->uc_mcontext.arm_r1,
            context->uc_mcontext.arm_r2,
            context->uc_mcontext.arm_r3);
        LOGE(OBFUSCATE("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x"),
            context->uc_mcontext.arm_r4,
            context->uc_mcontext.arm_r5,
            context->uc_mcontext.arm_r6,
            context->uc_mcontext.arm_r7);
        LOGE(OBFUSCATE("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x"),
            context->uc_mcontext.arm_r8,
            context->uc_mcontext.arm_r9,
            context->uc_mcontext.arm_r10,
            context->uc_mcontext.arm_fp);
        LOGE(OBFUSCATE("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x"),
            context->uc_mcontext.arm_ip,
            context->uc_mcontext.arm_sp,
            context->uc_mcontext.arm_lr,
            context->uc_mcontext.arm_pc);

        LOGE(OBFUSCATE("backtrace:"));
        LOGE(OBFUSCATE("1: libGTASA.so + 0x%X"), context->uc_mcontext.arm_pc - g_libGTASA);
        LOGE(OBFUSCATE("2: libGTASA.so + 0x%X"), context->uc_mcontext.arm_lr - g_libGTASA);

        LOGE(OBFUSCATE("1: libsamp.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
        LOGE(OBFUSCATE("2: libsamp.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

        LOGE(OBFUSCATE("1: libNARKOTA.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libNARKOTA.so"));
        LOGE(OBFUSCATE("2: libNARKOTA.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libNARKOTA.so"));


        LOGE(OBFUSCATE("1: libc.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
        LOGE(OBFUSCATE("2: libc.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libc.so"));

        //DumpLibraries();

        exit(0);
    }

    return;
}

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
        LOGE(OBFUSCATE("Crashed - 2. %d"), crashId);
        LOGE(OBFUSCATE(" "));
        LOGE(OBFUSCATE("ID: %d"), crashId);
        LOGE(OBFUSCATE("SIGSEGV | Fault address: 0x%X"), info->si_addr);
        LOGE(OBFUSCATE("libGTASA base address: 0x%X"), g_libGTASA);
        LOGE(OBFUSCATE("libsamp base address: 0x%X"), FindLibrary("libsamp.so"));
        LOGE(OBFUSCATE("libc base address: 0x%X"), FindLibrary("libc.so"));
        LOGE(OBFUSCATE("register states:"));
        LOGE(OBFUSCATE("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%x"),
            context->uc_mcontext.arm_r0,
            context->uc_mcontext.arm_r1,
            context->uc_mcontext.arm_r2,
            context->uc_mcontext.arm_r3);
        LOGE(OBFUSCATE("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x"),
            context->uc_mcontext.arm_r4,
            context->uc_mcontext.arm_r5,
            context->uc_mcontext.arm_r6,
            context->uc_mcontext.arm_r7);
        LOGE(OBFUSCATE("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x"),
            context->uc_mcontext.arm_r8,
            context->uc_mcontext.arm_r9,
            context->uc_mcontext.arm_r10,
            context->uc_mcontext.arm_fp);
        LOGE(OBFUSCATE("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x"),
            context->uc_mcontext.arm_ip,
            context->uc_mcontext.arm_sp,
            context->uc_mcontext.arm_lr,
            context->uc_mcontext.arm_pc);

        LOGE(OBFUSCATE("backtrace:"));
        LOGE(OBFUSCATE("1: libGTASA.so + 0x%X"), context->uc_mcontext.arm_pc - g_libGTASA);
        LOGE(OBFUSCATE("2: libGTASA.so + 0x%X"), context->uc_mcontext.arm_lr - g_libGTASA);

        LOGE(OBFUSCATE("1: libsamp.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
        LOGE(OBFUSCATE("2: libsamp.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

        LOGE(OBFUSCATE("1: libNARKOTA.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libNARKOTA.so"));
        LOGE(OBFUSCATE("2: libNARKOTA.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libNARKOTA.so"));

        LOGE(OBFUSCATE("1: libc.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
        LOGE(OBFUSCATE("2: libc.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libc.so"));
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
        LOGE(OBFUSCATE("Crashed - 3. %d"), crashId);
        LOGE(OBFUSCATE(" "));
        LOGE(OBFUSCATE("ID: %d"), crashId);
        LOGE(OBFUSCATE("SIGFPE | Fault address: 0x%X"), info->si_addr);
        LOGE(OBFUSCATE("libGTASA base address: 0x%X"), g_libGTASA);
        LOGE(OBFUSCATE("libsamp base address: 0x%X"), FindLibrary("libsamp.so"));
        LOGE(OBFUSCATE("libc base address: 0x%X"), FindLibrary("libc.so"));
        LOGE(OBFUSCATE("register states:"));
        LOGE(OBFUSCATE("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%x"),
            context->uc_mcontext.arm_r0,
            context->uc_mcontext.arm_r1,
            context->uc_mcontext.arm_r2,
            context->uc_mcontext.arm_r3);
        LOGE(OBFUSCATE("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x"),
            context->uc_mcontext.arm_r4,
            context->uc_mcontext.arm_r5,
            context->uc_mcontext.arm_r6,
            context->uc_mcontext.arm_r7);
        LOGE(OBFUSCATE("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x"),
            context->uc_mcontext.arm_r8,
            context->uc_mcontext.arm_r9,
            context->uc_mcontext.arm_r10,
            context->uc_mcontext.arm_fp);
        LOGE(OBFUSCATE("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x"),
            context->uc_mcontext.arm_ip,
            context->uc_mcontext.arm_sp,
            context->uc_mcontext.arm_lr,
            context->uc_mcontext.arm_pc);

        LOGE(OBFUSCATE("backtrace:"));
        LOGE(OBFUSCATE("1: libGTASA.so + 0x%X"), context->uc_mcontext.arm_pc - g_libGTASA);
        LOGE(OBFUSCATE("2: libGTASA.so + 0x%X"), context->uc_mcontext.arm_lr - g_libGTASA);

        LOGE(OBFUSCATE("1: libsamp.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
        LOGE(OBFUSCATE("2: libsamp.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

        LOGE(OBFUSCATE("1: libNARKOTA.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libNARKOTA.so"));
        LOGE(OBFUSCATE("2: libNARKOTA.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libNARKOTA.so"));


        LOGE(OBFUSCATE("1: libc.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
        LOGE(OBFUSCATE("2: libc.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libc.so"));

        //DumpLibraries();

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
        LOGE(OBFUSCATE("Crashed - 4. %d"), crashId);
        LOGE(OBFUSCATE(" "));
        LOGE(OBFUSCATE("ID: %d"), crashId);
        LOGE(OBFUSCATE("SIGABRT | Fault address: 0x%X"), info->si_addr);
        LOGE(OBFUSCATE("libGTASA base address: 0x%X"), g_libGTASA);
        LOGE(OBFUSCATE("libsamp base address: 0x%X"), FindLibrary("libsamp.so"));
        LOGE(OBFUSCATE("libc base address: 0x%X"), FindLibrary("libc.so"));
        LOGE(OBFUSCATE("register states:"));
        LOGE(OBFUSCATE("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%x"),
            context->uc_mcontext.arm_r0,
            context->uc_mcontext.arm_r1,
            context->uc_mcontext.arm_r2,
            context->uc_mcontext.arm_r3);
        LOGE(OBFUSCATE("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x"),
            context->uc_mcontext.arm_r4,
            context->uc_mcontext.arm_r5,
            context->uc_mcontext.arm_r6,
            context->uc_mcontext.arm_r7);
        LOGE(OBFUSCATE("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x"),
            context->uc_mcontext.arm_r8,
            context->uc_mcontext.arm_r9,
            context->uc_mcontext.arm_r10,
            context->uc_mcontext.arm_fp);
        LOGE(OBFUSCATE("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x"),
            context->uc_mcontext.arm_ip,
            context->uc_mcontext.arm_sp,
            context->uc_mcontext.arm_lr,
            context->uc_mcontext.arm_pc);

        LOGE(OBFUSCATE("backtrace:"));
        LOGE(OBFUSCATE("1: libGTASA.so + 0x%X"), context->uc_mcontext.arm_pc - g_libGTASA);
        LOGE(OBFUSCATE("2: libGTASA.so + 0x%X"), context->uc_mcontext.arm_lr - g_libGTASA);


        LOGE(OBFUSCATE("1: libsamp.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
        LOGE(OBFUSCATE("2: libsamp.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

        LOGE(OBFUSCATE("1: libNARKOTA.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libNARKOTA.so"));
        LOGE(OBFUSCATE("2: libNARKOTA.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libNARKOTA.so"));

        LOGE(OBFUSCATE("1: libc.so + 0x%X"), context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
        LOGE(OBFUSCATE("2: libc.so + 0x%X"), context->uc_mcontext.arm_lr - FindLibrary("libc.so"));
        exit(0);
    }

    return;
}

void (*sub_410F4)(uintptr_t *a1, float a2);
void sub_410F4_hook(uintptr_t *a1, float a2){
    return;
}

void (*CChatwindow__ProcessCommands)(char *haystack);
void CChatwindow__ProcessCommands_hook(char *haystack)
{
    return CChatwindow__ProcessCommands(haystack);
}

void InitHookStuff();
void InitScripting();
void InitThread(void* a1);
extern "C" JavaVM* javaVM = NULL;
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_libBR = FindLibrary(OBFUSCATE("libsamp.so"));
    g_libGTASA = FindLibrary(OBFUSCATE("libGTASA.so"));
    javaVM = vm;
    InitScripting();
    InitHookStuff();
    CHook::InlineHook(g_libBR, 0x029AE8, (uintptr_t)nullsub_2_hook, (uintptr_t*)&nullsub_2);
    CHook::InlineHook(g_libBR, 0x04541C, (uintptr_t)Render2dStuff_hook, (uintptr_t*)&Render2dStuff);
    CHook::InlineHook(g_libBR, 0x06EFDC, (uintptr_t)compileShadersBR_hook, (uintptr_t*)&compileShadersBR);

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


void InitThread(void* a1){

}

extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_sendJsonData2(JNIEnv *env, jobject thiz, jint guiid, int type) {

    CNetGame* pNetGame = CNetGame::Instance();

    switch(type) {
        case 398:
            pNetGame->SendChatCommand(OBFUSCATE("/gps"));
            break;
        case 1:
            break;
        case 2:
            pNetGame->SendChatCommand(OBFUSCATE("/mn"));
            break;
        case 4:
            pNetGame->SendChatCommand(OBFUSCATE("/inv"));
            break;
        case 5: {
            pNetGame->SendChatCommand(OBFUSCATE("/anim"));
            pNetGame->SendChatCommand(OBFUSCATE("/anim"));
            break;
        }
        case 6:
            pNetGame->SendChatCommand(OBFUSCATE("/donate"));
            break;
        case 7:
            pNetGame->SendChatCommand(OBFUSCATE("/car"));
            break;
        case 8:
            pNetGame->SendChatCommand(OBFUSCATE("/"));
            break;
    }
}