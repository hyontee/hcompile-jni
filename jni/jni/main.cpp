//
// Created by gen1z on 02/03/2024.
//

#include <string.h>
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include "main.h"
#include "util/armhook.h"
#include "util/patch.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "settings.h"
#include "network/CNetwork.h"
#include "gui/CFontRenderer.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "dialog.h"
#include "util/CJavaWrapper.h"

uintptr_t g_libBR;
uintptr_t g_libGTASA;
uintptr_t* g_CNetGame;
char* g_pszStorage;
uintptr_t g_iGameState;

CGame* pGame = nullptr;
CNetGame* pNetGame = nullptr;
CGUI* pGUI;
CKeyBoard *pKeyBoard = nullptr;
CDialogWindow *pDialogWindow;
CChatWindow *pChatWindow = nullptr;
CSettings *pSettings;
CSnapShotHelper *pSnapShotHelper;

extern uint32_t g_uiHeadMoveEnabled;
extern uint32_t g_uiBorderedText;

//int RemoveModelIDs[MAX_REMOVE_MODELS];
//VECTOR RemovePos[MAX_REMOVE_MODELS];
//float RemoveRad[MAX_REMOVE_MODELS];
//int iTotalRemovedObjects = 0;

//bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
//{
//    const int hexCount = (int)(end - start);
//    if (hexCount == 6 || hexCount == 8)
//    {
//        char hex[9];
//        strncpy(hex, start, hexCount);
//        hex[hexCount] = 0;
//
//        unsigned int hexColor = 0;
//        if (sscanf(hex, "%x", &hexColor) > 0)
//        {
//            color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
//            color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
//            color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
//            color.w = 1.0f;
//
//            if (hexCount == 8)
//                color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
//
//            return true;
//        }
//    }
//
//    return false;
//}

void Log(const char* fmt, ...) {
    char buffer[10000];
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    __android_log_write(ANDROID_LOG_INFO, OBFUSCATE("BR-NARKOTA"), buffer);
}

void LOGN(const char* fmt, ...){
    char buffer[10000];
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    __android_log_write(ANDROID_LOG_INFO, OBFUSCATE("BR-NARKOTA-NETWORK"), buffer);
}


void LogToFile(char* tag, const char *fmt, ...)
{
    char buffer[10000];
    static FILE* flLog = nullptr;
    static const char* g_pszStorage = OBFUSCATE("/storage/emulated/0/Android/data/blackrussia.online/files/SAMP/shader_log.txt");

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%s", g_pszStorage);
        flLog = fopen(buffer, "a");
    }
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

//	firebase::crashlytics::Log(buffer);

    //if(pDebug) pDebug->AddMessage(buffer);

    if(flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);

    __android_log_write(ANDROID_LOG_INFO, tag, buffer);

    return;
}


std::string g_6FF54_name;

// Функция для получения имени функции по адресу
std::string getFunctionNameFromAddress(void* addr, const char* libPath) {
    // Получаем адрес функции в шестнадцатеричном формате
    std::stringstream ss;
    ss << std::hex << addr;
    std::string addrHex = ss.str();
    std::string libStr = std::string(libPath);

    // Формируем команду для выполнения в командной строке
    std::string command = "nm -D " + libStr + " | grep ' " + addrHex + " '";

    // Открываем поток для выполнения команды и получения результата
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        Log("Failed to execute command: %s", command.c_str());
        return "Unknown function";
    }

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    // Извлекаем имя функции из результата
    std::istringstream iss(result);
    std::string functionName;
    while (iss >> functionName) {
        // Пропускаем адрес и получаем имя функции
        if (functionName != addrHex) {
            return functionName;
        }
    }

    return "Unknown function";
}

void LOGFN(const char *fmt, ...)
{
    char buffer[10000];
    char* tag = OBFUSCATE("UGHUR-RE-FUNCTIONS");
    static FILE* flLog = nullptr;
    static const char* g_pszStorage = OBFUSCATE("/storage/emulated/0/Android/data/blackrussia.online/files/SAMP/functions.txt");

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%s", g_pszStorage);
        flLog = fopen(buffer, "a");
    }
    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

//	firebase::crashlytics::Log(buffer);

    //if(pDebug) pDebug->AddMessage(buffer);

    if(flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);

    __android_log_write(ANDROID_LOG_INFO, tag, buffer);

    return;
}

void (*sub)(uintptr_t *, char*, int, char*, char*);
void sub_hook(uintptr_t * a1, char* a2, int a3, char* a4, char* a5){
    Log("3");
    Log(OBFUSCATE("IP: %s"), a2);
    Log(OBFUSCATE("PORT: %i"), a3);
    Log(OBFUSCATE("SERV PASS: %s"), a5);


    pNetGame = new CNetGame(
            g_sEncryptedAddresses[0].decrypt(),
            g_sEncryptedAddresses[0].getPort(),
            "Dev_Ughur",
            pSettings->GetReadOnly().szPassword);

    //uintptr_t* i = (uintptr_t*)pNetGame;
    *(uintptr_t* *)(g_libBR + 0x207F28) = *(uintptr_t* *)pNetGame;
    //static bool once = false;
    //if(!once){
    //    g_iGameState = *(uintptr_t *)(g_CNetGame + 10);
    //    once = true;
    //}
    //ScriptCommand(&toggle_player_controllable, 2, 0);
    //sub(a1, (char *)(g_sEncryptedAddresses[0].decrypt()), g_sEncryptedAddresses[0].getPort(), a4, a5);
    //strncpy((char *)((*(uintptr_t *)(g_libBR + 0x207F28)) + 703), (char *)(g_sEncryptedAddresses[0].decrypt()), 0x7Fu);
    //*(uintptr_t *)((*(uintptr_t *)(g_libBR + 0x207F28)) + 830) = g_sEncryptedAddresses[0].getPort();
    Log("IP: %s", (char *)((*(uintptr_t *)(g_libBR + 0x207F28)) + 703));
    Log("PORT: %i", *(uintptr_t *)((*(uintptr_t *)(g_libBR + 0x207F28)) + 830));
    return;
}

//PED_TYPE* GamePool_Ped_GetAt(int iID)
//{
//    return (( PED_TYPE* (*)(int))(g_libGTASA+0x41DD7C+1))(iID);
//}

void copyTwoDimTo1D(const char* const* twoDimArray, const char*& oneDimArray) {
    // Определяем размер двумерного массива
    int numRows = 0;
    int totalLength = 0;
    while (twoDimArray[numRows] != nullptr) {
        totalLength += strlen(twoDimArray[numRows]) + 1; // Для символа нулевого завершения
        numRows++;
    }

    // Выделяем память под одномерный массив
    oneDimArray = new char[totalLength];

    // Копируем данные из двумерного массива в одномерный
    int index = 0;
    for (int i = 0; i < numRows; ++i) {
        strcpy(const_cast<char*>(oneDimArray) + index, twoDimArray[i]);
        index += strlen(twoDimArray[i]) + 1;
        if (i < numRows - 1) {
            const_cast<char*>(oneDimArray)[index - 1] = ' '; // Добавляем пробел между строками
        }
    }
}
void (*glShader)(uintptr_t a1, size_t a2, char* const* a3, const int* a4);
void glShader_hook(uintptr_t a1, size_t a2, char* const* a3, const int* a4){

    //const char* shaders1DArray;
    //copyTwoDimTo1D(a3, shaders1DArray);
    __android_log_print(ANDROID_LOG_INFO, OBFUSCATE("BR-SHADERS0"), "%s", *a3);

    return glShader(a1, a2, a3, a4);
}

