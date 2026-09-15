#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CHud
{
	
public:
    static jobject activity;
    
	static void ShowHud();
	static void HideHud();
	static void UpdateDataHud(int health, int armour, int weaponid, int ammo, int ammoclip, int money, int wanted/*int iHP, int iArmour, int iHunger, int iWanted, int iWeapon, int iAllAmmo, int iAmmo, int iMoney, int iEventCoins, int iDonate, bool bEventUsed, bool bGZ, bool bE2y*/);
	static void hudButton(int i, int i2, int i3, int i4, int i5, int i6, int i7);
	static void buttonExtra(bool bVisible, bool bExtra, char* mes, int icon);
	static void giftLayout(int iconID, char* mes);
	static void giftHide();
	static void questShow(char* questCaption, char* questText, char* questButton, bool buttonCancel, bool showQuest);
	static void questHide();
	static void setActivity(JNIEnv* env, jobject thiz);
};