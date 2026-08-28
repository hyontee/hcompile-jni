//
// Created by Error on 24.04.2024.
//

#include "CFamily.h"
#include "main.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"



jclass CFamily::clazz = nullptr;
jobject CFamily::thiz = nullptr;
bool CFamily::bIsShow = false;
extern CNetGame *pNetGame;

void CFamily::startRender(){
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    if(CFamily::thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(CFamily::clazz, "<init>",
                                                 "(Landroid/app/Activity;)V");
        CFamily::thiz = env->NewObject(CFamily::clazz, constructor, g_pJavaWrapper->activity);
        CFamily::thiz = env->NewGlobalRef(CFamily::thiz);
    }

    //CFamily::show(toggle, name, money, coin);
}
void CFamily::show(bool toggle, const char name[], int money, int coin, bool ifLeader){
    if(CFamily::thiz == nullptr)return;

    //char name_utf[1024];
    //cp1251_to_utf8(name_utf, name);

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring jsprite = env->NewStringUTF( name);

    jmethodID method = env->GetMethodID(CFamily::clazz, "show", "(ZLjava/lang/String;IIZ)V");
    env->CallVoidMethod(CFamily::thiz, method, toggle, jsprite, money, coin, ifLeader);
    env->DeleteLocalRef(jsprite);
    CFamily::bIsShow = toggle;

}

void CFamily::LoadCar(const char name[], uint16_t vehid, uint16_t access, bool isspawn) {
    //char name_utf[1024];
   // cp1251_to_utf8(name_utf, name);

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    //jstring jname = env->NewStringUTF( name_utf);
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);
    jstring jname = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

    jmethodID method = env->GetMethodID(CFamily::clazz, "addCar", "(Ljava/lang/String;IIZ)V");
    env->CallVoidMethod(CFamily::thiz, method, jname, vehid, access, isspawn);
}

void CFamily::LoadNotif(const char text[], const char name[]) {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring jtext = env->NewStringUTF( text );
    jstring jname = env->NewStringUTF( name );

    jmethodID method = env->GetMethodID(CFamily::clazz, "addNotif", "(Ljava/lang/String;Ljava/lang/String;)V");
    env->CallVoidMethod(CFamily::thiz, method, jtext, jname);
    env->DeleteLocalRef(jtext);
    env->DeleteLocalRef(jname);
}

void CFamily::setInfoMainMenu(int status, int rep, int people,
                              int upd_sklad, int upd_weapon, int upd_people,
                              int money, int mats, int mask,
                              int oils, int ammo, int armor,
                              int skin_id, const char playername[]){
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring jplayername = env->NewStringUTF( playername );

    jmethodID method = env->GetMethodID(CFamily::clazz, "setInfoMainMenu", "(IIIIIIIIIIIIILjava/lang/String;)V");
    env->CallVoidMethod(CFamily::thiz, method, status, rep, people, upd_sklad, upd_weapon, upd_people, money, mats, mask, oils, ammo, armor, skin_id, jplayername);
    env->DeleteLocalRef(jplayername);
}
void CFamily::LoadPlayers(int id, const char name[], int lvl, const char nameRang[], int rang, int warn, int phone, int mute, bool isOnline){
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring jplayername = env->NewStringUTF( name );
    jstring jnamerang = env->NewStringUTF( nameRang );

    jmethodID method = env->GetMethodID(CFamily::clazz, "addPlayers", "(ILjava/lang/String;ILjava/lang/String;IIIIZ)V");

    env->CallVoidMethod(CFamily::thiz, method, id, jplayername, lvl, jnamerang, rang, warn, phone, mute, isOnline);

    env->DeleteLocalRef(jplayername);
    env->DeleteLocalRef(jnamerang);
}
void CFamily::UpdateAdapterCar(int access, int id, bool isspawn){
    JNIEnv *env = g_pJavaWrapper->GetEnv();


    jmethodID method = env->GetMethodID(CFamily::clazz, "UpdateAdapterCar", "(IIZ)V");

    env->CallVoidMethod(CFamily::thiz, method, access, id, isspawn);

}
void CFamily::UpdateAdapterPlayer(int id, int warn, int rang, int mute){
    JNIEnv *env = g_pJavaWrapper->GetEnv();


    jmethodID method = env->GetMethodID(CFamily::clazz, "UpdateAdapterPlayer", "(IIII)V");

    env->CallVoidMethod(CFamily::thiz, method, id, warn, rang, mute);

}

