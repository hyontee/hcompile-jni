#include <GLES2/gl2.h>

#include "util/CJavaWrapper.h"
#include "../main.h"

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"

#include "CFamily.h"
#include "CNoty.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;

jobject CFamily::activity;

void CNetGame::packetFamily(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

	int8_t typeInfo;
	bs.Read(typeInfo);
	
	if (typeInfo == 0) CFamily::familyHide();
	else
	{
		int8_t typeID;
		bs.Read(typeID);
		if (typeInfo == 2 && (typeID >= 3 && typeID <= 21))
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);

			char text[typeInfo][256];
			for (int i = 0; i < typeInfo; i ++)
			{
				memset(text[i], 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}
			
			if (typeID == 3) CFamily::familyShowUpgradePage(text[0], text[1]);
			if (typeID == 5) CFamily::familyShowTopPlayersPage(text[0], text[1]);
			if (typeID == 9) CFamily::familyShowWarehousePage(text[0], text[1]);
			if (typeID == 10) CFamily::familyShowWarehouseLogPage(text[0], text[1]);
			if (typeID == 11) CFamily::familyShowDiplomacyPage(text[0], text[1]);
			if (typeID == 12) CFamily::familyShowMembersPage(text[0], text[1]);
			if (typeID == 14) CFamily::familyShowMembersBlackPage(text[0], text[1]);
			if (typeID == 15) CFamily::familyShowMembersPaydayPage(text[0], text[1]);
			if (typeID == 16) CFamily::familyShowSettingsPage(text[0], text[1]);
			if (typeID == 18) CFamily::familyShowSettingsCarsPage(text[0], text[1]);
			if (typeID == 19) CFamily::familyShowSettingsLogPage(text[0], text[1]);
			if (typeID == 20) CFamily::familyShowMembersOnlinePage(text[0], text[1]);
			if (typeID == 21) CFamily::familyShowMembersMutedPage(text[0], text[1]);
		}
		else if (typeInfo == 2 && typeID == 41)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);

			uint8_t max_items;
			bs.Read(max_items);
			for (int q = 0; q < max_items; q ++)
			{
				char text[typeInfo][256];
				for (int i = 0; i < typeInfo; i ++)
				{
					memset(text[i], 0, 256);
					
					uint8_t textLen;
					bs.Read(textLen);
					bs.Read(text[i], textLen);
					text[i][textLen] = '\0';
				}
				
				CFamily::familyAddSettingsItem(text[0], text[1]);
			}
		}
		else if (typeInfo == 2 && (typeID == 27 || typeID == 48))
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			uint8_t max_items;
			bs.Read(max_items);	
			for (int i = 0; i < max_items; i ++)
			{
				uint32_t paramBS;
				bs.Read(paramBS);
		
				char text[256];
				memset(text, 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text, textLen);
				text[textLen] = '\0';
				
				if (typeID == 27) CFamily::familyAddGraffitiItem(paramBS, text);
				if (typeID == 48) CFamily::familyAddQuestsGraffitiItem(paramBS, text);
			}
		}
		else if (typeInfo == 2 && typeID == 40)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			uint8_t max_items;
			bs.Read(max_items);	
			for (int i = 0; i < max_items; i ++)
			{
				uint32_t paramBS;
				bs.Read(paramBS);
		
				char text[256];
				memset(text, 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text, textLen);
				text[textLen] = '\0';
				
				CFamily::familyAddWarehouseItem(paramBS, text);
			}
		}
		else if (typeInfo == 3 && typeID == 4)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			uint32_t paramBS;
			bs.Read(paramBS);
			
			char text[256];
			memset(text, 0, 256);
			
			uint8_t textLen;
			bs.Read(textLen);
			bs.Read(text, textLen);
			text[textLen] = '\0';
			
			char text2[256];
			memset(text2, 0, 256);
			
			bs.Read(textLen);
			bs.Read(text2, textLen);
			text2[textLen] = '\0';
			
			if (typeID == 4) CFamily::familyShowTopFamsPage(text, text2, paramBS);
		}
		else if (typeInfo == 3 && typeID == 29)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			uint8_t max_items;
			bs.Read(max_items);
			for (int i = 0; i < max_items; i ++)
			{
				uint32_t paramBS;
				bs.Read(paramBS);
				
				char text[256];
				memset(text, 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text, textLen);
				text[textLen] = '\0';
				
				char text2[256];
				memset(text2, 0, 256);
				
				bs.Read(textLen);
				bs.Read(text2, textLen);
				text2[textLen] = '\0';
			
				CFamily::familyAddTopPlayersItem(paramBS, text, text2);
			}
		}
		else if (typeInfo == 3 && typeID == 46)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			char text[256];
			memset(text, 0, 256);
				
			uint8_t textLen;
			bs.Read(textLen);
			bs.Read(text, textLen);
			text[textLen] = '\0';
				
			uint32_t paramBS;
			bs.Read(paramBS);
				
			uint32_t paramBS2;
			bs.Read(paramBS2);
				
			CFamily::familyAddQuestsContractsItem(text, paramBS, paramBS2);
		}
		else if (typeInfo == 3 && (typeID == 1 || typeID == 6 || typeID == 8 || typeID == 26))
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);

			char text[typeInfo][256];
			for (int i = 0; i < typeInfo; i ++)
			{
				memset(text[i], 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}
			
			if (typeID == 1) CFamily::familyShowMainMenuPage(text[0], text[1], text[2]);
			if (typeID == 6) CFamily::familyShowShopPage(text[0], text[1], text[2]);
			if (typeID == 8) CFamily::familyShowChangeGraffitiPage(text[0], text[1], text[2]);
			if (typeID == 26) CFamily::familyShowSafePage(text[0], text[1], text[2]);
		}
		else if (typeInfo == 3 && (typeID == 30 || typeID == 31 || typeID == 37 || typeID == 38 || typeID == 39 || typeID == 42 || typeID == 43))
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);

			uint8_t max_items;
			bs.Read(max_items);
			for (int q = 0; q < max_items; q ++)
			{
				char text[typeInfo][256];
				for (int i = 0; i < typeInfo; i ++)
				{
					memset(text[i], 0, 256);
					
					uint8_t textLen;
					bs.Read(textLen);
					bs.Read(text[i], textLen);
					text[i][textLen] = '\0';
				}
				
				if (typeID == 30) CFamily::familyAddMainMessage(text[0], text[1], text[2]);
				if (typeID == 31) CFamily::familyAddSettingsLogItem(text[0], text[1], text[2]);
				if (typeID == 37) CFamily::familyAddMembersPaydayItem(text[0], text[1], text[2]);
				if (typeID == 38) CFamily::familyAddDiplomacyItem(text[0], text[1], text[2]);
				if (typeID == 39) CFamily::familyAddWarehouseLogItem(text[0], text[1], text[2]);
				if (typeID == 42) CFamily::familyAddSettingsRanksItem(text[0], text[1], text[2]);
			}
		}
		else if (typeInfo == 4 && (typeID == 28 || typeID == 34 || typeID == 36 || typeID == 47))
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);

			uint8_t max_items;
			bs.Read(max_items);
			for (int q = 0; q < max_items; q ++)
			{
				char text[typeInfo][256];
				for (int i = 0; i < typeInfo; i ++)
				{
					memset(text[i], 0, 256);
					
					uint8_t textLen;
					bs.Read(textLen);
					bs.Read(text[i], textLen);
					text[i][textLen] = '\0';
				}
				
				if (typeID == 28) CFamily::familyAddTopFamsItem(text[0], text[1], text[2], text[3]);
				if (typeID == 34) CFamily::familyAddMembersMutedItem(text[0], text[1], text[2], text[3]);
				if (typeID == 36) CFamily::familyAddMembersBlackItem(text[0], text[1], text[2], text[3]);
				if (typeID == 47) CFamily::familyAddSafeItem(text[0], text[1], text[2], text[3]);
			}
		}
		else if (typeInfo == 5 && typeID == 45)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			int8_t position;
			bs.Read(position);
			
			char text[3][256];
			for (int i = 0; i < 3; i ++)
			{
				memset(text[i], 0, 256);
					
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}
			
			int8_t status;
			bs.Read(status);
				
			if (typeID == 45) CFamily::familyUpdateSettingsCarsItem(position, text[0], text[1], text[2], status);
		}
		else if (typeInfo == 5)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			uint8_t max_items;
			bs.Read(max_items);
			for (int q = 0; q < max_items; q ++)
			{
				char text[typeInfo][256];
				for (int i = 0; i < typeInfo; i ++)
				{
					memset(text[i], 0, 256);
					
					uint8_t textLen;
					bs.Read(textLen);
					bs.Read(text[i], textLen);
					text[i][textLen] = '\0';
				}
				
				if (typeID == 32) CFamily::familyAddMembersItem(text[0], text[1], text[2], text[3], text[4]);
				if (typeID == 33) CFamily::familyAddMembersOnlineItem(text[0], text[1], text[2], text[3], text[4]);
				if (typeID == 35) CFamily::familyAddMembersRanksItem(text[0], text[1], text[2], text[3], text[4]);
			}
		}
		else if (typeInfo == 6)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			char text[typeInfo][256];
			for (int i = 0; i < typeInfo; i ++)
			{
				memset(text[i], 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}

			if (typeID == 24) CFamily::familyShowQuestsGraffitiMapPage(text[0], text[1], text[2], text[3], text[4], text[5]);
			if (typeID == 25) CFamily::familyShowQuestsWarsPage(text[0], text[1], text[2], text[3], text[4], text[5]);
		}
		else if (typeInfo == 8 && typeID == 23)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			char text[7][256];
			for (int i = 0; i < 7; i ++)
			{
				memset(text[i], 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}
			
			uint8_t btnState;
			bs.Read(btnState);
			
			if (typeID == 23) CFamily::familyShowQuestsGraffitiInfoPage(text[0], text[1], text[2], text[3], text[4], text[5], text[6], btnState);
		}
		else if (typeInfo == 10 && (typeID == 48 || typeID == 49))
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			uint8_t max_items;
			bs.Read(max_items);
			for (int q = 0; q < max_items; q ++)
			{
				int8_t type;
				bs.Read(type);
				
				uint32_t model;
				bs.Read(model);
				
				char text[2][256];
				for (int i = 0; i < 2; i ++)
				{
					memset(text[i], 0, 256);
					
					uint8_t textLen;
					bs.Read(textLen);
					bs.Read(text[i], textLen);
					text[i][textLen] = '\0';
				}
				
				int8_t color1;
				bs.Read(color1);
				
				int8_t color2;
				bs.Read(color2);
				
				float rotX;
				bs.Read(rotX);
				
				float rotY;
				bs.Read(rotY);
				
				float rotZ;
				bs.Read(rotZ);
				
				float zoom;
				bs.Read(zoom);
				
				if (typeID == 48) CFamily::familyAddShopItem(type, model, text[0], text[1], color1, color2, rotX, rotY, rotZ, zoom);
				if (typeID == 49) CFamily::familyAddUpgradeItem(type, model, text[0], text[1], color1, color2, rotX, rotY, rotZ, zoom);
			}
		}
		else if (typeInfo == 11 && typeID == 44)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			uint8_t max_items;
			bs.Read(max_items);
			for (int q = 0; q < max_items; q ++)
			{
				int8_t type;
				bs.Read(type);
				
				uint32_t model;
				bs.Read(model);
				
				char text[256];
				memset(text, 0, 256);
					
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text, textLen);
				text[textLen] = '\0';
				
				int8_t color1;
				bs.Read(color1);
				
				int8_t color2;
				bs.Read(color2);
				
				float rotX;
				bs.Read(rotX);
				
				float rotY;
				bs.Read(rotY);
				
				float rotZ;
				bs.Read(rotZ);
				
				float zoom;
				bs.Read(zoom);
				
				char text2[256];
				memset(text2, 0, 256);
					
				bs.Read(textLen);
				bs.Read(text2, textLen);
				text2[textLen] = '\0';
				
				int8_t status;
				bs.Read(status);
				
				if (typeID == 44) CFamily::familyAddSettingsCarsItem(type, model, text, color1, color2, rotX, rotY, rotZ, zoom, text2, status);
			}
		}
		else if (typeInfo == 12 && typeID == 22)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			char text[6][256];
			for (int i = 0; i < 6; i ++)
			{
				memset(text[i], 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}
			
			uint32_t questProgress;
			bs.Read(questProgress);
			
			uint32_t questMaxProgress;
			bs.Read(questMaxProgress);
			
			uint32_t prizeCoints;
			bs.Read(prizeCoints);
			
			uint32_t prizeMoney;
			bs.Read(prizeMoney);
			
			char text7[256];
			memset(text7, 0, 256);
			
			uint8_t textLen;
			bs.Read(textLen);
			bs.Read(text7, textLen);
			text7[textLen] = '\0';
			
			uint8_t btnState;
			bs.Read(btnState);
			
			if (typeID == 22) CFamily::familyShowQuestsContractsPage(text[0], text[1], text[2], text[3], text[4], text[5], 
											questProgress, questMaxProgress, prizeCoints, prizeMoney, text7, btnState);
		}
		else if (typeInfo == 14)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			char text[typeInfo][256];
			for (int i = 0; i < typeInfo; i ++)
			{
				memset(text[i], 0, 256);
				
				uint8_t textLen;
				bs.Read(textLen);
				bs.Read(text[i], textLen);
				text[i][textLen] = '\0';
			}

			if (typeID == 2) CFamily::familyShowInformationPage(text[0], text[1], text[2], text[3], text[4], text[5], text[6], text[7], text[8], text[9], text[10], text[11], text[12], text[13]);
		}
		else if (typeInfo == 16 && typeID == 7)
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			char text[256];
			memset(text, 0, 256);
			
			uint8_t textLen;
			bs.Read(textLen);
			bs.Read(text, textLen);
			text[textLen] = '\0';
			
			char text2[256];
			memset(text2, 0, 256);
			
			bs.Read(textLen);
			bs.Read(text2, textLen);
			text2[textLen] = '\0';
			
			char text3[256];
			memset(text3, 0, 256);
			
			bs.Read(textLen);
			bs.Read(text3, textLen);
			text3[textLen] = '\0';
			
			uint32_t objectID;
			bs.Read(objectID);
			
			char text4[256];
			memset(text4, 0, 256);
			
			bs.Read(textLen);
			bs.Read(text4, textLen);
			text4[textLen] = '\0';
			
			int8_t color[11];
			for (int i = 0; i < 11; i ++)
			{
				bs.Read(color[i]);
			}
			
			if (typeID == 7) CFamily::familyShowBuyGraffitiPage(text, text2, text3, objectID, text4, 
										color[0], color[1], color[2], color[3], color[4], color[5], color[6], color[7], color[8], color[9], color[10]);
		}
		else if (typeInfo == 16 && (typeID == 13 || typeID == 17))
		{
			Log("typeInfo: %d, typeID: %d", typeInfo, typeID);
			
			char text[256];
			memset(text, 0, 256);
			
			uint8_t textLen;
			bs.Read(textLen);
			bs.Read(text, textLen);
			text[textLen] = '\0';
			
			char text2[256];
			memset(text2, 0, 256);
			
			bs.Read(textLen);
			bs.Read(text2, textLen);
			text2[textLen] = '\0';

			int8_t selectedRank;
			bs.Read(selectedRank);
			
			int8_t maxRank;
			bs.Read(maxRank);
			
			char rankName[12][256];
			for (int i = 0; i < 12; i ++)
			{
				memset(rankName[i], 0, 256);
				
				bs.Read(textLen);
				bs.Read(rankName[i], textLen);
				rankName[i][textLen] = '\0';
			}

			if (typeID == 13) CFamily::familyShowMembersRanksPage(text, text2, selectedRank, maxRank, 
										rankName[0], rankName[1], rankName[2], rankName[3], rankName[4], rankName[5], 
										rankName[6], rankName[7], rankName[8], rankName[9], rankName[10], rankName[11]);
			if (typeID == 17) CFamily::familyShowSettingsRanksPage(text, text2, selectedRank, maxRank, 
										rankName[0], rankName[1], rankName[2], rankName[3], rankName[4], rankName[5], 
										rankName[6], rankName[7], rankName[8], rankName[9], rankName[10], rankName[11]);
		}
	}
}

