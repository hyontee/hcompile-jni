#include "entry.h"
#include "xorstr.h"

namespace { constexpr int kInitAttempts = 1000; }

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
    (void)args;

    bool gameReady = false;
    for (int i = 0; i < kInitAttempts; ++i)
    {
        if (CGameAPI::GetBase() != 0)
        {
            gameReady = true;
            break;
        }
        usleep(10 * 1000);
    }
    if (!gameReady)
        return nullptr;

    const uintptr_t rwInitialised = CGameAPI::GetBase(xorstr("RwInitialised"));
    if (!rwInitialised)
        return nullptr;

    bool rwReady = false;
    for (int i = 0; i < kInitAttempts; ++i)
    {
        if (*reinterpret_cast<int*>(rwInitialised) != 0)
        {
            rwReady = true;
            break;
        }
        usleep(10 * 1000);
    }
    if (!rwReady)
        return nullptr;

    CApp::Initialise(eAppInit::APP_INIT_RW);
    RegisterRPCs(pRakClient);

    const uintptr_t processNetwork = CGameAPI::GetBase(xorstr("CNetGame::ProcessNetwork"));
    const uintptr_t addDebugMessage = CGameAPI::GetBase(xorstr("CChat::AddDebugMessage"));
    const uintptr_t rakClientMember = CGameAPI::GetBase(xorstr("CNetGame::m_pRakClient"));
    if (!processNetwork || !addDebugMessage || !rakClientMember)
        return nullptr;

    DobbyHook(reinterpret_cast<void*>(processNetwork), reinterpret_cast<void*>(&hook_CNetGame__ProcessNetwork),
              reinterpret_cast<void**>(&orig_CNetGame__ProcessNetwork));
    DobbyHook(reinterpret_cast<void*>(addDebugMessage), reinterpret_cast<void*>(&hook_CChat__AddDebugMessage),
              reinterpret_cast<void**>(&orig_CChat__AddDebugMessage));

    uintptr_t ng_pRakClient = *reinterpret_cast<uintptr_t*>(rakClientMember);
    for (int i = 0; i < kInitAttempts && !ng_pRakClient; ++i)
    {
        usleep(10 * 1000);
        ng_pRakClient = *reinterpret_cast<uintptr_t*>(rakClientMember);
    }
    if (!ng_pRakClient)
        return nullptr;

    const uintptr_t vtable = *reinterpret_cast<uintptr_t*>(ng_pRakClient);
    if (!vtable)
        return nullptr;

#ifdef __arm__
    const uintptr_t connectHook = *reinterpret_cast<uintptr_t*>(vtable + 8);
    const uintptr_t sendHook = *reinterpret_cast<uintptr_t*>(vtable + 32);
    const uintptr_t rpcHook = *reinterpret_cast<uintptr_t*>(vtable + 108);
    if (!connectHook || !sendHook || !rpcHook)
        return nullptr;

    MSHookFunction(reinterpret_cast<void*>(connectHook), reinterpret_cast<void*>(&hook_RakClient__Connect),
                   reinterpret_cast<void**>(&orig_RakClient__Connect));
    MSHookFunction(reinterpret_cast<void*>(sendHook), reinterpret_cast<void*>(&hook_RakClient__Send),
                   reinterpret_cast<void**>(&orig_RakClient__Send));
    MSHookFunction(reinterpret_cast<void*>(rpcHook), reinterpret_cast<void*>(&hook_RakClient__RPC),
                   reinterpret_cast<void**>(&orig_RakClient__RPC));
#endif

#ifdef __aarch64__
    const uintptr_t connectHook = *reinterpret_cast<uintptr_t*>(vtable + 16);
    const uintptr_t sendHook = *reinterpret_cast<uintptr_t*>(vtable + 64);
    const uintptr_t rpcHook = *reinterpret_cast<uintptr_t*>(vtable + 216);
    if (!connectHook || !sendHook || !rpcHook)
        return nullptr;

    DobbyHook(reinterpret_cast<void*>(connectHook), reinterpret_cast<void*>(&hook_RakClient__Connect),
              reinterpret_cast<void**>(&orig_RakClient__Connect));
    DobbyHook(reinterpret_cast<void*>(sendHook), reinterpret_cast<void*>(&hook_RakClient__Send),
              reinterpret_cast<void**>(&orig_RakClient__Send));
    DobbyHook(reinterpret_cast<void*>(rpcHook), reinterpret_cast<void*>(&hook_RakClient__RPC),
              reinterpret_cast<void**>(&orig_RakClient__RPC));
#endif

    return nullptr;
}

