// app/src/main/cpp/samp/java_systems/CPassportOverlay.cpp
#include "CPassportOverlay.h"
#include "main.h"
#include "CJavaWrapper.h"
#include "Statistics.h"
#include <string>
#include <vector>

jobject CPassportOverlay::j_passport = nullptr;

// ---- CP1251 ? UTF-16 (BMP) ----
static jstring JStringFromCP1251(JNIEnv* env, const char* s) {
    if (!s) s = "";
    static const uint16_t map[128] = {
            0x0402,0x0403,0x201A,0x0453,0x201E,0x2026,0x2020,0x2021,
            0x20AC,0x2030,0x0409,0x2039,0x040A,0x040C,0x040B,0x040F,
            0x0452,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
            0x0000,0x2122,0x0459,0x203A,0x045A,0x045C,0x045B,0x045F,
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

    // �����
    const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
    std::u16string u16;
    u16.reserve(strlen(s));

    for (; *p; ++p) {
        unsigned char c = *p;
        uint16_t cp;
        if (c < 0x80) {
            cp = c; // ASCII
        } else {
            cp = map[c - 0x80];
            if (cp == 0) cp = u'?'; // ���������� ������ ��� ������
        }
        u16.push_back(static_cast<char16_t>(cp));
    }

    return env->NewString(reinterpret_cast<const jchar*>(u16.data()), (jsize)u16.size());
}

void CPassportOverlay::show(){
    JNIEnv* env = g_pJavaWrapper->GetEnv(); if(!env || !j_passport) return;
    jclass c = env->GetObjectClass(j_passport); if(!c) return;
    jmethodID m = env->GetMethodID(c, "show", "()V");
    if(m) env->CallVoidMethod(j_passport, m);
    env->DeleteLocalRef(c);
}

void CPassportOverlay::hide(){
    JNIEnv* env = g_pJavaWrapper->GetEnv(); if(!env || !j_passport) return;
    jclass c = env->GetObjectClass(j_passport); if(!c) return;
    jmethodID m = env->GetMethodID(c, "hide", "()V");
    if(m) env->CallVoidMethod(j_passport, m);
    env->DeleteLocalRef(c);
}

void CPassportOverlay::updatePassport(const char* surname,
                                      const char* name,
                                      int lawAbidance,
                                      const char* militaryTicket,
                                      const char* fraction,
                                      int yearsInRegion,
                                      const char* sex,
                                      const char* issueDate,
                                      const char* serialNumber)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv(); if(!env || !j_passport) return;
    jclass c = env->GetObjectClass(j_passport); if(!c) return;

    jmethodID m = env->GetMethodID(
            c, "updatePassport",
            "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"
    );
    if(!m){ env->DeleteLocalRef(c); return; }

    jstring jSurname = JStringFromCP1251(env, surname);
    jstring jName    = JStringFromCP1251(env, name);
    jstring jMil     = JStringFromCP1251(env, militaryTicket);
    jstring jFrac    = JStringFromCP1251(env, fraction);
    jstring jSex     = JStringFromCP1251(env, sex);
    jstring jIssue   = JStringFromCP1251(env, issueDate);
    jstring jSerial  = JStringFromCP1251(env, serialNumber);

    env->CallVoidMethod(j_passport, m, jSurname, jName, (jint)lawAbidance,
                        jMil, jFrac, (jint)yearsInRegion, jSex, jIssue, jSerial);

    env->DeleteLocalRef(jSurname);
    env->DeleteLocalRef(jName);
    env->DeleteLocalRef(jMil);
    env->DeleteLocalRef(jFrac);
    env->DeleteLocalRef(jSex);
    env->DeleteLocalRef(jIssue);
    env->DeleteLocalRef(jSerial);
    env->DeleteLocalRef(c);
}

void CPassportOverlay::renderSkin(int skinId)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv(); if(!env || !j_passport) return;
    jclass c = env->GetObjectClass(j_passport); if(!c) return;
    jmethodID m = env->GetMethodID(c, "renderSkin", "(I)V");
    if(m) env->CallVoidMethod(j_passport, m, (jint)skinId);
    env->DeleteLocalRef(c);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_PassportOverlay_init(JNIEnv* env, jobject thiz){
    if (CPassportOverlay::j_passport) {
        env->DeleteGlobalRef(CPassportOverlay::j_passport);
        CPassportOverlay::j_passport = nullptr;
    }
    CPassportOverlay::j_passport = env->NewGlobalRef(thiz);
}
