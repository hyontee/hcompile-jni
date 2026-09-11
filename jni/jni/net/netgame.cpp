#include "../main.h"
#include "../game/game.h"
#include "../game/crosshair.h"
#include "../game/CRadarRect.h"
#include "netgame.h"
#include "CUDPSocket.h"
#include "vendor/raknet/RakClient.h"
#include "../util/CJavaWrapper.h"

extern CWidgetManager* g_pWidgetManager;
extern CCrossHair *pCrossHair;

#include "../chatwindow.h"

#include "..//CClientInfo.h"
#include "..//CLocalisation.h"

#define NETGAME_VERSION 4057
#define AUTH_BS OBFUSCATE("10B3D2B1317ADD02CC1F680BC500A8BC0FD7AD42CE7")
//#define AUTH_BS "E02262CF28BC542486C558D4BE9EFB716592AFAF8B"

extern CGame *pGame;
extern CChatWindow *pChatWindow;

int iVehiclePoolProcessFlag = 0;
int iPickupPoolProcessFlag = 0;

void RegisterRPCs(RakClientInterface* pRakClient);
void UnRegisterRPCs(RakClientInterface* pRakClient);
void RegisterScriptRPCs(RakClientInterface* pRakClient);
void UnRegisterScriptRPCs(RakClientInterface* pRakClient);

CUDPSocket sock;

unsigned char GetPacketID(Packet *p)
{
    if(p == 0) return 255;

    if ((unsigned char)p->data[0] == ID_TIMESTAMP)
        return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
    else
        return (unsigned char) p->data[0];
}

CNetGame::CNetGame(const char* szHostOrIp, int iPort, const char* szPlayerName, const char* szPass)
{
    strcpy(m_szHostName, OBFUSCATE("San Andreas Multiplayer"));
    strncpy(m_szHostOrIp, szHostOrIp, sizeof(m_szHostOrIp));
    m_iPort = iPort;

    CPlayerPool::Init();
    CPlayerPool::SetLocalPlayerName(szPlayerName);

	m_pVehiclePool = new CVehiclePool();
    m_pTextDrawPool = new CTextDrawPool();
    g_pWidgetManager = new CWidgetManager();
    m_pStreamPool = new CStreamPool();
    m_pNotificationPool = new NotificationPool();
    m_pGangZonePool = new CGangZonePool();

    m_pRakClient = RakNetworkFactory::GetRakClientInterface();
    RegisterRPCs(m_pRakClient);
    RegisterScriptRPCs(m_pRakClient);
    m_pRakClient->SetPassword(szPass);

    m_dwLastConnectAttempt = GetTickCount();
    m_iGameState = 	GAMESTATE_WAIT_CONNECT;

    m_iSpawnsAvailable = 0;
    m_bHoldTime = true;
    m_byteWorldMinute = 0;
    m_byteWorldTime = 12;
    m_byteWeather =	10;
    m_fGravity = (float)0.008000000;
    m_bUseCJWalk = false;
    m_bDisableEnterExits = false;
    m_fNameTagDrawDistance = 60.0f;
    m_bZoneNames = false;
    m_bInstagib = false;
    m_iDeathDropMoney = 0;
    m_bNameTagLOS = false;

    for(int i=0; i<100; i++)
        m_dwMapIcons[i] = 0;

    pGame->EnableClock(false);
    pGame->EnableZoneNames(false);

    //if(pChatWindow)
    //pChatWindow->AddDebugMessage(OBFUSCATE("{FFFFFF}SA-MP {B9C9BF}" SAMP_VERSION " {FFFFFF}Started"));

    if(pChatWindow)
        pChatWindow->AddDebugMessage("������ �������!");
}
#include "..//voice/CVoiceChatClient.h"
#include "util/armhook.h"

extern CVoiceChatClient* pVoice;
CNetGame::~CNetGame()
{
    m_pRakClient->Disconnect(0);

    UnRegisterRPCs(m_pRakClient);
    UnRegisterScriptRPCs(m_pRakClient);

    RakNetworkFactory::DestroyRakClientInterface(m_pRakClient);

    if (m_pTextDrawPool)
    {
        delete m_pTextDrawPool;
        m_pTextDrawPool = nullptr;
    }

    if (g_pWidgetManager)
    {
        delete g_pWidgetManager;
        g_pWidgetManager = nullptr;
    }

    if (m_pStreamPool)
    {
        delete m_pStreamPool;
        m_pStreamPool = nullptr;
    }

    SAFE_DELETE(m_pNotificationPool);
    SAFE_DELETE(m_pGangZonePool);

}
int tickUpdate;
void UpdateHud()
{
    if(g_pJavaWrapper && pGame->FindPlayerPed())
    {
        tickUpdate++;
        if(tickUpdate < 5) return;

        tickUpdate = 0;

        g_pJavaWrapper->SetHP((int)pGame->FindPlayerPed()->GetHealth());
        g_pJavaWrapper->SetArmour((int)pGame->FindPlayerPed()->GetArmour());
        g_pJavaWrapper->SetMoney(pGame->GetLocalMoney());
        PLAYERID playercount = CPlayerPool::list.size() + 1;
        g_pJavaWrapper->SetOnline((int)playercount);

        int patrons = (int)pGame->FindPlayerPed()->GetCurrentWeaponSlot()->dwAmmo;
        int patronsInClip = (int)pGame->FindPlayerPed()->GetCurrentWeaponSlot()->dwAmmoInClip;

        g_pJavaWrapper->SetAmmo(patronsInClip, patrons - patronsInClip);

        g_pJavaWrapper->SetHudServerInfo(CPlayerPool::GetLocalPlayerID(), CPlayerPool::GetLocalPlayerName());

        g_pJavaWrapper->UpdateTime();

        g_pJavaWrapper->SetHudOnline(CPlayerPool::list.size() + 1);

        g_pJavaWrapper->UpdateHudIcon((int) pGame->FindPlayerPed()->GetCurrentWeaponSlot()->dwType);
    }
}

bool g_IsVoiceServer();
static bool once = false;
int last_process_cnetgame = 0;
void CNetGame::Process()
{
    auto curTick = GetTickCount();
    if (curTick - last_process_cnetgame >= 33) {
        last_process_cnetgame = curTick;
    } else {
        return;
    }

    static auto nextClearTime = curTick + 40000;
    if(curTick > nextClearTime) {
        CallFunction<void>(g_libGTASA + 0x293325);// CStreaming::RemoveAllUnusedModels
        nextClearTime = curTick + 40000;
    }

    CVehiclePool::UpdateSpeed();
    UpdateHud();

    UpdateNetwork();

    // server checkpoints update
    if (CPlayerPool::GetLocalPlayer())
    {
        if (CPlayerPool::GetLocalPlayer()->m_bIsActive && CPlayerPool::GetLocalPlayer()->GetPlayerPed())
            pGame->UpdateCheckpoints();
    }

    if(m_bHoldTime)
        pGame->SetWorldTime(m_byteWorldTime, m_byteWorldMinute);

    if(GetGameState() == GAMESTATE_CONNECTED)
    {
        // pool process
        CPlayerPool::Process();
        CObjectPool::Process();
        CVehiclePool::Process();
        CPickupPool::Process();

        if(m_pNotificationPool) {
            m_pNotificationPool->Process();

            // if(m_pVoiceNotificationPool) {
            // m_pVoiceNotificationPool->Process();
            // }
        }
    }
    else
    {
        CPlayerPed *pPlayer = pGame->FindPlayerPed();
        CCamera *pCamera = pGame->GetCamera();

        if(pPlayer && pCamera)
        {
            if(pPlayer->IsInVehicle())
                pPlayer->RemoveFromVehicleAndPutAt(1093.4f, -2036.5f, 82.7106f);
            else pPlayer->TeleportTo(1093.4f, -2036.5f, 82.7106f);

            pCamera->SetPosition(1093.0f, -2036.0f, 90.0f, 0.0f, 0.0f, 0.0f);
            pCamera->LookAtPoint(384.0f, -1557.0f, 20.0f, 2);

            pGame->SetWorldWeather(m_byteWeather);
            pGame->DisplayWidgets(false);
        }
    }

    if(GetGameState() == GAMESTATE_WAIT_CONNECT && (GetTickCount() - m_dwLastConnectAttempt) > 3000)
    {
        /*if (CClientInfo::bSAMPModified)
        {
            if (pChatWindow) pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::MODIFIED_FILES));
            SetGameState(GAMESTATE_CONNECTING);
            m_dwLastConnectAttempt = GetTickCount();
            return;
        }

        if (!CClientInfo::bJoinedFromLauncher)
        {
            if (pChatWindow) pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMsg(E_MSG::NOT_FROM_LAUNCHER));
            SetGameState(GAMESTATE_CONNECTING);
            m_dwLastConnectAttempt = GetTickCount();
            return;
        }*/

        if(pChatWindow)
        {
            pChatWindow->AddDebugMessageNonFormatted("����������� � �������...");
        }

        if(g_pJavaWrapper)
        {
            g_pJavaWrapper->SetLoadingText(1);
        }

        for (int i = 0; i < 100; i++)
        {
            const char* ip = m_pRakClient->GetPlayerID().ToString();
            if (sock.Bind(5000 + i * 10 + (rand() % 100)))
            {
                CRawData data(250);
                while (data.GetWriteOffset() < 170)
                {
                    data.Write("SAMP", 4);
                    data.Write("00000000", 8);
                    data.Write(ip, strlen(ip));
                    data.Write("00000000", 8);
                    data.Write(m_szHostOrIp, strlen(m_szHostOrIp));
                    data.Write("00112233445566778899FFFFFFFFFFFFFFFF", 36);
                }
                std::string ip = m_szHostOrIp;
                std::stringstream s(ip);
                int a, b, c, d;
                char ch;
                s >> a >> ch >> b >> ch >> c >> ch >> d;
                CAddress dest(a, b, c, d);
                dest.usPort = m_iPort;

                for (int j = 0; j < 3; j++)
                {
                    //fix connect
                    //if(pChatWindow) pChatWindow->AddDebugMessageNonFormatted("????????? ???????????...");
                    sock.Send(dest, data);
                    usleep(10000);
                }

                break;
            }
        }
        usleep(1000000);

        m_pRakClient->Connect(m_szHostOrIp, m_iPort, 0, 0, 15);

        m_dwLastConnectAttempt = GetTickCount();

        SetGameState(GAMESTATE_CONNECTING);
    }

    if (pVoice && !once)
    {
        if (g_IsVoiceServer())
            pVoice->StartProcessing();

        once = true;
    }


}

