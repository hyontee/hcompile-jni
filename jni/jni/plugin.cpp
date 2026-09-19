#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <link.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <atomic>
#include <thread>
#include <chrono>
#include "dobby.h"

#define LOG_TAG "ServerRedirect"
#define LOGI(...) do { __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__); file_log("I: " __VA_ARGS__); } while (0)
#define LOGE(...) do { __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__); file_log("E: " __VA_ARGS__); } while (0)

static constexpr const char* TARGET_IP = "185.207.214.14";
static constexpr uint16_t TARGET_PORT = 3713;
static constexpr const char* CLIENT_SO = "libblackrussia-client.so";

#if defined(__aarch64__)
static constexpr uintptr_t GOT_SET_HOST = 0x1d9ca98;
static constexpr uintptr_t GOT_HOST_CONNECT = 0x1d9caa0;
static constexpr uintptr_t WRAPPER = 0x171197c;
#elif defined(__arm__)
static constexpr uintptr_t GOT_SET_HOST = 0x15789b8;
static constexpr uintptr_t GOT_HOST_CONNECT = 0x15789bc;
#endif

struct ENetAddressCompat { uint32_t host; uint16_t port; uint16_t reserved; };
struct ENetHostCompat;
struct ENetPeerCompat;
using SetHostFn = int (*)(ENetAddressCompat*, const char*);
using HostConnectFn = ENetPeerCompat* (*)(ENetHostCompat*, const ENetAddressCompat*, size_t, uint32_t);
using WrapperFn = int (*)(void*, const char*, uint16_t, uint32_t);
using SocketConnectFn = int (*)(int, const struct sockaddr*, socklen_t);
using SetHostIpFn = int (*)(ENetAddressCompat*, const char*);

static SetHostFn g_set_host = nullptr;
static SetHostIpFn g_set_host_ip = nullptr;
static HostConnectFn g_host_connect = nullptr;
static SocketConnectFn g_socket_connect = nullptr;
static WrapperFn g_wrapper = nullptr;
static std::atomic<bool> g_installed{false};
static std::atomic<bool> g_started{false};
static FILE* g_log = nullptr;

static void file_log(const char* fmt, ...) {
    static std::atomic_flag lock = ATOMIC_FLAG_INIT;
    while (lock.test_and_set(std::memory_order_acquire)) usleep(1000);
    if (!g_log) {
        g_log = fopen("/sdcard/ServerRedirect.log", "a");
        if (!g_log) g_log = fopen("/sdcard/Download/ServerRedirect.log", "a");
    }
    if (g_log) {
        va_list ap; va_start(ap, fmt); vfprintf(g_log, fmt, ap); va_end(ap);
        fputc('\n', g_log); fflush(g_log);
    }
    lock.clear(std::memory_order_release);
}

static uintptr_t module_base(const char* name) {
    struct Ctx { const char* name; uintptr_t base; } ctx{name, 0};
    dl_iterate_phdr([](struct dl_phdr_info* info, size_t, void* arg)->int {
        auto* c = static_cast<Ctx*>(arg);
        if (info->dlpi_name && strstr(info->dlpi_name, c->name)) {
            c->base = static_cast<uintptr_t>(info->dlpi_addr); return 1;
        }
        return 0;
    }, &ctx);
    return ctx.base;
}

static void* module_handle() {
    void* h = dlopen(CLIENT_SO, RTLD_NOW | RTLD_NOLOAD);
    return h ? h : dlopen(CLIENT_SO, RTLD_NOW);
}

static bool writable(void* p) {
    long ps = sysconf(_SC_PAGESIZE); if (ps <= 0) return false;
    uintptr_t a = reinterpret_cast<uintptr_t>(p);
    uintptr_t page = a & ~(static_cast<uintptr_t>(ps)-1);
    return mprotect(reinterpret_cast<void*>(page), (size_t)ps, PROT_READ|PROT_WRITE) == 0;
}

template<class T> static bool patch_slot(uintptr_t slot, T repl, T& orig) {
    if (!writable(reinterpret_cast<void*>(slot))) return false;
    auto* p = reinterpret_cast<uintptr_t*>(slot);
    orig = reinterpret_cast<T>(*p);
    if (!orig) return false;
    __atomic_store_n(p, reinterpret_cast<uintptr_t>(repl), __ATOMIC_SEQ_CST);
    __builtin___clear_cache(reinterpret_cast<char*>(slot), reinterpret_cast<char*>(slot)+sizeof(uintptr_t));
    return true;
}

static void force_target(ENetAddressCompat* a) {
    if (!a) return;
    in_addr ip{};
    if (inet_pton(AF_INET, TARGET_IP, &ip) == 1) {
        a->host = ip.s_addr; // ENet host = network byte order
        a->port = TARGET_PORT; // ENet port = host byte order
    }
}

static void describe(const ENetAddressCompat* a, char* out, size_t n) {
    if (!a) { snprintf(out,n,"<null>"); return; }
    char ip[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &a->host, ip, sizeof(ip));
    snprintf(out,n,"%s:%u", ip[0]?ip:"?", (unsigned)a->port);
}

static int hk_set_host(ENetAddressCompat* a, const char* name) {
    int rc = g_set_host ? g_set_host(a, name) : -1;
    char before[64]; describe(a,before,sizeof(before));
    force_target(a);
    LOGI("enet_address_set_host(%s): %s -> %s:%u", name?name:"<null>", before, TARGET_IP, TARGET_PORT);
    return rc;
}

