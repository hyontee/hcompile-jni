// app/src/main/cpp/samp/java_systems/CMvdIdOverlay.cpp
#include "CMvdIdOverlay.h"
#include "main.h"
#include "CJavaWrapper.h"
#include <string>
#include <vector>

jobject CMvdIdOverlay::j_obj = nullptr;

// ---- CP1251 ? UTF-16 (BMP) ----
// скопировано по шаблону из CPassportOverlay.cpp
static jstring JStringFromCP1251(JNIEnv* env, const char* s) {
    if (!s) s = "";
    static const uint16_t map[128] = {
            0x0402,0x0403,0x201A,0x0453,0x201E,0x2026,0x2020,0x2021,
            0x20AC,0x2030,0x0409,0x2039,0x040A,0x040C,0x040B,0x040F,
            0x0452,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
            0x0098,0x2122,0x0459,0x203A,0x045A,0x045C,0x045B,0x045F,
            0x00A0,0x040E,0x045E,0x0408,0x00A4,0x0490,0x00A6,0x00A7,
            0x0401,0x00A9,0x0404,0x00AB,0x00AC,0x00AD,0x00AE,0x0407,
            0x00B0,0x00B1,0x0406,0x0456,0x0491,0x00B5,0x00B6,0x00B7,
            0x0451,0x2116,0x0454,0x00BB,0x0458,0x0405,0x0455,0x0457,
            0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
            0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
            0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
            0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
            0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
            0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
            0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
            0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F
    };

    const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
    std::u16string u16;
    u16.reserve(strlen(s));

    for (; *p; ++p) {
        unsigned char c = *p;
        uint16_t cp;
        if (c < 0x80) cp = c;           // ASCII
        else { cp = map[c - 0x80]; if (cp == 0) cp = u'?'; }
        u16.push_back(static_cast<char16_t>(cp));
    }
    return env->NewString(reinterpret_cast<const jchar*>(u16.data()), (jsize)u16.size());
}

void CMvdIdOverlay::show() {
    JNIEnv* env = g_pJavaWrapper->GetEnv(); if (!env || !j_obj) return;
    jclass c = env->GetObjectClass(j_obj); if(!c) return;
    jmethodID m = env->GetMethodID(c, "show", "()V");
    if (m) env->CallVoidMethod(j_obj, m);
    env->DeleteLocalRef(c);
}

void CMvdIdOverlay::hide() {
    JNIEnv* env = g_pJavaWrapper->GetEnv(); if (!env || !j_obj) return;
    jclass c = env->GetObjectClass(j_obj); if(!c) return;
    jmethodID m = env->GetMethodID(c, "hide", "()V");
    if (m) env->CallVoidMethod(j_obj, m);
    env->DeleteLocalRef(c);
}

void CMvdIdOverlay::updateId(const char* nicknameOneLine, const char* rankText) {
    JNIEnv* env = g_pJavaWrapper->GetEnv(); if (!env || !j_obj) return;
    jclass c = env->GetObjectClass(j_obj); if(!c) return;

    jmethodID m = env->GetMethodID(c, "updateId",
                                   "(Ljava/lang/String;Ljava/lang/String;)V");
    if(!m){ env->DeleteLocalRef(c); return; }

    jstring jFio  = JStringFromCP1251(env, nicknameOneLine);
    jstring jRank = JStringFromCP1251(env, rankText);

    env->CallVoidMethod(j_obj, m, jFio, jRank);

    env->DeleteLocalRef(jFio);
    env->DeleteLocalRef(jRank);
    env->DeleteLocalRef(c);
}

void CMvdIdOverlay::renderSkin(int skinId) {
    JNIEnv* env = g_pJavaWrapper->GetEnv(); if (!env || !j_obj) return;
    jclass c = env->GetObjectClass(j_obj); if(!c) return;
    jmethodID m = env->GetMethodID(c, "renderSkin", "(I)V");
    if (m) env->CallVoidMethod(j_obj, m, (jint)skinId);
    env->DeleteLocalRef(c);
}

// JNI: должен соответствовать package/class/method из Java (MvdIdOverlay.init)
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_MvdIdOverlay_init(JNIEnv* env, jobject thiz) {
    if (CMvdIdOverlay::j_obj) {
        env->DeleteGlobalRef(CMvdIdOverlay::j_obj);
        CMvdIdOverlay::j_obj = nullptr;
    }
    CMvdIdOverlay::j_obj = env->NewGlobalRef(thiz);
}
