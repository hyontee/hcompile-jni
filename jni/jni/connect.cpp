#include <android/log.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "dobby.h"

#define TAG "PLUGIN_CONNECT"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

static const char* CONNECT_IP = "188.127.241.74";
static constexpr uint16_t CONNECT_PORT = 2564;

using connect_t = int (*)(int,const sockaddr*,socklen_t);
using sendto_t = ssize_t (*)(int,const void*,size_t,int,const sockaddr*,socklen_t);
using recvfrom_t = ssize_t (*)(int,void*,size_t,int,sockaddr*,socklen_t*);

static connect_t real_connect;
static sendto_t real_sendto;
static recvfrom_t real_recvfrom;

static void log_addr(const sockaddr* a,socklen_t n,const char* tag){
    if(!a || n < sizeof(sockaddr)) return;
    if(a->sa_family==AF_INET && n>=sizeof(sockaddr_in)){
        auto* x=(const sockaddr_in*)a;
        char ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET,&x->sin_addr,ip,sizeof(ip));
        LOGI("%s %s:%u",tag,ip,(unsigned)ntohs(x->sin_port));
    }
}

static int hook_connect(int fd,const sockaddr* a,socklen_t n){
    sockaddr_in dst{};
    if(a && a->sa_family==AF_INET && n>=sizeof(sockaddr_in)){
        dst=*(const sockaddr_in*)a;
        char old_ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET,&dst.sin_addr,old_ip,sizeof(old_ip));
        LOGI("CONNECT %s:%u -> %s:%u",old_ip,(unsigned)ntohs(dst.sin_port),
             CONNECT_IP,(unsigned)CONNECT_PORT);
        inet_pton(AF_INET,CONNECT_IP,&dst.sin_addr);
        dst.sin_port=htons(CONNECT_PORT);
        return real_connect ? real_connect(fd,(sockaddr*)&dst,sizeof(dst)) : -1;
    }
    return real_connect ? real_connect(fd,a,n) : -1;
}

static ssize_t hook_sendto(int fd,const void* b,size_t n,int f,const sockaddr* a,socklen_t l){
    sockaddr_in dst{};
    if(a && a->sa_family==AF_INET && l>=sizeof(sockaddr_in)){
        dst=*(const sockaddr_in*)a;
        char ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET,&dst.sin_addr,ip,sizeof(ip));
        if(dst.sin_port){
            inet_pton(AF_INET,CONNECT_IP,&dst.sin_addr);
            dst.sin_port=htons(CONNECT_PORT);
            LOGI("SENDTO %s:%u -> %s:%u",ip,(unsigned)ntohs(((const sockaddr_in*)a)->sin_port),
                 CONNECT_IP,(unsigned)CONNECT_PORT);
            a=(const sockaddr*)&dst;
            l=sizeof(dst);
        }
    }
    return real_sendto ? real_sendto(fd,b,n,f,a,l) : -1;
}

static ssize_t hook_recvfrom(int fd,void* b,size_t n,int f,sockaddr* a,socklen_t* l){
    return real_recvfrom ? real_recvfrom(fd,b,n,f,a,l) : -1;
}

static void hook_all(){
    void* p;
    p=DobbySymbolResolver("libc.so","connect");
    if(p) DobbyHook(p,(void*)hook_connect,(void**)&real_connect);
    p=DobbySymbolResolver("libc.so","sendto");
    if(p) DobbyHook(p,(void*)hook_sendto,(void**)&real_sendto);
    p=DobbySymbolResolver("libc.so","recvfrom");
    if(p) DobbyHook(p,(void*)hook_recvfrom,(void**)&real_recvfrom);
}

extern "C" __attribute__((constructor)) void init_connect(){
    hook_all();
}
