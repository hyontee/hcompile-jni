#include "main.h"
#include "jniutil.h"
#include <stdio.h>

uintptr_t g_libGTASA = 0;
uintptr_t g_libSAMP = 0;
const char* g_pAPKPackage;

jobject appContext;
JavaVM *mVm;
JNIEnv *mEnv;

//#define VERSION_2
#define BR

#ifdef VERSION_2
RakClientInterface* myInterface;

RakClientInterface* (*old_GetRakClientInterface)();
RakClientInterface* GetRakClientInterface_hook()
{
    myInterface = RakNetworkFactory::GetRakClientInterface();
    myInterface->Connect(OBFUSCATE("188.127.241.74"), 1972, 0, 0, 5);
    return myInterface;
}

#endif

#ifdef BR
void (*CNetGame__CNetGame)(char*, const char*, int, const char*, int);
void CNetGame__CNetGame_hook(char* a1, const char* a2, int a3, const char* a4, int a5)
{
    a2 = OBFUSCATE("65.108.99.34");
    a3 = 1505;
    CNetGame__CNetGame(a1, a2, a3, a4, a5);
}

void (*a)(uintptr_t*);
void a_hook(uintptr_t* a)
{
    return;
}

void (*b)(uintptr_t*);
void b_hook(uintptr_t* b)
{
    return;
}
#endif
void initSamp() 
{
#ifdef VERSION_2
    CHook::InlineHook(g_libSAMP, 0x075B10, (uintptr_t)GetRakClientInterface_hook, (uintptr_t*)&old_GetRakClientInterface);
#endif
#ifdef BR
    CHook::InlineHook(g_libSAMP, 0x, (uintptr_t)CNetGame__CNetGame_hook, (uintptr_t*)&CNetGame__CNetGame);
    CHook::InlineHook(g_libSAMP, 0x07DE34, (uintptr_t)a_hook, (uintptr_t*)&a);	
    CHook::InlineHook(g_libSAMP, 0x0883D0, (uintptr_t)b_hook, (uintptr_t*)&b);	
#endif
        
}

void Main() 
{
     // code for GTASA
}

void *InitialiseThread(void *p)
{
	Main();
	pthread_exit(0);
}          

JNIEnv *getEnv() 
{
	JNIEnv* env = nullptr;
    JavaVM* javaVM = mVm;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

	if (getEnvStat == JNI_EDETACHED)
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		  return nullptr;

	if (getEnvStat == JNI_EVERSION)
	    return nullptr;

	if (getEnvStat == JNI_ERR)
	   return nullptr;

	return env;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    mVm = vm;
    mEnv = getEnv();

	appContext = GetGlobalActivity(mEnv);
    
	g_libGTASA = ARMHook::getLibraryAddress(OBFUSCATE("libGTASA.so"));
	if(g_libGTASA)
	{
		srand(time(0));

		uintptr_t memlib_start = (g_libGTASA + 0x174D4);
		uintptr_t size = 0x1234A;
		
		ARMHook::InitialiseTrampolines(memlib_start, size);

		pthread_t thread;
		pthread_create(&thread, 0, InitialiseThread, 0);
	}

    g_libSAMP = FindLibrary(OBFUSCATE("libsamp.so"));
    if(g_libSAMP) initSamp();

	return JNI_VERSION_1_6;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);

	return (tv.tv_sec*1000 + tv.tv_usec/1000);
}
