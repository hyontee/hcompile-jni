#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"

#include "CNewBattlePass.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CNewBattlePass::activity;

void CNetGame::packetNewBattlePass(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t id;
	bs.Read(id);
	if (id == 1) CNewBattlePass::nbpHide();
	else if (id == 2 || id == 3 || id == 4 || id == 5)
	{
		char text[3][256];
		for (int i = 0; i < 3; i ++) memset(text[i], 0, 256);
		for(int i = 0; i < 2; i ++)
		{
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		uint32_t score;
		bs.Read(score);
				
		uint32_t maxScore;
		bs.Read(maxScore);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text[2], bLen);
		text[2][bLen] = '\0';
		
		if (id == 2) CNewBattlePass::nbpShowMainPage(text[0], text[1], score, maxScore, text[2]);
		if (id == 3) CNewBattlePass::nbpShowTasksPage(text[0], text[1], score, maxScore, text[2]);
		if (id == 4) CNewBattlePass::nbpShowExpPage(text[0], text[1], score, maxScore, text[2]);
		if (id == 5) CNewBattlePass::nbpShowStatPage(text[0], text[1], score, maxScore, text[2]);
	}
	else if (id == 6)
	{
		uint32_t itemLevel;
		bs.Read(itemLevel);
				
		bool canRoll;
		bs.Read(canRoll);
				
		uint32_t cost;
		bs.Read(cost);
				
		CNewBattlePass::nbpShowRoulette(itemLevel, canRoll, cost);
	}
	else if (id == 7)
	{
		uint32_t rouletteID;
		bs.Read(rouletteID);
				
		CNewBattlePass::nbpShowCasePage(rouletteID);
	}
	else if (id == 8)
	{
		int8_t type;
		bs.Read(type);
				
		uint32_t model;
		bs.Read(model);
				
		uint8_t color1;
		bs.Read(color1);
				
		uint8_t color2;
		bs.Read(color2);
				
		char text[256];
		memset(text, 0, 256);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
				
		CNewBattlePass::nbpShowPrize(type, model, color1, color2, text);
	}
	else if (id == 9)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			char text[2][256];
			for (int q = 0; q < 2; q ++)
			{
				memset(text[q], 0, 256);
					
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(text[q], bLen);
				text[q][bLen] = '\0';
			}
					
			int8_t taskBlock;
			bs.Read(taskBlock);
					
			CNewBattlePass::nbpTaskAdd(text[0], text[1], taskBlock);
		}
	}
	else if (id == 10)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
			
			char text[2][256];
			for (int i = 0; i < 2; i ++)
			{
				memset(text[i], 0, 256);
						
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(text[i], bLen);
				text[i][bLen] = '\0';
			}
					
			CNewBattlePass::nbpSetExpInfo(position, text[0], text[1]);
		}
	}
	else if (id == 11)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t position;
			bs.Read(position);
					
			int8_t type;
			bs.Read(type);
					
			uint32_t model;
			bs.Read(model);
			
			char text[256];
			memset(text, 0, 256);
						
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
					
			int8_t rare;
			bs.Read(rare);
					
			int8_t blocked;
			bs.Read(blocked);
				
			CNewBattlePass::nbpSetCaseInfo(position, type, model, text, rare, blocked);
		}
	}
	else if (id == 12)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			char text[4][256];
			for (int i = 0; i < 4; i ++) memset(text[i], 0, 256);
			for (int i = 0; i < 3; i ++)
			{
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(text[i], bLen);
				text[i][bLen] = '\0';
			}
				
			int8_t state;
			bs.Read(state);
					
			int8_t rare;
			bs.Read(rare);
					
			uint32_t icon;
			bs.Read(icon);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[3], bLen);
			text[3][bLen] = '\0';
					
			int8_t premState;
			bs.Read(premState);
					
			int8_t premRare;
			bs.Read(premRare);
					
			uint32_t premIcon;
			bs.Read(premIcon);
					
			int8_t render;
			bs.Read(render);
					
			int8_t renderPrem;
			bs.Read(renderPrem);
				
			int8_t headerLvl;
			bs.Read(headerLvl);
					
			CNewBattlePass::nbpAddBPItem(text[0], text[1], text[2], state, rare, icon, text[3], premState, premRare, premIcon, render, renderPrem, headerLvl);
		}
	}
	else if (id == 13)
	{
		int8_t position;
		bs.Read(position);
	
		char text[4][256];
		for (int i = 0; i < 4; i ++) memset(text[i], 0, 256);
		for (int i = 0; i < 3; i ++)
		{
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
				
		int8_t state;
		bs.Read(state);
				
		int8_t rare;
		bs.Read(rare);
				
		uint32_t icon;
		bs.Read(icon);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text[3], bLen);
		text[3][bLen] = '\0';
			
		int8_t premState;
		bs.Read(premState);
				
		int8_t premRare;
		bs.Read(premRare);
				
		uint32_t premIcon;
		bs.Read(premIcon);
				
		int8_t render;
		bs.Read(render);
				
		int8_t renderPrem;
		bs.Read(renderPrem);
				
		int8_t headerLvl;
		bs.Read(headerLvl);
		
		CNewBattlePass::nbpUpdateBPItem(position, text[0], text[1], text[2], state, rare, icon, text[3], premState, premRare, premIcon, render, renderPrem, headerLvl);
	}
	else if (id == 14)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int q = 0; q < max_items; q ++)
		{
			char text[3][256];
			for (int i = 0; i < 3; i ++) memset(text[i], 0, 256);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[0], bLen);
			text[0][bLen] = '\0';
			
			int8_t premium;
			bs.Read(premium);
					
			for (int i = 1; i < 3; i ++)
			{
				bs.Read(bLen);
				bs.Read(text[i], bLen);
				text[i][bLen] = '\0';
			}
					
			CNewBattlePass::nbpAddStatItem(text[0], premium, text[1], text[2]);
		}
	}
	else if (id == 15)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			int8_t type;
			bs.Read(type);
			
			uint32_t model;
			bs.Read(model);
					
			uint8_t color1;
			bs.Read(color1);
					
			uint8_t color2;
			bs.Read(color2);
					
			float rotX;
			bs.Read(rotX);
					
			float rotY;
			bs.Read(rotY);
					
			float rotZ;
			bs.Read(rotZ);
					
			float zoom;
			bs.Read(zoom);
					
			uint8_t rareLevel;
			bs.Read(rareLevel);
			
			CNewBattlePass::nbpAddRouletteItem(type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel);
		}
	}
	else if (id == 16)
	{
		char text[4][256];
		for (int i = 0; i < 4; i ++)
		{
			 memset(text[i], 0, 256);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
				
		CNewBattlePass::nbpDialog(text[0], text[1], text[2], text[3]);
	}
}