void CNetGame::UpdateNetwork()
{
    Packet* pkt = nullptr;
    unsigned char packetIdentifier;

    while(pkt = m_pRakClient->Receive())
    {
        packetIdentifier = GetPacketID(pkt);

        switch(packetIdentifier)
        {
            case ID_AUTH_KEY:
                Log(OBFUSCATE("Incoming packet: ID_AUTH_KEY"));
                Packet_AuthKey(pkt);
                break;

            case ID_CONNECTION_BANNED:
            case ID_CONNECTION_ATTEMPT_FAILED:
                pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMsg(E_MSG::CONNECTION_ATTEMPT_FAILED));
                SetGameState(GAMESTATE_WAIT_CONNECT);
                break;

            case ID_NO_FREE_INCOMING_CONNECTIONS:
                if(g_pJavaWrapper)
                {
                    g_pJavaWrapper->SetLoadingText(2);
                }
                pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMsg(E_MSG::FULL_SERVER));
                SetGameState(GAMESTATE_WAIT_CONNECT);
                break;

            case ID_DISCONNECTION_NOTIFICATION:
                Packet_DisconnectionNotification(pkt);
                break;

            case ID_CONNECTION_LOST:
                Packet_ConnectionLost(pkt);
                break;

            case ID_CONNECTION_REQUEST_ACCEPTED:
                Packet_ConnectionSucceeded(pkt);
                break;

            case ID_FAILED_INITIALIZE_ENCRIPTION:
                pChatWindow->AddDebugMessage(OBFUSCATE("Failed to initialize encryption."));
                break;

            case ID_INVALID_PASSWORD:
                pChatWindow->AddDebugMessage(OBFUSCATE("Wrong server password."));
                m_pRakClient->Disconnect(0);
                break;

            case ID_PLAYER_SYNC:
                Packet_PlayerSync(pkt);
                break;

            case ID_VEHICLE_SYNC:
                Packet_VehicleSync(pkt);
                break;

            case ID_PASSENGER_SYNC:
                Packet_PassengerSync(pkt);
                break;

            case ID_MARKERS_SYNC:
                Packet_MarkersSync(pkt);
                break;

            case ID_AIM_SYNC:
                Packet_AimSync(pkt);
                break;

            case ID_BULLET_SYNC:
                Packet_BulletSync(pkt);
                break;

            case ID_TRAILER_SYNC:
                Packet_TrailerSync(pkt);
                break;

            case ID_CUSTOM_RPC:
                Packet_CustomRPC(pkt);
                break;
        }

        m_pRakClient->DeallocatePacket(pkt);
    }
}

void CNetGame::Packet_TrailerSync(Packet* p)
{
    CRemotePlayer* pPlayer;
    RakNet::BitStream bsSpectatorSync((unsigned char*)p->data, p->length, false);

    if (GetGameState() != GAMESTATE_CONNECTED)
        return;

    BYTE bytePacketID = 0;
    BYTE bytePlayerID = 0;

    TRAILER_SYNC_DATA trSync;

    bsSpectatorSync.Read(bytePacketID);
    bsSpectatorSync.Read(bytePlayerID);
    bsSpectatorSync.Read((char*)& trSync, sizeof(TRAILER_SYNC_DATA));

    pPlayer = CPlayerPool::GetSpawnedPlayer(bytePlayerID);

    if (pPlayer)
        pPlayer->StoreTrailerFullSyncData(&trSync);
}

#define CUSTOM_RPC_TOGGLE_HUD_ELEMENT   0x1

#define RPC_STREAM_CREATE				0x2
#define RPC_STREAM_POS					0x3
#define RPC_STREAM_DESTROY				0x4
#define RPC_STREAM_INDIVIDUAL			0x5
#define RPC_STREAM_VOLUME				0x6
#define RPC_STREAM_ISENABLED			0x7
#define RPC_OPEN_LINK					0x8
#define RPC_TIMEOUT_CHAT				0x9
#define RPC_CUSTOM_COMPONENT 			0x10
#define RPC_CUSTOM_HANDLING 			0x11
#define RPC_CUSTOM_ADD_PED				0x12
#define RPC_CUSTOM_VISUALS				0x13
#define RPC_CUSTOM_HANDLING_DEFAULTS	0x14
#define RPC_OPEN_SETTINGS				0x15
#define RPC_CUSTOM_AIM 					0x30

#define RPC_CUSTOM_ACTOR_PUT_IN_VEH		0x20
#define RPC_CUSTOM_ACTOR_REMOVE_VEH		0x21
#define RPC_CUSTOM_ACTOR_ADD_ADDITIONAL	0x22

#define RPC_SET_SKY_SPEED 				0x23
#define RPC_SET_SKYBOX 					0x24
#define RPC_SET_RADARLINE				0x25

#define RPC_CHECK_CASH					0x26
#define RPC_OPEN_DONATE				    0x27
#define RPC_HIDE_ALL			    	0x28
#define RPC_CUSTOM_VEH_TONER            0x33
#define RPC_GPS_VIEW					0x35
#define RPC_GREENZONE_VIEW				0x36
#define RPC_CUSTOM_VEH_VINYLS           0x39
#define RPC_MUTE_VOICE 					0x65
#define RPC_VOLUME_VOICE 				0x66
#define RPC_CAPTCHA						0x67
#define RPC_MINE						0x68
#define RPC_SAWMILL						0x69
#define RPC_RUBBISH						0x70
#define RPC_COLLECTORS					0x71
#define RPC_JOBINFO						0x72
#define RPC_POTATO						0x73
#define RPC_CREATENOTIFY 				0x86
#define RPC_REMOVENOTIFY 				0x87
#define RPC_SERVERID					0x89
#define RPC_NOTIFICATION				0x90
#define RPC_CAPTUREBIZWAR				0x92
#define RPC_INVENTORY          			0x93
#define RPC_INVENTORY_INFO      		0x94
#define RPC_VEHICLE_SHOWROOM    		0x95
#define RPC_VEHICLE_SHOWROOM_INFO    	0x96
#define RPC_VEHICLE_TUNING    			0x97
#define RPC_MARKET_INFO    			    0x98
#define RPC_TRADE                       0x99
#define RPC_TRADE_INFO                  0x100
#define RPC_VEHICLE_SPAWNER             0x101
#define RPC_SKIN_SPAWNER                0x102
#define RPC_CAR_COLLISION               0x103
#define RPC_CUSTOM_VEH_NUMBER 0x121
#define RPC_CUSTOM_VEH_HEADLIGHTS 0x122
#define RPC_CUSTOM_VEH_NEON 0x123
#define RPC_SHOW_AUTOSALON 0x124
#define RPC_HIDE_AUTOSALON 0x125
#define RPC_SETCAR 0x126
#define RPC_SETINTEFRACE 0x127
#define RPC_SHOWWIN 0x128
#define RPC_SETFUEL 0x129
#define RPC_CASES 0x130
#define RPC_NEWS 0x131
int greenzone = 0;
#include "../game/CCustomPlateManager.h"

#include "graphics/CInventory.h"
#include "graphics/CInventoryUniversal.h"
#include "graphics/CBuyAuto.h"
#include "graphics/CTuning.h"
#include "graphics/CInventoryTrade.h"
#include "CSettings.h"

int showSpeed = 1;
int showHud = 1;

enum
{
    INVENTORY_ID_STANDART = 1,
    INVENTORY_ID_RIGHT,
    INVENTORY_ID_LEFT,
    INVENTORY_TYPE_START_INFO = 100,
    INVENTORY_TYPE_END_INFO = 101,
};

