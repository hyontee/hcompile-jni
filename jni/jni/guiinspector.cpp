#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <link.h>
#include <sys/mman.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define TAG "GUIInspector"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)

static std::atomic<bool> g_running{false};
static std::mutex g_file_mutex;
static const char* kTarget = "libblackrussia-client.so";

// This is the exact C++ mangled name observed in the client binary.
static const char* kOpenSymbol =
    "_ZN3Gui11CGuiManager4OpenERKN5eastl17basic_string_viewIcEEPNS_6BaseVMEjE3$_0";

static void appendLog(const std::string& s) {
    std::lock_guard<std::mutex> lock(g_file_mutex);
    const char* paths[] = {
        "/sdcard/GUIInspector.log",
        "/storage/emulated/0/Download/GUIInspector.log",
        "/data/local/tmp/GUIInspector.log"
    };
    for (auto p : paths) {
        std::ofstream f(p, std::ios::app);
        if (f) { f << s << '\n'; return; }
    }
}

static void logBoth(const char* fmt, ...) {
    char b[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof(b), fmt, ap);
    va_end(ap);
    LOGI("%s", b);
    appendLog(b);
}

struct ModuleInfo { uintptr_t base=0, end=0; std::string path; };

static bool findTarget(ModuleInfo& out) {
    std::ifstream f("/proc/self/maps");
    std::string line;
    while (std::getline(f, line)) {
        if (line.find(kTarget) == std::string::npos) continue;
        unsigned long long a=0,b=0; char perms[8]={}; char p[1024]={};
        if (sscanf(line.c_str(), "%llx-%llx %7s %*s %*s %*s %1023[^\\n]", &a,&b,perms,p) >= 3) {
            out.base=(uintptr_t)a; out.end=(uintptr_t)b; out.path=p; return true;
        }
    }
    return false;
}

static void dumpBytes(uintptr_t p) {
    char b[512]; size_t n=0;
    n += snprintf(b+n,sizeof(b)-n,"CODE @ 0x%zx:", (size_t)p);
    for (int i=0;i<32 && n+4<sizeof(b);++i) n += snprintf(b+n,sizeof(b)-n," %02x", ((unsigned char*)p)[i]);
    logBoth("%s", b);
}

static void inspectSymbol() {
    ModuleInfo m;
    if (!findTarget(m)) return;

    void* h = dlopen(kTarget, RTLD_NOW | RTLD_NOLOAD);
    if (!h) {
        logBoth("TARGET_FOUND base=0x%zx path=%s but dlopen(NOLOAD) failed: %s", (size_t)m.base, m.path.c_str(), dlerror());
        return;
    }

    dlerror();
    void* sym = dlsym(h, kOpenSymbol);
    const char* e = dlerror();
    if (sym && !e) {
        uintptr_t a=(uintptr_t)sym;
        logBoth("CGUI_OPEN_FOUND addr=0x%zx rva=0x%zx symbol=%s", (size_t)a, (size_t)(a-m.base), kOpenSymbol);
        dumpBytes(a);
    } else {
        logBoth("CGUI_OPEN_NOT_EXPORTED symbol=%s", kOpenSymbol);
        // dlsym cannot find stripped local symbols. Do not guess an RVA.
        // The diagnostic tells us whether the next build must use a pattern hook.
    }
    dlclose(h);
}

static void scanLoop() {
    bool done=false; uintptr_t last=0;
    while (g_running.load()) {
        ModuleInfo m;
        if (findTarget(m) && m.base != last) {
            last=m.base;
            logBoth("TARGET_LOADED name=%s base=0x%zx end=0x%zx size=0x%zx path=%s",
                kTarget,(size_t)m.base,(size_t)m.end,(size_t)(m.end-m.base),m.path.c_str());
            inspectSymbol();
            done=true;
        }
        if (done) std::this_thread::sleep_for(std::chrono::seconds(3));
        else std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_guiinspector_LoaderActivity_startInspector(JNIEnv*, jclass) {
    if (g_running.exchange(true)) return;
    appendLog("INSPECTOR_STARTED mode=CGuiManager::Open resolver");
    LOGI("Inspector started; waiting for %s", kTarget);
    std::thread(scanLoop).detach();
}

JNIEXPORT jint JNI_OnLoad(JavaVM*, void*) {
    LOGI("JNI_OnLoad GUIInspector CGuiManager resolver");
    return JNI_VERSION_1_6;
}
