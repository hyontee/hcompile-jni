#pragma once
#ifndef GTECH_BY_CROSS_CFAMILYACTION_H
#define GTECH_BY_CROSS_CFAMILYACTION_H

#include <jni.h>
#include <string>

class CFamilyAction
{
public:
    static void Show(const char* name, int reputation, int money, int skinId); // ✅ добавили 4-й параметр
    static void Hide();

private:
    static jobject jInstance;
    static jclass jClass;
    static bool bIsShow;

    static std::string sFamilyName;
    static int iFamilyReputation;
    static int iFamilyMoney;
};


#endif // GTECH_BY_CROSS_CFAMILYACTION_H