void (*ES2Shader__Build)(uintptr_t *thiz, const char *a2, const char *a3);
void ES2Shader__Build_hook(uintptr_t *thiz, const char *a2, const char *a3){

    //const char* shaders1DArray;
    //copyTwoDimTo1D(a3, shaders1DArray);

    __android_log_print(ANDROID_LOG_INFO, OBFUSCATE("BR-SHADERS0"), "%s\n%s", a2, a3);

    return ES2Shader__Build(thiz, a2, a3);
}

int (*compileShadersBR)(int a1, const char* a2, const char* a3, int a4);
int compileShadersBR_hook(int a1, const char* a2, const char* a3, int a4){

    //const char* shaders1DArray;
    //copyTwoDimTo1D(a3, shaders1DArray);

    LogToFile("BR-SHADERS", "flags: %i |\n %s\n%s", a4, a2, a3);

    return compileShadersBR(a1, a2, a3, a4);
}

extern "C"
{
JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initNARKOTA(JNIEnv* pEnv, jobject thiz)
{
    static bool once = false;
    if(!once){
        Log("1");
        //*(char *)(g_libBR + 0x207FA4) = 1;
        //UnFuck(g_libBR + 0x055814);
        //CHook::CallFunction<int>(g_libBR + 0x055814 + 1, &toggle_player_controllable, 2, 0);
        g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);
        pSettings = new CSettings();

        once = true;
    }
}
JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendConnect(JNIEnv* pEnv, jobject thiz, jint num)
{
    if(!pNetGame){
        pNetGame = new CNetGame(
                g_sEncryptedAddresses[0].decrypt(),
                g_sEncryptedAddresses[0].getPort(),
                "Dev_Ughur",
                "");
    }
}
}

void (*nullsub_2)(const char *a1);
void nullsub_2_hook(const char *a1)
{
    Log(a1);
}

//const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[2] = {
//        CSetServer::create(OBFUSCATE("188.127.241.74"), 1, 16, 8826, 1), // 0
//        CSetServer::create(OBFUSCATE("188.127.241.74"), 1, 16, 8826, 1) // 1
//};

//uintptr_t *(*connect)(uintptr_t * a1);
//uintptr_t *connect_hook(uintptr_t * a1)
//{
//
//    if(!pNetGame){
//        pNetGame = new CNetGame(
//                g_sEncryptedAddresses[0].decrypt(),
//                g_sEncryptedAddresses[0].getPort(),
//                OBFUSCATE("Dev_Ughur"),
//                "");
//    }
//
//    return a1;
//}


RakClientInterface* m_pRakClient;
uint32_t			m_dwLastConnectAttempt;
void RegisterRPCs(RakClientInterface* pRakClient);
void UnRegisterRPCs(RakClientInterface* pRakClient);
void RegisterScriptRPCs(RakClientInterface* pRakClient);
void UnRegisterScriptRPCs(RakClientInterface* pRakClient);
void (*connect)(uintptr_t *);
void connect_hook(uintptr_t * a1)
{

    //m_pRakClient = RakNetworkFactory::GetRakClientInterface();
    //RegisterRPCs(m_pRakClient);
    //RegisterScriptRPCs(m_pRakClient);
    //m_pRakClient->SetPassword(pSettings->GetReadOnly().szPassword);
//
    //m_pRakClient->Connect(OBFUSCATE("188.127.241.74"), 8826, 0, 0, 5);
    //m_dwLastConnectAttempt = GetTickCount();

    //if(!pNetGame){
    //pNetGame = new CNetGame(
    //        g_sEncryptedAddresses[0].decrypt(),
    //        g_sEncryptedAddresses[0].getPort(),
    //        OBFUSCATE("Dev_Ughur"),
    //        "");
    //}

    pNetGame = new CNetGame(
            g_sEncryptedAddresses[0].decrypt(),
            g_sEncryptedAddresses[0].getPort(),
            pSettings->GetReadOnly().szNickName,
            pSettings->GetReadOnly().szPassword);

    //uintptr_t* i = (uintptr_t*)pNetGame;
    //*(uintptr_t* *)(g_libBR + 0x207F28) = ()pNetGame;

    uintptr_t * i/* = connect(a1)*/;
    //*(uintptr_t* *)(g_libBR + 0x0207F8) = *(uintptr_t* *)pNetGame;
    //((void(*)(uintptr_t*, const char *, int, uintptr_t, uintptr_t, uintptr_t))((*(uintptr_t* *)(g_libBR + 0x207F28)) + 8))((*(uintptr_t**)((*(uintptr_t* *)(g_libBR + 0x207F28)) + 8)), "188.127.241.74", 8826, 0, 0, 5);
    return;
}

ImVec2		m_vecScale;
float 		m_fFontSize;

float ScaleX(float x) { return m_vecScale.x * x; }
float ScaleY(float y) { return m_vecScale.y * y; }
float GetFontSize() { return m_fFontSize; }

void RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr)
{
    int iOffset = 2;

    if (bOutline)
    {
        if (g_uiBorderedText)
        {
            posCur.x -= iOffset;
            ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
            posCur.x += iOffset;
            // right
            posCur.x += iOffset;
            ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
            posCur.x -= iOffset;
            // above
            posCur.y -= iOffset;
            ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
            posCur.y += iOffset;
            // below
            posCur.y += iOffset;
            ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
            posCur.y -= iOffset;
        }
        else
        {
            ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());

            ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
            ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
        }
    }

    ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}


