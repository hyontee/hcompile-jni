#include "entry.h"
#include "xorstr.h"

JavaVM* g_jvm = nullptr;
jclass g_jsonTransportClass = nullptr;
jmethodID g_onJsonDataMethod = nullptr;

extern "C"
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    g_jvm = vm;
    
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    
    CApp::Initialise(eAppInit::APP_INIT_OFFSETS);
    
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
    
    return JNI_VERSION_1_6;
}

extern RakClientInterface* pRakClient;

void* hack_thread(void* args)
{
	while(!CGameAPI::GetBase()) {}
	while(*(int *)(CGameAPI::GetBase(xorstr("RwInitialised"))) == 0) {}
	
	CApp::Initialise(eAppInit::APP_INIT_RW);
	
	RegisterRPCs(pRakClient);
	DobbyHook((void *)(CGameAPI::GetBase(xorstr("CNetGame::ProcessNetwork"))), (void *)&hook_CNetGame__ProcessNetwork, (void **)&orig_CNetGame__ProcessNetwork);

	uintptr_t ng_pRakClient = *(uintptr_t *)(CGameAPI::GetBase(xorstr("CNetGame::m_pRakClient")));
	while(!ng_pRakClient) {
		ng_pRakClient = *(uintptr_t *)(CGameAPI::GetBase(xorstr("CNetGame::m_pRakClient")));
	}

	#ifdef __arm__
	MSHookFunction((void *)( *(uintptr_t *)(*(uintptr_t *)ng_pRakClient + 8) ), (void *)&hook_RakClient__Connect, (void **)&orig_RakClient__Connect);
	MSHookFunction((void *)( *(uintptr_t *)(*(uintptr_t *)ng_pRakClient + 32) ), (void *)&hook_RakClient__Send, (void **)&orig_RakClient__Send);
	MSHookFunction((void *)( *(uintptr_t *)(*(uintptr_t *)ng_pRakClient + 108) ), (void *)&hook_RakClient__RPC, (void **)&orig_RakClient__RPC);
	#endif

	#ifdef __aarch64__
	DobbyHook((void *)( *(uintptr_t *)(*(uintptr_t *)ng_pRakClient + 16) ), (void *)&hook_RakClient__Connect, (void **)&orig_RakClient__Connect);
	DobbyHook((void *)( *(uintptr_t *)(*(uintptr_t *)ng_pRakClient + 64) ), (void *)&hook_RakClient__Send, (void **)&orig_RakClient__Send);
	DobbyHook((void *)( *(uintptr_t *)(*(uintptr_t *)ng_pRakClient + 216) ), (void *)&hook_RakClient__RPC, (void **)&orig_RakClient__RPC);

    // Crash-guard: libblackrussia-client.so+0x245de0 -> crash read at +0x245e18 ([x8 + 1248], x8 = [thiz]).
    DobbyHook((void *)(CGameAPI::GetBase() + 0x245DE0), (void *)&hook_CGame__TickCrashPoint, (void **)&orig_CGame__TickCrashPoint);
    // Crash-guard: libblackrussia-client.so+0x2040f4 null deref in local state tick path.
    DobbyHook((void *)(CGameAPI::GetBase() + 0x203FD0), (void *)&hook_CGame__LocalStateTick, (void **)&orig_CGame__LocalStateTick);
	#endif
	
	pthread_exit(nullptr);
    return nullptr;
}
