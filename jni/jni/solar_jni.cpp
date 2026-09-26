#include "solar_jni.h"
#include <string>
#include <cstring>

static std::string g_ip = "127.0.0.1";
static int g_port = 7777;

extern "C" void solar_configure_server(const char* ip, int port) {
    if (ip && *ip) g_ip = ip;
    if (port > 0 && port <= 65535) g_port = port;
}

extern "C" const char* solar_server_ip() {
    return g_ip.c_str();
}

extern "C" int solar_server_port() {
    return g_port;
}