PED_TYPE* m_pPed = nullptr;
ENTITY_TYPE* m_pEntity = nullptr;
CPlayerPed* pPlayerPed = nullptr;
CLocalPlayer* pLocalPlayer;
uintptr_t* g_CTouchInterface;
uintptr_t* g_CWL;
VECTOR m_pCameraPos;
uint32_t m_dwGTAId;
uintptr_t * g_Xuyna;
bool bInitedInGame = false;
void InitScripting();
extern char* PLAYERS_REALLOC;
uint32_t bProcessedRender2dstuff;
void (*Render2dStuff)();
void Render2dStuff_hook(){


    static bool once = false;
    ImGuiIO &io = ImGui::GetIO();
    static CCamera* pCamera = nullptr;
    static ImVec2 _ImVec2;
    static CFontInstance* pFont;
    static uint8_t* m_pBitmap;

    //*(uint16_t*)(g_libGTASA+0x8B82A0+0x10C) = 0;
    //*(uint8_t*)(g_libGTASA+0x8EF36B) = 1;

    if (!once){

        pGame->InitInGame();
        //InitScripting();
//
        //PLAYERS_REALLOC = ((char* (*)(uint32_t))(g_libGTASA + 0x179B40))(404 * 257 * sizeof(char));
        //m_pPed = *(PED_TYPE**)PLAYERS_REALLOC;
        //m_pEntity = *(ENTITY_TYPE**)PLAYERS_REALLOC;

        //ScriptCommand(&lock_camera_position, 0);
        //ScriptCommand(&restore_camera_to_user);
        //ScriptCommand(&set_camera_behind_player);
        //ScriptCommand(&restore_camera_jumpcut);
//
        //uint32_t dwPlayerActorID = 0;
        //int iPlayerNum = 5;
        //ScriptCommand(&create_player, &iPlayerNum, 431.8335, 250.1207, 11.9394, &dwPlayerActorID);
        //ScriptCommand(&create_actor_from_player, &iPlayerNum, &dwPlayerActorID);
////
        //m_dwGTAId = dwPlayerActorID;
        //m_pPed = GamePool_Ped_GetAt(m_dwGTAId);
        //m_pEntity = (ENTITY_TYPE*)GamePool_Ped_GetAt(m_dwGTAId);

//


        //pPlayerPed = new CPlayerPed(7, 208, 431.8335, 250.1207, 11.9394, 0.0f);
        //pPlayerPed->TogglePlayerControllable(true);
        //pPlayerPed->Add();
        ////pPlayerPed->
        ////pPlayerPed->m_bytePlayerNumber;
        ////pCamera->Restore();
        ////pCamera->SetBehindPlayer();
//
        //pPlayerPed->SetInitialState();
        //pPlayerPed->ClearAllWeapons();
        //pPlayerPed->SetModelIndex(122);
        ////pPlayerPed->SetCollisionChecking(false);
        //pPlayerPed->SetGravityProcessing(true);
//
        //pPlayerPed->TeleportTo(431.8335, 250.1207, 11.9394);
        if(pGame){
            pCamera = pGame->GetCamera();
        }
        //pCamera = new CCamera();
        //pPlayerPed->TogglePlayerControllable(true);
        //pPlayerPed->Add();
        ////pPlayerPed->
        ////pPlayerPed->m_bytePlayerNumber;
        //pCamera->Restore();
        //pCamera->SetBehindPlayer();

        //pPlayerPed->SetInitialState();
        //pPlayerPed->ClearAllWeapons();
        //pPlayerPed->SetModelIndex(122);
        //pPlayerPed->SetCollisionChecking(false);
        //pPlayerPed->SetGravityProcessing(true);

        //pPlayerPed->TeleportTo(431.8335, 250.1207, 11.9394);

        //ScriptCommand(&camera_on_player, //pPlayerPed->m_dwGTAId, 0, 0);

        //pLocalPlayer = new CLocalPlayer();
        //pLocalPlayer->SetSpawnInfo(&spawnInfo);
        //pLocalPlayer->Spawn();

        //ScriptCommand(&lock_camera_position, 0);
        ////ScriptCommand(&toggle_player_controllable, 0, 0);
        //ScriptCommand(&restore_camera_to_user);
        //ScriptCommand(&set_camera_behind_player);
        //ScriptCommand(&restore_camera_jumpcut);

        //pGame->DisplayWidgets(true);
        //pGame->ToggleRadar(false);


        //pGame->NewVehicle(402, 433.8335, 250.1207, 11.9394, 0, false);

        ////pPlayerPed->AttachObject(pGame->GetCamera(), );

        //CHook::InlineHook(g_libBR, 0x02A4B4, (uintptr_t)connect_hook, (uintptr_t*)&connect);

        //io = ImGui::GetIO();

        //m_pBitmap = new uint8_t[PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH];

        //CHook::InlineHook(g_libBR, 0x02A4B4, (uintptr_t)sub_hook, (uintptr_t*)&sub);

        CHook::InlineHook(g_libBR, 0x02A4B4, (uintptr_t)connect_hook, (uintptr_t*)&connect);
        //CHook::InlineHook(g_libBR, 0x02A4B4, (uintptr_t)sub_hook, (uintptr_t*)&sub);
        once = true;
    }


    //if(pLocalPlayer){
    //    if(!onceO){
    //        PLAYER_SPAWN_INFO spawnInfo;
    //        VECTOR pos;
    //        int emptyArr[3] = {0, 0, 0};
    //        pos.X = 431.8335;
    //        pos.Y = 250.1207;
    //        pos.Z = 11.9394;
    //        spawnInfo.iSkin = 255;
    //        spawnInfo.unk = 0;
    //        spawnInfo.vecPos = pos;
    //        spawnInfo.fRotation = 0.0f;
    //        spawnInfo.byteTeam = 0;
    //        spawnInfo.iSpawnWeapons[0] = emptyArr[0];
    //        spawnInfo.iSpawnWeapons[1] = emptyArr[1];
    //        spawnInfo.iSpawnWeapons[2] = emptyArr[2];
    //        spawnInfo.iSpawnWeaponsAmmo[0] = emptyArr[0];
    //        spawnInfo.iSpawnWeaponsAmmo[1] = emptyArr[1];
    //        spawnInfo.iSpawnWeaponsAmmo[2] = emptyArr[2];
    //        pLocalPlayer->SetSpawnInfo(&spawnInfo);
    //        pLocalPlayer->Spawn();
    //        onceO = true;
    //    }
    //}

    //if(*(uintptr_t *)(g_libBR + 0x207F24) != 0){
    //    uintptr_t i = *(uintptr_t *)(g_libBR + 0x207F24);
    //
    //    ((void (*)(uintptr_t))(g_libBR + 0x04103C))(*(uintptr_t *)(i + 5));
    //}


//
    //if ( !*(char *)(g_libBR + 0x207F3C) && !*(char *)(g_libBR + 0x207F38) ){
    //    uintptr_t i = *(uintptr_t *)(*(uintptr_t *)(*(uintptr_t *)(g_libBR + 0x207F28 + 4) + 2) + 47);
//
    //    //ScriptCommand(&freeze_player, *(uintptr_t *)(i + 16), 0);
//
    //    ((void(*)(uintptr_t, uint8_t))(g_libBR + 0x053258))(i, 0);
    //}

    //pPlayerPed->TogglePlayerControllable(true);

    //m_pCameraPos = m_pEntity->mat->pos;

    //ScriptCommand(&set_camera_pos, m_pCameraPos.X, m_pCameraPos.Y, m_pCameraPos.Z);

    //((void(*)(uintptr_t))(g_libBR + 0x076874))(*(uintptr_t *)(*(uintptr_t *)(g_libBR + 0x207F28 + 4) + 2));

    //*(char *)(g_libBR + 0x207FA4) = 1;

    //MATRIX4X4 mat;
    //ScriptCommand(&freeze_player, //pPlayerPed->m_dwGTAId, 0);
    ////pPlayerPed->GetMatrix(&mat);
    //ScriptCommand(&restore_camera_to_user);
    ////pPlayerPed->m_pEntity = nullptr;
    //ScriptCommand(&set_camera_position, 431.8335, 250.1207, 11.9394, mat.at.X, mat.at.Y, mat.at.Z);
    //ScriptCommand(&set_camera, mat.pos.X, mat.pos.Y, mat.pos.Z);
    //SetPlayerPedPtrRecord(//pPlayerPed->m_bytePlayerNumber, (uintptr_t)//pPlayerPed->m_pPed);
    //pChatWindow->RenderText();



//
    MATRIX4X4 matFromPlayer;
    //pPlayerPed->SetCollisionChecking(true);
    //pPlayerPed->TogglePlayerControllable(true);
    //pPlayerPed->GetMatrix(&matFromPlayer);
    //if(pLocalPlayer){
    //    ((void(*)(uintptr_t*, float fX, float fY, float fZ, int iType))(g_libBR + 0x0410F4))(*(uintptr_t **)(g_libBR + 0x207F24 + 5), matFromPlayer.pos.X, matFromPlayer.pos.Y, matFromPlayer.pos.Z, 2);
    //}
//
    //CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
    //pLocalPlayerPed->GetMatrix(&matFromPlayer);
//
    //ImVec2 _ImVec2 = ImVec2(ScaleX(10), io.DisplaySize.y - ImGui::GetFontSize() * 0.85);
////
    char text[256];
    //snprintf(&text[0], 256, "Position: %.4f, %.4f, %.4f", mat.pos.X, mat.pos.Y, mat.pos.Z);
    if(pFont){
        //CFontRenderer::RenderText(text, pFont, )
    }
    //ImGui::GetBackgroundDrawList()->AddText(_ImVec2, ImColor(255, 255, 255, 255), text, nullptr);
////
    //RenderText(_ImVec2, (ImU32)0xFFFFFFFF, true, &text[0]);

    //pGUI->RenderPosition();

    bProcessedRender2dstuff = GetTickCount();

    //*(uint16_t*)(g_libGTASA+0x8B82A0+0x10C) = 0;
    //*(uint8_t*)(g_libGTASA+0x7165E8) = 0;

    //if(pNetGame) pNetGame->Process();
    //if(pNetGame) pLocalPlayer->Process();

    //static bool once1 = false;

    //if(bInitedInGame){
    //    if(!once1){
    //        if(pPlayerPed){
    //            ////pPlayerPed->TeleportTo(431.8335, 250.1207, 11.9394);
    //            once1 = true;
    //        }
    //    }
    //}

    //ScriptCommand(&lock_camera_position, 0);
    ////ScriptCommand(&restore_camera_to_user);
    //ScriptCommand(&set_camera_behind_player);
    //ScriptCommand(&restore_camera_jumpcut);
//
//
//
    //ScriptCommand(&restore_camera_to_user);
    //pPlayerPed->m_pEntity = nullptr;
    //ScriptCommand(&set_camera_position, matFromPlayer.pos.X, matFromPlayer.pos.Y, matFromPlayer.pos.Z, 0, 0, 0);

    //*(uint8_t *)(g_libGTASA + 0x8C9BA3) = 0;
    //((void(*)())(g_libGTASA + 0x3BF7A0 + 1))();

    //if(pLocalPlayer){
    //    ((void(*)(uintptr_t, uint8_t))(g_libBR + 0x053258))(*(uintptr_t *)(pLocalPlayer + 47), 1);
    //    ((void(*)(uintptr_t*))(g_libBR + 0x04103C))(*(uintptr_t **)(g_libBR + 0x207F24 + 5));
    //}

    //*(uint8_t*)(g_libGTASA + 0x8E864C) = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //(*(uint8_t**)(g_libGTASA + 0x5D021C))[101 * 1] = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //(*(uint8_t**)(g_libGTASA + 0x8E8650))[101 * 1] = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //ScriptCommand(&camera_on_player, &pLocalPlayer->m_pPlayerPed, 1, 1);

    //pLocalPlayer->Process();

    //((void(*)())(g_libBR + 0x02A2F8))();
    //((void(*)())(g_libBR + 0x61E030))();
    //((void(*)())(g_libGTASA + 0x26EE80 + 1))();
    //((void (*)(uintptr_t*))(g_libGTASA + 0x27AF4C + 1))(g_CWL);
    //((void(*)(bool))(g_libGTASA + 0x26EF20 + 1))(true);
    //if(*(uintptr_t**)(g_libBR + 0x207F28) && *(uintptr_t *)(*(uintptr_t *)(((*(uintptr_t**)(g_libBR + 0x207F28)) + 4) + 2) + 47)){
    //    //((int(*)(uintptr_t, uint8_t))(g_libBR + 0x053168))(*(uintptr_t *)(*(uintptr_t *)(((*(uintptr_t**)(g_libBR + 0x207F28)) + 4) + 2) + 47), 0);
    //    *(uintptr_t* *)((*(uintptr_t* *)(g_libBR + 0x207F28 + 4)) + 2) = *(uintptr_t**)pLocalPlayer;
    //}

    //if(pGUI) pGUI->Render();

    //*(int *)(g_libGTASA + 0x8B83AC) = 1 ^ 1;
    //*(char *)(g_libGTASA + 0x8EF36B) = 1 ^ 1;
    //*(uint8_t *)(g_libGTASA + 0x8C9BA3) = 0;
    //*(uint8_t *)(g_libGTASA + 0x8EF3CA) = 1;
    //*(uint8_t *)(g_libBR + 0x61E024) = 0;
    //*(uint8_t *)(g_libBR + 0x748F54) = 0;
    //*(uint8_t *)(g_libBR + 0x1FE083) = 1;
    //*(uint8_t *)(g_libBR + 0x201A04) = 1;

    Render2dStuff();

    return ;
}