void CFamily::familyHide()
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyHide"), OBFUSCATE("()V"));
    env->CallVoidMethod(activity, method);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowMainMenuPage(char* textFamily, char* textName, char* textRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(textFamily));
	env->SetByteArrayRegion(bytes, 0, strlen(textFamily), (jbyte*)textFamily);

   	jbyteArray bytes2 = env->NewByteArray(strlen(textName));
	env->SetByteArrayRegion(bytes2, 0, strlen(textName), (jbyte*)textName);

   	jbyteArray bytes3 = env->NewByteArray(strlen(textRank));
	env->SetByteArrayRegion(bytes3, 0, strlen(textRank), (jbyte*)textRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowMainMenuPage"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowInformationPage(char* famName, char* textMoney, char* textCoins, char* textOnline, char* textOffline, char* textName, char* textRank, char* moneyInWareHouse,
										char* textGraffiti, char* textInWar, char* textPlayerCoins, char* familyLeader, char* countGraffity, char* countZone)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(famName));
	env->SetByteArrayRegion(bytes, 0, strlen(famName), (jbyte*)famName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(textMoney));
	env->SetByteArrayRegion(bytes2, 0, strlen(textMoney), (jbyte*)textMoney);

   	jbyteArray bytes3 = env->NewByteArray(strlen(textCoins));
	env->SetByteArrayRegion(bytes3, 0, strlen(textCoins), (jbyte*)textCoins);
	
   	jbyteArray bytes4 = env->NewByteArray(strlen(textOnline));
	env->SetByteArrayRegion(bytes4, 0, strlen(textOnline), (jbyte*)textOnline);
	
   	jbyteArray bytes5 = env->NewByteArray(strlen(textOffline));
	env->SetByteArrayRegion(bytes5, 0, strlen(textOffline), (jbyte*)textOffline);
	
   	jbyteArray bytes6 = env->NewByteArray(strlen(textName));
	env->SetByteArrayRegion(bytes6, 0, strlen(textName), (jbyte*)textName);
	
   	jbyteArray bytes7 = env->NewByteArray(strlen(textRank));
	env->SetByteArrayRegion(bytes7, 0, strlen(textRank), (jbyte*)textRank);
	
   	jbyteArray bytes8 = env->NewByteArray(strlen(moneyInWareHouse));
	env->SetByteArrayRegion(bytes8, 0, strlen(moneyInWareHouse), (jbyte*)moneyInWareHouse);
	
   	jbyteArray bytes9 = env->NewByteArray(strlen(textGraffiti));
	env->SetByteArrayRegion(bytes9, 0, strlen(textGraffiti), (jbyte*)textGraffiti);
	
   	jbyteArray bytes10 = env->NewByteArray(strlen(textInWar));
	env->SetByteArrayRegion(bytes10, 0, strlen(textInWar), (jbyte*)textInWar);
	
   	jbyteArray bytes11 = env->NewByteArray(strlen(textPlayerCoins));
	env->SetByteArrayRegion(bytes11, 0, strlen(textPlayerCoins), (jbyte*)textPlayerCoins);
	
   	jbyteArray bytes12 = env->NewByteArray(strlen(familyLeader));
	env->SetByteArrayRegion(bytes12, 0, strlen(familyLeader), (jbyte*)familyLeader);
	
   	jbyteArray bytes13 = env->NewByteArray(strlen(countGraffity));
	env->SetByteArrayRegion(bytes13, 0, strlen(countGraffity), (jbyte*)countGraffity);
	
   	jbyteArray bytes14 = env->NewByteArray(strlen(countZone));
	env->SetByteArrayRegion(bytes14, 0, strlen(countZone), (jbyte*)countZone);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowInformationPage"), OBFUSCATE("([B[B[B[B[B[B[B[B[B[B[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5, bytes6, bytes7, bytes8, bytes9, bytes10, bytes11, bytes12, bytes14, bytes13);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(bytes8);
	env->DeleteLocalRef(bytes9);
	env->DeleteLocalRef(bytes10);
	env->DeleteLocalRef(bytes11);
	env->DeleteLocalRef(bytes12);
	env->DeleteLocalRef(bytes13);
	env->DeleteLocalRef(bytes14);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowUpgradePage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowUpgradePage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowTopFamsPage(char* playerName, char* playerRank, int type)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowTopFamsPage"), OBFUSCATE("([B[BI)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, type);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowTopPlayersPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowTopPlayersPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowShopPage(char* playerName, char* playerRank, char* playerCoins)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(playerCoins));
	env->SetByteArrayRegion(bytes3, 0, strlen(playerCoins), (jbyte*)playerCoins);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowShopPage"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowBuyGraffitiPage(char* playerName, char* playerRank, char* playerCoins, int objectID, char* textPrice, 
										int color1, int color2, int color3, int color4, int color5, int color6, int color7, int color8, int color9, int color10, int color11)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(playerCoins));
	env->SetByteArrayRegion(bytes3, 0, strlen(playerCoins), (jbyte*)playerCoins);

   	jbyteArray bytes4 = env->NewByteArray(strlen(textPrice));
	env->SetByteArrayRegion(bytes4, 0, strlen(textPrice), (jbyte*)textPrice);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowBuyGraffitiPage"), OBFUSCATE("([B[B[BI[BIIIIIIIIIII)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, objectID, bytes4, color1, color2, color3, color4, color5, color6, color7, color8, color9, color10, color11);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowChangeGraffitiPage(char* playerName, char* playerRank, char* playerCoins)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(playerCoins));
	env->SetByteArrayRegion(bytes3, 0, strlen(playerCoins), (jbyte*)playerCoins);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowChangeGraffitiPage"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowWarehousePage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowWarehousePage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowWarehouseLogPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowWarehouseLogPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowDiplomacyPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowDiplomacyPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowMembersPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowMembersPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowMembersRanksPage(char* playerName, char* playerRank, int selectedRank, int maxRank, 
										char* rankName, char* rankName2, char* rankName3, char* rankName4, char* rankName5, char* rankName6, char* rankName7, char* rankName8, char* rankName9, char* rankName10, char* rankName11, char* rankName12)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(rankName));
	env->SetByteArrayRegion(bytes3, 0, strlen(rankName), (jbyte*)rankName);

   	jbyteArray bytes4 = env->NewByteArray(strlen(rankName2));
	env->SetByteArrayRegion(bytes4, 0, strlen(rankName2), (jbyte*)rankName2);

   	jbyteArray bytes5 = env->NewByteArray(strlen(rankName3));
	env->SetByteArrayRegion(bytes5, 0, strlen(rankName3), (jbyte*)rankName3);

   	jbyteArray bytes6 = env->NewByteArray(strlen(rankName4));
	env->SetByteArrayRegion(bytes6, 0, strlen(rankName4), (jbyte*)rankName4);

   	jbyteArray bytes7 = env->NewByteArray(strlen(rankName5));
	env->SetByteArrayRegion(bytes7, 0, strlen(rankName5), (jbyte*)rankName5);

   	jbyteArray bytes8 = env->NewByteArray(strlen(rankName6));
	env->SetByteArrayRegion(bytes8, 0, strlen(rankName6), (jbyte*)rankName6);

   	jbyteArray bytes9 = env->NewByteArray(strlen(rankName7));
	env->SetByteArrayRegion(bytes9, 0, strlen(rankName7), (jbyte*)rankName7);

   	jbyteArray bytes10 = env->NewByteArray(strlen(rankName8));
	env->SetByteArrayRegion(bytes10, 0, strlen(rankName8), (jbyte*)rankName8);

   	jbyteArray bytes11 = env->NewByteArray(strlen(rankName9));
	env->SetByteArrayRegion(bytes11, 0, strlen(rankName9), (jbyte*)rankName9);

   	jbyteArray bytes12 = env->NewByteArray(strlen(rankName10));
	env->SetByteArrayRegion(bytes12, 0, strlen(rankName10), (jbyte*)rankName10);

   	jbyteArray bytes13 = env->NewByteArray(strlen(rankName11));
	env->SetByteArrayRegion(bytes13, 0, strlen(rankName11), (jbyte*)rankName11);

   	jbyteArray bytes14 = env->NewByteArray(strlen(rankName12));
	env->SetByteArrayRegion(bytes14, 0, strlen(rankName12), (jbyte*)rankName12);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowMembersRanksPage"), OBFUSCATE("([B[BII[B[B[B[B[B[B[B[B[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, selectedRank, maxRank, bytes3, bytes4, bytes5, bytes6, bytes7, bytes8, bytes9, bytes10, bytes11, bytes12, bytes13, bytes14);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(bytes8);
	env->DeleteLocalRef(bytes9);
	env->DeleteLocalRef(bytes10);
	env->DeleteLocalRef(bytes11);
	env->DeleteLocalRef(bytes12);
	env->DeleteLocalRef(bytes13);
	env->DeleteLocalRef(bytes14);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowMembersBlackPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowMembersBlackPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowMembersPaydayPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowMembersPaydayPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowSettingsPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowSettingsPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowSettingsRanksPage(char* playerName, char* playerRank, int selectedRank, int maxRank, 
										char* rankName, char* rankName2, char* rankName3, char* rankName4, char* rankName5, char* rankName6, char* rankName7, char* rankName8, char* rankName9, char* rankName10, char* rankName11, char* rankName12)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(rankName));
	env->SetByteArrayRegion(bytes3, 0, strlen(rankName), (jbyte*)rankName);

   	jbyteArray bytes4 = env->NewByteArray(strlen(rankName2));
	env->SetByteArrayRegion(bytes4, 0, strlen(rankName2), (jbyte*)rankName2);

   	jbyteArray bytes5 = env->NewByteArray(strlen(rankName3));
	env->SetByteArrayRegion(bytes5, 0, strlen(rankName3), (jbyte*)rankName3);

   	jbyteArray bytes6 = env->NewByteArray(strlen(rankName4));
	env->SetByteArrayRegion(bytes6, 0, strlen(rankName4), (jbyte*)rankName4);

   	jbyteArray bytes7 = env->NewByteArray(strlen(rankName5));
	env->SetByteArrayRegion(bytes7, 0, strlen(rankName5), (jbyte*)rankName5);

   	jbyteArray bytes8 = env->NewByteArray(strlen(rankName6));
	env->SetByteArrayRegion(bytes8, 0, strlen(rankName6), (jbyte*)rankName6);

   	jbyteArray bytes9 = env->NewByteArray(strlen(rankName7));
	env->SetByteArrayRegion(bytes9, 0, strlen(rankName7), (jbyte*)rankName7);

   	jbyteArray bytes10 = env->NewByteArray(strlen(rankName8));
	env->SetByteArrayRegion(bytes10, 0, strlen(rankName8), (jbyte*)rankName8);

   	jbyteArray bytes11 = env->NewByteArray(strlen(rankName9));
	env->SetByteArrayRegion(bytes11, 0, strlen(rankName9), (jbyte*)rankName9);

   	jbyteArray bytes12 = env->NewByteArray(strlen(rankName10));
	env->SetByteArrayRegion(bytes12, 0, strlen(rankName10), (jbyte*)rankName10);

   	jbyteArray bytes13 = env->NewByteArray(strlen(rankName11));
	env->SetByteArrayRegion(bytes13, 0, strlen(rankName11), (jbyte*)rankName11);

   	jbyteArray bytes14 = env->NewByteArray(strlen(rankName12));
	env->SetByteArrayRegion(bytes14, 0, strlen(rankName12), (jbyte*)rankName12);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowSettingsRanksPage"), OBFUSCATE("([B[BII[B[B[B[B[B[B[B[B[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, selectedRank, maxRank, bytes3, bytes4, bytes5, bytes6, bytes7, bytes8, bytes9, bytes10, bytes11, bytes12, bytes13, bytes14);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(bytes8);
	env->DeleteLocalRef(bytes9);
	env->DeleteLocalRef(bytes10);
	env->DeleteLocalRef(bytes11);
	env->DeleteLocalRef(bytes12);
	env->DeleteLocalRef(bytes13);
	env->DeleteLocalRef(bytes14);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowSettingsCarsPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowSettingsCarsPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowSettingsLogPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowSettingsLogPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowMembersOnlinePage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowMembersOnlinePage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowMembersMutedPage(char* playerName, char* playerRank)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowMembersMutedPage"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowQuestsContractsPage(char* playerName, char* playerRank, char* textOnline, char* textOffline, char* questInfo, char* questTarget, 
											uint32_t questProgress, uint32_t questMaxProgress, uint32_t prizeCoints, uint32_t prizeMoney, char* btnText, uint8_t btnState)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(textOnline));
	env->SetByteArrayRegion(bytes3, 0, strlen(textOnline), (jbyte*)textOnline);

   	jbyteArray bytes4 = env->NewByteArray(strlen(textOffline));
	env->SetByteArrayRegion(bytes4, 0, strlen(textOffline), (jbyte*)textOffline);

   	jbyteArray bytes5 = env->NewByteArray(strlen(questInfo));
	env->SetByteArrayRegion(bytes5, 0, strlen(questInfo), (jbyte*)questInfo);

   	jbyteArray bytes6 = env->NewByteArray(strlen(questTarget));
	env->SetByteArrayRegion(bytes6, 0, strlen(questTarget), (jbyte*)questTarget);

   	jbyteArray bytes7 = env->NewByteArray(strlen(btnText));
	env->SetByteArrayRegion(bytes7, 0, strlen(btnText), (jbyte*)btnText);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowQuestsContractsPage"), OBFUSCATE("([B[B[B[B[B[BIIII[BI)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5, bytes6, questProgress, questMaxProgress, prizeCoints, prizeMoney, bytes7, btnState);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowQuestsGraffitiInfoPage(char* playerName, char* playerRank, char* textOnline, char* textOffline, char* textCoins, char* textMoney, char* btnText, uint8_t btnState)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(textOnline));
	env->SetByteArrayRegion(bytes3, 0, strlen(textOnline), (jbyte*)textOnline);

   	jbyteArray bytes4 = env->NewByteArray(strlen(textOffline));
	env->SetByteArrayRegion(bytes4, 0, strlen(textOffline), (jbyte*)textOffline);

   	jbyteArray bytes5 = env->NewByteArray(strlen(textCoins));
	env->SetByteArrayRegion(bytes5, 0, strlen(textCoins), (jbyte*)textCoins);

   	jbyteArray bytes6 = env->NewByteArray(strlen(textMoney));
	env->SetByteArrayRegion(bytes6, 0, strlen(textMoney), (jbyte*)textMoney);

   	jbyteArray bytes7 = env->NewByteArray(strlen(btnText));
	env->SetByteArrayRegion(bytes7, 0, strlen(btnText), (jbyte*)btnText);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowQuestsGraffitiInfoPage"), OBFUSCATE("([B[B[B[B[B[B[BI)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5, bytes6, bytes7, btnState);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(bytes7);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowQuestsGraffitiMapPage(char* playerName, char* playerRank, char* textOnline, char* textOffline, char* textCoins, char* textMoney)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(textOnline));
	env->SetByteArrayRegion(bytes3, 0, strlen(textOnline), (jbyte*)textOnline);

   	jbyteArray bytes4 = env->NewByteArray(strlen(textOffline));
	env->SetByteArrayRegion(bytes4, 0, strlen(textOffline), (jbyte*)textOffline);

   	jbyteArray bytes5 = env->NewByteArray(strlen(textCoins));
	env->SetByteArrayRegion(bytes5, 0, strlen(textCoins), (jbyte*)textCoins);

   	jbyteArray bytes6 = env->NewByteArray(strlen(textMoney));
	env->SetByteArrayRegion(bytes6, 0, strlen(textMoney), (jbyte*)textMoney);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowQuestsGraffitiMapPage"), OBFUSCATE("([B[B[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5, bytes6);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowQuestsWarsPage(char* playerName, char* playerRank, char* textOnline, char* textOffline, char* textCoins, char* textMoney)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(textOnline));
	env->SetByteArrayRegion(bytes3, 0, strlen(textOnline), (jbyte*)textOnline);

   	jbyteArray bytes4 = env->NewByteArray(strlen(textOffline));
	env->SetByteArrayRegion(bytes4, 0, strlen(textOffline), (jbyte*)textOffline);

   	jbyteArray bytes5 = env->NewByteArray(strlen(textCoins));
	env->SetByteArrayRegion(bytes5, 0, strlen(textCoins), (jbyte*)textCoins);

   	jbyteArray bytes6 = env->NewByteArray(strlen(textMoney));
	env->SetByteArrayRegion(bytes6, 0, strlen(textMoney), (jbyte*)textMoney);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowQuestsWarsPage"), OBFUSCATE("([B[B[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5, bytes6);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(bytes6);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyShowSafePage(char* playerName, char* playerRank, char* textMoney)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);

   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);

   	jbyteArray bytes3 = env->NewByteArray(strlen(textMoney));
	env->SetByteArrayRegion(bytes3, 0, strlen(textMoney), (jbyte*)textMoney);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyShowSafePage"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddGraffitiItem(uint32_t objectID, char* graffitiName)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(graffitiName));
	env->SetByteArrayRegion(bytes, 0, strlen(graffitiName), (jbyte*)graffitiName);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddGraffitiItem"), OBFUSCATE("(I[B)V"));
    env->CallVoidMethod(activity, method, objectID, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddQuestsGraffitiItem(uint32_t objectID, char* graffitiName)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(graffitiName));
	env->SetByteArrayRegion(bytes, 0, strlen(graffitiName), (jbyte*)graffitiName);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddQuestsGraffitiItem"), OBFUSCATE("(I[B)V"));
    env->CallVoidMethod(activity, method, objectID, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddTopFamsItem(char* topID, char* familyName, char* playerCount, char* familyRating)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

   	jbyteArray bytes = env->NewByteArray(strlen(topID));
	env->SetByteArrayRegion(bytes, 0, strlen(topID), (jbyte*)topID);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(familyName));
	env->SetByteArrayRegion(bytes2, 0, strlen(familyName), (jbyte*)familyName);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(playerCount));
	env->SetByteArrayRegion(bytes3, 0, strlen(playerCount), (jbyte*)playerCount);
	
   	jbyteArray bytes4 = env->NewByteArray(strlen(familyRating));
	env->SetByteArrayRegion(bytes4, 0, strlen(familyRating), (jbyte*)familyRating);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddTopFamsItem"), OBFUSCATE("([B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddTopPlayersItem(int topID, char* playerName, char* playerStr)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(playerStr));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerStr), (jbyte*)playerStr);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddTopPlayersItem"), OBFUSCATE("(I[B[B)V"));
    env->CallVoidMethod(activity, method, topID, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddShopItem(int8_t type, uint32_t model, char* shopName, char* shopPrice, int color1, int color2, float rotX, float rotY, float rotZ, float zoom)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(shopName));
	env->SetByteArrayRegion(bytes, 0, strlen(shopName), (jbyte*)shopName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(shopPrice));
	env->SetByteArrayRegion(bytes2, 0, strlen(shopPrice), (jbyte*)shopPrice);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddShopItem"), OBFUSCATE("(II[B[BIIFFFF)V"));
    env->CallVoidMethod(activity, method, type, model, bytes, bytes2, color1, color2, rotX, rotY, rotZ, zoom);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddUpgradeItem(int8_t type, uint32_t model, char* shopName, char* shopPrice, int color1, int color2, float rotX, float rotY, float rotZ, float zoom)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(shopName));
	env->SetByteArrayRegion(bytes, 0, strlen(shopName), (jbyte*)shopName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(shopPrice));
	env->SetByteArrayRegion(bytes2, 0, strlen(shopPrice), (jbyte*)shopPrice);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddUpgradeItem"), OBFUSCATE("(II[B[BIIFFFF)V"));
    env->CallVoidMethod(activity, method, type, model, bytes, bytes2, color1, color2, rotX, rotY, rotZ, zoom);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddMainMessage(char* playerName, char* dateMessage, char* textMessage)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(dateMessage));
	env->SetByteArrayRegion(bytes2, 0, strlen(dateMessage), (jbyte*)dateMessage);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textMessage));
	env->SetByteArrayRegion(bytes3, 0, strlen(textMessage), (jbyte*)textMessage);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddMainMessage"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddSettingsLogItem(char* playerName, char* textMessage, char* dateMessage)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(dateMessage));
	env->SetByteArrayRegion(bytes2, 0, strlen(dateMessage), (jbyte*)dateMessage);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textMessage));
	env->SetByteArrayRegion(bytes3, 0, strlen(textMessage), (jbyte*)textMessage);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddSettingsLogItem"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes3, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddMembersItem(char* topID, char* playerName, char* textDeposit, char* onlineWeek, char* status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(topID));
	env->SetByteArrayRegion(bytes, 0, strlen(topID), (jbyte*)topID);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textDeposit));
	env->SetByteArrayRegion(bytes3, 0, strlen(textDeposit), (jbyte*)textDeposit);
	
   	jbyteArray bytes4 = env->NewByteArray(strlen(onlineWeek));
	env->SetByteArrayRegion(bytes4, 0, strlen(onlineWeek), (jbyte*)onlineWeek);
	
   	jbyteArray bytes5 = env->NewByteArray(strlen(status));
	env->SetByteArrayRegion(bytes5, 0, strlen(status), (jbyte*)status);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddMembersItem"), OBFUSCATE("([B[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddMembersOnlineItem(char* topID, char* playerName, char* textDeposit, char* onlineWeek, char* status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(topID));
	env->SetByteArrayRegion(bytes, 0, strlen(topID), (jbyte*)topID);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textDeposit));
	env->SetByteArrayRegion(bytes3, 0, strlen(textDeposit), (jbyte*)textDeposit);
	
   	jbyteArray bytes4 = env->NewByteArray(strlen(onlineWeek));
	env->SetByteArrayRegion(bytes4, 0, strlen(onlineWeek), (jbyte*)onlineWeek);
	
   	jbyteArray bytes5 = env->NewByteArray(strlen(status));
	env->SetByteArrayRegion(bytes5, 0, strlen(status), (jbyte*)status);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddMembersOnlineItem"), OBFUSCATE("([B[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddMembersMutedItem(char* topID, char* playerName, char* textTime, char* textReason)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(topID));
	env->SetByteArrayRegion(bytes, 0, strlen(topID), (jbyte*)topID);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textTime));
	env->SetByteArrayRegion(bytes3, 0, strlen(textTime), (jbyte*)textTime);
	
   	jbyteArray bytes4 = env->NewByteArray(strlen(textReason));
	env->SetByteArrayRegion(bytes4, 0, strlen(textReason), (jbyte*)textReason);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddMembersMutedItem"), OBFUSCATE("([B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddMembersRanksItem(char* topID, char* playerName, char* textDeposit, char* onlineWeek, char* status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(topID));
	env->SetByteArrayRegion(bytes, 0, strlen(topID), (jbyte*)topID);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textDeposit));
	env->SetByteArrayRegion(bytes3, 0, strlen(textDeposit), (jbyte*)textDeposit);
	
   	jbyteArray bytes4 = env->NewByteArray(strlen(onlineWeek));
	env->SetByteArrayRegion(bytes4, 0, strlen(onlineWeek), (jbyte*)onlineWeek);
	
   	jbyteArray bytes5 = env->NewByteArray(strlen(status));
	env->SetByteArrayRegion(bytes5, 0, strlen(status), (jbyte*)status);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddMembersRanksItem"), OBFUSCATE("([B[B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddMembersBlackItem(char* topID, char* playerName, char* textReason, char* textDate)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(topID));
	env->SetByteArrayRegion(bytes, 0, strlen(topID), (jbyte*)topID);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textReason));
	env->SetByteArrayRegion(bytes3, 0, strlen(textReason), (jbyte*)textReason);
	
   	jbyteArray bytes4 = env->NewByteArray(strlen(textDate));
	env->SetByteArrayRegion(bytes4, 0, strlen(textDate), (jbyte*)textDate);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddMembersBlackItem"), OBFUSCATE("([B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3, bytes4);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddMembersPaydayItem(char* topID, char* nameRank, char* textPay)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(topID));
	env->SetByteArrayRegion(bytes, 0, strlen(topID), (jbyte*)topID);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(nameRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(nameRank), (jbyte*)nameRank);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textPay));
	env->SetByteArrayRegion(bytes3, 0, strlen(textPay), (jbyte*)textPay);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddMembersPaydayItem"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddDiplomacyItem(char* playerName, char* playerRank, char* textMessage)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(playerRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerRank), (jbyte*)playerRank);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textMessage));
	env->SetByteArrayRegion(bytes3, 0, strlen(textMessage), (jbyte*)textMessage);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddDiplomacyItem"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddWarehouseLogItem(char* playerName, char* textAction, char* textDate)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(textAction));
	env->SetByteArrayRegion(bytes2, 0, strlen(textAction), (jbyte*)textAction);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textDate));
	env->SetByteArrayRegion(bytes3, 0, strlen(textDate), (jbyte*)textDate);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddWarehouseLogItem"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddWarehouseItem(int type, char* text)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddWarehouseItem"), OBFUSCATE("(I[B)V"));
    env->CallVoidMethod(activity, method, type, bytes);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddSettingsItem(char* textName, char* textDostup)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(textName));
	env->SetByteArrayRegion(bytes, 0, strlen(textName), (jbyte*)textName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(textDostup));
	env->SetByteArrayRegion(bytes2, 0, strlen(textDostup), (jbyte*)textDostup);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddSettingsItem"), OBFUSCATE("([B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddSettingsRanksItem(char* topID, char* playerName, char* playerOnline)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(topID));
	env->SetByteArrayRegion(bytes, 0, strlen(topID), (jbyte*)topID);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes2, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(playerOnline));
	env->SetByteArrayRegion(bytes3, 0, strlen(playerOnline), (jbyte*)playerOnline);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddSettingsRanksItem"), OBFUSCATE("([B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes2, bytes3);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddSettingsCarsItem(int8_t type, uint32_t model, char* vehicleName, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* textStatus, int status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(vehicleName));
	env->SetByteArrayRegion(bytes, 0, strlen(vehicleName), (jbyte*)vehicleName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(textStatus));
	env->SetByteArrayRegion(bytes2, 0, strlen(textStatus), (jbyte*)textStatus);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddSettingsCarsItem"), OBFUSCATE("(II[BIIFFFF[BI)V"));
    env->CallVoidMethod(activity, method, type, model, bytes, color1, color2, rotX, rotY, rotZ, zoom, bytes2, status);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyUpdateSettingsCarsItem(int position, char* vehicleName, char* vehicleRank, char* textStatus, int status)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(vehicleName));
	env->SetByteArrayRegion(bytes, 0, strlen(vehicleName), (jbyte*)vehicleName);
	
   	jbyteArray bytes2 = env->NewByteArray(strlen(vehicleRank));
	env->SetByteArrayRegion(bytes2, 0, strlen(vehicleRank), (jbyte*)vehicleRank);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textStatus));
	env->SetByteArrayRegion(bytes3, 0, strlen(textStatus), (jbyte*)textStatus);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyUpdateSettingsCarsItem"), OBFUSCATE("(I[B[B[BI)V"));
    env->CallVoidMethod(activity, method, position, bytes, bytes2, bytes3, status);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes2);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddQuestsContractsItem(char* questName, int questType, int questState)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(questName));
	env->SetByteArrayRegion(bytes, 0, strlen(questName), (jbyte*)questName);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddQuestsContractsItem"), OBFUSCATE("([BII)V"));
    env->CallVoidMethod(activity, method, bytes, questType, questState);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

void CFamily::familyAddSafeItem(char* playerName, char* textAction, char* textMoney, char* textDate)
{
	JNIEnv* env = g_pJavaWrapper->GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
   	jbyteArray bytes = env->NewByteArray(strlen(playerName));
	env->SetByteArrayRegion(bytes, 0, strlen(playerName), (jbyte*)playerName);
	
   	jbyteArray bytes3 = env->NewByteArray(strlen(textAction));
	env->SetByteArrayRegion(bytes3, 0, strlen(textAction), (jbyte*)textAction);
	
   	jbyteArray bytes4 = env->NewByteArray(strlen(textMoney));
	env->SetByteArrayRegion(bytes4, 0, strlen(textMoney), (jbyte*)textMoney);
	
   	jbyteArray bytes5 = env->NewByteArray(strlen(textDate));
	env->SetByteArrayRegion(bytes5, 0, strlen(textDate), (jbyte*)textDate);

    jclass clazz = env->GetObjectClass(activity);
	jmethodID method = env->GetMethodID(clazz, OBFUSCATE("familyAddSafeItem"), OBFUSCATE("([B[B[B[B)V"));
    env->CallVoidMethod(activity, method, bytes, bytes3, bytes4, bytes5);
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(bytes3);
	env->DeleteLocalRef(bytes4);
	env->DeleteLocalRef(bytes5);
	env->DeleteLocalRef(clazz);
	
   	EXCEPTION_CHECK(env);
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_rockstargames_gui_family_FamilyManager_SendResponse(JNIEnv* pEnv, jobject thiz, jint i, jint i2, jint i3)
	{
		int32_t typeSelect = i;
		int32_t typeSelectId = i2;
		int32_t typeIncId = i3;

		RakNet::BitStream bsParams;
		bsParams.Write((uint8_t)ID_CUSTOM_RPC);
		bsParams.Write((uint32_t)RPC_Family);

		bsParams.Write(typeSelect);
		bsParams.Write(typeSelectId);
		bsParams.Write(typeIncId);
		pNetGame->GetRakClient()->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
	}
}

void CFamily::setActivity(JNIEnv* env, jobject thiz)
{
	Log(OBFUSCATE("CFamily class"));
	activity = env->NewGlobalRef(thiz);
	Log(OBFUSCATE("CFamily done!"));
}