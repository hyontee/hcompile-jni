#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"

#include "CBattlePass.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CBattlePass::activity;

void CNetGame::packetBattlePass(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CBattlePass::bpHide();
	else if (typeInfo == 1)
	{
		char text[256];
		memset(text, 0, 256);

		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		bool button;
		bs.Read(button);
		
		char text2[256];
		memset(text2, 0, 256);

		bs.Read(bLen);
		bs.Read(text2, bLen);
		text2[bLen] = '\0';
		
		char text3[256];
		memset(text3, 0, 256);

		bs.Read(bLen);
		bs.Read(text3, bLen);
		text3[bLen] = '\0';

		CBattlePass::bpShow(text, button, text2, text3);
	}
	else if (typeInfo == 2)
	{
		bool show;
		bs.Read(show);
		
		char text[4][2000];
		for (int i = 0; i < 4; i ++)
		{
			memset(text[i], 0, 2000);
			
			uint16_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
			
		CBattlePass::buyBattlePass(show, text[0], text[1], text[2], text[3]);
	}
	else if (typeInfo == 3)
	{
		int8_t position;
		bs.Read(position);
		
		bool status;
		bs.Read(status);
		
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
		
		char text[256];
		memset(text, 0, 256);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		char text2[256];
		memset(text2, 0, 256);
		
		bs.Read(bLen);
		bs.Read(text2, bLen);
		text2[bLen] = '\0';
		
		CBattlePass::updateBuyBattlePass(position, status, type, model, color1, color2, rotX, rotY, rotZ, zoom, text, text2);
	}
	else if (typeInfo == 4)
	{
		int8_t selectId;
		bs.Read(selectId);
		
		CBattlePass::selectBattlePass(selectId);
	}
	else if (typeInfo == 5)
	{
		char text[3][256];
		for (int i = 0; i < 3; i ++) memset(text[i], 0, 256);
		for (int i = 0; i < 2; i ++)
		{
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		bool limitButton;
		bs.Read(limitButton);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text[2], bLen);
		text[2][bLen] = '\0';
		
		CBattlePass::bpUpdateMainLayout(text[0], text[1], limitButton, text[2]);
	}
	else if (typeInfo == 6)
	{
		int8_t type;
		bs.Read(type);
		
		char text[6][256];
		for (int i = 0; i < 6; i ++) memset(text[i], 0, 256);
		for (int i = 0; i < 4; i ++)
		{
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[i], bLen);
			text[i][bLen] = '\0';
		}
		
		int32_t max;
		bs.Read(max);
		
		int32_t progress;
		bs.Read(progress);
		
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text[4], bLen);
		text[4][bLen] = '\0';
		
		bs.Read(bLen);
		bs.Read(text[5], bLen);
		text[5][bLen] = '\0';
		
		CBattlePass::bpUpdateDayTaskLayout(type, text[0], text[1], text[2], text[3], max, progress, text[4], text[5]);
	}
	else if (typeInfo == 7)
	{
		int8_t position;
		bs.Read(position);
		
		bool status;
		bs.Read(status);
		
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
		
		char text[256];
		memset(text, 0, 256);

		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CBattlePass::bpUpdateDayTasksPrize(position, status, type, model, color1, color2, rotX, rotY, rotZ, zoom, text);
	}
	else if (typeInfo == 8)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int q = 0; q < max_items; q ++)
		{
			char text[4][2000];
			for (int i = 0; i < 4; i ++) memset(text[i], 0, 2000);
			for (int i = 0; i < 3; i ++)
			{
				uint16_t bLen;
				bs.Read(bLen);
				bs.Read(text[i], bLen);
				text[i][bLen] = '\0';
			}
			
			int32_t max;
			bs.Read(max);
			
			int32_t progress;
			bs.Read(progress);
			
			int8_t type;
			bs.Read(type);
			
			int32_t model;
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
			
			int8_t rareLevel;
			bs.Read(rareLevel);
			
			uint16_t bLen;
			bs.Read(bLen);
			bs.Read(text[3], bLen);
			text[3][bLen] = '\0';
			
			CBattlePass::bpAddTasks(text[0], text[1], text[2], max, progress, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, text[3]);
		}
	}
	else if (typeInfo == 9)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int q = 0; q < max_items; q ++)
		{
			int8_t position;
			bs.Read(position);
			
			char text[2][2000];
			for (int i = 0; i < 2; i ++)
			{
				memset(text[i], 0, 2000);
				
				uint16_t bLen;
				bs.Read(bLen);
				bs.Read(text[i], bLen);
				text[i][bLen] = '\0';
			}
			
			CBattlePass::bpAddGuideMainLayout(position, text[0], text[1]);
		}
	}
	else if (typeInfo == 10)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
		{
			char text[5][256];
			for (int i = 0; i < 5; i ++) memset(text[i], 0, 256);
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
			
			uint32_t max;
			bs.Read(max);
			
			uint32_t progress;
			bs.Read(progress);
			
			bs.Read(bLen);
			bs.Read(text[4], bLen);
			text[4][bLen] = '\0';
					
			CBattlePass::bpAddBPItem(text[0], text[1], text[2], state, rare, icon, text[3], premState, premRare, premIcon, render, renderPrem, headerLvl, max, progress, text[4]);
		}
	}
	else if (typeInfo == 11)
	{
		int8_t position;
		bs.Read(position);
		
		char text[5][256];
		for (int i = 0; i < 5; i ++) memset(text[i], 0, 256);
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
			
		uint32_t max;
		bs.Read(max);
			
		uint32_t progress;
		bs.Read(progress);
			
		bs.Read(bLen);
		bs.Read(text[4], bLen);
		text[4][bLen] = '\0';
				
		CBattlePass::bpUpdateBPItem(position, text[0], text[1], text[2], state, rare, icon, text[3], premState, premRare, premIcon, render, renderPrem, headerLvl, max, progress, text[4]);
	}
	else if (typeInfo == 12) CBattlePass::caseRouletteHide();
	else if (typeInfo == 13)
	{
		uint32_t itemLevel;
		bs.Read(itemLevel);
		
		bool canRoll;
		bs.Read(canRoll);
		
		uint32_t maxItems;
		bs.Read(maxItems);
		
		CBattlePass::caseRouletteShow(itemLevel, canRoll, maxItems);
	}
	else if (typeInfo == 14)
	{
		uint32_t model;
		bs.Read(model);
			
		char text[256];
		memset(text, 0, 256);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
		
		CBattlePass::caseRouletteShowPrize(model, text);
	}
	else if (typeInfo == 15)
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
			
			char text[256];
			memset(text, 0, 256);
				
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';
					
			CBattlePass::caseRouletteAddItem(type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, text);
		}
	}
	else if (typeInfo == 16)
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
			
		char text[256];
		memset(text, 0, 256);
				
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';
					
		CBattlePass::caseRouletteGlass(type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, text);
	}
}