//void (*CNetGame)(uintptr_t *a1);
//void CNetGame_hook(uintptr_t *a1)
//{
//    return;
//}
extern char* PLAYERS_REALLOC;
void GTASAInit();
//void (*InitInMenuBR)();
void InitInMenuBR()
{
    PLAYERS_REALLOC = *(char**)(g_libGTASA + 0x5D021C);
    GTASAInit();
    Log("init in menu");
    pGame = new CGame();
    pGame->InitInMenu();
    pGUI = new CGUI();
    pKeyBoard = new CKeyBoard();
    pChatWindow = new CChatWindow();
    pDialogWindow = new CDialogWindow();
    //pNetGame = *(CNetGame**)(g_libBR + 0x207F28);
    //CFontRenderer::Initialise();

    return;
}

void (*CGame_InitInGame)(uintptr_t *);
void CGame_InitInGame_hook(uintptr_t *a1)
{

    //pGame->InitInGame();

    //if(pNetGame) pNetGame->Process();

    return;
}

void (*connect2)(uintptr_t *a1);
void connect2_hook(uintptr_t *a1)
{

    g_iGameState = 14;

    return connect2(a1);
}

int (*ToggleSpectating)(uintptr_t *a1, int a2);
int ToggleSpectating_hook(uintptr_t *a1, int a2)
{

    char v3; // r5
    int result; // r0

    g_Xuyna = a1;

    v3 = a2;
    *(char *)(a1 + 268) = 1;
    ((void (*)(uintptr_t *))(g_libBR + 0x076874))(a1);
    result = 0;
    *(char *)(a1 + 3) = 0;
    *(char *)(a1 + 1) = v3;
    *(uintptr_t *)(a1 + 4) = 0xFFFFFFFF;
    *(char *)(a1 + 8) = 0;

    return result;

    //return ToggleSpectating(a1, a2);
}



void extractValuesAndCommand(const char* input, std::vector<float>& floats, char* command) {
    const char* pos = input;
    bool foundCommand = false;

    while (*pos) {
        if (*pos == '/') {
            foundCommand = true;
            const char* start = pos;
            while (*pos && *pos != ' ') ++pos; // идем до конца команды или до пробела
            strncpy(command, start, pos - start);
            command[pos - start] = '\0'; // завершаем строку нулевым символом
        } else if (isdigit(*pos) || (*pos == '-' && isdigit(*(pos + 1)))) {
            char* end;
            float value = strtof(pos, &end);
            floats.push_back(value);
            pos = end;
        } else {
            ++pos;
        }
    }

    if (!foundCommand) {
        command[0] = '\0'; // если команда не найдена, вернуть пустую строку
    }
}

uint8_t FindFirstFreePlayerPedSlot()
{
    uint8_t x = 2;
    while(x != 120)
    {
        if(!((*(char **)(g_libBR + 0x61DF38))[x])) return x;
        x++;
    }

    return 0;
}

