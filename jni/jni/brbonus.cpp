#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <link.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <atomic>
#include <thread>
#include <chrono>
#include "dobby.h"

// libbrbonus replacement: bonus-like modular native redirect for the supplied APK.
// Target: 185.207.214.14:3713
// IMPORTANT: offsets below are for the exact supplied libblackrussia-client.so
// hashes documented in the previous analysis. They are not universal offsets.

#define LOG_TAG "BRBonusReplacement"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static constexpr const char* TARGET_IP = "185.207.214.14";
static constexpr uint16_t TARGET_PORT = 3713;
static constexpr const char* CLIENT_SO = "libblackrussia-client.so";

#if defined(__aarch64__)
static constexpr uintptr_t GOT_ENET_ADDRESS_SET_HOST_IP = 0x1d9ca90;
static constexpr uintptr_t GOT_ENET_ADDRESS_SET_HOST    = 0x1d9ca98;
static constexpr uintptr_t GOT_ENET_HOST_CONNECT        = 0x1d9caa0;
static constexpr uintptr_t CONNECT_WRAPPER              = 0x171197c;
#elif defined(__arm__)
static constexpr uintptr_t GOT_ENET_ADDRESS_SET_HOST_IP = 0x15789b4;
static constexpr uintptr_t GOT_ENET_ADDRESS_SET_HOST    = 0x15789b8;
static constexpr uintptr_t GOT_ENET_HOST_CONNECT        = 0x15789bc;
#endif

struct ENetAddressCompat {
    uint32_t host;
    uint16_t port;
    uint16_t reserved;
};
struct ENetHostCompat;
struct ENetPeerCompat;

using HostConnectFn = ENetPeerCompat* (*)(ENetHostCompat*, const ENetAddressCompat*, size_t, uint32_t);
using AddressSetHostFn = int (*)(ENetAddressCompat*, const char*);
using AddressSetHostIpFn = int (*)(ENetAddressCompat*, uint32_t);
using WrapperFn = int (*)(void*, const char*, uint16_t, uint32_t);

static HostConnectFn g_host_connect = nullptr;
static AddressSetHostFn g_address_set_host = nullptr;
static AddressSetHostIpFn g_address_set_host_ip = nullptr;
static WrapperFn g_wrapper = nullptr;

static std::atomic<bool> g_installed{false};
static std::atomic<bool> g_started{false};

static uintptr_t find_module(const char* name) {
    struct Context { const char* wanted; uintptr_t base; } ctx{name, 0};
    dl_iterate_phdr([](struct dl_phdr_info* info, size_t, void* opaque) -> int {
        auto* c = static_cast<Context*>(opaque);
        if (info->dlpi_name && strstr(info->dlpi_name, c->wanted)) {
            c->base = static_cast<uintptr_t>(info->dlpi_addr);
            return 1;
        }
        return 0;
    }, &ctx);
    return ctx.base;
}

static void log_address(const char* prefix, const ENetAddressCompat* a) {
    if (!a) {
        LOGI("%s <null>", prefix);
        return;
    }
    char ip[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &a->host, ip, sizeof(ip));
    LOGI("%s %s:%u", prefix, ip[0] ? ip : "<invalid>", (unsigned)a->port);
}

static bool make_rw(uintptr_t address) {
    const long page = sysconf(_SC_PAGESIZE);
    if (page <= 0) return false;
    const uintptr_t mask = ~(static_cast<uintptr_t>(page) - 1u);
    void* page_start = reinterpret_cast<void*>(address & mask);
    return mprotect(page_start, static_cast<size_t>(page), PROT_READ | PROT_WRITE) == 0;
}

template<class Fn>
static bool patch_got(uintptr_t slot, Fn replacement, Fn& original) {
    if (!make_rw(slot)) return false;
    auto* cell = reinterpret_cast<uintptr_t*>(slot);
    original = reinterpret_cast<Fn>(*cell);
    if (!original) return false;
    __atomic_store_n(cell, reinterpret_cast<uintptr_t>(replacement), __ATOMIC_SEQ_CST);
    __builtin___clear_cache(reinterpret_cast<char*>(slot), reinterpret_cast<char*>(slot + sizeof(uintptr_t)));
    return true;
}

static void force_target(ENetAddressCompat* address) {
    if (!address) return;
    in_addr target{};
    if (inet_pton(AF_INET, TARGET_IP, &target) != 1) return;
    // ENet stores host in network byte order and port in host byte order.
    address->host = target.s_addr;
    address->port = TARGET_PORT;
}

