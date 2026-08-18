#include "CFamilyAction.h"
#include "../util/CJavaWrapper.h"
#include "../main.h"

jobject CFamilyAction::jInstance = nullptr;
jclass  CFamilyAction::jClass    = nullptr;
bool    CFamilyAction::bIsShow   = false;

std::string CFamilyAction::sFamilyName = "";
int CFamilyAction::iFamilyReputation   = 0;
int CFamilyAction::iFamilyMoney        = 0;

extern CJavaWrapper* g_pJavaWrapper;

void CFamilyAction::Show(const char* name, int reputation, int money, int skinId)
{
    if (bIsShow)
    {
        Log("CFamilyAction::Show(): ⚠ Уже открыто");
        return;
    }

    JNIEnv* env = g_pJavaWrapper ? g_pJavaWrapper->GetEnv() : nullptr;
    if (!env)
    {
        Log("CFamilyAction::Show(): ❌ env == nullptr");
        return;
    }

    jobject activityObj = g_pJavaWrapper->activity;
    if (!activityObj)
    {
        Log("CFamilyAction::Show(): ❌ activity == nullptr");
        return;
    }

    // 1️⃣ Перед открытием — тихо обновим снапшот
    jclass sampCls = env->FindClass("com/criminal/moscow/core/Samp");
    if (sampCls)
    {
        jmethodID midSetSkin = env->GetStaticMethodID(sampCls, "setLocalPlayerSkinStatic", "(I)V");
        jmethodID midUpdate  = env->GetStaticMethodID(sampCls, "updatePlayerSnapshotSilentlyStatic", "()V");

        if (midSetSkin) env->CallStaticVoidMethod(sampCls, midSetSkin, (jint)skinId);
        if (midUpdate)  env->CallStaticVoidMethod(sampCls, midUpdate);

        env->DeleteLocalRef(sampCls);
    }

    // 2️⃣ Создание FamilyAction
    jclass tmpCls = env->FindClass("com/criminal/moscow/gui/family/FamilyAction");
    if (!tmpCls)
    {
        Log("CFamilyAction::Show(): ❌ Класс FamilyAction не найден");
        return;
    }

    jClass = reinterpret_cast<jclass>(env->NewGlobalRef(tmpCls));
    env->DeleteLocalRef(tmpCls);

    jmethodID ctor = env->GetMethodID(jClass, "<init>", "(Landroid/content/Context;Ljava/lang/String;II)V");
    if (!ctor)
    {
        Log("CFamilyAction::Show(): ❌ Конструктор не найден");
        env->DeleteGlobalRef(jClass);
        jClass = nullptr;
        return;
    }

    jstring jName = env->NewStringUTF(name ? name : "Без семьи");
    jobject obj = env->NewObject(jClass, ctor, activityObj, jName, (jint)reputation, (jint)money);
    env->DeleteLocalRef(jName);

    if (!obj)
    {
        Log("CFamilyAction::Show(): ❌ Не удалось создать объект FamilyAction");
        env->DeleteGlobalRef(jClass);
        jClass = nullptr;
        return;
    }

    jInstance = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);

    // 3️⃣ Показываем меню
    jmethodID showView = env->GetMethodID(jClass, "showView", "()V");
    if (showView)
        env->CallVoidMethod(jInstance, showView);

    sFamilyName       = name ? name : "Без семьи";
    iFamilyReputation = reputation;
    iFamilyMoney      = money;
    bIsShow           = true;

    Log("CFamilyAction::Show(): ✅ Открыто '%s' rep=%d money=%d skin=%d",
        sFamilyName.c_str(), iFamilyReputation, iFamilyMoney, skinId);
}

void CFamilyAction::Hide()
{
    if (!bIsShow || !jInstance)
    {
        Log("CFamilyAction::Hide(): ⚠ Уже скрыто");
        return;
    }

    JNIEnv* env = g_pJavaWrapper ? g_pJavaWrapper->GetEnv() : nullptr;
    if (!env)
    {
        Log("CFamilyAction::Hide(): ❌ env == nullptr");
        return;
    }

    jmethodID hideMethod = env->GetMethodID(jClass, "hideViewWithAnim", "()V");
    if (hideMethod)
        env->CallVoidMethod(jInstance, hideMethod);

    env->DeleteGlobalRef(jInstance);
    env->DeleteGlobalRef(jClass);
    jInstance = nullptr;
    jClass    = nullptr;
    bIsShow   = false;

    Log("CFamilyAction::Hide(): 🧹 Скрыто");
}