bool isNumber(const std::string& s) {
    for (char c : s) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Function to parse words and digits from the input string
void parseString(const std::string& input, std::vector<std::string>& words, std::vector<std::string>& digits) {
    std::istringstream stream(input);
    std::string token;

    while (stream >> token) {
        if (isNumber(token)) {
            digits.push_back(token);
        } else {
            words.push_back(token);
        }
    }
}

void (*CChatwindow__ProcessCommands)(char *haystack);
void CChatwindow__ProcessCommands_hook(char *haystack)
{

    if(strstr(haystack, OBFUSCATE("/teleport"))) {
        std::vector<float> args;
        float* arrF;
        char command[256];
        extractValuesAndCommand(haystack, args, command);

        //arrF[0] = args[0];
        //arrF[1] = args[1];
        //arrF[2] = args[2];
        //((void (*)(float *))(g_libGTASA + 0x197E0C + 1))(arrF);

        //*(float *)(g_libGTASA + 0x612808 + 0) = args[0];
        //*(float *)(g_libGTASA + 0x612808 + 4) = args[1];
        //*(float *)(g_libGTASA + 0x612808 + 8) = args[2];

        MATRIX4X4 mat;

        pGame->FindPlayerPed()->TeleportTo(args[0], args[1], args[2]);
        //ScriptCommand(&freeze_player, //pPlayerPed->m_dwGTAId, 0);
        //pPlayerPed->GetMatrix(&mat);
        //ScriptCommand(&restore_camera_to_user);
        ////pPlayerPed->m_pEntity = nullptr;
        //ScriptCommand(&set_camera_position, 431.8335, 250.1207, 11.9394, mat.at.X, mat.at.Y, mat.at.Z);
        //ScriptCommand(&set_camera, args[0],
        //              args[1],
        //              args[2]);

        //*(uint8_t*)(g_libGTASA + 0x008E864C) = 5;

        //((void (*)(uintptr_t*, uintptr_t, uintptr_t, uintptr_t))(g_libBR + 0x043010))(*(uintptr_t* *)(g_libBR + 0x207F24), 1000.0, 500.0, 50.0);
        //(( void (*)(uintptr_t *, float, float, float, bool))(*(void**)(*(uintptr_t *)((*(uintptr_t* *)((*(uintptr_t* *)(g_libBR + 0x207F24)) + 4)) + 60))))(*(uintptr_t* *)((*(uintptr_t* *)(g_libBR + 0x207F24)) + 8), 1000.0, 500.0, 50.0, 0);
        if (m_pEntity && m_pEntity->vtable != (g_libGTASA + 0x5C7358)){
            ((void (*)(ENTITY_TYPE *, float, float, float, bool)) (*(void **) (m_pEntity->vtable +
                                                                               0x3C)))(m_pEntity,
                                                                                       args[0],
                                                                                       args[1],
                                                                                       args[2],
                                                                                       0);
        }

        //GamePool_Ped_GetAt()
    }

    if(strstr(haystack, OBFUSCATE("/smg"))) {
        if(*(int*)(g_libGTASA + 0x63E384) < 2){ *(int*)(g_libGTASA + 0x63E384) = 2; } // effects
        if(*(int*)(g_libGTASA + 0x63E3A4) != 100){ *(int*)(g_libGTASA + 0x63E3A4) = 100; } // resolution
        if(*(int*)(g_libGTASA + 0x63E3C4) != 100){ *(int*)(g_libGTASA + 0x63E3C4) = 100; } // draw distance
        if(*(int*)(g_libGTASA + 0x63E424) != 2){ *(int*)(g_libGTASA + 0x63E424) = 2; } // shadows
        if(*(int*)(g_libGTASA + 0x63E464) < 2){ *(int*)(g_libGTASA + 0x63E464) = 2; } // reflections
    }
    if(strstr(haystack, OBFUSCATE("/settime"))) {
        std::vector<std::string> words;
        std::vector<std::string> digits;
        parseString(haystack, words, digits);
        pGame->SetWorldTime(*(uintptr_t *)(digits[0].c_str()), 00);
    }
    if(strstr(haystack, OBFUSCATE("/sets"))) {
        std::vector<std::string> words;
        std::vector<std::string> digits;
        parseString(haystack, words, digits);
        pSettings->WriteG(*(uintptr_t *)(digits[0].c_str()), *(uintptr_t *)(digits[1].c_str()));
        pChatWindow->AddDebugMessageNonFormatted("restart game for apply changes");
        //((void (*)(char*))(g_libBR + 0x02E074))(nullptr);
    }
    if(strstr(haystack, OBFUSCATE("/veh"))) {
            //uint8_t i = FindFirstFreePlayerPedSlot()-1;
            //if(i > 1){
            //    ScriptCommand(&toggle_player_controllable, i, 1);
            //    ScriptCommand(&lock_actor, m_dwGTAId, 0);
            //}

            //CPlayerPed *pPlayer = pPlayerPed;
            //CCamera *pCamera = pGame->GetCamera();
            //CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

            //pCamera->Restore();
            //pCamera->SetBehindPlayer();

            //if(pPlayer->IsInVehicle())
            //    pPlayer->RemoveFromVehicleAndPutAt(431.8335, 250.1207, 11.9394);
            //else
            //    pPlayer->TeleportTo(431.8335, 250.1207, 11.9394);

            //pPlayer->SetInitialState();
            //pPlayer->ClearAllWeapons();
            ////pGame->DisableTrainTraffic();
            //
            //pPlayer->TogglePlayerControllable(true);
            //pPlayer->SetModelIndex(122);

            //pGame->DisplayWidgets(true);
            //pGame->ToggleRadar(false);

            std::vector<float> args;
            char command[256];
            extractValuesAndCommand(haystack, args, command);
            CVehicle* pVehicle = new CVehicle(args[3], args[0], args[1], args[2], 0, false);
            //pLocalPlayer->m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, false);
            //pVehicle->Add();
    }

    //if(strstr(haystack, OBFUSCATE("/hook"))){
    //    std::vector<std::string> words;
    //    std::vector<std::string> digits;
//
    //    parseString(haystack, words, digits);
    //    if(!words.empty()){
    //        uintptr_t lib;
    //        if(words[0] == "libGTA"){
    //            lib = g_libGTASA;
    //        }else if(words[0] == "libBR"){
    //            lib = g_libBR;
    //        }
    //        CHook::InlineHook(lib, words[1], words[2], words[3]);
    //    }
    //}

    return CChatwindow__ProcessCommands(haystack);
}

//((void (*)(uintptr_t, float, float , float ))(g_libBR + 0x043010))(*(uintptr_t *)(g_libBR + 0x207F24), a1, a2, a3);

void (*InitInGame)();
void InitInGame_hook(){
    if(!bInitedInGame){
        InitInMenuBR();
    }
    //static bool onceO = false;
    //if(!onceO){
    //    pLocalPlayer = new CLocalPlayer();
    //    PLAYER_SPAWN_INFO spawnInfo;
    //    VECTOR pos;
    //    int emptyArr[3] = {0, 0, 0};
    //    pos.X = 431.8335;
    //    pos.Y = 250.1207;
    //    pos.Z = 11.9394;
    //    spawnInfo.iSkin = 255;
    //    spawnInfo.unk = 0;
    //    spawnInfo.vecPos = pos;
    //    spawnInfo.fRotation = 0.0f;
    //    spawnInfo.byteTeam = 0;
    //    spawnInfo.iSpawnWeapons[0] = emptyArr[0];
    //    spawnInfo.iSpawnWeapons[1] = emptyArr[1];
    //    spawnInfo.iSpawnWeapons[2] = emptyArr[2];
    //    spawnInfo.iSpawnWeaponsAmmo[0] = emptyArr[0];
    //    spawnInfo.iSpawnWeaponsAmmo[1] = emptyArr[1];
    //    spawnInfo.iSpawnWeaponsAmmo[2] = emptyArr[2];
    //    //*(uint8_t*)(g_libGTASA + 0x8E864C) = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //    //pLocalPlayer->m_pPlayerPed->SetGravityProcessing(true);
    //    //*(uintptr_t *)(pLocalPlayer + 47) = *(uintptr_t *)pPlayerPed;
    //    //((void (*)(uintptr_t, PLAYER_SPAWN_INFO *))(g_libBR + 0x07751C))(pLocalPlayer, &spawnInfo);
    //    pLocalPlayer->SetSpawnInfo(&spawnInfo);
    //    pLocalPlayer->ToggleSpectating(0);
    //    //((void(*)(uintptr_t, uint8_t))(g_libBR + 0x074900))(pLocalPlayer, 0);
    //    //((void (*)(uintptr_t))(g_libBR + 0x076874))(pLocalPlayer);
    //    //pLocalPlayer->Spawn();
    //    *(uint8_t*)(g_libGTASA + 0x8E864C) = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //    //(*(uint8_t**)(g_libGTASA + 0x5D021C))[7] = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //    //(*(uint8_t**)(g_libGTASA + 0x8E8650))[101 * 1] = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //    pLocalPlayer->m_pPlayerPed->SetInitialState();
    //    pLocalPlayer->m_pPlayerPed->ClearAllWeapons();
    //    pLocalPlayer->m_pPlayerPed->SetModelIndex(122);
    //    //((void(*)(uintptr_t, uint8_t))(g_libBR + 0x053258))(*(uintptr_t *)(pLocalPlayer + 47), true);
    //    pLocalPlayer->m_pPlayerPed->TogglePlayerControllable(true);
    //    //ScriptCommand(&lock_camera_position, 0);
    //    //ScriptCommand(&restore_camera_to_user);
    //    //ScriptCommand(&set_camera_behind_player);
    //    //ScriptCommand(&restore_camera_jumpcut);
    //    //pLocalPlayer->m_pPlayerPed->SetCameraMode(1);
    //    pLocalPlayer->m_pPlayerPed->AddPhysical();
    //    //pLocalPlayer->m_bIsSpectating = false;
    //    ScriptCommand(&restore_camera_jumpcut);
    //    //((void(*)(uintptr_t*))(g_libBR + 0x04103C))(*(uintptr_t **)(g_libBR + 0x207F24 + 5));
    //    /*else{
    //            pCamera = pGame->GetCamera();
    //            pCamera->Restore();
    //            pCamera->SetBehindPlayer();
    //        }*/
//
    //    //pCamera->Restore(); // убираем камеру после инициализации
    //    //pCamera->SetBehindPlayer();
    //    //CVehicle* pVehicle = pGame->NewVehicle(411, 433.8335, 250.1207, 11.9394, 0, false);
    //    //pVehicle->Add();
    //    //pVehicle->SetGravityProcessing(true);
    //    //pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, false);
    //    //(*(uintptr_t **)(g_libBR + 0x61DF38))[0] = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //    pLocalPlayer->m_bIsActive = true;
    //    //CHook::UnFuck(g_libBR + 0x67F718);
    //    //*(uint8_t *)(g_libGTASA + 0x5D021C) = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
    //    onceO = true;
    //}
    bInitedInGame = true;
    if(pNetGame) { pNetGame->Process(); }
    if(pGUI) { pGUI->Render(); }
    /* return */InitInGame();
    //*(uintptr_t *)(g_libGTASA + 0x8B83AC) = 1;
    //((void(*)(bool))(g_libGTASA + 0x26EF20 + 1))(true);
    /*static bool once0 = false;
    if(!once0){
        Log("removing standard player");
        //((void(*)(uintptr_t))(g_libGTASA + 0x454334 + 1))(0);
        Log("standard player removed");
        pLocalPlayer = new CLocalPlayer();
        PLAYER_SPAWN_INFO spawnInfo;
        VECTOR pos;
        int emptyArr[3] = {0, 0, 0};
        pos.X = 431.8335;
        pos.Y = 250.1207;
        pos.Z = 11.9394;
        spawnInfo.iSkin = 255;
        spawnInfo.unk = 0;
        spawnInfo.vecPos = pos;
        spawnInfo.fRotation = 0.0f;
        spawnInfo.byteTeam = 0;
        spawnInfo.iSpawnWeapons[0] = emptyArr[0];
        spawnInfo.iSpawnWeapons[1] = emptyArr[1];
        spawnInfo.iSpawnWeapons[2] = emptyArr[2];
        spawnInfo.iSpawnWeaponsAmmo[0] = emptyArr[0];
        spawnInfo.iSpawnWeaponsAmmo[1] = emptyArr[1];
        spawnInfo.iSpawnWeaponsAmmo[2] = emptyArr[2];
//*(uint8_t*)(g_libGTASA + 0x8E864C) = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
//pLocalPlayer->m_pPlayerPed->SetGravityProcessing(true);
//*(uintptr_t *)(pLocalPlayer + 47) = *(uintptr_t *)pPlayerPed;
//((void (*)(uintptr_t, PLAYER_SPAWN_INFO *))(g_libBR + 0x07751C))(pLocalPlayer, &spawnInfo);
        pLocalPlayer->SetSpawnInfo(&spawnInfo);
        pLocalPlayer->ToggleSpectating(0);
//((void(*)(uintptr_t, uint8_t))(g_libBR + 0x074900))(pLocalPlayer, 0);
//((void (*)(uintptr_t))(g_libBR + 0x076874))(pLocalPlayer);
//pLocalPlayer->Spawn();
        //                       0x5D1B24
        *(uint8_t*)(g_libGTASA + 0x8E864C) = pLocalPlayer->GetPlayerPed()->m_bytePlayerNumber;
        //*(uintptr_t*)((*(uintptr_t* *)(g_libGTASA + 0x8E8650))[101 * 7]) = pLocalPlayer->GetPlayerPed()->m_bytePlayerNumber;
        //*(ENTITY_TYPE* *)((*(uintptr_t* *)((*(uintptr_t **)(g_libGTASA + 0x5D021C))[101 * 2])) + 1420) = pLocalPlayer->GetPlayerPed()->m_pEntity;
//(*(uint8_t**)(g_libGTASA + 0x5D021C))[7] = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
//(*(uint8_t**)(g_libGTASA + 0x8E8650))[101 * 1] = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
        pLocalPlayer->GetPlayerPed()->SetInitialState();
        pLocalPlayer->GetPlayerPed()->ClearAllWeapons();
        pLocalPlayer->GetPlayerPed()->SetModelIndex(122);
//((void(*)(uintptr_t, uint8_t))(g_libBR + 0x053258))(*(uintptr_t *)(pLocalPlayer + 47), true);
        pLocalPlayer->GetPlayerPed()->TogglePlayerControllable(true);
//ScriptCommand(&lock_camera_position, 0);
//ScriptCommand(&restore_camera_to_user);
//ScriptCommand(&set_camera_behind_player);
//ScriptCommand(&restore_camera_jumpcut);
//pLocalPlayer->m_pPlayerPed->SetCameraMode(1);
        pLocalPlayer->GetPlayerPed()->AddPhysical();
//pLocalPlayer->m_bIsSpectating = false;
            //ScriptCommand(&restore_camera_jumpcut);
//((void(*)(uintptr_t*))(g_libBR + 0x04103C))(*(uintptr_t **)(g_libBR + 0x207F24 + 5));
//pCamera->Restore(); // убираем камеру после инициализации
//pCamera->SetBehindPlayer();
//CVehicle* pVehicle = pGame->NewVehicle(411, 433.8335, 250.1207, 11.9394, 0, false);
//pVehicle->Add();
//pVehicle->SetGravityProcessing(true);
//pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, false);
//(*(uintptr_t **)(g_libBR + 0x61DF38))[0] = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
        pLocalPlayer->m_bIsActive = true;
//CHook::UnFuck(g_libBR + 0x67F718);
//*(uint8_t *)(g_libGTASA + 0x5D021C) = pLocalPlayer->m_pPlayerPed->m_bytePlayerNumber;
            //*(CNetGame* *)(g_libBR + 0x207F28) = *(CNetGame* *)pNetGame;
        once0 = true;
    }*/

    //if(pLocalPlayer){
    //    if(pLocalPlayer->GetPlayerPed()){
    //        MATRIX4X4 pPlayerMat;
    //        pLocalPlayer->GetPlayerPed()->GetMatrix(&pPlayerMat);
    //        uintptr_t *v3;
    //        uintptr_t v4;
    //        uintptr_t v5;
    //        uintptr_t *v6;
    //        uintptr_t v10;
    //        v3 = *(uintptr_t**)((*(uintptr_t* *)(g_libGTASA + 0x8E8650))[101 * 1]);
    //        if ( v3 )
    //        {
    //            if ( (*(char *)(v3 + 1153) & 1) != 0 && (v4 = *(uintptr_t *)(v3 + 1420)) != 0 )
    //            {
    //                v5 = *(uintptr_t *)(v4 + 20);
    //                if ( v5 )
    //                    v6 = *(uintptr_t* *)(v5 + 48);
    //                else
    //                    v6 = *(uintptr_t* *)(v4 + 4);
    //            }
    //            else
    //            {
    //                v10 = *(uintptr_t *)(v3 + 20);
    //                v6 = *(uintptr_t* *)(v3 + 4);
    //                if ( v10 )
    //                    v6 = *(uintptr_t* *)(v10 + 48);
    //            }
    //            if(v6){
    //                *(float *)(v6 + 0) = pPlayerMat.pos.X;
    //                *(float *)(v6 + 1) = pPlayerMat.pos.Y;
    //                *(float *)(v6 + 2) = pPlayerMat.pos.Z;
    //            }
    //        }
    //        //*(uintptr_t *)(*(uintptr_t* *)(((*(uintptr_t* *)((*(uintptr_t* *)((*(uintptr_t**)((*(uintptr_t* *)(g_libGTASA + 0x8E8650))[101 * 2]))) + 1420) + 20)) + 48)[0])) = pPlayerMat.pos.X;
    //        //*(uintptr_t *)(*(uintptr_t* *)(((*(uintptr_t* *)((*(uintptr_t* *)((*(uintptr_t**)((*(uintptr_t* *)(g_libGTASA + 0x8E8650))[101 * 2]))) + 1420) + 20)) + 48)[1])) = pPlayerMat.pos.Y;
    //        //*(uintptr_t *)(*(uintptr_t* *)(((*(uintptr_t* *)((*(uintptr_t* *)((*(uintptr_t**)((*(uintptr_t* *)(g_libGTASA + 0x8E8650))[101 * 2]))) + 1420) + 20)) + 48)[2])) = pPlayerMat.pos.Z;
    //    }
    //}
}

void (*CNetGame_process)(uintptr_t *);
void CNetGame_process_hook(uintptr_t * a1){
    //*(uintptr_t*)(g_libBR + 0x7BC620) = 0;
    //*(uintptr_t *)((*(uintptr_t**)(g_libBR + 0x207F28))  + 36) = 203;
    //static bool once = false;
    //if(!once){
    //    (*(void (__fastcall **)(uintptr_t, const char *, uintptr_t, uintptr_t, uintptr_t, int))(*(uintptr_t *)*a1 + 8))(
    //            *a1,
    //            *(const char **)((*(uintptr_t *)(g_libBR + 0x207F28)) + 703),
    //            *(uintptr_t *)((*(uintptr_t *)(g_libBR + 0x207F28)) + 830),
    //            0,
    //            0,
    //            5);
    //    a1[11] = GetTickCount();
    //    a1[10] = 13;
    //    once = true;
    //    return;
    //}
    CNetGame_process(a1);
    //ScriptCommand(&lock_camera_position, 0);
    ////ScriptCommand(&restore_camera_to_user);
    //ScriptCommand(&restore_camera_to_user);
    //ScriptCommand(&set_camera_behind_player);
    //ScriptCommand(&restore_camera_jumpcut);
    //ScriptCommand(&restore_camera);

    //MATRIX4X4 matFromPlayer;
    //pPlayerPed->GetMatrix(&matFromPlayer);
    //ScriptCommand(&set_camera, matFromPlayer.pos.X, matFromPlayer.pos.Y, matFromPlayer.pos.Z);
}

void (*CNetGame_updateNetwork)(uintptr_t *);
void CNetGame_updateNetwork_hook(uintptr_t * a1){
    //*(uintptr_t*)(g_libBR + 0x7BC620) = 0;
    //*(uintptr_t *)((*(uintptr_t**)(g_libBR + 0x207F28))  + 36) = 203;
    CNetGame_updateNetwork(a1);

}

void (*sub_410F4)(uintptr_t *a1, float a2);
void sub_410F4_hook(uintptr_t *a1, float a2){
    return;
}
void ApplyPatches_level0();
void (*sub_511F0)();
void sub_511F0_hook(){
    ApplyPatches_level0();
    return;
}

void (*sub_45AC0)(int a1, int a2, int a3, int a4);
void sub_45AC0_hook(int a1, int a2, int a3, int a4){
    ((void(*)(int, int, int, int))(g_libBR + 0x61E158))(a1, a2, a3, a4);
    sub_45AC0(a1, a2, a3, a4);
    return;
}

void (*sub_45B38)();
void sub_45B38_hook(){
    ((void(*)(int))(g_libGTASA + 0x239088 + 1))(1);
    return;
}

uintptr_t (*sub_96AB0)(uintptr_t);
uintptr_t sub_96AB0_hook(uintptr_t a1){
    //((void(*)(int))(g_libGTASA + 0x239088 + 1))(1);
    return a1;
}

void (*CWidgetList__CWidgetList)(uintptr_t *a1, int a2, int a3, const char **a4, int a5, uint8_t a6, char a7,char a8);
void CWidgetList__CWidgetList_hook(uintptr_t *a1, int a2, int a3, const char **a4, int a5, uint8_t a6, char a7,char a8){
    g_CWL = a1;
    return CWidgetList__CWidgetList(a1, a2, a3, a4, a5, a6, a7, a8);
}

void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY){
    pGUI->OnTouchEvent(type, num, posX, posY);
    return TouchEvent(type, num, posX, posY);
}

