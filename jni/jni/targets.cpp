#include <android/log.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <dobby.h>

#define TAG "PLUGIN_HOOKS"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

static uintptr_t module_base(){
    FILE* f=fopen("/proc/self/maps","r");
    if(!f) return 0;
    char s[512];
    while(fgets(s,sizeof(s),f)){
        if(strstr(s,"libplugin.so")){
            uintptr_t b=0;
#if defined(__aarch64__)
            sscanf(s,"%lx-%*lx",&b);
#else
            unsigned long v=0;
            sscanf(s,"%lx-%*lx",&v);
            b=(uintptr_t)v;
#endif
            fclose(f);
            return b;
        }
    }
    fclose(f);
    return 0;
}

#if defined(__aarch64__)
using fn = uintptr_t(*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t);

static fn o_2581C;
static uintptr_t h_2581C(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x2581C %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_2581C?o_2581C(a,b,c,d):0;}
static fn o_25840;
static uintptr_t h_25840(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x25840 %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_25840?o_25840(a,b,c,d):0;}
static fn o_26300;
static uintptr_t h_26300(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x26300 %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_26300?o_26300(a,b,c,d):0;}
static fn o_25D50;
static uintptr_t h_25D50(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x25D50 %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_25D50?o_25D50(a,b,c,d):0;}
static fn o_20198;
static uintptr_t h_20198(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x20198 %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_20198?o_20198(a,b,c,d):0;}
static fn o_1FDC4;
static uintptr_t h_1FDC4(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x1FDC4 %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_1FDC4?o_1FDC4(a,b,c,d):0;}
static fn o_20098;
static uintptr_t h_20098(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x20098 %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_20098?o_20098(a,b,c,d):0;}
static fn o_200CC;
static uintptr_t h_200CC(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x200CC %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_200CC?o_200CC(a,b,c,d):0;}
static fn o_201E0;
static uintptr_t h_201E0(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d){LOGI("0x201E0 %p %p %lx %lx",(void*)a,(void*)b,(unsigned long)c,(unsigned long)d);return o_201E0?o_201E0(a,b,c,d):0;}

static void hook64(){
    uintptr_t b=module_base();
    if(!b){LOGI("libplugin.so base not found");return;}
    DobbyHook((void*)(b+0x2581C),(void*)h_2581C,(void**)&o_2581C);
    DobbyHook((void*)(b+0x25840),(void*)h_25840,(void**)&o_25840);
    DobbyHook((void*)(b+0x26300),(void*)h_26300,(void**)&o_26300);
    DobbyHook((void*)(b+0x25D50),(void*)h_25D50,(void**)&o_25D50);
    DobbyHook((void*)(b+0x20198),(void*)h_20198,(void**)&o_20198);
    DobbyHook((void*)(b+0x1FDC4),(void*)h_1FDC4,(void**)&o_1FDC4);
    DobbyHook((void*)(b+0x20098),(void*)h_20098,(void**)&o_20098);
    DobbyHook((void*)(b+0x200CC),(void*)h_200CC,(void**)&o_200CC);
    DobbyHook((void*)(b+0x201E0),(void*)h_201E0,(void**)&o_201E0);
}
#endif

extern "C" __attribute__((constructor)) void init_hooks(){
#if defined(__aarch64__)
    LOGI("architecture arm64-v8a");
    hook64();
#elif defined(__arm__)
    LOGI("architecture armeabi-v7a");
    LOGI("32-bit build: 64-bit internal RVAs disabled");
#else
    LOGI("unsupported architecture");
#endif
}
