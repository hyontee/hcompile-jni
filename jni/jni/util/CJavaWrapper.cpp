#include "CJavaWrapper.h"
#include "../main.h"

extern "C" JavaVM* javaVM;

#include "..//keyboard.h"
#include "..//CSettings.h"
#include "..//net/netgame.h"
#include "../game/game.h"
#include "../str_obfuscator_no_template.hpp"
#include "java_systems/CTab.h"
#include "java_systems/CHUD.h"

extern CNetGame* pNetGame;
extern CGame* pGame;

JNIEnv* CJavaWrapper::GetEnv()
{
	JNIEnv* env = nullptr;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

	if (getEnvStat == JNI_EDETACHED)
	{
		Log("GetEnv: not attached");
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		{
			Log("Failed to attach");
			return nullptr;
		}
	}
	if (getEnvStat == JNI_EVERSION)
	{
		Log("GetEnv: version not supported");
		return nullptr;
	}

	if (getEnvStat == JNI_ERR)
	{
		Log("GetEnv: JNI_ERR");
		return nullptr;
	}

	return env;
}

void CJavaWrapper::ShowClientSettings()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ShowClientSettings);

	EXCEPTION_CHECK(env);
}

#include "..//CDebugInfo.h"
#include "chatwindow.h"
#include "java_systems/CMedic.h"
#include "java_systems/CSpeedometr.h"
#include "java_systems/CChooseSpawn.h"
#include "java_systems/CAuthrization.h"
#include "java_systems/CRegistration.h"
#include "java_systems/CGameActionMenu.h"
#include "CJavaGui.h"
#include "game/CSnapShotWrapper.h"

void CJavaWrapper::ShowFuelStation(int type, int price1, int price2, int price3, int price4, int price5, int maxCount)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showFuelStation, type, price1, price2, price3, price4, price5, maxCount);
}

void CJavaWrapper::ShowGunShopManager()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showGunShopManager);
}

void CJavaWrapper::HideGunShopManager()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideGunShopManager);
}

void CJavaWrapper::ToggleShopStoreManager(bool toggle, int type, int price)
{
	pGame->isShopStoreActive = toggle;
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_ToggleShopStoreManager, toggle, type, price);
}

void CJavaWrapper::ShowArmyGame(int quantity)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showArmyGame, quantity);
}

void CJavaWrapper::HideArmyGame()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideArmyGame);
}

void CJavaWrapper::ShowOilFactoryGame()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showOilFactoryGame);
}

void CJavaWrapper::ShowSamwill()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showSamwill);
}

void CJavaWrapper::Vibrate(int milliseconds)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	env->CallVoidMethod(this->activity, this->j_Vibrate, milliseconds);
}

void CJavaWrapper::SetPauseState(bool a1)
{
    JNIEnv* env = GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    env->CallVoidMethod(this->activity, this->s_setPauseState, a1);
}

//void CJavaWrapper::ShowSplash() {
//
//	JNIEnv* env = GetEnv();
//
//	if (!env)
//	{
//		Log("No env");
//		return;
//	}
//
//	env->CallVoidMethod(this->activity, this->s_showSplash);
//}

void CJavaWrapper::ExitGame() {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_ExitGame);
}

void CJavaWrapper::ShowMenu() 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_showMenu);
}

void CJavaWrapper::ToggleAutoShop(bool toggle)
{
	JNIEnv* env = GetEnv();
    Log("CJavaWrapper::ToggleAutoShop(%d)", toggle);
    if (!env)
	{
		Log("No env");
		return;
	}
	pGame->isAutoShopActive = toggle;

	env->CallVoidMethod(this->activity, this->j_toggleAutoShop, toggle);
}
void CJavaWrapper::UpdateAutoShop(const char name[], int price, int count, float maxspeed, float acceleration, int gear)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
//	jstring j_name = env->NewStringUTF( name );

	jclass strClass3 = env->FindClass("java/lang/String");
	jmethodID ctorID3 = env->GetMethodID(strClass3, "<init>", "([BLjava/lang/String;)V");
	jbyteArray bytes3 = env->NewByteArray(strlen(name));
	jstring encoding3 = env->NewStringUTF("UTF-8");
	auto str4 = (jstring) env->NewObject(strClass3, ctorID3, bytes3, encoding3);

	env->CallVoidMethod(this->activity, this->j_updateAutoShop, str4, price, count, maxspeed, acceleration, gear);

	env->DeleteLocalRef(str4);
}