uintptr_t (*sub_6FF54)(uintptr_t thiz);
uintptr_t sub_6FF54_hook(uintptr_t thiz){
    LOGFN("%s", g_6FF54_name.c_str());
    return sub_6FF54(thiz);
}

void InitHookStuff();
void InitScripting();
void InstallSpecialHooks();
void InitRenderWareFunctions();
void InitThread(void* a1);
JavaVM* javaVM;
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    g_libBR = FindLibrary(OBFUSCATE("libsamp.so"));
    g_libGTASA = FindLibrary(OBFUSCATE("libGTASA.so"));
    g_pszStorage = OBFUSCATE("/storage/emulated/0/Android/data/blackrussia.online/files/");// /storage/emulated/0/Android/data/blackrussia.online/files/
    //pthread_t* newthread;
    //sleep(2);
    //pthread_create(newthread, 0, (void *(*)(void *))InitThread, 0);
    //InlineHook(g_libGTASA, 0x39AEF4, (uintptr_t)Render2dStuff_hook, (uintptr_t*)&Render2dStuff);
    //InlineHook(g_libBR, 0x02A4B4, (uintptr_t)sub_hook, (uintptr_t*)&sub);
    //InitScripting();
    //CHook::CallFunction<void>(g_libBR + 0x055814, &toggle_player_controllable, 2, 0);
    //ScriptCommand(&toggle_player_controllable, 2, 0);
    //NOP(g_libBR + 0x02A54C, 2);
    //NOP(g_libBR + 0x08DDE6, 4);
    InitHookStuff();
    InitRenderWareFunctions();
    //CHook::NOP(g_libBR + 0x0487E6, 4);
    //CHook::NOP(g_libBR + 0x02B444, 4);
    //CHook::NOP(g_libBR + 0x02B456, 4);
    //CHook::NOP(g_libBR + 0x078D02, 4);
    InstallSpecialHooks();
    //CHook::NOP(g_libBR + 0x07825C, 4);
    //CHook::NOP(g_libBR + 0x078444, 4);
    //CHook::NOP(g_libBR + 0x02A148, 2);
    //CHook::NOP(g_libBR + 0x02A14A, 2);
    //CHook::InlineHook(g_libBR, 0x02A4B4, (uintptr_t)sub_hook, (uintptr_t*)&sub);
    CHook::InlineHook(g_libBR, 0x01CABC, (uintptr_t)CChatwindow__ProcessCommands_hook, (uintptr_t*)&CChatwindow__ProcessCommands);
    //CHook::InlineHook(g_libBR, 0x078124, (uintptr_t)connect2_hook, (uintptr_t*)&connect2);
    //CHook::InlineHook(g_libBR, 0x077E74, (uintptr_t)CNetGame_hook, (uintptr_t*)&CNetGame);
    //CHook::NOP(g_libBR + 0x077FE4, 4);
    //CHook::NOP(g_libBR + 0x077FEC, 4);
    //CHook::NOP(g_libBR + 0x02A140, 4);
    //CHook::NOP(g_libBR + 0x02A148, 2);
    //CHook::NOP(g_libBR + 0x02A14C, 4);
    //CHook::NOP(g_libBR + 0x02A14C, 4);
    //
    //CHook::NOP(g_libBR + 0x02B41C, 4);
    //CHook::NOP(g_libBR + 0x043D02, 4);
    //CHook::NOP(g_libBR + 0x0782C6, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x07841E, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x078444, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x07825C, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x07827E, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x078466, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x0783E0, 4); // CNetGame::Process
    //CHook::UnFuck(g_libBR + 0x0783E0); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x0783E0, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x07841E, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x078444, 4); // CNetGame::Process
    //CHook::NOP(g_libBR + 0x078466, 4); // CNetGame::Process
    //
    //CHook::NOP(g_libBR + 0x04CFEC, 4); //4CFEC installhooks
    //CHook::NOP(g_libBR + 0x048660, 4); //48660 installspecialhooks
    //CHook::NOP(g_libBR + 0x02A16C, 4);
    //CHook::NOP(g_libBR + 0x08DDE6, 4);
    //CHook::NOP(g_libBR + 0x0297E2, 4); // nop settings init
    //CHook::NOP(g_libBR + 0x02A3AE, 4); // gold lines
    //CHook::NOP(g_libBR + 0x04D78A, 4); // gold lines
    //CHook::NOP(g_libBR + 0x04D7A6, 4); // gold lines
    //CHook::NOP(g_libBR + 0x02A332, 4);
    //CHook::NOP(g_libBR + 0x02A16C, 4);
    CHook::NOP(g_libBR + 0x04D9C4, 4);
    CHook::NOP(g_libBR + 0x04D9D6, 4);
    //
    //CHook::NOP(g_libBR + 0x04A8EE, 4);
    //CHook::NOP(g_libBR + 0x0513A2, 2);
    //CHook::NOP(g_libBR + 0x02A174, 2);
    CHook::InlineHook(g_libBR, 0x029B10, (uintptr_t)nullsub_2_hook, (uintptr_t*)&nullsub_2);
    //CHook::InlineHook(g_libBR, 0x078868, (uintptr_t)CNetGame_updateNetwork_hook, (uintptr_t*)&CNetGame_updateNetwork);
    //CHook::InlineHook(g_libBR, 0x0410F4, (uintptr_t)sub_410F4_hook, (uintptr_t*)&sub_410F4);
    //CHook::InlineHook(g_libBR, 0x0511F0, (uintptr_t)sub_511F0_hook, (uintptr_t*)&sub_511F0);
    //CHook::InlineHook(g_libGTASA, 0x279E2C, (uintptr_t)CWidgetList__CWidgetList_hook, (uintptr_t*)&CWidgetList__CWidgetList);
    //CHook::InlineHook(g_libBR, 0x045AC0, (uintptr_t)sub_45AC0_hook, (uintptr_t*)&sub_45AC0);
    //CHook::InlineHook(g_libBR, 0x096AB0, (uintptr_t)sub_96AB0_hook, (uintptr_t*)&sub_96AB0);
    //CHook::InlineHook(g_libBR, 0x045B38, (uintptr_t)sub_45B38_hook, (uintptr_t*)&sub_45B38);
    //CHook::InlineHook(g_libBR, 0x078124, (uintptr_t)CNetGame_process_hook, (uintptr_t*)&CNetGame_process);
    CHook::InlineHook(g_libBR, 0x02A2F8, (uintptr_t)InitInGame_hook, (uintptr_t*)&InitInGame);
    CHook::InlineHook(g_libBR, 0x045AC0, (uintptr_t)TouchEvent_hook, (uintptr_t*)&TouchEvent);
    //CHook::InlineHook(g_libBR, 0x02A134, (uintptr_t)InitInMenuBR_hook, (uintptr_t*)&InitInMenuBR);
    //CHook::InlineHook(g_libBR, 0x043D38, (uintptr_t)CGame_InitInGame_hook, (uintptr_t*)&CGame_InitInGame);
    //CHook::InlineHook(g_libBR, 0x074900, (uintptr_t)ToggleSpectating_hook, (uintptr_t*)&ToggleSpectating);
    CHook::InlineHook(g_libBR, 0x045454, (uintptr_t)Render2dStuff_hook, (uintptr_t*)&Render2dStuff);
    CHook::InlineHook(g_libBR, 0x06FCC8, (uintptr_t)compileShadersBR_hook, (uintptr_t*)&compileShadersBR);
    CHook::InlineHook(g_libGTASA, 0x1A3794, (uintptr_t)ES2Shader__Build_hook, (uintptr_t*)&ES2Shader__Build);


    //g_6FF54_name = getFunctionNameFromAddress((void*)(0x06FF54), OBFUSCATE("/data/data/blackrussia.online/lib/libsamp.so"));
    CHook::InlineHook(g_libBR, 0x06FF54, (uintptr_t)sub_6FF54_hook, (uintptr_t*)&sub_6FF54);



    //CHook::InlineHook(g_libBR, 0x179F98, (uintptr_t)glShader_hook, (uintptr_t*)&glShader);
    //*(uintptr_t *)(g_libBR + 0x7BEE71) = 1;
    //UnFuck(g_libBR + 0x02A4DE);
    //UnFuck(g_libBR + 0x02A54C);
    //UnFuck(g_libBR + 0x02A4F0);
    //NOP(g_libBR + 0x02A4F0, 4);
    //NOP(g_libBR + 0x02A4DE, 2);
    //NOP(g_libBR + 0x02A4E2, 2);
    //CHook::InlineHook(g_libGTASA , 0x179F98, (uintptr_t)glShader_hook, (uintptr_t*)&glShader);
    return JNI_VERSION_1_4;
}

void InitThread(void* a1){

}

uint32_t GetTickCount()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (tv.tv_sec*1000+tv.tv_usec/1000);
}