void CNetGame::Packet_CustomRPC(Packet* p)
{

    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;

    bs.Read(packetID);
    bs.Read(rpcID);

    //pChatWindow->AddDebugMessage("p %d rpc %d", packetID, rpcID);

    switch (rpcID)
    {
        case RPC_CAR_COLLISION:
        {
            uint16_t vehicleId;
            uint8_t toggle;
            bs.Read(vehicleId);
            bs.Read(toggle);
            break;
        }
        case RPC_VEHICLE_SPAWNER:
        {
            uint8_t status;
            bs.Read(status);

            if(status == 1)
            {
                g_pJavaWrapper->ShowVehicleSpawner();
            }
            else
            {
                g_pJavaWrapper->HideVehicleSpawner();
            }
            break;
        }
        case RPC_SKIN_SPAWNER:
        {
            uint8_t status;
            bs.Read(status);

            if(status == 1)
            {
                g_pJavaWrapper->ShowSkinSpawner();
            }
            else
            {
                g_pJavaWrapper->HideSkinSpawner();
            }
            break;
        }
        case RPC_VEHICLE_TUNING:
        {
            CTuning::show();
            break;
        }
        case RPC_VEHICLE_SHOWROOM:
        {
            uint8_t status;
            bs.Read(status);
            if(status)
                CBuyAuto::show();
            else
                CBuyAuto::hide();
            break;
        }
        case RPC_SHOW_AUTOSALON:
        {
            g_pJavaWrapper->showAutoSalon();
            pGame->ToggleHUDElement(HUD_ELEMENT_CHAT, 0);
                pGame->ToggleHUDElement(HUD_ELEMENT_HUD, 0);
                pGame->ToggleHUDElement(HUD_ELEMENT_MAP, 0);
                pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, 0);
                g_pJavaWrapper->CallLauncherActivity(1235);
                g_pJavaWrapper->CallLauncherActivity(1238);
            break;
        }
        case RPC_HIDE_AUTOSALON:
        {
            g_pJavaWrapper->hideAutoSalon();
            pGame->ToggleHUDElement(HUD_ELEMENT_CHAT, 1);
            pGame->ToggleHUDElement(HUD_ELEMENT_HUD, 1);
            pGame->ToggleHUDElement(HUD_ELEMENT_MAP, 1);
            pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, 1);
            g_pJavaWrapper->CallLauncherActivity(1234);
            g_pJavaWrapper->CallLauncherActivity(1237);
            break;
        }
        case RPC_SETCAR:
        {
            int pricecar;
            bs.Read(pricecar);

            char szBuff[4096+1];

            uint16_t len1;
            bs.Read(len1);

            char link1[64*54];
            memset(link1, 0, sizeof(link1));
            memset(szBuff, 0, sizeof(szBuff));

            bs.Read(szBuff, len1);
            cp1251_to_utf8(link1, szBuff);

            g_pJavaWrapper->SetCar(pricecar, link1);
            break;
        }
        case RPC_SHOWWIN:
        {
            int number;
            bs.Read(number);

            g_pJavaWrapper->ShowWin(number);
            break;
        }
        case RPC_SETFUEL:
        {
            int fuel;
            bs.Read(fuel);

            g_pJavaWrapper->SetFuel(fuel);
            break;
        }
        case RPC_CASES:
        {
            g_pJavaWrapper->ShowCases();
            break;
        }
        case RPC_NEWS:
        {
            g_pJavaWrapper->ShowAd();
            break;
        }
        case RPC_SETINTEFRACE:
        {
            int type;
            bs.Read(type);

            g_pJavaWrapper->SetInterface(type);
            break;
        }

        case RPC_VEHICLE_SHOWROOM_INFO:
        {
            uint16_t modelId, maxSpeed, maxFuel, inStock;
            float time0To100;
            uint32_t price;
            uint8_t len;
            char strName[256];

            bs.Read(modelId);

            if(modelId == 0)
            {
                CBuyAuto::addCarToRecycler(0, 0, 0, 0, 0, 0, "");
                return;
            }

            bs.Read(price);
            bs.Read(maxSpeed);
            bs.Read(maxFuel);

            bs.Read(time0To100);
            bs.Read(inStock);

            bs.Read(len);
            bs.Read(&strName[0], len);

            strName[len] = '\0';

            CBuyAuto::addCarToRecycler(modelId, price, maxSpeed, maxFuel, time0To100, inStock, strName);

            break;
        }

        case RPC_INVENTORY_INFO:
        {
            uint8_t type, statusUse, item_type, acc_premium, skin_premium, lenName;
            int8_t acc_slot;
            uint32_t mysqlId;
            int16_t slotId;
            uint16_t itemId, amount, objectid, typeUpdate, inventoryId, lenInfo;
            float rotX, rotY, rotZ, rotZoom, acc_wear, skin_wear;
            char szBuff[4096+1];
            char strName[64*3+1];
            char strInfo[256*3+1];
            CVector vecRot1;
            vecRot1.x = 0.0;
            vecRot1.y = 0.0;
            vecRot1.z = 0.0;

            bs.Read(inventoryId);
            bs.Read(typeUpdate);



            /*
            typeUpdate:
             ��� �������� ��� ���� ��������� �������� ��������� �� ����� ����:
              100 = ������ �����,
              101 = ���������(���� �������� ������)
               0 = ��� ������������� � �������� �������� ����� ������ � ���������
            */
            if(typeUpdate == INVENTORY_TYPE_START_INFO && inventoryId == INVENTORY_ID_STANDART)
            {
                Log("INVENTORY_TYPE_START_INFO");
                CInventory::clearInventory();
            }
            else if(typeUpdate == INVENTORY_TYPE_START_INFO && inventoryId == INVENTORY_ID_RIGHT)
            {
                Log("INVENTORY_TYPE_START_INFO right");
                CInventoryUniversal::clearRightInventory();
            }
            else if(typeUpdate == INVENTORY_TYPE_START_INFO && inventoryId == INVENTORY_ID_LEFT)
            {
                Log("INVENTORY_TYPE_START_INFO left");
                CInventoryUniversal::clearLeftInventory();
            }

            bs.Read(slotId);
            if(slotId == -1 && typeUpdate == INVENTORY_TYPE_END_INFO)
            {
                if(inventoryId == INVENTORY_ID_STANDART) {
                    Log("INVENTORY_TYPE_END_INFO");
                    CInventory::updateAcsSlotsInfo(-1, 0, vecRot1, 0, "", "");
                    CInventory::updateSlotsInfo(0, slotId, 0, 0, 0, vecRot1, 0, "", "");
                }
                else if(inventoryId == INVENTORY_ID_RIGHT) {
                    Log("INVENTORY_TYPE_END_INFO right");
                    CInventoryUniversal::updateSlotsRightInfo(0, slotId, 0, 0, 0, vecRot1, 0, "", "");
                }
                else if(inventoryId == INVENTORY_ID_LEFT) {
                    Log("INVENTORY_TYPE_END_INFO left");
                    CInventoryUniversal::updateSlotsLeftInfo(0, slotId, 0, 0, 0, vecRot1, 0, "", "", 0);
                }
            }
            else if(typeUpdate == 0)
            {
                bs.Read(mysqlId);
                if(mysqlId != 0)
                {
                    bs.Read(itemId);
                    bs.Read(amount);
                    bs.Read(statusUse);
                    bs.Read(objectid);
                    bs.Read(rotX);
                    bs.Read(rotY);
                    bs.Read(rotZ);
                    bs.Read(rotZoom);

                    Log("RPC_INVENTORY_INFO | statusUse: %d, objectid: %d, itemId: %d", statusUse, objectid, itemId);

                    bs.Read(item_type); // Item Type
                    if(item_type == TYPE_ITEMS_ACCESSORIES)
                    {
                        bs.Read(acc_slot);
                        bs.Read(acc_wear);
                        bs.Read(acc_premium);
                    }
                    if(item_type == TYPE_ITEMS_SKINS)
                    {
                        bs.Read(skin_wear);
                        bs.Read(skin_premium);
                    }

                    bs.Read(lenName);
                    bs.Read(szBuff, lenName);
                    szBuff[lenName] = '\0';
                    cp1251_to_utf8(strName, szBuff);

                    bs.Read(lenInfo);
                    bs.Read(szBuff, lenInfo);
                    szBuff[lenInfo] = '\0';
                    cp1251_to_utf8(strInfo, szBuff);

                    CVector vecRot;
                    vecRot.x = rotX;
                    vecRot.y = rotY;
                    vecRot.z = rotZ;

                    if (inventoryId == INVENTORY_ID_STANDART)
                    {
                        if (item_type != TYPE_ITEMS_INVENTORY)
                            CInventory::updateSlotsInfo(inventoryId, slotId, item_type, statusUse, objectid, vecRot, rotZoom, strName, strInfo);
                        else
                            CInventory::updateSlotsInfo(inventoryId, slotId, item_type, amount, objectid, vecRot, rotZoom, strName, strInfo);

                        if (item_type == TYPE_ITEMS_ACCESSORIES && statusUse != 0 && acc_slot != -1)
                        {
                            CInventory::updateAcsSlotsInfo(acc_slot, objectid, vecRot, rotZoom, strName, strInfo);
                        }

                        if (item_type == TYPE_ITEMS_SKINS && statusUse == 1)
                            CInventory::setSkin(pGame->FindPlayerPed()->m_pEntity->nModelIndex);

                        if(typeUpdate == INVENTORY_TYPE_END_INFO) // ��� �������� ��� ���� ��������� �������� ���������
                        {
                            Log("INVENTORY_TYPE_END_INFO");
                            CInventory::updateAcsSlotsInfo(-1, 0, vecRot1, 0, "", "");
                            CInventory::updateSlotsInfo(0, -1, 0, 0, 0, vecRot1, 0, "", "");
                        }

                    }
                    else if (inventoryId == INVENTORY_ID_RIGHT)
                    {
                        if (item_type != TYPE_ITEMS_INVENTORY)
                            CInventoryUniversal::updateSlotsRightInfo(inventoryId, slotId, item_type, statusUse, objectid, vecRot, rotZoom, strName, strInfo);
                        else
                            CInventoryUniversal::updateSlotsRightInfo(inventoryId, slotId, item_type, amount, objectid, vecRot, rotZoom, strName, strInfo);

                        if(typeUpdate == INVENTORY_TYPE_END_INFO) // ��� �������� ��� ���� ��������� �������� ���������
                        {
                            Log("INVENTORY_TYPE_END_INFO right");
                            CInventoryUniversal::updateSlotsRightInfo(0, -1, 0, 0, 0, vecRot1, 0, "", "");
                        }
                    }
                    else if (inventoryId == INVENTORY_ID_LEFT)
                    {
                        if (item_type != TYPE_ITEMS_INVENTORY)
                            CInventoryUniversal::updateSlotsLeftInfo(0, slotId, item_type, statusUse, objectid, vecRot, rotZoom, strName, strInfo, 0);
                        else
                            CInventoryUniversal::updateSlotsLeftInfo(0, slotId, item_type, amount, objectid, vecRot, rotZoom, strName, strInfo, 0);

                        if(typeUpdate == INVENTORY_TYPE_END_INFO) // ��� �������� ��� ���� ��������� �������� ���������
                        {
                            Log("INVENTORY_TYPE_END_INFO left");
                            CInventoryUniversal::updateSlotsLeftInfo(0, -1, 0, 0, 0, vecRot1, 0, "", "", 0);
                        }
                    }
                }
                else
                {
                    if(inventoryId == INVENTORY_ID_STANDART)
                        CInventory::setNullSlot(slotId);
                    else if(inventoryId == INVENTORY_ID_RIGHT)
                        CInventoryUniversal::setNullSlotRight(slotId);
                    else if(inventoryId == INVENTORY_ID_LEFT)
                        CInventoryUniversal::setNullSlotLeft(slotId);

                    if(typeUpdate == INVENTORY_TYPE_END_INFO) // ��� �������� ��� ���� ��������� ����
                    {
                        if(inventoryId == INVENTORY_ID_STANDART) {
                            Log("INVENTORY_TYPE_END_INFO");
                            CInventory::updateAcsSlotsInfo(-1, 0, vecRot1, 0, "", "");
                            CInventory::updateSlotsInfo(0, slotId, 0, 0, 0, vecRot1, 0, "", "");
                        }
                        else if(inventoryId == INVENTORY_ID_RIGHT) {
                            Log("INVENTORY_TYPE_END_INFO right null");
                            CInventoryUniversal::updateSlotsRightInfo(0, slotId, 0, 0, 0, vecRot1, 0, "", "");
                        }
                        else if(inventoryId == INVENTORY_ID_LEFT) {
                            Log("INVENTORY_TYPE_END_INFO left null");
                            CInventoryUniversal::updateSlotsLeftInfo(0, slotId, 0, 0, 0, vecRot1, 0, "", "", 0);
                        }
                    }
                }
            }
            break;
        }

        case RPC_INVENTORY:
        {
            uint8_t status, type, lenUniversal1, lenUniversal2;
            float satiety, thirst;
            uint16_t slots, maxSlotPlayer, maxSlotUniversal, biasUniversal;
            uint32_t money, donate;
            char universal1[128], universal2[128];
            bs.Read(status);
            if(status != 0)
            {
                bs.Read(type);
                bs.Read(maxSlotPlayer);

                if (!maxSlotPlayer)
                    return;

                Log("playerSlots: %d", maxSlotPlayer);

                if(type == 0) {
                    bs.Read(money);
                    bs.Read(donate);
                    bs.Read(satiety);
                    bs.Read(thirst);

                    bs.Read(lenUniversal1);
                    bs.Read(&universal1[0], lenUniversal1);

                    universal1[lenUniversal1] = '\0';

                    CInventory::show(maxSlotPlayer);
                    CInventory::setSkin(pGame->FindPlayerPed()->m_pEntity->nModelIndex);
                    int health = pGame->FindPlayerPed()->GetHealth();
                    if(health > 100) health = 100;
                    CInventory::setInfo(health, thirst, satiety, money,
                                        donate);
                }
                else
                {
                    bs.Read(biasUniversal);
                    bs.Read(maxSlotUniversal);
                    Log("universalSlots: %d, biasUniversal: %d", maxSlotUniversal, biasUniversal);
                    if (!maxSlotUniversal)
                        return;

                    bs.Read(money);
                    bs.Read(donate);

                    bs.Read(lenUniversal1);
                    bs.Read(&universal1[0], lenUniversal1);
                    bs.Read(lenUniversal2);
                    bs.Read(&universal2[0], lenUniversal2);

                    universal1[lenUniversal1] = '\0';
                    universal2[lenUniversal2] = '\0';

                    CInventoryUniversal::show(maxSlotPlayer, maxSlotUniversal, biasUniversal);
                    CInventoryUniversal::setInfo(money, donate, universal1,universal2);
                }

            }
            else
            {
                CInventory::hide();
            }
            break;
        }
        case RPC_CUSTOM_VEH_VINYLS:
		{
			int number, vehId;

			bs.Read(number);
			bs.Read(vehId);

			char szTex[128];
			sprintf(&szTex[0], "remapbody%d", number);

			CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
			if (pVeh)
			{
				// Удаляем старую текстуру
				pVeh->RemoveTexture("remap_cbody_0");

				// Применяем новую текстуру
				pVeh->ApplyTexture("remap_cbody_0", &szTex[0]);
			}

			break;
		}
        case RPC_CUSTOM_VEH_TONER:
        {
            int number, vehId;

            bs.Read(number);
            bs.Read(vehId);

            char szTex[128];
            sprintf(&szTex[0], "c_remap_%d", number);

            CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
            if (pVeh)
            {
                // Удаляем старую текстуру
                pVeh->RemoveTexture("remap_toner_1");
                pVeh->RemoveTexture("remap_toner_2");
                pVeh->RemoveTexture("remap_toner_3");

                // Применяем новую текстуру
                pVeh->ApplyTexture("remap_toner_1", &szTex[0]);
                pVeh->ApplyTexture("remap_toner_2", &szTex[0]);
                pVeh->ApplyTexture("remap_toner_3", &szTex[0]);


            }

            break;
        }
        case RPC_CUSTOM_VEH_HEADLIGHTS:
		{
	        int vehId, r, g, b;
	
	        bs.Read(vehId);
	        bs.Read(r);
	        bs.Read(g);
	        bs.Read(b);
	
	        if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
					pVeh->SetHeadlightsColor(r, g, b);
				}
			}
			break;
	    }
        case RPC_CUSTOM_VEH_NEON:
		{
            int vehId;
            int neontype;
            bs.Read(vehId);
            bs.Read(neontype);

            if (GetVehiclePool())
            {
                CVehicle* pVehicle = GetVehiclePool()->GetAt(vehId);
                if (pVehicle)
                {
                    char textureName[32];
                    sprintf(textureName, "neon%d", neontype);

                    pVehicle->SetCustomShadow(255, 255, 255, 0.2f, 0.6f, textureName);

                }
            }

        }
    
        case RPC_TRADE:
        {
            uint8_t playerLocalLen, player1Len, inventoryNameLen, readinessPlayerLocal, readinessPlayer1;
            uint32_t moneyLocal, money1;
            char szBuff[128];
            char strNameLocal[128];
            char strName1[128];
            char inventoryName[128];
            uint8_t status;
            bs.Read(status);

            if(status > 0)
            {
                bs.Read(playerLocalLen);
                bs.Read(szBuff, playerLocalLen);
                szBuff[playerLocalLen] = '\0';
                cp1251_to_utf8(strNameLocal, szBuff);

                bs.Read(player1Len);
                bs.Read(szBuff, player1Len);
                szBuff[player1Len] = '\0';
                cp1251_to_utf8(strName1, szBuff);

                bs.Read(readinessPlayerLocal);
                bs.Read(readinessPlayer1);

                bs.Read(inventoryNameLen);
                bs.Read(szBuff, inventoryNameLen);
                szBuff[inventoryNameLen] = '\0';
                cp1251_to_utf8(inventoryName, szBuff);

                if(status == 1)
                {
                    Log("RPC_TRADE status 1");
                    CInventoryTrade::show(100);
                }

                CInventoryTrade::setPlayerName(0, strNameLocal);
                CInventoryTrade::setPlayerName(1, strName1);
                CInventoryTrade::setPlayerReadiness(0, readinessPlayerLocal);
                CInventoryTrade::setPlayerReadiness(1, readinessPlayer1);
                CInventoryTrade::setMoney(0, 0, inventoryName);
            }
            else
            {
                Log("RPC_TRADE status 0");
                CInventoryTrade::hide();
            }
            break;
        }
        case RPC_TRADE_INFO:
        {
            uint16_t type_bet, type_side, type_update, lenInfo;
            int16_t slot_id;
            uint8_t lenName, lenStatus;
            float rotX, rotY, rotZ, rotZoom;
            uint16_t objectid;
            bs.Read(type_side);
            bs.Read(type_update);
            bs.Read(slot_id);

            char szBuff[4096+1];
            char strName[64*3+1];
            char strInfo[256*3+1];
            char strStatus[64*3+1];

            CVector vecRot;
            vecRot.x = 0.0f;
            vecRot.y = 0.0f;
            vecRot.z = 0.0f;

            if(slot_id == -1)
            {
                if(type_update == INVENTORY_TYPE_START_INFO)
                {
                    if(type_side == INVENTORY_ID_RIGHT)
                    {
                        Log("TRADE_TYPE_START_INFO right");
                        CInventoryTrade::clearRightInventory();
                    }
                    else if(type_side == INVENTORY_ID_LEFT)
                    {
                        Log("TRADE_TYPE_START_INFO left");
                        CInventoryTrade::clearLeftInventory();
                    }
                }
                if(type_update == INVENTORY_TYPE_END_INFO)
                {
                    if(type_side == INVENTORY_ID_RIGHT) {
                        CInventoryTrade::updateRightSlotsInfo(0, slot_id, 0, 0, 0, vecRot, 0, "", "", "");
                        uint16_t maxpages;
                        bs.Read(maxpages);
                        //set MAXPAGES
                        Log("TRADE_TYPE_END_INFO right");
                        CInventoryTrade::setMaxPages(maxpages);
                    }
                    else if(type_side == INVENTORY_ID_LEFT) {
                        Log("TRADE_TYPE_END_INFO left");
                        CInventoryTrade::updateLeftSlotsInfo(0, slot_id, 0, 0, 0, vecRot, 0, "", "", "");
                    }
                }
            }
            else
            {
                bs.Read(type_bet);

                bs.Read(lenStatus);
                bs.Read(szBuff, lenStatus);
                szBuff[lenStatus] = '\0';
                cp1251_to_utf8(strStatus, szBuff);

                bs.Read(objectid);
                bs.Read(rotX);
                bs.Read(rotY);
                bs.Read(rotZ);
                bs.Read(rotZoom);

                bs.Read(lenName);
                bs.Read(szBuff, lenName);
                szBuff[lenName] = '\0';
                cp1251_to_utf8(strName, szBuff);

                bs.Read(lenInfo);
                bs.Read(szBuff, lenInfo);
                szBuff[lenInfo] = '\0';
                cp1251_to_utf8(strInfo, szBuff);

                Log("RPC_TRADE_INFO modelid: %d %d %d %d", objectid, type_side, type_update, type_bet);

                vecRot.x = rotX;
                vecRot.y = rotY;
                vecRot.z = rotZ;

                if(type_side == INVENTORY_ID_RIGHT)
                {
                    CInventoryTrade::updateRightSlotsInfo(0, slot_id, type_bet, 0, objectid, vecRot, rotZoom, strName, strInfo, strStatus);
                }
                else if(type_side == INVENTORY_ID_LEFT)
                {
                    CInventoryTrade::updateLeftSlotsInfo(0, slot_id, type_bet, 0, objectid, vecRot, rotZoom, strName, strInfo, strStatus);
                }
            }
            break;
        }

        case RPC_MARKET_INFO:
        {
            uint16_t type_id, type_update;
            int16_t slot_id;
            float rotX, rotY, rotZ, rotZoom;
            uint16_t itemId, amount, objectid, lenName, lenInfo;
            int32_t market_price, market_amount;
            uint8_t item_type;
            char szBuff[4096+1];
            char strName[64*3+1];
            char strInfo[256*3+1];

            bs.Read(type_id);
            Log("INVENTORY_UPDATE_INFO type_id == %d", type_id);
            bs.Read(type_update);
            bs.Read(slot_id);

            CVector vecRot;
            vecRot.x = 0.0f;
            vecRot.y = 0.0f;
            vecRot.z = 0.0f;

            if(slot_id == -1)
            {
                if(type_update == INVENTORY_TYPE_START_INFO)
                {
                    Log("INVENTORY_TYPE_START_INFO market");
                    CInventoryUniversal::clearLeftInventory();
                }
                else if(type_update == INVENTORY_TYPE_END_INFO)
                {
                    Log("INVENTORY_TYPE_END_INFO");
                    CInventoryUniversal::updateSlotsLeftInfo(1, -1, 0, 0, 0, vecRot, 0, "", "", 0);
                }
            }
            else
            {
                Log("INVENTORY_UPDATE_INFO market");
                bs.Read(amount);
                bs.Read(objectid);
                bs.Read(rotX);
                bs.Read(rotY);
                bs.Read(rotZ);
                bs.Read(rotZoom);
                bs.Read(item_type);

                vecRot.x = rotX;
                vecRot.y = rotY;
                vecRot.z = rotZ;

                bs.Read(market_amount);
                bs.Read(market_price);

                if(item_type == TYPE_ITEMS_VEHICLE_NUMBER || item_type == TYPE_ITEMS_SIM_CARD) {
                    bs.Read(lenInfo);
                    bs.Read(szBuff, lenInfo);
                    szBuff[lenInfo] = '\0';
                    cp1251_to_utf8(strInfo, szBuff);
                }

                Log("market count: %d price: %d", market_amount, market_price);
                CInventoryUniversal::updateSlotsLeftInfo(1, slot_id, item_type, market_amount, objectid, vecRot, rotZoom, "", strInfo, market_price);
            }
            break;
        }

        case RPC_GPS_VIEW:
        {
            uint32_t value = 0;
            bs.Read(value);
            if(value == 1) g_pJavaWrapper->CallLauncherActivity(1230);
            if(value == 2) g_pJavaWrapper->CallLauncherActivity(1231);
            break;
        }

        case RPC_CAPTUREBIZWAR: {
            uint8_t status;
            char str[256];
            uint8_t len;
            uint16_t time;
            int16_t points1;
            int16_t points2;
            int32_t colorLeft;
            int32_t colorRight;
            uint16_t kills;
            uint16_t deaths;
            float damage;
            float take;
            bs.Read(status);
            if(status)
            {
                bs.Read(time);
                bs.Read(points1);
                bs.Read(points2);
                bs.Read(colorLeft);
                bs.Read(colorRight);
                bs.Read(kills);
                bs.Read(deaths);
                bs.Read(damage);
                bs.Read(take);
                bs.Read(len);
                bs.Read(&str[0], len);

                str[len] = '\0';

                Log("%x %x", colorLeft, colorRight);

                if(!len)
                    g_pJavaWrapper->showBizWar(time, colorLeft, colorRight, kills, deaths, damage, take, "", points1, points2);
                else
                    g_pJavaWrapper->showBizWar(time, colorLeft, colorRight, kills, deaths, damage, take, str, points1, points2);
            }
            else
            {
                g_pJavaWrapper->hideBizWar();
            }

            break;
        }

        case RPC_NOTIFICATION: {
            uint16_t type;
            char str[256];
            char str2[256];
            char str3[256];
            uint8_t len, len2, len3;
            uint8_t time;
            uint16_t actionId;
            bs.Read(type);
            bs.Read(len);
            bs.Read(&str[0], len);
            bs.Read(len2);
            bs.Read(&str2[0], len2);
            bs.Read(len3);
            bs.Read(&str3[0], len3);
            bs.Read(time);
            bs.Read(actionId);

            str[len] = '\0';
            str2[len2] = '\0';
            str3[len3] = '\0';

            if(type != 255)
                g_pJavaWrapper->showNotification(type, str, time, actionId, str2, str3);
            else
                g_pJavaWrapper->hideNotification();

            break;
        }

        case RPC_POTATO:
        {
            g_pJavaWrapper->showPotato();
            break;
        }

        case RPC_JOBINFO: {
            char str[256];
            uint8_t len;
            uint8_t value;
            uint8_t progress;
            uint16_t money;
            bs.Read(value);
            bs.Read(progress);
            bs.Read(money);
            bs.Read(len);
            bs.Read(&str[0], len);

            str[len] = '\0';

            if(value)
                g_pJavaWrapper->ShowJobInfo(progress, money, str);
            else
                g_pJavaWrapper->HideJobInfo();

            break;
        }

        case RPC_CAPTCHA:
        {
            char str1[256];
            uint8_t len1;
            bs.Read(len1);
            bs.Read(&str1[0], len1);
            str1[len1] = '\0';
            pChatWindow->AddDebugMessage("RPC_CAPTCHA %d %s", len1, str1);
            g_pJavaWrapper->ShowCaptcha(str1);
            break;
        }

        case RPC_MINE:
        {
            g_pJavaWrapper->ShowMine();
            break;
        }

        case RPC_SAWMILL:
        {
            g_pJavaWrapper->ShowSawmill();
            break;
        }

        case RPC_RUBBISH:
        {
            g_pJavaWrapper->ShowRubbish();
            break;
        }

        case RPC_COLLECTORS:
        {
            g_pJavaWrapper->ShowCollectors();
            break;
        }

        case RPC_CHECK_CASH:
        {
            uint8_t bLen, bLen1;
            uint32_t bVersion;
            char szText[30];
            char szText1[30];

            memset(szText, 0, 30);
            memset(szText1, 0, 30);

            bs.Read(bLen);
            if (bLen >= sizeof(szText) - 1)
                return;

            bs.Read(&szText[0], bLen);

            bs.Read(bLen1);
            if (bLen1 >= sizeof(szText1) - 1)
                return;

            bs.Read(&szText1[0], bLen1);

            bs.Read(bVersion);

            RwTexture* pCashTexture = nullptr;
            pCashTexture = (RwTexture*)LoadTextureFromDB(szText1, szText);

            int iVersion;
            if (pCashTexture) iVersion = bVersion;
            else iVersion = 0;

            RakNet::BitStream bsParams;

            bsParams.Write(iVersion);
            m_pRakClient->RPC(&RPC_CustomHash, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
            break;
        }

        case RPC_MUTE_VOICE: {
            uint32_t playerId = 0;
            uint8_t muteState = 0;
            bs.Read(playerId);
            bs.Read(muteState);
            if(pVoice) {
                if(muteState) {
                    pVoice->MutePlayer(playerId);
                } else {
                    pVoice->UnMutePlayer(playerId);
                }
            }

            break;
        }

        case RPC_VOLUME_VOICE: {
            uint32_t playerId = 0;
            uint8_t volume = 0;
            bs.Read(playerId);
            bs.Read(volume);
            if(pVoice) {
                if(playerId == CPlayerPool::GetLocalPlayerID()) {
                    pVoice->SetVolume(volume);
                }
                else
                {
                    pVoice->SetVolumePlayer(playerId, volume);
                }
            }
            break;
        }
        case RPC_REMOVENOTIFY: {
            uint16_t notifyId;
            bs.Read(notifyId);
            this->GetNotificationPool()->Remove(notifyId);
            break;
        }

        case RPC_CREATENOTIFY:
        {
            uint8_t type;
            uint32_t duration;
            float screenX, screenY;
            uint8_t textLen;
            char textBuffer[256+1] = { '\0' };
            float fTextFontSize;
            uint32_t textColor;
            uint8_t bHasOutline;
            uint32_t textOutlineColor;
            int textOutlineOffset;
            uint8_t bHasIcon;
            float iconScaleX, iconScaleY;
            uint8_t dbLen;
            char dbName[256+1] = { '\0' };
            uint8_t texLen;
            char texName[256+1] = { '\0' };
            uint8_t bRecreate;

            memset(textBuffer, 0, ARRAY_SIZE(textBuffer));
            memset(dbName, 0, ARRAY_SIZE(dbName));
            memset(texName, 0, ARRAY_SIZE(texName));

            bs.Read(type);
            bs.Read(duration);
            bs.Read(screenX);
            bs.Read(screenY);

            bs.Read(textLen);
            if (textLen >= sizeof(textBuffer) - 1) {
                return;
            }

            bs.Read(&textBuffer[0], textLen);

            bs.Read(fTextFontSize);
            bs.Read(textColor);
            bs.Read(bHasOutline);
            bs.Read(textOutlineColor);
            bs.Read(textOutlineOffset);
            bs.Read(bHasIcon);
            bs.Read(iconScaleX);
            bs.Read(iconScaleY);

            bs.Read(dbLen);
            if (dbLen >= sizeof(dbName) - 1) {
                return;
            }

            bs.Read(&dbName[0], dbLen);

            bs.Read(texLen);
            if (texLen >= sizeof(texName) - 1) {
                return;
            }

            bs.Read(&texName[0], texLen);

            bs.Read(bRecreate);

            uintptr_t icon_texture = LoadTextureFromDB(dbName, texName);
            if(!icon_texture) break;

            this->GetNotificationPool()->Add(type, duration, ImVec2(screenX, screenY), textBuffer, fTextFontSize, textColor, bHasOutline, textOutlineColor, textOutlineOffset, bHasIcon, ImVec2(iconScaleX, iconScaleY), icon_texture, bRecreate);
            break;
        }
        case RPC_OPEN_SETTINGS:
        {
            g_pJavaWrapper->ShowClientSettings();
            break;
        }
        case RPC_OPEN_DONATE:
        {
            g_pJavaWrapper->CallLauncherActivity(15001);
            break;
        }
        case RPC_GREENZONE_VIEW:
        {
            uint32_t value = 0;
            bs.Read(value);
            greenzone = value;
            if(value == 1) {
                g_pJavaWrapper->CallLauncherActivity(1232);
            }
            if(value == 2) {
                g_pJavaWrapper->CallLauncherActivity(1233);
            }
            break;
        }
        case RPC_SERVERID:
        {
            uint32_t value = 0;
            bs.Read(value);
            g_pJavaWrapper->SetHudServerID(value);
            break;
        }
        case RPC_HIDE_ALL:
        {
            uint8_t toggle;
            bs.Read(toggle);

            showHud = toggle;
            showSpeed = toggle;

            pGame->DisplayWidgets(toggle);

            if(showHud == 0)
            {
                Log("RPC_HIDE_ALL 0");
                pGame->ToggleHUDElement(HUD_ELEMENT_HUD, 0);
                pGame->ToggleHUDElement(HUD_ELEMENT_MAP, 0);
                pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, 0);
                g_pJavaWrapper->CallLauncherActivity(1235);
                g_pJavaWrapper->CallLauncherActivity(1238);
            }
            else
            {
                Log("RPC_HIDE_ALL 1");
                pGame->ToggleHUDElement(HUD_ELEMENT_HUD, 1);
                pGame->ToggleHUDElement(HUD_ELEMENT_MAP, 1);
                pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, 1);
                g_pJavaWrapper->CallLauncherActivity(1234);
                g_pJavaWrapper->CallLauncherActivity(1237);
            }
            pGame->HandleChangedHUDStatus();
            break;
        }

        case RPC_SET_SKY_SPEED:
        {
            uint8_t speed;
            bs.Read(speed);

            break;
        }

        case RPC_SET_SKYBOX:
        {
            uint8_t bLen;
            char szText[30];

            memset(szText, 0, 30);

            bs.Read(bLen);
            if (bLen >= sizeof(szText) - 1)
                return;

            bs.Read(&szText[0], bLen);

            break;
        }

        case RPC_SET_RADARLINE:
        {
            uint8_t bLen, bType;
            char szText[30];

            memset(szText, 0, 30);

            bs.Read(bType);
            bs.Read(bLen);
            if (bLen >= sizeof(szText) - 1)
                return;

            bs.Read(&szText[0], bLen);

            CRadarRect::ChangeTextures(bType, szText);
            break;
        }

        case RPC_CUSTOM_HANDLING_DEFAULTS:
        {
            uint16_t vehId;
            bs.Read(vehId);

            CVehicle* pVeh = CVehiclePool::GetAt(vehId);
            if (pVeh)
                pVeh->ResetVehicleHandling();

            break;
        }

        case RPC_CUSTOM_VISUALS:
        {
            uint16_t vehId;
            uint8_t bVehicleColor[3];
            uint8_t bLightsColor[3];
            int8_t bWheelAlignX;
            int8_t bWheelAlignY;
            int16_t sWheelOffsetXX;
            int16_t sWheelOffsetXY;
            uint8_t bToner[3];
            uint8_t bVinyls[2];
            uint16_t fWheelWidth;

            uint8_t bPlateType, bLen;

            char szText[30];
            char szRegion[10];

            memset(szText, 0, 30);
            memset(szRegion, 0, 10);

            bs.Read(vehId);

            CVehicle *pVeh = CVehiclePool::GetAt(vehId);
            if (pVeh) {
                bs.Read(bLightsColor[0]);
                bs.Read(bLightsColor[1]);
                bs.Read(bLightsColor[2]);
                bs.Read(fWheelWidth);
                bs.Read(bWheelAlignX);
                bs.Read(bWheelAlignY);
                bs.Read(sWheelOffsetXX);
                bs.Read(sWheelOffsetXY);
                bs.Read(bToner[0]);
                bs.Read(bToner[1]);
                bs.Read(bToner[2]);
                bs.Read(bVinyls[0]);
                bs.Read(bVinyls[1]);
                bs.Read(bPlateType);

                if (bPlateType == 1 || bPlateType == 4 || bPlateType == 5) {
                    bs.Read(bLen);
                    if (bLen >= sizeof(szText) - 1)
                        return;

                    bs.Read(&szText[0], bLen);

                    bs.Read(bLen);
                    if (bLen >= sizeof(szRegion) - 1)
                        return;

                    bs.Read(&szRegion[0], bLen);
                } else if (bPlateType == 2 || bPlateType == 3) {
                    bs.Read(bLen);
                    if (bLen >= sizeof(szText) - 1)
                        return;

                    bs.Read(&szText[0], bLen);
                }

                uint8_t bShadowColor[3];
                uint8_t bShadowSizeX, bShadowSizeY;
                char szName[32];

                memset(szName, 0, sizeof(szName));

                bs.Read(bShadowColor[0]);
                bs.Read(bShadowColor[1]);
                bs.Read(bShadowColor[2]);
                bs.Read(bShadowSizeX);
                bs.Read(bShadowSizeY);
                bs.Read(bLen);

                if (bLen >= sizeof(szName) + 1)
                    return;

                bs.Read(szName, bLen);

                pVeh->SetCustomShadow(bShadowColor[0], bShadowColor[1], bShadowColor[2],
                                      (float) bShadowSizeX / 10.0f,
                                      (float) bShadowSizeY / 10.0f,
                                      szName);

                if (bLightsColor[0] != 0xFF || bLightsColor[1] != 0xFF ||
                    bLightsColor[2] != 0xFF)
                    pVeh->SetHeadlightsColor(bLightsColor[0], bLightsColor[1], bLightsColor[2]);

                if (fWheelWidth)
                    pVeh->SetWheelWidth((float) fWheelWidth / 100.0f);

                if (bWheelAlignX)
                    pVeh->SetWheelAlignment(0, (float) bWheelAlignX);

                if (bWheelAlignY)
                    pVeh->SetWheelAlignment(1, (float) bWheelAlignY);

                if (sWheelOffsetXX) {
                    auto fValueX = (float) ((float) sWheelOffsetXX / 100.0f);
                    pVeh->SetWheelOffset(0, fValueX);
                    //pVeh->ProcessWheelsOffset();
                }

                if (sWheelOffsetXY) {
                    auto fValueX = (float) ((float) sWheelOffsetXY / 100.0f);
                    pVeh->SetWheelOffset(1, fValueX);
                    //pVeh->ProcessWheelsOffset();
                }

                pVeh->ApplyToner(1, bToner[0]);
                pVeh->ApplyToner(2, bToner[1]);
                pVeh->ApplyToner(3, bToner[2]);
                pVeh->ApplyVinyls(bVinyls[0], bVinyls[1]);

                //pChatWindow->AddDebugMessage("%d %d %d %d %d", bToner[0], bToner[1], bToner[2], bVinyls[0], bVinyls[1]);
                if (bPlateType) {
                    pVeh->CreatePlate((ePlateType) bPlateType, szText,
                                      szRegion);
                }
            }

            break;
        }

        case RPC_CUSTOM_ACTOR_PUT_IN_VEH:
        {
            uint16_t actorId;
            VEHICLEID vehicleId;
            uint8_t seat;

            bs.Read(actorId);
            bs.Read(vehicleId);
            bs.Read(seat);

#ifdef _CDEBUG
            pChatWindow->AddDebugMessage(OBFUSCATE("Put actor %d to %d in %d"), actorId, vehicleId, seat);
#endif

            if (CActorPool::GetAt(actorId) && CVehiclePool::GetAt(vehicleId))
            {
                int iCarID = CVehiclePool::FindGtaIDFromID((int)vehicleId);

                CActorPool::GetAt(actorId)->PutDirectlyInVehicle(iCarID, (int)seat);
            }
            break;
        }

        case RPC_CUSTOM_ACTOR_REMOVE_VEH:
        {
            uint16_t actorId;
            bs.Read(actorId);

            if (CActorPool::GetAt(actorId))
                CActorPool::GetAt(actorId)->RemoveFromVehicle();
            break;
        }

        case RPC_CUSTOM_ACTOR_ADD_ADDITIONAL:
        {
            uint16_t actorId;
            VEHICLEID vehicleId;
            uint8_t seat;

            bs.Read(actorId);
            bs.Read(vehicleId);
            bs.Read(seat);

            if (CActorPool::GetAt(actorId) && CVehiclePool::GetAt(vehicleId))
            {
                int iCarID = CVehiclePool::FindGtaIDFromID((int)vehicleId);

                CActorPool::GetAt(actorId)->PutDirectlyInVehicle(iCarID, (int)seat);
            }
            break;
        }

        case RPC_CUSTOM_ADD_PED:
        {
            uint16_t player;
            uint8_t moveAnim;
            bs.Read(player);
            bs.Read(moveAnim);

            if (player == CPlayerPool::GetLocalPlayerID())
            {
                if (CPlayerPool::GetLocalPlayer())
                {
                    if (CPlayerPool::GetLocalPlayer()->GetPlayerPed())
                        CPlayerPool::GetLocalPlayer()->GetPlayerPed()->SetMoveAnim((int)moveAnim);
                }
            }

            if (CPlayerPool::GetSpawnedPlayer(player))
            {
                if (CPlayerPool::GetSpawnedPlayer(player)->GetPlayerPed())
                    CPlayerPool::GetSpawnedPlayer(player)->GetPlayerPed()->SetMoveAnim((int)moveAnim);
            }
            break;
        }

        case RPC_CUSTOM_HANDLING:
        {
            uint16_t veh;
            uint8_t value;
            bs.Read(veh);
            bs.Read(value);
            std::vector<SHandlingData> comps;

            for (uint8_t i = 0; i < value; i++)
            {
                uint8_t id;
                float fvalue;

                bs.Read(id);
                bs.Read(fvalue);

                comps.push_back(SHandlingData(id, fvalue, 0));
                Log(OBFUSCATE("Pushed %d %f"), id, fvalue);
            }

            if (CVehiclePool::GetAt(veh))
                CVehiclePool::GetAt(veh)->SetHandlingData(comps);
            break;
        }

        case RPC_CUSTOM_COMPONENT:
        {
            uint16_t veh, extra_comp;
            uint8_t comp;
            bs.Read(veh);
            CVehicle* pVehicle = CVehiclePool::GetAt(veh);

            if (!pVehicle)
                return;

            for (int i = 0; i < E_CUSTOM_COMPONENTS::ccMax; i++)
            {
                if (i == E_CUSTOM_COMPONENTS::ccExtra)
                {
                    bs.Read(extra_comp);
                    pVehicle->SetComponentVisible(i, (uint16_t)extra_comp);
                }
                else
                {
                    bs.Read(comp);
                    pVehicle->SetComponentVisible(i, (uint16_t)comp);
                }
            }
            break;
        }

        case CUSTOM_RPC_TOGGLE_HUD_ELEMENT:
        {
            uint32_t hud, toggle;
            bs.Read(hud);
            bs.Read(toggle);

            pGame->ToggleHUDElement(hud, toggle);
            pGame->HandleChangedHUDStatus();
            break;
        }
        case RPC_STREAM_CREATE:
        {
            char str[255];

            uint8_t len;
            uint16_t id, vw, interior;

            CVector pos;

            float fDistance;
            bs.Read(id);
            bs.Read(pos.x);
            bs.Read(pos.y);
            bs.Read(pos.z);
            bs.Read(fDistance);
            bs.Read(vw);
            bs.Read(interior);
            bs.Read(len);
            bs.Read(&str[0], len);
            str[len] = 0;

            GetStreamPool()->AddStream(id, &pos, vw, interior, fDistance, (const char*)&str[0]);
            break;
        }

        case RPC_STREAM_INDIVIDUAL:
        {
            char str[255];
            uint8_t len;

            bs.Read(len);
            bs.Read(&str[0], len);
            str[len] = 0;

            GetStreamPool()->PlayIndividualStream(&str[0]);
            break;
        }

        case RPC_STREAM_POS:
        {
            break;
        }

        case RPC_STREAM_DESTROY:
        {
            uint32_t id;
            bs.Read(id);

            m_pStreamPool->DeleteStreamByID(id);
            break;
        }

        case RPC_STREAM_VOLUME:
        {
            uint16_t id;
            float fVolume;
            bs.Read(id);
            bs.Read(fVolume);

            m_pStreamPool->SetStreamVolume(id, fVolume);
            break;
        }

        case RPC_STREAM_ISENABLED:
        {
            uint32_t isEnabled;
            bs.Read(isEnabled);
            if (isEnabled)
                m_pStreamPool->Activate();
            else if (isEnabled == 0)
                m_pStreamPool->Deactivate();

            break;
        }
        case RPC_OPEN_LINK:
        {
            uint16_t len;
            bs.Read(len);

            if (len >= 0xFA)
                break;

            char link[0xFF];
            memset(&link[0], 0, 0xFF);
            bs.Read((char*)& link, len);

            AND_OpenLink(&link[0]);

            break;
        }

        case RPC_TIMEOUT_CHAT:
        {
            uint32_t timeoutStart = 0;
            uint32_t timeoutEnd = 0;

            bs.Read(timeoutStart);
            bs.Read(timeoutEnd);

            if (pChatWindow)
                pChatWindow->SetChatDissappearTimeout(timeoutStart, timeoutEnd);

            break;
        }

        case RPC_CUSTOM_AIM:
        {
            uint8_t szLen;
            char szName[32];

            memset(szName, 0, sizeof(szName));

            bs.Read(szLen);

            if (szLen >= sizeof(szName) + 1)
                return;

            bs.Read(szName, szLen);
            pCrossHair->ChangeAim(szName);
            break;
        }
    }
}

