/*
 ╔════════════════════════════════════════════════╗
 ║               ☠ GTECH MOBILE ☠              ║
 ║    Developed by CROSS | @namedspace             ║
 ║         Built for mobile CRMP engine           ║
 ╚════════════════════════════════════════════════╝
*/


#include "main.h"
#include "MobileMenu.h"
#include "patch.h"
#include "CJavaWrapper.h"
#include "java_systems/CHUD.h"

bool m_bCloseMap = false;
bool m_bInitForMap = false;

jobject MobileMenu::thiz = nullptr;

void (*MobileMenu_InitForPause)(uintptr_t* thiz);
void MobileMenu_InitForPause_hook(uintptr_t* thiz)
{
    if (m_bInitForMap) {
        MobileMenu_InitForPause(thiz);
        *(uint8_t *) ((uintptr_t) thiz + 0x6D) = 1;
    }
    else {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        if (!env)
        {
            Log("No env");
            return;
        }
        jclass Pause = env->GetObjectClass(MobileMenu::thiz);

        jmethodID show = env->GetMethodID(Pause, "show", "()V");
        env->CallVoidMethod(MobileMenu::thiz, show);
    }
}

void (*MobileMenu_DrawBack)(uintptr_t* thiz, bool wrap);
void MobileMenu_DrawBack_hook(uintptr_t* thiz, bool wrap)
{
    //����� ��� � ����� ������� ��� ����������, �� � ����� ��� ;)
}

void (*MobileMenu_Update)(uintptr_t* thiz);
void MobileMenu_Update_hook(uintptr_t* thiz) {
    if (m_bInitForMap) {
        CHook::CallFunction<void>("_ZN10MobileMenu12InitForPauseEv", (g_libGTASA + 0x6E0074));
        m_bInitForMap = false;
    }
    if (m_bCloseMap) {
        CHook::CallFunction<void>("_ZN14MainMenuScreen8OnResumeEv");
        m_bCloseMap = false;
        CHUD::toggleAll(false);
    }
    return MobileMenu_Update(thiz);
}

void MobileMenu::InjectMobileMenuHooks(){
    CHook::HookFunc(OBFUSCATE("_ZN10MobileMenu12InitForPauseEv"), &MobileMenu_InitForPause_hook, &MobileMenu_InitForPause);
    CHook::HookFunc(OBFUSCATE("_ZN10MobileMenu6UpdateEv"), &MobileMenu_Update_hook, &MobileMenu_Update);
    CHook::Redirect(OBFUSCATE("_ZN10MenuScreen8DrawBackEb"), &MobileMenu_DrawBack_hook);
}

extern "C"
{
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_mobilemenu_PauseManager_initPause(JNIEnv *env, jobject thiz) {MobileMenu::thiz = env->NewGlobalRef(thiz);}
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_mobilemenu_PauseManager_closeMap(JNIEnv *env, jobject thiz) {m_bCloseMap = true; }
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_mobilemenu_PauseManager_initMap(JNIEnv *env, jobject thiz) {m_bInitForMap = true; }
}