static int hooked_address_set_host(ENetAddressCompat* address, const char* host) {
    LOGI("enet_address_set_host(%s) -> %s:%u", host ? host : "<null>", TARGET_IP, TARGET_PORT);
    force_target(address);
    return 0;
}

static int hooked_address_set_host_ip(ENetAddressCompat* address, uint32_t) {
    LOGI("enet_address_set_host_ip() -> %s:%u", TARGET_IP, TARGET_PORT);
    force_target(address);
    return 0;
}

static ENetPeerCompat* hooked_host_connect(ENetHostCompat* host,
                                           const ENetAddressCompat* address,
                                           size_t channels,
                                           uint32_t data) {
    if (!g_host_connect) return nullptr;
    ENetAddressCompat redirected{};
    if (address) redirected = *address;
    log_address("original ENet target:", address);
    force_target(&redirected);
    log_address("redirected ENet target:", &redirected);
    return g_host_connect(host, &redirected, channels, data);
}

static int hooked_wrapper(void* self, const char* host, uint16_t port, uint32_t data) {
    LOGI("game wrapper: %s:%u -> %s:%u", host ? host : "<null>",
         (unsigned)port, TARGET_IP, TARGET_PORT);
    if (!g_wrapper) return 0;
    return g_wrapper(self, TARGET_IP, TARGET_PORT, data);
}

static void try_got(uintptr_t base) {
#if defined(__aarch64__) || defined(__arm__)
    if (patch_got(base + GOT_ENET_ADDRESS_SET_HOST_IP,
                  hooked_address_set_host_ip, g_address_set_host_ip)) {
        LOGI("GOT: enet_address_set_host_ip installed");
    } else {
        LOGE("GOT: enet_address_set_host_ip failed");
    }

    if (patch_got(base + GOT_ENET_ADDRESS_SET_HOST,
                  hooked_address_set_host, g_address_set_host)) {
        LOGI("GOT: enet_address_set_host installed");
    } else {
        LOGE("GOT: enet_address_set_host failed");
    }

    if (patch_got(base + GOT_ENET_HOST_CONNECT,
                  hooked_host_connect, g_host_connect)) {
        LOGI("GOT: enet_host_connect installed");
    } else {
        LOGE("GOT: enet_host_connect failed");
    }
#endif
}

static void try_dobby(uintptr_t base) {
    void* handle = dlopen(CLIENT_SO, RTLD_NOW | RTLD_NOLOAD);
    if (!handle) handle = dlopen(CLIENT_SO, RTLD_NOW);

#if defined(__aarch64__)
    // Exact wrapper discovered in the supplied arm64 client.
    void* wrapper = reinterpret_cast<void*>(base + CONNECT_WRAPPER);
    void* original = nullptr;
    int rc = DobbyHook(wrapper, reinterpret_cast<void*>(hooked_wrapper), &original);
    if (rc == 0 && original) {
        g_wrapper = reinterpret_cast<WrapperFn>(original);
        LOGI("Dobby: wrapper installed at %p", wrapper);
    } else {
        LOGE("Dobby: wrapper failed rc=%d", rc);
    }
#endif

    if (handle) {
        void* symbol = dlsym(handle, "enet_host_connect");
        if (symbol) {
            void* original = nullptr;
            int rc = DobbyHook(symbol, reinterpret_cast<void*>(hooked_host_connect), &original);
            if (rc == 0 && original) {
                if (!g_host_connect) g_host_connect = reinterpret_cast<HostConnectFn>(original);
                LOGI("Dobby: enet_host_connect symbol installed");
            } else {
                LOGE("Dobby: symbol hook failed rc=%d", rc);
            }
        }
        dlclose(handle);
    }
}

static void install_all() {
    if (g_started.exchange(true)) return;

    for (int attempt = 0; attempt < 300; ++attempt) {
        uintptr_t base = find_module(CLIENT_SO);
        if (base) {
            LOGI("client loaded base=%p attempt=%d", reinterpret_cast<void*>(base), attempt);
            try_got(base);
            try_dobby(base);
            g_installed.store(true);
            LOGI("libbrbonus replacement hooks initialized for %s:%u", TARGET_IP, TARGET_PORT);
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    LOGE("libblackrussia-client.so was not found");
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    LOGI("libplugin loaded; bonus-like native hook architecture");
    std::thread(install_all).detach();
    return JNI_VERSION_1_6;
}
