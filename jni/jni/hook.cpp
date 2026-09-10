#include <android/log.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

#include <cstdint>
#include <cstddef>

#define LOG_TAG "BlackHook"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/*
 * Test configuration.
 * Replace these with your own server.
 */
static constexpr const char* SERVER_IP = "185.204.0.27";
static constexpr uint16_t SERVER_PORT = 7777;

struct ENetAddress {
    uint32_t host;
    uint16_t port;
};

struct ENetPeer;
using enet_host_connect_t =
    ENetPeer* (*)(void*, ENetAddress*, size_t, uint32_t);

static enet_host_connect_t original_enet_host_connect = nullptr;

/*
 * This project intentionally does not contain a custom ARM64 trampoline.
 * Use it as the ndk-build base and connect the hook backend you use for
 * your own client. Calling a function with the wrong ABI/trampoline can
 * crash the target process.
 */

static ENetPeer* observe_connection(
    void* host,
    ENetAddress* address,
    size_t channelCount,
    uint32_t data)
{
    if (address) {
        struct in_addr addr{};
        addr.s_addr = address->host;

        char ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET, &addr, ip, sizeof(ip));

        LOGI("Original connection: %s:%u", ip, address->port);

        /*
         * For your own client, redirect here:
         *
         * address->host = inet_addr(SERVER_IP);
         * address->port = htons(SERVER_PORT);
         *
         * Then call the original function.
         *
         * This line is left disabled until a real hook backend is installed.
         */
    }

    if (!original_enet_host_connect)
        return nullptr;

    return original_enet_host_connect(
        host, address, channelCount, data);
}

static void* worker(void*) {
    LOGI("BlackHook loaded");
    LOGI("Configured server: %s:%u", SERVER_IP, SERVER_PORT);

    for (int i = 0; i < 100; ++i) {
        void* h = dlopen(
            "libblackrussia-client.so",
            RTLD_NOLOAD | RTLD_NOW);

        if (h) {
            LOGI("libblackrussia-client.so is loaded: %p", h);

            void* sym = dlsym(h, "enet_host_connect");
            if (sym)
                LOGI("enet_host_connect: %p", sym);
            else
                LOGE("enet_host_connect was not found");

            return nullptr;
        }

        usleep(100000);
    }

    LOGE("Timed out waiting for libblackrussia-client.so");
    return nullptr;
}

__attribute__((constructor))
static void init() {
    pthread_t t{};
    if (pthread_create(&t, nullptr, worker, nullptr) == 0)
        pthread_detach(t);
}