void CJavaWrapper::ClearScreen()
{
	Log("ClearScreen");
	ShowMiningGame1(false);
	ShowMiningGame2(false);
	ShowMiningGame3(false);
	ToggleShopStoreManager(false);
	CHUD::hideTargetNotify();
	CChooseSpawn::hide();
	CSpeedometr::hide();
	HideArmyGame();
	this->ToggleAutoShop(false);
	CHUD::hideBusInfo();
	CHUD::toggleGps(false);
	CHUD::toggleGreenZone(false);
	CMedic::hide();
    CAuthrization::hide();
    CRegistration::hide();
    CGameActionMenu::hide();
    CJavaGui::DestroyAll();

	ShowCasinoDice(false, 0, 0, 0, 0, "--", 0, "--", 0, "--", 0, "--", 0, "--", 0);
}

void CJavaWrapper::StartSamp(int inter){
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	env->CallVoidMethod(this->activity, this->j_startSamp, inter);
}

const uint32_t cRegisterSkin[2][10] = {
        {9,  10, 11, 12, 13,   108,   231,}, // female
        {16, 15,  17, 21, 47, 19, 20,} // male
};

uint32_t CJavaWrapper::ChangeRegisterSkin(int skin)
{
	uint32_t uiSkin = 16;
	bool bIsMan = g_pJavaWrapper->RegisterSexMale == 1 ? true : false;
	uint32_t uiMaxSkins = bIsMan ? 9 : 4;

	if (!(0 < skin <= uiMaxSkins)) {
		g_pJavaWrapper->RegisterSkinId = uiSkin;
		return uiSkin;
	}

	uiSkin = cRegisterSkin[(int)bIsMan][skin - 1];
	g_pJavaWrapper->RegisterSkinId = uiSkin;

	return uiSkin;
}

CJavaWrapper::CJavaWrapper(JNIEnv* env, jobject activity)
{
	this->activity = env->NewGlobalRef(activity);

	jclass nvEventClass = env->GetObjectClass(activity);

	if (!nvEventClass)
	{
		Log("nvEventClass null");
		return;
	}

	s_ShowClientSettings = env->GetMethodID(nvEventClass, "showClientSettings", "()V");

	s_showOilFactoryGame = env->GetMethodID(nvEventClass, "showOilFactoryGame", "()V");
	s_showArmyGame = env->GetMethodID(nvEventClass, "showArmyGame", "(I)V");
	s_hideArmyGame = env->GetMethodID(nvEventClass, "hideArmyGame", "()V");

	s_showFuelStation = env->GetMethodID(nvEventClass, "showFuelStation", "(IIIIIII)V");

	s_ToggleShopStoreManager = env->GetMethodID(nvEventClass, "toggleShopStoreManager", "(ZII)V");

	s_showGunShopManager = env->GetMethodID(nvEventClass, "showGunShopManager", "()V");
	s_hideGunShopManager = env->GetMethodID(nvEventClass, "hideGunShopManager", "()V");

	j_Vibrate = env->GetMethodID(nvEventClass, "goVibrate", "(I)V");

    s_showSamwill = env->GetMethodID(nvEventClass, "showSamwill", "()V");

	s_showMenu = env->GetMethodID(nvEventClass, "showMenu", "()V");

	j_toggleAutoShop = env->GetMethodID(nvEventClass, "toggleAutoShop", "(Z)V");

	j_updateAutoShop = env->GetMethodID(nvEventClass, "updateAutoShop", "(Ljava/lang/String;IIFFI)V");

    s_setPauseState = env->GetMethodID(nvEventClass, "setPauseState", "(Z)V");

	s_ExitGame = env->GetMethodID(nvEventClass, "ExitGame", "()V");

	j_startSamp = env->GetMethodID(nvEventClass, "StartSamp", "(I)V");
	//s_showSplash = env->GetMethodID(nvEventClass, "showSplash", "()V");

	env->DeleteLocalRef(nvEventClass);
}