static int hk_set_host_ip(ENetAddressCompat* a, const char* name) {
    int rc = g_set_host_ip ? g_set_host_ip(a, name) : -1;
    char before[64]; describe(a,before,sizeof(before));
    force_target(a);
    LOGI("enet_address_set_host_ip(%s): %s -> %s:%u", name?name:"<null>", before, TARGET_IP, TARGET_PORT);
    return rc;
}

static ENetPeerCompat* hk_host_connect(ENetHostCompat* host, const ENetAddressCompat* a, size_t channels, uint32_t data) {
    if (!g_host_connect) return nullptr;
    ENetAddressCompat x{}; if (a) x=*a;
    char before[64]; describe(a,before,sizeof(before));
    force_target(&x);
    LOGI("enet_host_connect: %s -> %s:%u channels=%zu data=%u", before, TARGET_IP, TARGET_PORT, channels, data);
    return g_host_connect(host,&x,channels,data);
}

static int hk_wrapper(void* self, const char* host, uint16_t port, uint32_t data) {
    LOGI("game connect wrapper: %s:%u -> %s:%u", host?host:"<null>",(unsigned)port,TARGET_IP,TARGET_PORT);
    return g_wrapper ? g_wrapper(self,TARGET_IP,TARGET_PORT,data) : 0;
}

static int hk_socket_connect(int fd, const struct sockaddr* sa, socklen_t len) {
    if (!g_socket_connect) return -1;
    if (sa && len >= sizeof(sockaddr_in) && sa->sa_family == AF_INET) {
        sockaddr_in dst = *reinterpret_cast<const sockaddr_in*>(sa);
        char old[64]; inet_ntop(AF_INET,&dst.sin_addr,old,sizeof(old));
        unsigned oldport = ntohs(dst.sin_port);
        // This is the final ENet socket boundary. Redirect only UDP sockets;
        // CDN HTTPS remains untouched because it uses TCP.
        int type = 0; socklen_t tl = sizeof(type);
        if (getsockopt(fd,SOL_SOCKET,SO_TYPE,&type,&tl)==0 && type==SOCK_DGRAM) {
            inet_pton(AF_INET,TARGET_IP,&dst.sin_addr);
            dst.sin_port = htons(TARGET_PORT);
            LOGI("enet_socket_connect UDP: %s:%u -> %s:%u",old,oldport,TARGET_IP,TARGET_PORT);
            return g_socket_connect(fd,reinterpret_cast<const sockaddr*>(&dst),sizeof(dst));
        }
    }
    return g_socket_connect(fd,sa,len);
}

static bool hook_symbol(void* h, const char* name, void* replacement, void** original) {
    void* p = dlsym(h,name); if (!p) { LOGE("dlsym failed: %s",name); return false; }
    int rc = DobbyHook(p,replacement,original);
    LOGI("Dobby %s target=%p rc=%d original=%p",name,p,rc,original?*original:nullptr);
    return rc==0 && original && *original;
}

static bool install(uintptr_t base) {
    void* h = module_handle(); if (!h) { LOGE("cannot open %s",CLIENT_SO); return false; }
    bool any=false;

    // Hook exported ENet functions directly. This covers calls that do not use the
    // module's PLT/GOT and is the main fallback against the original-server path.
    any |= hook_symbol(h,"enet_address_set_host",(void*)hk_set_host,(void**)&g_set_host);
    any |= hook_symbol(h,"enet_address_set_host_ip",(void*)hk_set_host_ip,(void**)&g_set_host_ip);
    any |= hook_symbol(h,"enet_host_connect",(void*)hk_host_connect,(void**)&g_host_connect);
    any |= hook_symbol(h,"enet_socket_connect",(void*)hk_socket_connect,(void**)&g_socket_connect);

#if defined(__aarch64__)
    void* orig=nullptr;
    if (DobbyHook(reinterpret_cast<void*>(base+WRAPPER),(void*)hk_wrapper,&orig)==0 && orig) {
        g_wrapper=(WrapperFn)orig; any=true; LOGI("wrapper hook target=%p original=%p",(void*)(base+WRAPPER),orig);
    } else LOGE("wrapper hook failed target=%p",(void*)(base+WRAPPER));
#endif

#if defined(__aarch64__) || defined(__arm__)
    if (patch_slot(base+GOT_SET_HOST,(SetHostFn)hk_set_host,g_set_host)) { any=true; LOGI("GOT set_host patched"); }
    if (patch_slot(base+GOT_HOST_CONNECT,(HostConnectFn)hk_host_connect,g_host_connect)) { any=true; LOGI("GOT host_connect patched"); }
#endif

    g_installed.store(any);
    return any;
}

static void worker() {
    if (g_started.exchange(true)) return;
    for (int i=0;i<300;i++) {
        uintptr_t base=module_base(CLIENT_SO);
        if (base) {
            LOGI("client base=%p attempt=%d",(void*)base,i);
            if (install(base)) { LOGI("SERVER REDIRECT INSTALLED -> %s:%u",TARGET_IP,TARGET_PORT); return; }
        }
        usleep(100000);
    }
    LOGE("FAILED: client module not hooked");
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    LOGI("libplugin loaded; multi-layer ENet redirect");
    std::thread(worker).detach();
    return JNI_VERSION_1_6;
}