void CNetGame::ResetVehiclePool()
{
    Log(OBFUSCATE("ResetVehiclePool"));
    CVehiclePool::Free();
}

void CNetGame::ResetObjectPool()
{
    Log(OBFUSCATE("ResetObjectPool"));
    CObjectPool::Free();
}

void CNetGame::ResetNotificationPool()
{
    Log(OBFUSCATE("ResetNotificationPool"));
    SAFE_DELETE(m_pNotificationPool);

    m_pNotificationPool = new NotificationPool();
}

void CNetGame::ResetPickupPool()
{
    Log(OBFUSCATE("ResetPickupPool"));
    CPickupPool::Free();
}

void CNetGame::ResetGangZonePool()
{
    Log(OBFUSCATE("ResetGangZonePool"));
    m_pGangZonePool = new CGangZonePool();
}

void CNetGame::ResetLabelPool()
{
    Log(OBFUSCATE("ResetLabelPool"));
    CText3DLabelsPool::Free();
}

void CNetGame::ResetActorPool()
{
    Log(OBFUSCATE("ResetActorPool"));
    CActorPool::Free();
}

void CNetGame::ResetTextDrawPool()
{
    Log(OBFUSCATE("ResetTextDrawPool"));
    if (m_pTextDrawPool)
        delete m_pTextDrawPool;

    m_pTextDrawPool = new CTextDrawPool();
}