void CFamily::LoadNameRang(const char rang[]){
    /*JNIEnv *env = g_pJavaWrapper->GetEnv();

    jstring jrang = env->NewStringUTF( ConvertColorToHtml(rang) );

    jmethodID method = env->GetMethodID(CFamily::clazz, "addRang", "(Ljava/lang/String;)V");

    env->CallVoidMethod(CFamily::thiz, method, jrang);

    env->DeleteLocalRef(jrang);*/

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    //jstring jname = env->NewStringUTF( name_utf);
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray bytes = env->NewByteArray(strlen(rang));
    env->SetByteArrayRegion(bytes, 0, strlen(rang), (jbyte*)rang);
    jstring jrang= (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

    jmethodID method = env->GetMethodID(CFamily::clazz, "addRang", "(Ljava/lang/String;)V");
    env->CallVoidMethod(CFamily::thiz, method, jrang);
}
void CFamily::SetAccessRang(uint16_t sklad, uint16_t invite, uint16_t uninvite, uint16_t mute, uint16_t giverang){
    JNIEnv *env = g_pJavaWrapper->GetEnv();


    jmethodID method = env->GetMethodID(CFamily::clazz, "SetAccessRang", "(IIIII)V");

    env->CallVoidMethod(CFamily::thiz, method, sklad, invite, uninvite, mute, giverang);
}
void CFamily::AddInfoStock(int allmoney, int mats, int mask, int oil, int armor){
    JNIEnv *env = g_pJavaWrapper->GetEnv();


    jmethodID method = env->GetMethodID(CFamily::clazz, "AddInfoStock", "(IIIII)V");

    env->CallVoidMethod(CFamily::thiz, method, allmoney, mats, mask, oil, armor);
}
void CNetGame::packetFamilyShow(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t toggle;
    uint16_t len;
    uint32_t money;
    uint32_t coin;
    uint8_t ifLeader;

    char name[256];

    bs.Read(toggle);
    bs.Read(len);
    bs.Read(name, len);
    name[len] = '\0';
    bs.Read(money);
    bs.Read(coin);
    bs.Read(ifLeader);


    char utf8[123];
    cp1251_to_utf8(utf8, name);
    CFamily::show(toggle, utf8, money, coin, ifLeader);
}
void CNetGame::packetLoadRangName(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t len;

    char rang[32];
    //memset(rang, 0, sizeof(rang));
    bs.Read(len);
    bs.Read(rang, len);
    rang[len] = '\0';

    char utf8[32];
    //memset(utf8, 0, sizeof(utf8));
    cp1251_to_utf8(utf8, rang);

    CFamily::LoadNameRang(utf8);
}

void CFamily::LoadRaiting(const char name[], uint32_t rep, uint16_t capture) {
    //char name_utf[1024];
    // cp1251_to_utf8(name_utf, name);

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    //jstring jname = env->NewStringUTF( name_utf);
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);
    jstring jname = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

    jmethodID method = env->GetMethodID(CFamily::clazz, "addRaiting", "(Ljava/lang/String;II)V");
    env->CallVoidMethod(CFamily::thiz, method, jname, rep, capture);
}

void CNetGame::packetFamilyLoadCar(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    char szBuff[4096+1];
    char name[256];


    uint16_t len_name, vehid, access;
    bool isspawn;
    bs.Read(len_name);

    memset(name, 0, sizeof(name));
    memset(szBuff, 0, sizeof(szBuff));

    bs.Read(szBuff, len_name);
    bs.Read(vehid);
    bs.Read(access);
    bs.Read(isspawn);
    cp1251_to_utf8(name, szBuff);

    CFamily::LoadCar(name, vehid, access, isspawn);
}
void CNetGame::packetFamilyLoadNotif(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    char name[26];
    char text[256];
    uint16_t len, len2;

    bs.Read(len2);
    bs.Read(text, len2);
    text[len2] = '\0';

    bs.Read(len);
    bs.Read(name, len);
    name[len] = '\0';

    char utf8_1[123];
    cp1251_to_utf8(utf8_1, text);
    char utf8_2[123];
    cp1251_to_utf8(utf8_2, name);

    CFamily::LoadNotif(utf8_1, utf8_2);
}

