#include <jni.h>
#include "solar_jni.h"

extern "C" JNIEXPORT void JNICALL
Java_com_grand_launcher_activity_NativeBridge_configureServer(
        JNIEnv* env, jclass, jstring ip, jint port) {
    if (!ip) return;
    const char* value = env->GetStringUTFChars(ip, nullptr);
    solar_configure_server(value, static_cast<int>(port));
    env->ReleaseStringUTFChars(ip, value);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_grand_launcher_activity_NativeBridge_getServerIp(
        JNIEnv* env, jclass) {
    return env->NewStringUTF(solar_server_ip());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_grand_launcher_activity_NativeBridge_getServerPort(
        JNIEnv*, jclass) {
    return static_cast<jint>(solar_server_port());
}