extern int RemoveModelIDs[1200];
extern CVector RemovePos[1200];
extern float RemoveRad[1200];
extern int iTotalRemovedObjects;

void CNetGame::ShutDownForGameRestart()
{
    iTotalRemovedObjects = 0;
    for (int & RemoveModelID : RemoveModelIDs)
        RemoveModelID = -1;

    for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
    {
        CRemotePlayer* pPlayer = CPlayerPool::GetAt(playerId);
        if(pPlayer)
        {
            //pPlayer->SetTeam(NO_TEAM);
            //pPlayer->ResetAllSyncAttributes();
        }
    }

    CLocalPlayer *pLocalPlayer = CPlayerPool::GetLocalPlayer();
    if(pLocalPlayer)
    {
        pLocalPlayer->ResetAttachedObjects();
        pLocalPlayer->ResetAllSyncAttributes();
        pLocalPlayer->ToggleSpectating(false);
    }

    CObjectPool::ResetColoredObject();

    m_iGameState = GAMESTATE_RESTARTING;

    //CPlayerPool::DeactivateAll();
    CPlayerPool::Free();

    ResetVehiclePool();
    ResetObjectPool();
    ResetPickupPool();
    ResetGangZonePool();
    ResetLabelPool();
    ResetTextDrawPool();
    ResetActorPool();
    ResetNotificationPool();

    m_bDisableEnterExits = false;
    m_fNameTagDrawDistance = 60.0f;
    m_byteWorldTime = 12;
    m_byteWorldMinute = 0;
    m_byteWeather = 1;
    m_bHoldTime = true;
    m_bNameTagLOS = true;
    m_bUseCJWalk = false;
    m_fGravity = 0.008f;
    m_iDeathDropMoney = 0;

    CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
    if(pPlayerPed)
    {
        pPlayerPed->SetInterior(0);
        //pPlayerPed->SetDead();
        pPlayerPed->SetArmour(0.0f);
    }

    pGame->ToggleCJWalk(false);
    pGame->ResetLocalMoney();
    pGame->EnableZoneNames(false);
    m_bZoneNames = false;
    GameResetRadarColors();
    pGame->SetGravity(m_fGravity);
    pGame->EnableClock(false);
}