void CNetGame::packetFamilyLoadRaiting(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    char szBuff[4096+1];
    char name[256];

    uint32_t rep;
    uint16_t len_name, capture;
    bs.Read(len_name);

    memset(name, 0, sizeof(name));
    memset(szBuff, 0, sizeof(szBuff));

    bs.Read(szBuff, len_name);
    bs.Read(rep);
    bs.Read(capture);
    cp1251_to_utf8(name, szBuff);

    CFamily::LoadRaiting(name, rep, capture);
}

void CNetGame::packetFamilyInfoMainMenu(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    char name[26];
    uint16_t len;
    uint8_t status;
    uint32_t rep;
    uint16_t people;
    uint8_t upd_sklad;
    uint8_t upd_weapon;
    uint8_t upd_people;
    uint32_t money;
    uint32_t mats;
    uint16_t mask;
    uint16_t oils;
    uint32_t ammo;
    uint32_t armor;
    uint16_t skin_id;

    bs.Read(status);
    bs.Read(rep);
    bs.Read(people);
    bs.Read(upd_sklad);
    bs.Read(upd_weapon);
    bs.Read(upd_people);
    bs.Read(money);
    bs.Read(mats);
    bs.Read(mask);
    bs.Read(oils);
    bs.Read(ammo);
    bs.Read(armor);
    bs.Read(skin_id);

    bs.Read(len);
    bs.Read(name, len);
    name[len] = '\0';

    char utf8_2[123];
    cp1251_to_utf8(utf8_2, name);

    CFamily::setInfoMainMenu(status, rep, people, upd_sklad, upd_weapon, upd_people, money, mats, mask, oils, ammo, armor, skin_id, utf8_2);
}

void CNetGame::packetFamilyLoadPlayers(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    char name[26], rangname[26];
    uint16_t lvl, len, len2;
    uint8_t rang, warn;
    uint32_t phone, mute, id;
    bool isOnline;

    bs.Read(id);

    bs.Read(len);
    bs.Read(name, len);
    name[len] = '\0';

    bs.Read(lvl);

    bs.Read(len2);
    bs.Read(rangname, len2);
    rangname[len2] = '\0';

    bs.Read(rang);
    bs.Read(warn);
    bs.Read(phone);
    bs.Read(mute);
    bs.Read(isOnline);


    char utf8_2[123];
    cp1251_to_utf8(utf8_2, name);

    char utf8_1[123];
    cp1251_to_utf8(utf8_1, rangname);
    CFamily::LoadPlayers(id,utf8_2, lvl, utf8_1, rang, warn, phone, mute, isOnline);
}

