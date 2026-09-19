#include "chat.h"
#include "xorstr.h"
#include "plugin.h"

#if defined(__aarch64__) && __has_include(<android/log.h>)
#include <android/log.h>
#endif

void CChat::AddDebugMessage(const char* msg, ...) {
    char buffer[1024];
    va_list args;

    va_start(args, msg);
    vsnprintf(buffer, sizeof(buffer), msg, args);
    va_end(args);

#ifdef __aarch64__
    #if __has_include(<android/log.h>)
    __android_log_print(ANDROID_LOG_INFO, "BRBridge-16.34", "%s", buffer);
    #endif
#else
    uintptr_t fn = CGameAPI::GetBase(xorstr("CChat::AddDebugMessage"));
    if(fn) {
        reinterpret_cast<void(*)(char*)>(fn)(buffer);
    }
#endif
}
