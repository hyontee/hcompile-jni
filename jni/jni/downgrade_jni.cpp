/**
 * downgrade_jni.cpp
 * Вызывает libScDowngrade.so (by Invoker4k) через dlopen/dlsym
 * Т.к. JNI функция привязана к пакету com.scdowngrader.app,
 * вызываем её напрямую через указатель.
 */
#include <jni.h>
#include <dlfcn.h>
#include <string>
#include <android/log.h>

#define TAG "Downgrade"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

#ifndef SCTOOL_JNI_CLASS
#define SCTOOL_JNI_CLASS Java_com_compose_sctool_ScProcessor
#endif
#define _CAT(a,b) a##b
#define CAT(a,b) _CAT(a,b)
#define FN(name) CAT(SCTOOL_JNI_CLASS, name)

// Тип JNI функции downgrade из libScDowngrade.so
typedef jstring (*DowngradeFn)(JNIEnv*, jobject, jstring, jstring, jfloat);

static void* g_lib    = nullptr;
static DowngradeFn g_fn = nullptr;

static bool load_lib(JNIEnv* env) {
    if (g_fn) return true;

    // Путь к либе в APK после установки
    // Пробуем найти через ClassLoader
    jclass actClass = env->FindClass("android/app/ActivityThread");
    jmethodID curApp = env->GetStaticMethodID(actClass, "currentApplication",
        "()Landroid/app/Application;");
    jobject app = env->CallStaticObjectMethod(actClass, curApp);

    jmethodID getAppInfo = env->GetMethodID(
        env->FindClass("android/content/Context"),
        "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
    jobject appInfo = env->CallObjectMethod(app, getAppInfo);

    jfieldID nativeLibField = env->GetFieldID(
        env->FindClass("android/content/pm/ApplicationInfo"),
        "nativeLibraryDir", "Ljava/lang/String;");
    jstring nativeLibDir = (jstring)env->GetObjectField(appInfo, nativeLibField);

    const char* dir = env->GetStringUTFChars(nativeLibDir, nullptr);
    std::string libPath = std::string(dir) + "/libScDowngrade.so";
    env->ReleaseStringUTFChars(nativeLibDir, dir);

    LOGI("Loading: %s", libPath.c_str());
    g_lib = dlopen(libPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!g_lib) {
        LOGE("dlopen failed: %s", dlerror());
        return false;
    }

    // Символ JNI функции из оригинального пакета
    const char* sym = "Java_com_scdowngrader_app_NativeLib_downgrade";
    g_fn = (DowngradeFn)dlsym(g_lib, sym);
    if (!g_fn) {
        LOGE("dlsym failed: %s", dlerror());
        dlclose(g_lib); g_lib = nullptr;
        return false;
    }

    LOGI("libScDowngrade loaded OK");
    return true;
}

std::string sc_downgrade_invoker(const std::string& input,
                                  const std::string& output,
                                  float version) {
    // Получаем JNIEnv через JavaVM (хранится в sc_core или отдельно)
    // Используем простой подход — вызов через обёртку
    return "NEED_ENV"; // заглушка, реальный вызов ниже через JNI экспорт
}

extern "C" {

JNIEXPORT jstring JNICALL FN(_nativeDowngrade)(JNIEnv* env, jclass,
        jstring input, jstring output, jstring out_dir) {

    if (!load_lib(env)) {
        return env->NewStringUTF("Error: cannot load libScDowngrade.so");
    }

    // Получаем строки
    const char* in_c  = env->GetStringUTFChars(input,  nullptr);
    const char* out_c = env->GetStringUTFChars(output, nullptr);

    std::string in_str(in_c);
    std::string out_str;

    // Определяем выходной путь
    // out_dir = папка, filename берём из input
    const char* od = env->GetStringUTFChars(out_dir, nullptr);
    std::string od_str(od);
    env->ReleaseStringUTFChars(out_dir, od);

    // stem из input
    size_t slash = in_str.rfind('/');
    std::string fname = (slash == std::string::npos) ? in_str : in_str.substr(slash+1);
    size_t dot = fname.rfind('.');
    std::string stem = (dot == std::string::npos) ? fname : fname.substr(0, dot);
    out_str = od_str + "/" + stem + "_downgraded.sc";

    env->ReleaseStringUTFChars(input,  in_c);
    env->ReleaseStringUTFChars(output, out_c);

    // Создаём jobject (NativeLib.INSTANCE не нужен т.к. вызываем напрямую)
    // Передаём nullptr как this — функция статическая по сути
    jstring j_in  = env->NewStringUTF(in_str.c_str());
    jstring j_out = env->NewStringUTF(out_str.c_str());
    jfloat  j_ver = -1.0f; // авто-определение версии

    jstring result = g_fn(env, nullptr, j_in, j_out, j_ver);

    env->DeleteLocalRef(j_in);
    env->DeleteLocalRef(j_out);

    if (!result) {
        return env->NewStringUTF(("OK: Downgraded\n  Output: " + stem + "_downgraded.sc\n  Dir: " + od_str).c_str());
    }

    // Добавляем путь к сообщению
    const char* res_c = env->GetStringUTFChars(result, nullptr);
    std::string res_str(res_c);
    env->ReleaseStringUTFChars(result, res_c);

    if (res_str.empty() || res_str == "Success" || res_str.find("uccess") != std::string::npos) {
        return env->NewStringUTF(("OK: Downgraded\n  Output: " + stem + "_downgraded.sc\n  Dir: " + od_str).c_str());
    }
    return env->NewStringUTF(("Error: " + res_str).c_str());
}

// Версия с выбором версии (0.5 или 1.0)
JNIEXPORT jstring JNICALL FN(_nativeDowngradeV)(JNIEnv* env, jclass,
        jstring input, jstring version_str, jstring out_dir) {

    if (!load_lib(env)) {
        return env->NewStringUTF("Error: cannot load libScDowngrade.so");
    }

    const char* in_c = env->GetStringUTFChars(input, nullptr);
    const char* vs_c = env->GetStringUTFChars(version_str, nullptr);
    const char* od_c = env->GetStringUTFChars(out_dir, nullptr);

    std::string in_str(in_c);
    std::string od_str(od_c);
    float ver = -1.0f;
    try { ver = std::stof(vs_c); } catch (...) {}

    env->ReleaseStringUTFChars(input, in_c);
    env->ReleaseStringUTFChars(version_str, vs_c);
    env->ReleaseStringUTFChars(out_dir, od_c);

    size_t slash = in_str.rfind('/');
    std::string fname = (slash == std::string::npos) ? in_str : in_str.substr(slash+1);
    size_t dot = fname.rfind('.');
    std::string stem = (dot == std::string::npos) ? fname : fname.substr(0, dot);
    std::string out_str = od_str + "/" + stem + "_dg.sc";

    jstring j_in  = env->NewStringUTF(in_str.c_str());
    jstring j_out = env->NewStringUTF(out_str.c_str());

    jstring result = g_fn(env, nullptr, j_in, j_out, ver);

    env->DeleteLocalRef(j_in);
    env->DeleteLocalRef(j_out);

    const char* res_c2 = result ? env->GetStringUTFChars(result, nullptr) : nullptr;
    std::string res_str = res_c2 ? res_c2 : "";
    if (res_c2) env->ReleaseStringUTFChars(result, res_c2);

    if (res_str.empty() || res_str.find("uccess") != std::string::npos) {
        return env->NewStringUTF(("OK: Downgraded (v" + std::to_string(ver) + ")\n  Output: " + stem + "_dg.sc\n  Dir: " + od_str).c_str());
    }
    return env->NewStringUTF(("Error: " + res_str).c_str());
}

} // extern "C"
