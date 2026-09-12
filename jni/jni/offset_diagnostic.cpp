#include <android/log.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

#define TAG "PLUGIN_OFFSETS"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

static uintptr_t module_base(const char* name){
    FILE* f=fopen("/proc/self/maps","r");
    if(!f) return 0;
    char s[512];
    while(fgets(s,sizeof(s),f)){
        if(strstr(s,name)){
            uintptr_t b=0;
            if(sscanf(s,"%lx-%*lx",&b)==1){
                fclose(f);
                return b;
            }
        }
    }
    fclose(f);
    return 0;
}

static bool executable_range(const char* name, uintptr_t rva){
    FILE* f=fopen("/proc/self/maps","r");
    if(!f) return false;
    char s[512];
    while(fgets(s,sizeof(s),f)){
        char path[256]{};
        unsigned long lo=0,hi=0;
        int n=sscanf(s,"%lx-%lx %*4s %*lx %*x:%*x %*u %255s",&lo,&hi,path);
        if(n>=2 && strstr(s,name)){
            if(rva>=lo && rva<hi){
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}

extern "C" __attribute__((constructor)) void init_offset_diagnostic(){
#if defined(__arm__)
    const uintptr_t off=0x00110007u;
    uintptr_t base=module_base("libblackrussia-client.so");
    if(base && executable_range("libblackrussia-client.so",base+(off&~1u))){
        LOGI("ABI armeabi-v7a");
        LOGI("0x%08lx resolved in libblackrussia-client.so at %p",(unsigned long)off,(void*)(base+off));
    }else{
        LOGI("ABI armeabi-v7a");
        LOGI("0x%08lx not mapped in libblackrussia-client.so",(unsigned long)off);
    }
#elif defined(__aarch64__)
    LOGI("ABI arm64-v8a");
#endif
}