void CNetGame::SendChatMessage(const char* szMsg)
{
    if (GetGameState() != GAMESTATE_CONNECTED) return;

    RakNet::BitStream bsSend;
    uint8_t byteTextLen = strlen(szMsg);

    bsSend.Write(byteTextLen);
    bsSend.Write(szMsg, byteTextLen);

    m_pRakClient->RPC(&RPC_Chat,&bsSend,HIGH_PRIORITY,RELIABLE,0,false, UNASSIGNED_NETWORK_ID, NULL);
}


void CNetGame::SendRPCUID(const char* szMsg)
{
    RakNet::BitStream bsSendHash;
    uintptr_t strLen = strlen(szMsg);
    bsSendHash.Write(strLen);
    bsSendHash.Write(szMsg, strLen);
    m_pRakClient->RPC(&RPC_CustomUID, &bsSendHash, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
void CNetGame::SendRPCUIP(const char* szMsg)
{
    RakNet::BitStream bsSendHash;
    uintptr_t strLen = strlen(szMsg);
    bsSendHash.Write(strLen);
    bsSendHash.Write(szMsg, strLen);
    m_pRakClient->RPC(&RPC_CustomUIP, &bsSendHash, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    g_pJavaWrapper->CallLauncherActivity(101);
}


void CNetGame::SendChatCommand(const char* szCommand)
{
    if (GetGameState() != GAMESTATE_CONNECTED)
        return;

    RakNet::BitStream bsParams;
    int iStrlen = strlen(szCommand);

    bsParams.Write(iStrlen);
    bsParams.Write(szCommand, iStrlen);
    m_pRakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

extern int greenzone;
extern CSettings* pSettings;
void CNetGame::SendDialogResponse(int16_t wDialogID, uint8_t byteButtonID, int16_t wListBoxItem, char* szInput)
{
    if(showHud)
    {
        if(pSettings->GetReadOnly().iNewHud)
            g_pJavaWrapper->ShowHUD(true);
        g_pJavaWrapper->CallLauncherActivity(1237);
        g_pJavaWrapper->CallLauncherActivity(1334);
        if(greenzone = 1)
        {
            g_pJavaWrapper->CallLauncherActivity(1332);
        }
    }

    uint8_t respLen = strlen(szInput);

    RakNet::BitStream bsSend;
    bsSend.Write(wDialogID);
    bsSend.Write(byteButtonID);
    bsSend.Write(wListBoxItem);
    bsSend.Write(respLen);
    bsSend.Write((char*)szInput, (uint32_t)respLen);
    m_pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, int iColor, int style)
{
    if(byteIndex >= 100)
        return;

    if(m_dwMapIcons[byteIndex]) DisableMapIcon(byteIndex);

    m_dwMapIcons[byteIndex] = pGame->CreateRadarMarkerIcon(byteIcon, fX, fY, fZ, iColor, style);
}

void CNetGame::DisableMapIcon(uint8_t byteIndex)
{
    if(byteIndex >= 100)
        return;

    ScriptCommand(&disable_marker, m_dwMapIcons[byteIndex]);
    m_dwMapIcons[byteIndex] = 0;
}

void CNetGame::UpdatePlayerScoresAndPings()
{
    static uint32_t dwLastUpdateTick = 0;

    if ((GetTickCount() - dwLastUpdateTick) >= 3000)
    {
        dwLastUpdateTick = GetTickCount();
        RakNet::BitStream bsParams;
        m_pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
    }
}

void gen_auth_key(char buf[260], char* auth_in);
void CNetGame::Packet_AuthKey(Packet* pkt)
{
    RakNet::BitStream bsAuth((unsigned char *)pkt->data, pkt->length, false);

    uint8_t byteAuthLen;
    char szAuth[260];

    bsAuth.IgnoreBits(8);
    bsAuth.Read(byteAuthLen);
    bsAuth.Read(szAuth, byteAuthLen);
    szAuth[byteAuthLen] = '\0';

    char szAuthKey[260];
    gen_auth_key(szAuthKey, szAuth);

    RakNet::BitStream bsKey;
    auto byteAuthKeyLen = (uint8_t)strlen(szAuthKey);

    bsKey.Write((uint8_t)ID_AUTH_KEY);
    bsKey.Write((uint8_t)byteAuthKeyLen);
    bsKey.Write(szAuthKey, byteAuthKeyLen);
    m_pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);

    Log(OBFUSCATE("[AUTH] %s -> %s"), szAuth, szAuthKey);
}

void CNetGame::Packet_DisconnectionNotification(Packet* pkt)
{
    if(pChatWindow)
        pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMsg(E_MSG::SERVER_CLOSED_CONNECTION));

    if(g_pJavaWrapper)
        g_pJavaWrapper->SetLoadingText(4);

    m_pRakClient->Disconnect(2000);

    if(pVoice)
        pVoice->FullDisconnect();
}

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
    if(m_pRakClient) m_pRakClient->Disconnect(0);

    if(pChatWindow)
        pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMsg(E_MSG::CONNECTION_LOST));

    if(pVoice)
        pVoice->Disconnect();

    ShutDownForGameRestart();

    for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
    {
        CRemotePlayer *pPlayer = CPlayerPool::GetAt(playerId);
        if(pPlayer) CPlayerPool::Delete(playerId, 0);
    }

    SetGameState(GAMESTATE_WAIT_CONNECT);
}
//#define SUM_MAS_ENCR	10
//int g_sumMas[SUM_MAS_ENCR] = { 290, 291, 417, 424, 477, 54+38+142+49, 51+91+91+84, 54+38+142+50, 54 + 38 + 142 + 51, 51 + 77 + 238 + 92 };

