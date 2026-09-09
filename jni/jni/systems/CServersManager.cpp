#include "CServersManager.h"
#include "obfuscate/str_obfuscate.hpp"

std::string* CServersManager::mServersIp = nullptr;
int32_t* CServersManager::mServersPort = nullptr;
int16_t CServersManager::mCurServer = 0;

extern uintptr_t g_libBR;

extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_sendServersData(JNIEnv *env, jobject thiz, jint curServer
                                                             /*jobjectArray servers, jintArray ports*/,
                                                             jint a2) {
//    jsize arraySize = env->GetArrayLength(servers);
//    std::vector<std::string> tmpServers;
//    jint* jintArr = (jint*) env->GetIntArrayElements(ports, nullptr);
//
//    for (jsize i = 0; i < arraySize; ++i) {
//        jstring jstr = (jstring) env->GetObjectArrayElement(servers, i);
//        const char* cstr = env->GetStringUTFChars(jstr, nullptr);
//        tmpServers.push_back(std::string(cstr));
//        env->ReleaseStringUTFChars(jstr, cstr);
//        env->DeleteLocalRef(jstr);
//    }
//
//    CServersManager::mServersIp = new std::string[arraySize];
//    CServersManager::mServersPort = new int32_t[arraySize];
//    for (jsize i = 0; i < arraySize; ++i) {
//        CServersManager::mServersIp[i] = tmpServers[i];
//        CServersManager::mServersPort[i] = jintArr[i];
//    }


    // your servers
    // esli 1 servak bOnce = 1;
    // esli >2 servak bOnce = 0;
    bool bOnce = 0;
    CServersManager::mServersIp = new std::string[a2];
    CServersManager::mServersPort = new int32_t[a2];
    if(bOnce){
        for (int i = 0; i < a2; ++i) {
            CServersManager::mServersIp[i] = OBFUSCATE("178.208.177.121");
            CServersManager::mServersPort[i] = 30003;
        }
    }else{
        CServersManager::mServersIp[0] = OBFUSCATE("178.208.177.121");
        CServersManager::mServersPort[0] = 30003;
        CServersManager::mServersIp[1] = OBFUSCATE("51.75.232.66");
        CServersManager::mServersPort[1] = 2197;
        // ...
    }

    // saving new last server info
//    *(uintptr_t*)((*(uintptr_t*)(g_libBR + 0x205360)) + 70) = curServer;
//    *(uintptr_t*)((*(uintptr_t*)(g_libBR + 0x205360)) + 280) = curServer;
//    ((uintptr_t(*)(uintptr_t))(g_libBR + 0x2E740 + 1))(*(uintptr_t*)(g_libBR + 0x205360));

    CServersManager::mCurServer = curServer;
}