CJavaWrapper::~CJavaWrapper()
{
	JNIEnv* pEnv = GetEnv();
	if (pEnv)
	{
		pEnv->DeleteGlobalRef(this->activity);
	}
}

void CJavaWrapper::TempToggleCasinoDice(bool toggle) {
	JNIEnv* env = GetEnv();
	if (!env)
	{
		Log("No env");
		return;
	}

	jclass _class = env->GetObjectClass(jCasinoDice);
	jmethodID TempToggle = env->GetMethodID(_class, "TempToggle", "(Z)V");

	env->CallVoidMethod(jCasinoDice, TempToggle, toggle);

}

void CJavaWrapper::ShowCasinoDice(bool show, int tableID, int tableBet, int tableBank, int money,
								  char player1name[], int player1stat,
								  char player2name[], int player2stat,
								  char player3name[], int player3stat,
								  char player4name[], int player4stat,
								  char player5name[], int player5stat) {



	JNIEnv* env = GetEnv();

    jclass clazz = env->GetObjectClass(jCasinoDice);


	jmethodID Toggle = env->GetMethodID(clazz, "Toggle", "(ZIIIILjava/lang/String;ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;I)V");

	jstring jPlayer1Name = env->NewStringUTF( player1name );
	jstring jPlayer2Name = env->NewStringUTF( player2name );
	jstring jPlayer3Name = env->NewStringUTF( player3name );
	jstring jPlayer4Name = env->NewStringUTF( player4name );
	jstring jPlayer5Name = env->NewStringUTF( player5name );

	env->CallVoidMethod(jCasinoDice, Toggle, show, tableID, tableBet, tableBank, money, jPlayer1Name, player1stat, jPlayer2Name, player2stat, jPlayer3Name, player3stat, jPlayer4Name, player4stat, jPlayer5Name, player5stat);

	env->DeleteLocalRef(jPlayer1Name);
	env->DeleteLocalRef(jPlayer2Name);
	env->DeleteLocalRef(jPlayer3Name);
	env->DeleteLocalRef(jPlayer4Name);
	env->DeleteLocalRef(jPlayer5Name);
}

void CJavaWrapper::ShowCasinoLuckyWheel(int count, int time) {

	pGame->isCasinoWheelActive = true;
	JNIEnv* env = GetEnv();

	jclass clazz = env->GetObjectClass(jCasino_LuckyWheel);
	jmethodID Show = env->GetMethodID(clazz, "show", "(II)V");

	env->CallVoidMethod(jCasino_LuckyWheel, Show, count, time);
}

void CJavaWrapper::SendBuffer(const char string[]) {
	JNIEnv* env = GetEnv();

	jstring jstring = env->NewStringUTF( string );
	//jclass clazz = env->GetObjectClass(jCasino_LuckyWheel);
	jclass nvEventClass = env->GetObjectClass(activity);
	jmethodID CopyTextToBuffer = env->GetMethodID(nvEventClass, "CopyTextToBuffer", "(Ljava/lang/String;)V");

	env->CallVoidMethod(activity, CopyTextToBuffer, jstring);
	env->DeleteLocalRef(jstring);
}