#include "..//CServerManager.h"

bool g_isValidSum(int a)
{
    for (const auto & g_sEncryptedAddresse : g_sEncryptedAddresses)
        if (g_sEncryptedAddresse.getSum() == a) return true;

    return false;
}
void WriteVerified1();
void CNetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
    if(pChatWindow)
        pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMsg(E_MSG::CONNECTED));
    SetGameState(GAMESTATE_AWAIT_JOIN);

    RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
    PLAYERID MyPlayerID;
    unsigned int uiChallenge;

    bsSuccAuth.IgnoreBits(8); // ID_CONNECTION_REQUEST_ACCEPTED
    bsSuccAuth.IgnoreBits(32); // binaryAddress
    bsSuccAuth.IgnoreBits(16); // port
    bsSuccAuth.Read(MyPlayerID);
    bsSuccAuth.Read(uiChallenge);

    char ip[0x7F];
    strncpy(ip, m_szHostOrIp, sizeof(ip));

    std::vector<std::string> strings;
    std::istringstream f((const char*)&ip[0]);
    std::string s;
    int sum = 0;
    while (getline(f, s, '.'))
        sum += std::atoi(s.c_str());

    if (g_isValidSum(sum))
        WriteVerified1();

    CPlayerPool::SetLocalPlayerID(MyPlayerID);

    int iVersion = NETGAME_VERSION;
    char byteMod = 0x01;
    unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

    char byteAuthBSLen = (char)strlen(AUTH_BS);
    char byteNameLen = (char)strlen(CPlayerPool::GetLocalPlayerName());
    char byteClientverLen = (char)strlen(SAMP_VERSION);

    RakNet::BitStream bsSend;
    bsSend.Write(iVersion);
    bsSend.Write(byteMod);
    bsSend.Write(byteNameLen);
    bsSend.Write(CPlayerPool::GetLocalPlayerName(), byteNameLen);
    bsSend.Write(uiClientChallengeResponse);
    bsSend.Write(byteAuthBSLen);
    bsSend.Write(AUTH_BS, byteAuthBSLen);
    bsSend.Write(byteClientverLen);
    bsSend.Write(SAMP_VERSION, byteClientverLen);

    CClientInfo::WriteClientInfoToBitStream(bsSend);

    m_pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);

    // Custom Packet
    RakNet::BitStream bsParams;
    iVersion = 432634;

    bsParams.Write(iVersion);
    m_pRakClient->RPC(&RPC_CustomJoin, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);

}


