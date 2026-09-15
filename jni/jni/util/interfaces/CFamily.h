#pragma once

#include <jni.h>

#include <string>
#include <vector>

class CFamily
{
	
public:
    static jobject activity;
	
	static void familyHide(); // 0
	static void familyShowMainMenuPage(char* textFamily, char* textName, char* textRank); // 1
	static void familyShowInformationPage(char* famName, char* textMoney, char* textCoins, char* textOnline, char* textOffline, char* textName, char* textRank, char* playerWarns,
										char* textGraffiti, char* textInWar, char* textPlayerCoins, char* familyLeader, char* countZone, char* countGraffity); // 2
	static void familyShowUpgradePage(char* playerName, char* playerRank); // 3
	static void familyShowTopFamsPage(char* playerName, char* playerRank, int type); // 4
	static void familyShowTopPlayersPage(char* playerName, char* playerRank); // 5
	static void familyShowShopPage(char* playerName, char* playerRank, char* playerCoins); // 6
	static void familyShowBuyGraffitiPage(char* playerName, char* playerRank, char* playerCoins, int objectID, char* textPrice, 
										int color1, int color2, int color3, int color4, int color5, int color6, int color7, int color8, int color9, int color10, int color11); // 7
	static void familyShowChangeGraffitiPage(char* playerName, char* playerRank, char* playerCoins); // 8
	static void familyShowWarehousePage(char* playerName, char* playerRank); // 9
	static void familyShowWarehouseLogPage(char* playerName, char* playerRank); // 10
	static void familyShowDiplomacyPage(char* playerName, char* playerRank); // 11
	static void familyShowMembersPage(char* playerName, char* playerRank); // 12
	static void familyShowMembersRanksPage(char* playerName, char* playerRank, int selectedRank, int maxRank, 
										char* rankName, char* rankName2, char* rankName3, char* rankName4, char* rankName5, char* rankName6, char* rankName7, char* rankName8, char* rankName9, char* rankName10, char* rankName11, char* rankName12); // 13
	static void familyShowMembersBlackPage(char* playerName, char* playerRank); // 14
	static void familyShowMembersPaydayPage(char* playerName, char* playerRank); // 15
	static void familyShowSettingsPage(char* playerName, char* playerRank); // 16
	static void familyShowSettingsRanksPage(char* playerName, char* playerRank, int selectedRank, int maxRank, 
										char* rankName, char* rankName2, char* rankName3, char* rankName4, char* rankName5, char* rankName6, char* rankName7, char* rankName8, char* rankName9, char* rankName10, char* rankName11, char* rankName12); // 17
	static void familyShowSettingsCarsPage(char* playerName, char* playerRank); // 18
	static void familyShowSettingsLogPage(char* playerName, char* playerRank); // 19
	static void familyShowMembersOnlinePage(char* playerName, char* playerRank); // 20
	static void familyShowMembersMutedPage(char* playerName, char* playerRank); // 21
	static void familyShowQuestsContractsPage(char* playerName, char* playerRank, char* textOnline, char* textOffline, char* questInfo, char* questTarget, 
											uint32_t questProgress, uint32_t questMaxProgress, uint32_t prizeCoints, uint32_t prizeMoney, char* btnText, uint8_t btnState); // 22
	static void familyShowQuestsGraffitiInfoPage(char* playerName, char* playerRank, char* textOnline, char* textOffline, char* textCoins, char* textMoney, char* btnText, uint8_t btnState); // 23
	static void familyShowQuestsGraffitiMapPage(char* playerName, char* playerRank, char* textOnline, char* textOffline, char* textCoins, char* textMoney); // 24
	static void familyShowQuestsWarsPage(char* playerName, char* playerRank, char* textOnline, char* textOffline, char* textCoins, char* textMoney); // 25
	static void familyShowSafePage(char* playerName, char* playerRank, char* textMoney); // 26
	static void familyAddGraffitiItem(uint32_t objectID, char* graffitiName); // 27
	static void familyAddTopFamsItem(char* topID, char* familyName, char* playerCount, char* familyRating); // 28
	static void familyAddTopPlayersItem(int topID, char* playerName, char* playerStr); // 29
	static void familyAddShopItem(int8_t type, uint32_t model, char* shopName, char* shopPrice, int color1, int color2, float rotX, float rotY, float rotZ, float zoom); // 48
	static void familyAddUpgradeItem(int8_t type, uint32_t model, char* shopName, char* shopPrice, int color1, int color2, float rotX, float rotY, float rotZ, float zoom); // 49
	static void familyAddMainMessage(char* playerName, char* dateMessage, char* textMessage); // 30
	static void familyAddSettingsLogItem(char* playerName, char* textMessage, char* dateMessage); // 31
	static void familyAddMembersItem(char* topID, char* playerName, char* textDeposit, char* onlineWeek, char* status); // 32
	static void familyAddMembersOnlineItem(char* topID, char* playerName, char* textDeposit, char* onlineWeek, char* status); // 33
	static void familyAddMembersMutedItem(char* topID, char* playerName, char* textTime, char* textReason); // 34
	static void familyAddMembersRanksItem(char* topID, char* playerName, char* textDeposit, char* onlineWeek, char* status); // 35
	static void familyAddMembersBlackItem(char* topID, char* playerName, char* textReason, char* textDate); // 36
	static void familyAddMembersPaydayItem(char* topID, char* nameRank, char* textPay); // 37
	static void familyAddDiplomacyItem(char* playerName, char* playerRank, char* textMessage); // 38
	static void familyAddWarehouseLogItem(char* playerName, char* textAction, char* textDate); // 39
	static void familyAddWarehouseItem(int type, char* text); // 40
	static void familyAddSettingsItem(char* textName, char* textDostup); // 41
	static void familyAddSettingsRanksItem(char* topID, char* playerName, char* playerOnline); // 42
	static void familyAddSettingsCarsItem(int8_t type, uint32_t model, char* vehicleName, int color1, int color2, float rotX, float rotY, float rotZ, float zoom, char* textStatus, int status); // 44
    static void familyUpdateSettingsCarsItem(int position, char* vehicleName, char* vehicleRank, char* textStatus, int status); // 45
	static void familyAddQuestsContractsItem(char* questName, int questType, int questState); // 46
	static void familyAddSafeItem(char* playerName, char* textAction, char* textMoney, char* textDate); // 47
	static void familyAddQuestsGraffitiItem(uint32_t objectID, char* graffitiName); // 48
	static void setActivity(JNIEnv* env, jobject thiz);
};