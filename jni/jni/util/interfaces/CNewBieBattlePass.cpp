#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/snapshothelper.h"

#include "CNewBieBattlePass.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CNewBieBattlePass::activity;

void CNetGame::packetNewBieBattle(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	if (typeInfo == 0) CNewBieBattlePass::bpHide();
	else if (typeInfo == 1)
	{
		char text[256];
		memset(text, 0, 256);

		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text, bLen);
		text[bLen] = '\0';

		CNewBieBattlePass::bpShow(text);
	}
	else if (typeInfo == 2)
	{
		int8_t selectId;
		bs.Read(selectId);
		
		CNewBieBattlePass::selectBattlePass(selectId);
	}
	else if (typeInfo == 3)
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
		
		CNewBieBattlePass::bpUpdateMainLayout(text[0], text[1], limitButton, text[2]);
	}
	else if (typeInfo == 4)
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
			
			CNewBieBattlePass::bpAddTasks(text[0], text[1], text[2], max, progress, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, text[3]);
		}
	}
	else if (typeInfo == 5)
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
			
			CNewBieBattlePass::bpAddGuideMainLayout(position, text[0], text[1]);
		}
	}
	else if (typeInfo == 6)
	{
		uint8_t max_items;
		bs.Read(max_items);
		for (int i = 0; i < max_items; i ++)
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
				
			int8_t state;
			bs.Read(state);
					
			int8_t rare;
			bs.Read(rare);
		
			uint32_t icon;
			bs.Read(icon);
		
			int8_t render;
			bs.Read(render);
		
			int8_t headerLvl;
			bs.Read(headerLvl);
			
			uint32_t max;
			bs.Read(max);
			
			uint32_t progress;
			bs.Read(progress);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(text[2], bLen);
			text[2][bLen] = '\0';
					
			CNewBieBattlePass::bpAddBPItem(text[0], text[1], state, rare, icon, render, headerLvl, max, progress, text[2]);
		}
	}
	else if (typeInfo == 7)
	{
		int8_t position;
		bs.Read(position);
		
		char text[3][256];
		for (int i = 0; i < 3; i ++) memset(text[i], 0, 256);
		for (int i = 0; i < 2; i ++)
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
		
		int8_t render;
		bs.Read(render);

		int8_t headerLvl;
		bs.Read(headerLvl);
			
		uint32_t max;
		bs.Read(max);
			
		uint32_t progress;
		bs.Read(progress);
			
		uint8_t bLen;
		bs.Read(bLen);
		bs.Read(text[2], bLen);
		text[2][bLen] = '\0';
				
		CNewBieBattlePass::bpUpdateBPItem(position, text[0], text[1], state, rare, icon, render, headerLvl, max, progress, text[2]);
	}
	else if (typeInfo == 8)
	{
		int8_t id;
		bs.Read(id);
		
		CNewBieBattlePass::newbieMiniPrize(id);
	}
}

void CNewBieBattlePass::bpHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newbieHide"), OBFUSCATE("()V"));
	env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBieBattlePass::bpShow(char* text)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newbieShow"), OBFUSCATE("([B)V"));
	env->CallVoidMethod(activity, method, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBieBattlePass::selectBattlePass(int selectId)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("selectNewBie"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, selectId);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBieBattlePass::bpUpdateMainLayout(char* text, char* text2, bool limitButton, char* text3)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newbieUpdateMainLayout"), OBFUSCATE("([B[BZ[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, limitButton, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBieBattlePass::bpAddTasks(char* text, char* text2, char* text3, int max, int progress, int type, uint32_t model, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, int rareLevel, char* text4)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newbieAddTasks"), OBFUSCATE("([B[B[BIIIIIIFFFFI[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, max, progress, type, model, color1, color2, rotX, rotY, rotZ, zoom, rareLevel, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBieBattlePass::bpAddGuideMainLayout(int position, char* text, char* text2)
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
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newbieAddGuideMainLayout"), OBFUSCATE("(I[B[B)V"));
	env->CallVoidMethod(activity, method, position, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBieBattlePass::bpAddBPItem(char* count, char* name, int8_t state, int8_t rare, uint32_t icon, int8_t render, int8_t headerLvl, int max, int progress, char* exp)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(count));
    env->SetByteArrayRegion(bytes, 0, strlen(count), (jbyte*)count);

    jbyteArray bytes2 = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes2, 0, strlen(name), (jbyte*)name);

    jbyteArray bytes3 = env->NewByteArray(strlen(exp));
    env->SetByteArrayRegion(bytes3, 0, strlen(exp), (jbyte*)exp);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newbieAddBPItem"), OBFUSCATE("([B[BIIIIIII[B)V"));
	env->CallVoidMethod(activity, method, bytes, bytes2, state, rare, icon, render, headerLvl, max, progress, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBieBattlePass::bpUpdateBPItem(int8_t position, char* count, char* name, int8_t state, int8_t rare, uint32_t icon, int8_t render, int8_t headerLvl, int max, int progress, char* exp)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jbyteArray bytes = env->NewByteArray(strlen(count));
    env->SetByteArrayRegion(bytes, 0, strlen(count), (jbyte*)count);

    jbyteArray bytes2 = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes2, 0, strlen(name), (jbyte*)name);

    jbyteArray bytes3 = env->NewByteArray(strlen(exp));
    env->SetByteArrayRegion(bytes3, 0, strlen(exp), (jbyte*)exp);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newbieUpdateBPItem"), OBFUSCATE("(I[B[BIIIIIII[B)V"));
	env->CallVoidMethod(activity, method, position, bytes, bytes2, state, rare, icon, render, headerLvl, max, progress, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CNewBieBattlePass::newbieMiniPrize(int8_t id)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();
   	if (!env)
   	{
		Log(OBFUSCATE("No env"));
		return;
    }

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("newbieMiniPrize"), OBFUSCATE("(I)V"));
	env->CallVoidMethod(activity, method, id);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_battlenewbie_NewBieManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_NewBieBattle);
					
		bsParams.Write((int32_t)i);
		bsParams.Write((int32_t)i2);
		bsParams.Write((int32_t)i3);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CNewBieBattlePass::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CNewBieBattlePass class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CNewBieBattlePass done!"));
}