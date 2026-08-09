#include "jniutil.h"
#include "chat.h"
#include "xorstr.h"


extern "C" JavaVM* javaVM;

#define SCOPED_LOCAL_REF(env, ref) ScopedLocalRef<decltype(ref)> scoped_##ref(env, ref)

template <typename T>
class ScopedLocalRef {
public:
    ScopedLocalRef(JNIEnv* env, T ref) : env_(env), ref_(ref) {}
    ~ScopedLocalRef() { if (ref_) env_->DeleteLocalRef(ref_); }
    T get() const { return ref_; }
private:
    JNIEnv* env_;
    T ref_;
};

JNIEnv* CJavaWrapper::GetEnv() {
    JNIEnv* env = nullptr;
    if (javaVM->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_EDETACHED) {
        if (javaVM->AttachCurrentThread(&env, nullptr) != 0) return nullptr;
    }
    return env;
}

void CJavaWrapper::CallGUI(int guiId, const std::string& jsonString, const char* methodName) {
    JNIEnv* env = GetEnv();
    if (!env || !guiManagerClass) return;

    jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
    if (!getInstanceMethod) return;

    jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);
    SCOPED_LOCAL_REF(env, guiManagerInstance);

    jstring jsonJString = env->NewStringUTF(jsonString.c_str());
    SCOPED_LOCAL_REF(env, jsonJString);

    jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
    if (!jsonObjectClass) return;

    jmethodID jsonCtor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
    if (!jsonCtor) return;

    jobject jsonObject = env->NewObject(jsonObjectClass, jsonCtor, jsonJString);
    SCOPED_LOCAL_REF(env, jsonObject);

    jmethodID guiMethod = env->GetMethodID(guiManagerClass, methodName, "(ILorg/json/JSONObject;)V");
    if (!guiMethod) return;

    env->CallVoidMethod(guiManagerInstance, guiMethod, guiId, jsonObject);
    EXCEPTION_CHECK(env);
}

CJavaWrapper::CJavaWrapper(JNIEnv* env) {
    jclass localClass = env->FindClass("com/blackhub/bronline/game/core/JNIJSONTransport");
    s_JSONTransportClass = (jclass) env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
}


CJavaWrapper::~CJavaWrapper() {
    JNIEnv* env = GetEnv();
    if (env && guiManagerClass) {
        env->DeleteGlobalRef(guiManagerClass);
        guiManagerClass = nullptr;
    }
}

CJavaWrapper* g_pJavaWrapper = nullptr;