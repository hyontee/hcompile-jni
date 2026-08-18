#include "CFamilyMenu.h"
#include "main.h"
#include "CJavaWrapper.h"
#include "game/game.h"
#include "../net/netgame.h"

jobject CFamilyMenu::j_family_menu = nullptr;
jclass  CFamilyMenu::j_class       = nullptr;
bool    CFamilyMenu::bIsShow       = false;

std::string CFamilyMenu::sFamilyName;
int         CFamilyMenu::iFamilyBalance    = 0;
int         CFamilyMenu::iFamilyReputation = 0;
int         CFamilyMenu::iFamilySkin       = 0;

extern CJavaWrapper* g_pJavaWrapper;

void CFamilyMenu::show(const char* name, int balance, int reputation)
{
    show(name, balance, reputation, 0);
}

void CFamilyMenu::show(const char* name, int balance, int reputation, int skinId)
{
    JNIEnv* env = g_pJavaWrapper ? g_pJavaWrapper->GetEnv() : nullptr;
    if (!env || !j_family_menu || !j_class) return;

    sFamilyName = name ? name : "Без семьи";
    iFamilyBalance = balance;
    iFamilyReputation = reputation;
    iFamilySkin = skinId;

    jmethodID setData = env->GetMethodID(j_class, "setFamilyData", "(Ljava/lang/String;II)V");
    if (setData)
    {
        jstring jName = env->NewStringUTF(sFamilyName.c_str());
        env->CallVoidMethod(j_family_menu, setData, jName, (jint)iFamilyBalance, (jint)iFamilyReputation);
        env->DeleteLocalRef(jName);
    }

    jmethodID showView = env->GetMethodID(j_class, "showView", "()V");
    if (showView)
        env->CallVoidMethod(j_family_menu, showView);

    bIsShow = true;
}

void CFamilyMenu::hide()
{
    JNIEnv* env = g_pJavaWrapper ? g_pJavaWrapper->GetEnv() : nullptr;
    if (!env || !j_family_menu || !j_class) return;

    jmethodID hideView = env->GetMethodID(j_class, "hideViewWithAnim", "()V");
    if (hideView)
        env->CallVoidMethod(j_family_menu, hideView);

    bIsShow = false;
}

void CFamilyMenu::destroy()
{
    JNIEnv* env = g_pJavaWrapper ? g_pJavaWrapper->GetEnv() : nullptr;
    if (!env) return;

    if (j_family_menu)
    {
        env->DeleteGlobalRef(j_family_menu);
        j_family_menu = nullptr;
    }

    if (j_class)
    {
        env->DeleteGlobalRef(j_class);
        j_class = nullptr;
    }

    bIsShow = false;
}

extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_family_FamilyMenu_init(JNIEnv* env, jobject /*unused*/, jobject instance)
{
    if (CFamilyMenu::j_family_menu)
        env->DeleteGlobalRef(CFamilyMenu::j_family_menu);

    CFamilyMenu::j_family_menu = env->NewGlobalRef(instance);
    CFamilyMenu::j_class = reinterpret_cast<jclass>(env->NewGlobalRef(env->GetObjectClass(instance)));

    Log("FAMILY_MENU: JNI init linked to FamilyMenu instance");
}

extern "C" JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_family_FamilyMenu_nativeHide(JNIEnv* env, jobject /*unused*/)
{
    if (!pNetGame) return;
    RakNet::BitStream bs;
    bs.Write((uint32_t)RPC_FAMILY_MENU);
    bs.Write((uint8_t)0);
    pNetGame->GetRakClient()->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
}