CJavaWrapper* g_pJavaWrapper = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_Casino_1LuckyWheel_ClickButt(JNIEnv *env, jobject thiz, jint button_id) {
	pGame->isCasinoWheelActive = false;
	if(button_id == 228)
	{// Закрыл
		return;
	}
	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_CASINO_LUCKY_WHEEL_MENU;
	uint8_t button = button_id;


	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(button);
	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_core_Samp_playUrlSound(JNIEnv *env, jclass clazz, jstring url) {
	const char *_url = env->GetStringUTFChars(url, nullptr);

	pNetGame->GetStreamPool()->PlayIndividualStream(_url, BASS_STREAM_AUTOFREE);

	env->ReleaseStringUTFChars(url, _url);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_OilFactoryManager_onOilFactoryGameClose(JNIEnv *env, jobject thiz,
																   jboolean success) {
	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_SHOW_OILGAME;

	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write((uint8_t)success);

	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_AutoShop_sendAutoShopButton(JNIEnv *env, jobject thiz, jint button_id) {
	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_CLICK_AUTOSHOP;
	uint8_t button = button_id;


	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(button);
	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_SamwillManager_onSamwillHideGame(JNIEnv *env, jobject thiz,
															jint samwillpacket) {
	pNetGame->SendCustomPacket(251, 20, samwillpacket);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_ArmyGameManager_onArmyGameClose(JNIEnv *env, jobject thiz) {
	pNetGame->SendCustomPacket(251, 45, 1);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_FuelStationManager_onFuelStationClick(JNIEnv *env, jobject thiz,
																 jint fueltype, jint fuelliters) {
	pNetGame->SendCustomPacketFuelData(251, 39, fueltype, fuelliters);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_ShopStoreManager_onShopStoreClick(JNIEnv *env, jobject thiz,
															 jint buttonid) {
	pNetGame->SendCustomPacket(251, 42, buttonid);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_GunShopManager_onGunShopClick(JNIEnv *env, jobject thiz, jint weaponid) {
	pNetGame->SendCustomPacket(251, 44, weaponid);
}
extern "C" {

// 🔹 Меню (главное)
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_Menu_sendRPC(JNIEnv *env, jclass clazz, jint type, jbyteArray str, jint action) {
    if (!pNetGame) return;

    switch (type) {
        case 1:
            switch (action) {
                case 1:  pNetGame->SendChatCommand("/i");          break;
                case 2:  pNetGame->SendChatCommand("/music");      break;
                case 3:  pNetGame->SendChatCommand("/inv");        break;
                case 4:  pNetGame->SendChatCommand("/leaders");    break;
                case 5:  pNetGame->SendChatCommand("/familyss");      break;
                case 6:  pNetGame->SendChatCommand("/hmenu");      break;
                case 7:  pNetGame->SendChatCommand("/car");        break;
                case 8:  pNetGame->SendChatCommand("/report");     break;
                case 9:  pNetGame->SendChatCommand("/donate");     break;
                case 10: pNetGame->SendChatCommand("/everyprize"); break;
                case 11: pNetGame->SendChatCommand("/anim");       break;
                case 12: pNetGame->SendChatCommand("/tab");        break;
                case 14: pNetGame->SendChatCommand("/binder");     break;
                default: break;
            }
            break;

        case 2:
            switch (action) {
                case 0:
                    // ...
                    break;
                default: break;
            }
            break;

        default: break;
    }
}


// 🔹 Статистика (окно StatisticsView)
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_stats_StatisticsView_sendRPC(JNIEnv *env, jclass clazz, jint type, jbyteArray str, jint action) {
    if (!pNetGame) return;

    switch (type) {
        case 1:
            switch (action) {
                case 1:  pNetGame->SendChatCommand("/i");          break;
                case 2:  pNetGame->SendChatCommand("/music");      break;
                case 3:  pNetGame->SendChatCommand("/inv");        break;
                case 4:  pNetGame->SendChatCommand("/leaders");    break;
                case 5:  pNetGame->SendChatCommand("/fmenu");      break;
                case 6:  pNetGame->SendChatCommand("/hmenu");      break;
                case 7:  pNetGame->SendChatCommand("/car");        break;
                case 8:  pNetGame->SendChatCommand("/report");     break;
                case 9:  pNetGame->SendChatCommand("/donate");     break;
                case 10: pNetGame->SendChatCommand("/everyprize"); break;
                case 11: pNetGame->SendChatCommand("/anim");       break;
                case 12: pNetGame->SendChatCommand("/tab");        break;
                case 14: pNetGame->SendChatCommand("/binder");     break;
                default: break;
            }
            break;

        case 2:
            switch (action) {
                case 0:
                    // ...
                    break;
                default: break;
            }
            break;

        default: break;
    }
}


// 🔹 Меню паузы (PauseManager)
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_mobilemenu_PauseManager_sendRPC(JNIEnv *env, jclass clazz, jint type, jbyteArray str, jint action) {
    if (!pNetGame) return;

    switch (type) {
        case 1:
            switch (action) {
                case 1:  pNetGame->SendChatCommand("/rep");        break; // администрация
                case 2:  pNetGame->SendChatCommand("/help");        break; // карта
                case 3:  pNetGame->SendChatCommand("/exit");       break; // выйти
                default: break;
            }
            break;

        default: break;
    }
}

}


extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_CasinoDice_SendCasinoButt(JNIEnv *env, jobject thiz, jint button_id) {
	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_SHOW_DICE_TABLE;
	uint8_t button = button_id;


	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(button);
	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_core_Samp_togglePlayer(JNIEnv *env, jobject thiz, jint toggle) {
	if(toggle)
		pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(false, true);
	else
		pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(true, true);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_SnapShot_00024Companion_nativeEntitySnapAddToQueue(
        JNIEnv* env, jobject /*thiz*/,
        jint id, jobject image_view,
        jint type, jint modelid,
        jint color1, jint color2,
        jfloat zoom,
        jfloat rotX, jfloat rotY, jfloat rotZ,     // ← это ИМЕННО ВРАЩЕНИЯ
        jfloat offX, jfloat offY, jfloat offZ      // ← это ИМЕННО СМЕЩЕНИЯ (offset)
) {
    if (!image_view) {
        Log("Snap: image_view is null, skip (modelId %d)", modelid);
        return;
    }

    // Нормализуем немного входные значения (на всякий случай)
    if (zoom <= 0.0f) zoom = 0.01f;

    // Собираем элемент очереди
    CSnapShotWrapper::queueMutex.lock();
    auto* q = new CSnapShotWrapper::QueueItem();
    q->jid       = static_cast<int>(id);
    q->type      = static_cast<int>(type);
    q->id        = static_cast<int>(modelid);
    q->ImageView = env->NewGlobalRef(image_view);
    q->color1    = static_cast<int>(color1);
    q->color2    = static_cast<int>(color2);

    // Камерные параметры
    q->zoom      = zoom;
    q->vecRot.x  = rotX;   // distance/rotX (как у тебя принято)
    q->vecRot.y  = rotY;   // yaw
    q->vecRot.z  = rotZ;   // pitch

    // Позиционные оффсеты цели/педа — ОБЯЗАТЕЛЬНО применяй их в рендере!
    // Важно: offZ — это «вверх/вниз». Чтобы «опустить» педа в кадре, offZ должен быть ОТРИЦАТЕЛЕН.
    q->vecOffset = CVector{ offX, offY, offZ };

    CSnapShotWrapper::list.push(q);
    CSnapShotWrapper::isProcessing = true;
    CSnapShotWrapper::queueMutex.unlock();

    Log("Snap: queued modelId %d, zoom=%.3f, rot[%.2f, %.2f, %.2f], off[%.3f, %.3f, %.3f]",
        modelid, zoom, rotX, rotY, rotZ, offX, offY, offZ);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_BoomBox_ClickBoomBox(JNIEnv *env, jobject thiz, jint button) {
    RakNet::BitStream bsSend;

    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t) 0x39);
    bsSend.Write((uint8_t)button);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tablet_TabletBridge_startBoombox(JNIEnv *env, jclass clazz,
                                                              jstring url, jstring title) {
    // TODO: implement startBoombox()

    Log("startBoombox");
    const char *_url = env->GetStringUTFChars(url, nullptr);

    if(pNetGame){
        uint16_t urlLen = strlen(_url);
        RakNet::BitStream bsSend;

        bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
        bsSend.Write((uint8_t) 0x70);
        bsSend.Write(urlLen);
        bsSend.Write(_url, urlLen);

        pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
    }
    env->ReleaseStringUTFChars(url, _url);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tablet_TabletBridge_stopBoombox(JNIEnv *env, jclass clazz) {
    // TODO: implement stopBoombox()

    Log("stopBoombox");
    RakNet::BitStream bsSend;

    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t) 0x71);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tablet_TabletBridge_setBoomboxMode(JNIEnv *env, jclass clazz,
                                                                jboolean enabled) {
    // TODO: implement setBoomboxMode()

    Log("setBoomboxMode");
    uint8_t action = enabled ? 1 : 0;

    RakNet::BitStream bsSend;

    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t) 0x72);
    bsSend.Write((uint8_t) action);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_tablet_TabletBridge_setSubwooferMode(JNIEnv *env, jclass clazz,
                                                                  jboolean enabled) {
    // TODO: implement setSubwooferMode()

    Log("setSubwooferMode");
    uint8_t action = enabled ? 1 : 0;

    RakNet::BitStream bsSend;

    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t) 0x89);
    bsSend.Write((uint8_t) action);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}