void CBattlePass::bpHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::bpShow(char* text, bool button, char* text2, char* text3)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jbyteArray bytes3 = env->NewByteArray(strlen(text3));
    env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpShow"), OBFUSCATE("([BZ[B[B)V"));
	env->CallVoidMethod(activity, method, bytes, button, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::buyBattlePass(bool show, char* text, char* text2, char* text3, char* text4)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jbyteArray bytes3 = env->NewByteArray(strlen(text3));
    env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jbyteArray bytes4 = env->NewByteArray(strlen(text4));
    env->SetByteArrayRegion(bytes4, 0, strlen(text4), (jbyte*)text4);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("buyBattlePass"), OBFUSCATE("(Z[B[B[B[B)V"));
	env->CallVoidMethod(activity, method, show, bytes, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::updateBuyBattlePass(int position, bool status, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* text, char* text2)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("updateBuyBattlePass"), OBFUSCATE("(IZIIIIFFFF[B[B)V"));
	env->CallVoidMethod(activity, method, position, status, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::selectBattlePass(int selectId)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("selectBattlePass"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, selectId);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::bpUpdateMainLayout(char* text, char* text2, bool limitButton, char* text3)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jbyteArray bytes3 = env->NewByteArray(strlen(text3));
    env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpUpdateMainLayout"), OBFUSCATE("([B[BZ[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, limitButton, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::bpUpdateDayTaskLayout(int type, char* text, char* text2, char* text3, char* text4, int max, int progress, char* text5, char* text6)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jbyteArray bytes3 = env->NewByteArray(strlen(text3));
    env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jbyteArray bytes4 = env->NewByteArray(strlen(text4));
    env->SetByteArrayRegion(bytes4, 0, strlen(text4), (jbyte*)text4);

    jbyteArray bytes5 = env->NewByteArray(strlen(text5));
    env->SetByteArrayRegion(bytes5, 0, strlen(text5), (jbyte*)text5);

    jbyteArray bytes6 = env->NewByteArray(strlen(text6));
    env->SetByteArrayRegion(bytes6, 0, strlen(text6), (jbyte*)text6);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpUpdateDayTaskLayout"), OBFUSCATE("(I[B[B[B[BII[B[B)V"));
	env->CallVoidMethod(activity, method, type, bytes, bytes2, bytes3, bytes4, max, progress, bytes5, bytes6);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::bpUpdateDayTasksPrize(int position, bool status, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* text)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpUpdateDayTasksPrize"), OBFUSCATE("(IZIIIIFFFF[B)V"));
	env->CallVoidMethod(activity, method, position, status, type, model, color1, color2, rotX, rotY, rotZ, zoom, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::bpAddTasks(char* text, char* text2, char* text3, int max, int progress, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, int rareLevel, char* text4)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jbyteArray bytes3 = env->NewByteArray(strlen(text3));
    env->SetByteArrayRegion(bytes3, 0, strlen(text3), (jbyte*)text3);

    jbyteArray bytes4 = env->NewByteArray(strlen(text4));
    env->SetByteArrayRegion(bytes4, 0, strlen(text4), (jbyte*)text4);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpAddTasks"), OBFUSCATE("([B[B[BIIIIIIFFFFI[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, max, progress, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::bpAddGuideMainLayout(int position, char* text, char* text2)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jbyteArray bytes2 = env->NewByteArray(strlen(text2));
    env->SetByteArrayRegion(bytes2, 0, strlen(text2), (jbyte*)text2);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpAddGuideMainLayout"), OBFUSCATE("(I[B[B)V"));
	env->CallVoidMethod(activity, method, position, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::bpAddBPItem(char* count, char* premCount, char* name, int8_t state, int8_t rare, uint32_t icon, char* premName, int8_t premState, int8_t premRare, uint32_t premIcon, int8_t render, int8_t renderPrem, int8_t headerLvl, int max, int progress, char* exp)
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

    jbyteArray bytes5 = env->NewByteArray(strlen(exp));
    env->SetByteArrayRegion(bytes5, 0, strlen(exp), (jbyte*)exp);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpAddBPItem"), OBFUSCATE("([B[B[BIII[BIIIIIIII[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, state, rare, icon, bytes4, premState, premRare, premIcon, render, renderPrem, headerLvl, max, progress, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::bpUpdateBPItem(int8_t position, char* count, char* premCount, char* name, int8_t state, int8_t rare, uint32_t icon, char* premName, int8_t premState, int8_t premRare, uint32_t premIcon, int8_t render, int8_t renderPrem, int8_t headerLvl, int max, int progress, char* exp)
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

    jbyteArray bytes5 = env->NewByteArray(strlen(exp));
    env->SetByteArrayRegion(bytes5, 0, strlen(exp), (jbyte*)exp);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("bpUpdateBPItem"), OBFUSCATE("(I[B[B[BIII[BIIIIIIII[B)V"));
	env->CallVoidMethod(activity, method, position, bytes, bytes2, bytes3, state, rare, icon, bytes4, premState, premRare, premIcon, render, renderPrem, headerLvl, max, progress, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::caseRouletteHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
   	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("caseRouletteHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::caseRouletteShow(uint32_t itemLevel, bool canRoll, uint32_t maxItems)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("caseRouletteShow"), OBFUSCATE("(IZI)V"));
	env->CallVoidMethod(activity, method, itemLevel, canRoll, maxItems);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::caseRouletteShowPrize(uint32_t model, char* name)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("caseRouletteShowPrize"), OBFUSCATE("(I[B)V"));
	env->CallVoidMethod(activity, method, model, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::caseRouletteAddItem(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("caseRouletteAddItem"), OBFUSCATE("(IIIIFFFFI[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CBattlePass::caseRouletteGlass(int8_t type, uint32_t model, uint8_t color1, uint8_t color2, float rotX, float rotY, float rotZ, float zoom, uint8_t rareLevel, char* name)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("caseRouletteGlass"), OBFUSCATE("(IIIIFFFFI[B)V"));
	env->CallVoidMethod(activity, method, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_battlepass_BattlePassManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_BattlePass);
					
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		bsParams.Write((int32_t)1);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
	
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_battlepass_CaseRouletteManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_BattlePass);
					
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		bsParams.Write((int32_t)1);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CBattlePass::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CBattlePass class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CBattlePass done!"));
}