void CNetGame::Packet_PlayerSync(Packet* pkt)
{
    CRemotePlayer * pPlayer;
    RakNet::BitStream bsPlayerSync((unsigned char *)pkt->data, pkt->length, false);
    ONFOOT_SYNC_DATA ofSync;
    uint8_t bytePacketID=0;
    PLAYERID playerId;

    bool bHasLR,bHasUD;
    bool bHasVehicleSurfingInfo;

    if(GetGameState() != GAMESTATE_CONNECTED) return;

    memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));

    bsPlayerSync.Read(bytePacketID);
    bsPlayerSync.Read(playerId);

    // LEFT/RIGHT KEYS
    bsPlayerSync.Read(bHasLR);
    if(bHasLR) bsPlayerSync.Read(ofSync.lrAnalog);

    // UP/DOWN KEYS
    bsPlayerSync.Read(bHasUD);
    if(bHasUD) bsPlayerSync.Read(ofSync.udAnalog);

    // GENERAL KEYS
    bsPlayerSync.Read(ofSync.wKeys);

    // CVector POS
    bsPlayerSync.Read((char*)&ofSync.vecPos,sizeof(CVector));

    // QUATERNION
    float tw, tx, ty, tz;
    bsPlayerSync.ReadNormQuat(tw, tx, ty, tz);
    ofSync.quat.w = tw;
    ofSync.quat.x = tx;
    ofSync.quat.y = ty;
    ofSync.quat.z = tz;

    // HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
    uint8_t byteHealthArmour;
    uint8_t byteArmTemp=0,byteHlTemp=0;

    bsPlayerSync.Read(byteHealthArmour);
    byteArmTemp = (byteHealthArmour & 0x0F);
    byteHlTemp = (byteHealthArmour >> 4);

    if(byteArmTemp == 0xF) ofSync.byteArmour = 100;
    else if(byteArmTemp == 0) ofSync.byteArmour = 0;
    else ofSync.byteArmour = byteArmTemp * 7;

    if(byteHlTemp == 0xF) ofSync.byteHealth = 100;
    else if(byteHlTemp == 0) ofSync.byteHealth = 0;
    else ofSync.byteHealth = byteHlTemp * 7;

    // CURRENT WEAPON
    bsPlayerSync.Read(ofSync.byteCurrentWeapon);
    // SPECIAL ACTION
    bsPlayerSync.Read(ofSync.byteSpecialAction);

    // READ MOVESPEED CVectorS
    bsPlayerSync.ReadVector(tx, ty, tz);
    ofSync.vecMoveSpeed.x = tx;
    ofSync.vecMoveSpeed.y = ty;
    ofSync.vecMoveSpeed.z = tz;

    bsPlayerSync.Read(bHasVehicleSurfingInfo);
    if (bHasVehicleSurfingInfo)
    {
        bsPlayerSync.Read(ofSync.wSurfInfo);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.x);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.y);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.z);
    }
    else
        ofSync.wSurfInfo = INVALID_VEHICLE_ID;

    bool bHasAnimInfo;
    bsPlayerSync.Read(bHasAnimInfo);

    if (bHasAnimInfo)
        bsPlayerSync.Read(ofSync.dwAnimation);
    else ofSync.dwAnimation = 0b10000000000000000000000000000000;

    uint8_t key = 0;

    pPlayer = CPlayerPool::GetAt(playerId);
    if(pPlayer)
        pPlayer->StoreOnFootFullSyncData(&ofSync, 0, key);
}

void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    CRemotePlayer *pPlayer;
    RakNet::BitStream bsSync((unsigned char *)pkt->data, pkt->length, false);
    uint8_t bytePacketID = 0;
    PLAYERID playerId;
    INCAR_SYNC_DATA icSync;

    if(GetGameState() != GAMESTATE_CONNECTED) return;

    memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

    bsSync.Read(bytePacketID);
    bsSync.Read(playerId);
    bsSync.Read(icSync.VehicleID);

    // keys
    bsSync.Read(icSync.lrAnalog);
    bsSync.Read(icSync.udAnalog);
    bsSync.Read(icSync.wKeys);

    // quaternion
    bsSync.ReadNormQuat(icSync.quat.w, icSync.quat.x, icSync.quat.y, icSync.quat.z);

    // position
    bsSync.Read((char*)&icSync.vecPos, sizeof(CVector));

    // speed
    bsSync.ReadVector(icSync.vecMoveSpeed.x, icSync.vecMoveSpeed.y, icSync.vecMoveSpeed.z);

    // vehicle health
    uint16_t wTempVehicleHealth;
    bsSync.Read(wTempVehicleHealth);
    icSync.fCarHealth = (float)wTempVehicleHealth;

    // health/armour
    uint8_t byteHealthArmour;
    uint8_t byteArmTemp=0, byteHlTemp=0;

    bsSync.Read(byteHealthArmour);
    byteArmTemp = (byteHealthArmour & 0x0F);
    byteHlTemp = (byteHealthArmour >> 4);

    if(byteArmTemp == 0xF) icSync.bytePlayerArmour = 100;
    else if(byteArmTemp == 0) icSync.bytePlayerArmour = 0;
    else icSync.bytePlayerArmour = byteArmTemp * 7;

    if(byteHlTemp == 0xF) icSync.bytePlayerHealth = 100;
    else if(byteHlTemp == 0) icSync.bytePlayerHealth = 0;
    else icSync.bytePlayerHealth = byteHlTemp * 7;

    // CURRENT WEAPON
    uint8_t byteTempWeapon;
    bsSync.Read(byteTempWeapon);
    icSync.byteCurrentWeapon ^= (byteTempWeapon ^ icSync.byteCurrentWeapon) & 0x3F;

    bool bCheck;

    // siren
    bsSync.Read(bCheck);
    if(bCheck) icSync.byteSirenOn = 1;
    // landinggear
    bsSync.Read(bCheck);
    if(bCheck) icSync.byteLandingGearState = 1;
    // train speed
    bsSync.Read(bCheck);
    if(bCheck) bsSync.Read(icSync.fTrainSpeed);
    // triler id
    bsSync.Read(bCheck);
    if(bCheck) bsSync.Read(icSync.TrailerID);

    pPlayer = CPlayerPool::GetAt(playerId);
    if(pPlayer)
        pPlayer->StoreInCarFullSyncData(&icSync, 0);
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
    CRemotePlayer *pPlayer;
    uint8_t bytePacketID;
    PLAYERID playerId;
    PASSENGER_SYNC_DATA psSync;

    if(GetGameState() != GAMESTATE_CONNECTED)
        return;

    RakNet::BitStream bsPassengerSync((unsigned char *)pkt->data, pkt->length, false);
    bsPassengerSync.Read(bytePacketID);
    bsPassengerSync.Read(playerId);
    bsPassengerSync.Read((char*)&psSync, sizeof(PASSENGER_SYNC_DATA));

    pPlayer = CPlayerPool::GetAt(playerId);
    if(pPlayer)
        pPlayer->StorePassengerFullSyncData(&psSync);
}

void CNetGame::Packet_MarkersSync(Packet *pkt)
{
    CRemotePlayer *pPlayer;
    int			iNumberOfPlayers = 0;
    PLAYERID	playerId;
    short		sPos[3];
    bool		bIsPlayerActive;
    uint8_t 	unk0 = 0;

    if(GetGameState() != GAMESTATE_CONNECTED)
        return;

    RakNet::BitStream bsMarkersSync((unsigned char *)pkt->data, pkt->length, false);
    bsMarkersSync.Read(unk0);
    bsMarkersSync.Read(iNumberOfPlayers);

    if(iNumberOfPlayers)
    {
        for(int i=0; i<iNumberOfPlayers; i++)
        {
            bsMarkersSync.Read(playerId);
            bsMarkersSync.ReadCompressed(bIsPlayerActive);

            if(bIsPlayerActive)
            {
                bsMarkersSync.Read(sPos[0]);
                bsMarkersSync.Read(sPos[1]);
                bsMarkersSync.Read(sPos[2]);
            }

            if(playerId < MAX_PLAYERS && CPlayerPool::GetAt(playerId))
            {
                pPlayer = CPlayerPool::GetAt(playerId);
                if(pPlayer)
                {
                    if(bIsPlayerActive)
                        pPlayer->ShowGlobalMarker(sPos[0], sPos[1], sPos[2]);
                    else pPlayer->HideGlobalMarker();
                }
            }
        }
    }
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
    uint8_t bytePacketID;
    uint16_t PlayerID;
    BULLET_SYNC_DATA btSync;
    RakNet::BitStream  bsBulletSync((unsigned char*)pkt->data, pkt->length, false);

    if (GetGameState() != GAMESTATE_CONNECTED)
        return;

    bsBulletSync.Read(bytePacketID);
    bsBulletSync.Read(PlayerID);
    bsBulletSync.Read((char*)&btSync, sizeof(BULLET_SYNC_DATA));

    CRemotePlayer* pRemotePlayer = CPlayerPool::GetAt(PlayerID);
    if (pRemotePlayer)
        pRemotePlayer->StoreBulletSyncData(&btSync);
}

void CNetGame::Packet_AimSync(Packet * p)
{
    CRemotePlayer * pPlayer;
    RakNet::BitStream bsAimSync((unsigned char*)p->data, p->length, false);
    AIM_SYNC_DATA aimSync;
    uint8_t bytePacketID = 0;
    uint16_t bytePlayerID = 0;

    if (GetGameState() != GAMESTATE_CONNECTED)
        return;

    bsAimSync.Read(bytePacketID);
    bsAimSync.Read(bytePlayerID);
    bsAimSync.Read((char*)&aimSync, sizeof(AIM_SYNC_DATA));

    pPlayer = CPlayerPool::GetAt(bytePlayerID);

    if (pPlayer)
        pPlayer->UpdateAimFromSyncData(&aimSync);
}