void CNetGame::packetFamilyLoadRangCar(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t vehid, access;
    bool isspawn;
    bs.Read(vehid);
    bs.Read(access);
    bs.Read(isspawn);

    CFamily::UpdateAdapterCar(access, vehid, isspawn);
}
void CNetGame::packetFamilyLoadPlayerInfo(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t id, rang, warn;
    uint32_t mute;

    bs.Read(id);
    bs.Read(rang);
    bs.Read(warn);
    bs.Read(mute);

    CFamily::UpdateAdapterPlayer(id, warn, rang, mute);
}
void CNetGame::packetFamilyAccessRang(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t sclad, invite, uninvite, mute, giverang;

    bs.Read(sclad);
    bs.Read(invite);
    bs.Read(uninvite);
    bs.Read(mute);
    bs.Read(giverang);

    CFamily::SetAccessRang(sclad, invite, uninvite, mute, giverang);
}
void CNetGame::packetFamilyInfoStock(Packet* p){
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint32_t allmoney, mats, mask, oil, armor;

    bs.Read(allmoney);
    bs.Read(mats);
    bs.Read(mask);
    bs.Read(oil);
    bs.Read(armor);

    CFamily::AddInfoStock(allmoney, mats, mask, oil, armor);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickManagerFamily(JNIEnv *env, jobject thiz, jint buttonid) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x40);
    bsSend.Write((uint8_t)buttonid);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilyMain(JNIEnv *env, jobject thiz,
                                                                     jint button) {
    switch (button) {
        case 0: {
            RakNet::BitStream bsSend;
            bsSend.Write((uint8_t)ID_CUSTOM_RPC);
            bsSend.Write((uint8_t)0x35);
            bsSend.Write((uint8_t)button);

            pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
        }
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilyCar(JNIEnv *env, jobject thiz,
                                                                       jint button, jint id) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x36);
    bsSend.Write((uint8_t)button);
    bsSend.Write((uint16_t)id);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilyControl(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jint id, jint button) {
    // TODO: implement ClickFamilyControl()
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x37);
    bsSend.Write((uint32_t)id);
    bsSend.Write((uint8_t)button);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilyNotification(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jbyteArray text) {
    // TODO: implement ClickFamilyNotification()
    jboolean isCopy = true;
    jbyte* pMsg = env->GetByteArrayElements(text, &isCopy);
    jsize length = env->GetArrayLength(text);

    std::string szStr((char*)pMsg, length);


    uint16_t textLen = strlen((char*)szStr.c_str());
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x41);
    bsSend.Write(textLen);
    bsSend.Write((char*)szStr.c_str(), textLen);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
    env->ReleaseByteArrayElements(text, pMsg, JNI_ABORT);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilySettingColorChat(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jstring color) {
    // TODO: implement ClickFamilySettingColorChat()
    const char *_color = env->GetStringUTFChars(color, nullptr);
    RakNet::BitStream bsSend;
    uint16_t colorLen = strlen(_color);
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t) 0x42);
    bsSend.Write(colorLen);
    bsSend.Write(_color, colorLen);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
    env->ReleaseStringUTFChars(color, _color);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilySettingChangeRangName(
        JNIEnv *env, jobject thiz, jint id, jbyteArray name) {
    // TODO: implement ClickFamilySettingChangeRangName()
    jboolean isCopy = true;
    jbyte* pMsg = env->GetByteArrayElements(name, &isCopy);
    jsize length = env->GetArrayLength(name);

    std::string szStr((char*)pMsg, length);


    uint16_t textLen = strlen((char*)szStr.c_str());
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x43);
    bsSend.Write(textLen);
    bsSend.Write((char*)szStr.c_str(), textLen);
    bsSend.Write((uint16_t) id);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
    env->ReleaseByteArrayElements(name, pMsg, JNI_ABORT);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilySettingSetAccess(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jint rang,
                                                                                    jint access,
                                                                                    jboolean toggle) {
    // TODO: implement ClickFamilySettingSetAccess()


    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x44);
    bsSend.Write((uint16_t) rang);
    bsSend.Write((uint16_t) access);
    bsSend.Write((bool) toggle);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilyMainMenu(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint button) {
    // TODO: implement ClickFamilyMainMenu()
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x45);
    bsSend.Write((uint16_t) button);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickQuestFamily(JNIEnv *env, jobject thiz,
                                                                         jint button) {
    // TODO: implement ClickQuestFamily()
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x46);
    bsSend.Write((uint16_t) button);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickBuyTokenFamily(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint coin) {
    // TODO: implement ClickBuyTokenFamily()
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x47);
    bsSend.Write((uint16_t) coin);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilyStokeClick(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint button) {
    // TODO: implement ClickFamilyStokeClick()
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x48);
    bsSend.Write((uint16_t) button);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilyUpgrade(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jint button) {
    // TODO: implement ClickFamilyUpgrade()
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x49);
    bsSend.Write((uint16_t) button);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtacr_gui_family_BrFamilyManager_ClickFamilyShop(JNIEnv *env, jobject thiz,
                                                                        jint itemid) {
    // TODO: implement ClickFamilyShop()
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)0x50);
    bsSend.Write((uint16_t) itemid);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}