void CNewBattlePass::nbpHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpShowMainPage(char* strEnd, char* lvl, uint32_t score, uint32_t maxScore, char* txtPrize)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(strEnd));
    env->SetByteArrayRegion(bytes, 0, strlen(strEnd), (jbyte*)strEnd);

    jbyteArray bytes2 = env->NewByteArray(strlen(lvl));
    env->SetByteArrayRegion(bytes2, 0, strlen(lvl), (jbyte*)lvl);

    jbyteArray bytes3 = env->NewByteArray(strlen(txtPrize));
    env->SetByteArrayRegion(bytes3, 0, strlen(txtPrize), (jbyte*)txtPrize);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpShowMainPage"), OBFUSCATE("([B[BII[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, score, maxScore, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpShowTasksPage(char* strEnd, char* lvl, uint32_t score, uint32_t maxScore, char* txtPrize)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(strEnd));
    env->SetByteArrayRegion(bytes, 0, strlen(strEnd), (jbyte*)strEnd);

    jbyteArray bytes2 = env->NewByteArray(strlen(lvl));
    env->SetByteArrayRegion(bytes2, 0, strlen(lvl), (jbyte*)lvl);

    jbyteArray bytes3 = env->NewByteArray(strlen(txtPrize));
    env->SetByteArrayRegion(bytes3, 0, strlen(txtPrize), (jbyte*)txtPrize);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpShowTasksPage"), OBFUSCATE("([B[BII[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, score, maxScore, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpShowExpPage(char* strEnd, char* lvl, uint32_t score, uint32_t maxScore, char* txtPrize)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(strEnd));
    env->SetByteArrayRegion(bytes, 0, strlen(strEnd), (jbyte*)strEnd);

    jbyteArray bytes2 = env->NewByteArray(strlen(lvl));
    env->SetByteArrayRegion(bytes2, 0, strlen(lvl), (jbyte*)lvl);

    jbyteArray bytes3 = env->NewByteArray(strlen(txtPrize));
    env->SetByteArrayRegion(bytes3, 0, strlen(txtPrize), (jbyte*)txtPrize);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpShowExpPage"), OBFUSCATE("([B[BII[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, score, maxScore, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpShowStatPage(char* strEnd, char* lvl, uint32_t score, uint32_t maxScore, char* txtPrize)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(strEnd));
    env->SetByteArrayRegion(bytes, 0, strlen(strEnd), (jbyte*)strEnd);

    jbyteArray bytes2 = env->NewByteArray(strlen(lvl));
    env->SetByteArrayRegion(bytes2, 0, strlen(lvl), (jbyte*)lvl);

    jbyteArray bytes3 = env->NewByteArray(strlen(txtPrize));
    env->SetByteArrayRegion(bytes3, 0, strlen(txtPrize), (jbyte*)txtPrize);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpShowStatPage"), OBFUSCATE("([B[BII[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, score, maxScore, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpShowRoulette(uint32_t itemLevel, bool canRoll, uint32_t cost)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpShowRoulette"), OBFUSCATE("(IZI)V"));
	env->CallVoidMethod(activity, method, itemLevel, canRoll, cost);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpShowCasePage(uint32_t rouletteID)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpShowCasePage"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, rouletteID);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpShowPrize(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, char* name)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpShowPrize"), OBFUSCATE("(IIII[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpTaskAdd(char* taskName, char* taskProgress, int8_t taskBlock)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(taskName));
    env->SetByteArrayRegion(bytes, 0, strlen(taskName), (jbyte*)taskName);

    jbyteArray bytes2 = env->NewByteArray(strlen(taskProgress));
    env->SetByteArrayRegion(bytes2, 0, strlen(taskProgress), (jbyte*)taskProgress);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpTaskAdd"), OBFUSCATE("([B[BI)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, taskBlock);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpSetExpInfo(int8_t position, char* lvl, char* price)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(lvl));
    env->SetByteArrayRegion(bytes, 0, strlen(lvl), (jbyte*)lvl);

    jbyteArray bytes2 = env->NewByteArray(strlen(price));
    env->SetByteArrayRegion(bytes2, 0, strlen(price), (jbyte*)price);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpSetExpInfo"), OBFUSCATE("(I[B[B)V"));
	env->CallVoidMethod(activity, method, position, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpSetCaseInfo(int8_t position, int8_t type, uint32_t model, char* name, int8_t rare, int8_t blocked)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpSetCaseInfo"), OBFUSCATE("(III[BII)V"));
	env->CallVoidMethod(activity, method, position, type, model, bytes, rare, blocked);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpAddBPItem(char* count, char* premCount, char* name, int8_t state, int8_t rare, uint32_t icon, char* premName, int8_t premState, int8_t premRare, uint32_t premIcon, int8_t render, int8_t renderPrem, int8_t headerLvl)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(count));
    env->SetByteArrayRegion(bytes, 0, strlen(count), (jbyte*)count);

    jbyteArray bytes2 = env->NewByteArray(strlen(premCount));
    env->SetByteArrayRegion(bytes2, 0, strlen(premCount), (jbyte*)premCount);

    jbyteArray bytes3 = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes3, 0, strlen(name), (jbyte*)name);

    jbyteArray bytes4 = env->NewByteArray(strlen(premName));
    env->SetByteArrayRegion(bytes4, 0, strlen(premName), (jbyte*)premName);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpAddBPItem"), OBFUSCATE("([B[B[BIII[BIIIIII)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, state, rare, icon, bytes4, premState, premRare, premIcon, render, renderPrem, headerLvl);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpUpdateBPItem(int8_t position, char* count, char* premCount, char* name, int8_t state, int8_t rare, uint32_t icon, char* premName, int8_t premState, int8_t premRare, uint32_t premIcon, int8_t render, int8_t renderPrem, int8_t headerLvl)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(count));
    env->SetByteArrayRegion(bytes, 0, strlen(count), (jbyte*)count);

    jbyteArray bytes2 = env->NewByteArray(strlen(premCount));
    env->SetByteArrayRegion(bytes2, 0, strlen(premCount), (jbyte*)premCount);

    jbyteArray bytes3 = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes3, 0, strlen(name), (jbyte*)name);

    jbyteArray bytes4 = env->NewByteArray(strlen(premName));
    env->SetByteArrayRegion(bytes4, 0, strlen(premName), (jbyte*)premName);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpUpdateBPItem"), OBFUSCATE("(I[B[B[BIII[BIIIIII)V"));
	env->CallVoidMethod(activity, method, position, bytes, bytes2, bytes3, state, rare, icon, bytes4, premState, premRare, premIcon, render, renderPrem, headerLvl);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpAddStatItem(char* name, int8_t premium, char* lvl, char* score)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);

    jbyteArray bytes2 = env->NewByteArray(strlen(lvl));
    env->SetByteArrayRegion(bytes2, 0, strlen(lvl), (jbyte*)lvl);

    jbyteArray bytes3 = env->NewByteArray(strlen(score));
    env->SetByteArrayRegion(bytes3, 0, strlen(score), (jbyte*)score);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpAddStatItem"), OBFUSCATE("([BI[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, premium, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpAddRouletteItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpAddRouletteItem"), OBFUSCATE("(IIIIFFFFI)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBattlePass::nbpDialog(char* caption, char* text, char* btnUse, char* btnCancel)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(caption));
    env->SetByteArrayRegion(bytes, 0, strlen(caption), (jbyte*)caption);

    jbyteArray bytes2 = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes2, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes3 = env->NewByteArray(strlen(btnUse));
    env->SetByteArrayRegion(bytes3, 0, strlen(btnUse), (jbyte*)btnUse);

    jbyteArray bytes4 = env->NewByteArray(strlen(btnCancel));
    env->SetByteArrayRegion(bytes4, 0, strlen(btnCancel), (jbyte*)btnCancel);
	
    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("nbpDialog"), OBFUSCATE("([B[B[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_newbattlepass_NewBattlePassManager_HideResponse(JNIEnv* pEnv, jobject thiz)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_BattlePass);
					
		bsParams.Write((int32_t)255);
		bsParams.Write((int32_t)255);
		bsParams.Write((int32_t)255);
		bsParams.Write((int32_t)2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_newbattlepass_NewBattlePassManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_BattlePass);
					
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		bsParams.Write((int32_t)2);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CNewBattlePass::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CNewBattlePass class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CNewBattlePass done!"));
}