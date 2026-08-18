#include "../main.h"
#include "../game/game.h"
#include "..//util/CJavaWrapper.h"
#include "netgame.h"
#include "java_systems/CHUD.h"
#include "CSettings.h"
#include <thread>
#include <chrono>
#include "badges.h"
#include "Statistics.h"
#include "game/vehicle_utils.h"


extern CWidgetManager* g_pWidgetManager;
extern CNetGame *pNetGame;

extern "C" void BB_OnRpcShowTabletBoombox();

#include "../chatwindow.h"

#include "..//CLocalisation.h"
#include "../vendor/hash/sha256.h"
#include "RemoveBuildings.h"
#include "java_systems/CTabletMusic.h"   // ����� �������� �������
#include "playerpool.h"
#include "localplayer.h"
#include "CPassportOverlay.h"

#include <string>

// cp1251 -> UTF-8
static inline void utf8_append(std::string& out, unsigned int cp) {
    if (cp <= 0x7F) out.push_back((char)cp);
    else if (cp <= 0x7FF) { out.push_back((char)(0xC0 | ((cp >> 6) & 0x1F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
    else if (cp <= 0xFFFF){ out.push_back((char)(0xE0 | ((cp >> 12) & 0x0F))); out.push_back((char)(0x80 | ((cp >> 6) & 0x3F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
    else { out.push_back((char)(0xF0 | ((cp >> 18) & 0x07))); out.push_back((char)(0x80 | ((cp >> 12) & 0x3F))); out.push_back((char)(0x80 | ((cp >> 6) & 0x3F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
}

static std::string cp1251_to_utf8(const char* s) {
    std::string out;
    for (const unsigned char* p = (const unsigned char*)s; *p; ++p) {
        unsigned char c = *p;
        if (c < 128) { out.push_back((char)c); continue; }
        unsigned int cp = 0;
        if (c >= 0xC0 && c <= 0xFF) cp = 0x0410 + (c - 0xC0);       // �..�, �..�
        else if (c == 0xA8) cp = 0x0401;                             // �
        else if (c == 0xB8) cp = 0x0451;                             // �
        else { // ������ ����� cp1251 ? ������ �� ����� �����
            static const unsigned short map[32] = {
                    0x0402,0x0403,0x201A,0x0453,0x201E,0x2026,0x2020,0x2021,
                    0x20AC,0x2030,0x0409,0x2039,0x040A,0x040C,0x040B,0x040F,
                    0x0452,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
                    0x0098,0x2122,0x0459,0x203A,0x045A,0x045C,0x045B,0x045F
            };
            if (c >= 0x80 && c <= 0x9F) cp = map[c - 0x80];
            else { out.push_back((char)c); continue; } // ����� ������ ��� ����
        }
        utf8_append(out, cp);
    }
    return out;
}

// LP-������: uint16 ����� + len ����
static bool ReadLP(RakNet::BitStream& bs, char* out, int outSize)
{
    uint16_t len = 0;
    if (!bs.Read(len)) { if (outSize>0) out[0]='\0'; return false; }
    if (outSize <= 0) return false;
    if (len >= (uint16_t)outSize) len = (uint16_t)(outSize - 1);
    for (uint16_t i=0;i<len;++i){ unsigned char ch=0; if(!bs.Read(ch)){ out[0]='\0'; return false; } out[i]=(char)ch; }
    out[len]='\0'; return true;
}
// Универсалка: LP-строка (uint16 len + len байт) -> char* (0-terminated)
static bool ReadLPString_Generic(RakNet::BitStream& bs, char* outBuf, size_t outCap)
{
    if(outCap == 0) return false;
    unsigned short len = 0;
    if(!bs.Read(len)) { outBuf[0] = 0; return false; }
    size_t toCopy = (len < outCap-1) ? len : (outCap-1);
    for(size_t i=0;i<toCopy;i++){
        unsigned char ch=0; if(!bs.Read(ch)) { outBuf[0]=0; return false; }
        outBuf[i] = (char)ch;
    }
    outBuf[toCopy] = 0;
    // дочитываем остаток если был
    for(size_t i=toCopy;i<len;i++){ unsigned char dummy=0; if(!bs.Read(dummy)) { return false; } }
    return true;
}


// === BOOMBOX RPC (client<->server) ==================================
#ifndef RPC_BB_SHOW_TABLET
#define RPC_BB_SHOW_TABLET 241  // server -> client: �������� ������� (����� �������)
#endif
#ifndef RPC_BB_SELECT
#define RPC_BB_SELECT      242  // client -> server: ������ ���� (URL)
#endif
#ifndef RPC_BB_STOP
#define RPC_BB_STOP        243  // client -> server: ���� ��������
#endif


#define NETGAME_VERSION 4057
#define AUTH_BS "E02262CF28BC542486C558D4BE9EFB716592AFAF8B"
//#define AUTH_BS "1528354F18550C00AB504591304D0379BB0ACA99043"

extern CGame *pGame;

int iVehiclePoolProcessFlag = 0;
int iPickupPoolProcessFlag = 0;

void RegisterRPCs(RakClientInterface* pRakClient);
void UnRegisterRPCs(RakClientInterface* pRakClient);
void RegisterScriptRPCs(RakClientInterface* pRakClient);
void UnRegisterScriptRPCs(RakClientInterface* pRakClient);

unsigned char GetPacketID(Packet *p)
{
	if(p == 0) return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char) p->data[0];
}

class string_encryptor;

CNetGame::CNetGame(const char* szHostOrIp, int iPort, const char* szPlayerName, const char* szPass)
{
	strcpy(m_szHostName, "San Andreas Multiplayer");
	strncpy(m_szHostOrIp, szHostOrIp, sizeof(m_szHostOrIp));
	m_iPort = iPort;

	m_pPlayerPool = new CPlayerPool();
	m_pPlayerPool->SetLocalPlayerName(szPlayerName);
//
	m_pVehiclePool = new CVehiclePool();
	m_pObjectPool = new CObjectPool();
	m_pPickupPool = new CPickupPool();
	m_pGangZonePool = new CGangZonePool();
	m_pLabelPool = new CText3DLabelsPool();
//
//	m_pTextDrawPool = new CTextDrawPool();
	g_pWidgetManager = new CWidgetManager();
	m_pStreamPool = new CStreamPool();
	m_pActorPool = new CActorPool();

	m_pRakClient = RakNetworkFactory::GetRakClientInterface();
	RegisterRPCs(m_pRakClient);
	RegisterScriptRPCs(m_pRakClient);
	// key

	m_pRakClient->SetPassword(szPass);

	m_dwLastConnectAttempt = GetTickCount();
	m_iGameState = 	GAMESTATE_WAIT_CONNECT;

	m_GreenZoneState = false;
	m_iSpawnsAvailable = 0;
	m_byteWorldMinute = 0;
	m_byteWorldTime = 12;
	m_byteWeather =	0;
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
    CVehicle::Init();
    Badges_Init();
}

CNetGame::~CNetGame()
{
	m_pRakClient->Disconnect(0);
	UnRegisterRPCs(m_pRakClient);
	UnRegisterScriptRPCs(m_pRakClient);
	RakNetworkFactory::DestroyRakClientInterface(m_pRakClient);
	m_pRakClient = nullptr;

	if(m_pPlayerPool) 
	{
		delete m_pPlayerPool;
		m_pPlayerPool = nullptr;
	}

	if(m_pVehiclePool)
	{
		delete m_pVehiclePool;
		m_pVehiclePool = nullptr;
	}

	if(m_pPickupPool)
	{
		delete m_pPickupPool;
		m_pPickupPool = nullptr;
	}

	if(m_pGangZonePool)
	{
		delete m_pGangZonePool;
		m_pGangZonePool = nullptr;
	}

	if(m_pLabelPool)
	{
		delete m_pLabelPool;
		m_pLabelPool = nullptr;
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

	CRemoveBuildings::clearList();

                  if(!pGame->bIsGameExiting) g_pJavaWrapper->ClearScreen();

}

#include "CUDPSocket.h"
#include "..//CServerManager.h"
#include "java_systems/CSpeedometr.h"
#include "voice/Network.h"
#include "CJavaGui.h"
// �������� ������� ���������� ������ ����� ������� ���� (��� � ���� �����-�)
static bool GetLocalPlayerPosByMatrix(CNetGame* ng, float& x, float& y, float& z)
{
    if (!ng) return false;
    CPlayerPool* pPool = ng->GetPlayerPool();
    if (!pPool) return false;

    CLocalPlayer* pLocal = pPool->GetLocalPlayer();
    if (!pLocal) return false;

    CPlayerPed* pPed = pLocal->GetPlayerPed();
    if (!pPed) return false;

    RwMatrix mat{};
    pPed->GetMatrix(&mat);    // ��� �� �����, ��� �� ���� ������
    x = mat.pos.x;
    y = mat.pos.y;
    z = mat.pos.z;
    return true;
}

int last_process_cnetgame = 0;
void CNetGame::Process() {

    // 30 fps
    if (GetTickCount() - last_process_cnetgame >= 1000 / 30) {
        last_process_cnetgame = GetTickCount();
    } else {
        return;
    }
    //CSkyBox::Process();

    CSpeedometr::update();

    UpdateNetwork();

    // server checkpoints update
    if (pGame->m_bCheckpointsEnabled) {
        CPlayerPed *pPlayerDed = m_pPlayerPool->GetLocalPlayer()->GetPlayerPed();
        if (pPlayerDed) {
            ScriptCommand(&is_actor_near_point_3d, pPlayerDed->m_dwGTAId,
                          pGame->m_vecCheckpointPos.x,
                          pGame->m_vecCheckpointPos.y,
                          pGame->m_vecCheckpointPos.z,
                          pGame->m_vecCheckpointExtent.x,
                          pGame->m_vecCheckpointExtent.y,
                          pGame->m_vecCheckpointExtent.z,
                          1);
        }
    }

    if (GetGameState() == GAMESTATE_CONNECTED) {
        // pool process
        if (m_pPlayerPool) m_pPlayerPool->Process();
        if (m_pObjectPool) m_pObjectPool->Process();
        if (m_pVehiclePool) m_pVehiclePool->Process();
        if (m_pPickupPool) m_pPickupPool->Process();

    } else {
        CPlayerPed *pPlayer = pGame->FindPlayerPed();
        CCamera *pCamera = pGame->GetCamera();

        // ?????? ??? ???????????

        if (pPlayer && pCamera) {
            if (pPlayer->IsInVehicle())
                pPlayer->RemoveFromVehicleAndPutAt(314.0f, 160.0f, 39.0f);
            else
                pPlayer->TeleportTo(314.0f, 160.0f, 39.0f);

            pCamera->SetPosition(429.0f, 240.0f, 12.0f, 0.0f, 0.0f, 0.0f);
            pCamera->LookAtPoint(429.0f, 240.0f, 12.0f, 2);
            CHUD::toggleAll(false);
            pGame->SetWorldWeather(m_byteWeather);
        }
    }

    if (GetGameState() == GAMESTATE_WAIT_CONNECT &&
        (GetTickCount() - m_dwLastConnectAttempt) > 3000) {
        CChatWindow::AddDebugMessageNonFormatted("{bbbbbb}���������� � VOLYA{ffffff}");

        m_pRakClient->Connect(m_szHostOrIp, m_iPort, 0, 0, 5);
        m_dwLastConnectAttempt = GetTickCount();
        SetGameState(GAMESTATE_CONNECTING);
        Network::OnRaknetConnect(m_szHostOrIp, 36813);
//        Log("%s", m_szHostOrIp);
    }

    }


void CNetGame::UpdateNetwork()
{
	Packet* pkt;
	unsigned char packetIdentifier;

	while(pkt = m_pRakClient->Receive())
	{
		packetIdentifier = GetPacketID(pkt);

		switch(packetIdentifier)
		{
			case ID_AUTH_KEY:
				Log("Incoming packet: ID_AUTH_KEY");
				packetAuthKey(pkt);
				break;

			case ID_CONNECTION_ATTEMPT_FAILED:
				CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTION_ATTEMPT_FAILED));
				SetGameState(GAMESTATE_WAIT_CONNECT);
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::FULL_SERVER));
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
				CChatWindow::AddDebugMessage("Failed to initialize encryption.");
				break;

			case ID_CONNECTION_BANNED:
				CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::BANNED));
				break;

			case ID_INVALID_PASSWORD:
				CChatWindow::AddDebugMessage("�� ������ ������!");
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
            case 222:
                Network::OnRaknetReceive(*pkt);
                break;
			case ID_CUSTOM_RPC:
				Packet_CustomRPC(pkt);
				break;

			case PACKET_SPECIALCUSTOM:
				Packet_SpecialCustomRPC(pkt);
				break;
            case 254:
                CJavaGui::ReceivePacket(pkt);
                break;
		}

		m_pRakClient->DeallocatePacket(pkt);
	}
}

void CNetGame::Packet_TrailerSync(Packet* p)
{
	CRemotePlayer* pPlayer;
	RakNet::BitStream bsSpectatorSync((unsigned char*)p->data, p->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	BYTE bytePacketID = 0;
	BYTE bytePlayerID = 0;
	TRAILER_SYNC_DATA trSync;

	bsSpectatorSync.Read(bytePacketID);
	bsSpectatorSync.Read(bytePlayerID);
	bsSpectatorSync.Read((char*)& trSync, sizeof(TRAILER_SYNC_DATA));

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)
	{
		pPlayer->StoreTrailerFullSyncData(&trSync);
	}
}

//#include "..//game/CCustomPlateManager.h"
#include "java_systems/CDuelsGui.h"
#include "java_systems/CChooseSpawn.h"
#include "voice/SpeakerList.h"
#include "voice/MicroIcon.h"
#include "java_systems/CAuthrization.h"
#include "java_systems/CRegistration.h"
#include "CMvdIdOverlay.h"
#include "CFamilyMenu.h"
#include "CFamilyAction.h"

void CNetGame::Packet_SpecialCustomRPC(Packet *p)
{
    RakNet::BitStream bs((unsigned char *)p->data, p->length, false);
    uint8_t  packetID;
    uint32_t rpcID;
    bs.Read(packetID);
    bs.Read(rpcID);

    switch (rpcID)
    {
        /* ---------- Авторизация ---------- */
        case 1:
        {
            bool toggle, isEmail, isAutoAuth;
            bs.Read(toggle);
            bs.Read(isEmail);
            bs.Read(isAutoAuth);

            // читаем skinId, если сервер его отправил
            uint8_t skinId = 0;
            if (bs.GetNumberOfUnreadBits() >= 8)
                bs.Read(skinId);

            Log("AuthGUI toggle=%d email=%d auto=%d skin=%d",
                toggle, isEmail, isAutoAuth, (int)skinId);

            if (CSettings::m_Settings.szAutoLogin && isAutoAuth)
                SendLoginPacket(CSettings::m_Settings.player_password);

            if (toggle)
                CAuthrization::show(isEmail, isAutoAuth, (int) skinId); // передаём skin
            else
                CAuthrization::hide();

            break;
        }

            /* ---------- Регистрация ---------- */
        case 3:
        {
            bool toggle;
            bs.Read(toggle);

            if (toggle)
                CRegistration::show();
            else
                CRegistration::hide();

            break;
        }

            /* ---------- Выбор спавна ---------- */
        case RPC_TOGGLE_CHOOSE_SPAWN:
        {
            uint8_t toggle, org, home, family, pos;
            bs.Read(toggle);
            bs.Read(org);
            bs.Read(home);
            bs.Read(family);
            bs.Read(pos);

            Log("spawn org %d home %d family %d pos %d", org, home, family, pos);

            if (toggle == 1)
                CChooseSpawn::show(org, home, family, pos);
            else
                CChooseSpawn::hide();

            break;
        }
    }
}

void CNetGame::Packet_CustomRPC(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    uint8_t packetID = 0;
    uint32_t rpcID = 0;

    if (!bs.Read(packetID) || !bs.Read(rpcID))
    {
        Log("Packet_CustomRPC: invalid header");
        return;
    }

    switch (rpcID)
    {
        case RPC_FAMILY_ACTION_MENU:
        {
            uint8_t toggle = 0;
            if (!bs.Read(toggle))
            {
                Log("FAMILY_ACTION_MENU: failed to read toggle");
                break;
            }

            if (!toggle)
            {
                CFamilyAction::Hide();
                Log("FAMILY_ACTION_MENU: hide()");
                break;
            }

            uint8_t len = 0;
            if (!bs.Read(len))
            {
                Log("FAMILY_ACTION_MENU: failed to read family name length");
                break;
            }

            char famName[64] = {0};
            if (len > 0 && len < sizeof(famName))
            {
                for (int i = 0; i < len; i++)
                {
                    char c = 0;
                    if (!bs.Read(c))
                    {
                        Log("FAMILY_ACTION_MENU: failed to read name byte %d", i);
                        famName[0] = '\0';
                        break;
                    }
                    famName[i] = c;
                }
                famName[len] = '\0';
            }
            else strcpy(famName, "Без семьи");

            uint32_t famMoney = 0, famRep = 0, skinId = 0;
            if (!bs.Read(famMoney)) { Log("FAMILY_ACTION_MENU: failed to read money"); break; }
            if (!bs.Read(famRep))   { Log("FAMILY_ACTION_MENU: failed to read reputation"); break; }
            if (!bs.Read(skinId))   { Log("FAMILY_ACTION_MENU: failed to read skinId"); skinId = 0; }

            Log("FAMILY_ACTION_MENU: show('%s', money=%u, rep=%u, skin=%u)",
                famName, (unsigned)famMoney, (unsigned)famRep, (unsigned)skinId);

            CFamilyAction::Show(famName, (int)famRep, (int)famMoney, (int)skinId);
            break;
        }

        case RPC_FAMILY_MENU:
        {
            uint8_t toggle = 0;
            if (!bs.Read(toggle))
            {
                Log("FAMILY_MENU: failed to read toggle");
                break;
            }

            if (toggle == 0)
            {
                CFamilyMenu::hide();
                Log("FAMILY_MENU: hide()");
                break;
            }

            uint8_t len = 0;
            if (!bs.Read(len))
            {
                Log("FAMILY_MENU: failed to read family name length");
                break;
            }

            char famName[64] = {0};
            if (len > 0 && len < sizeof(famName))
            {
                for (int i = 0; i < len; i++)
                {
                    char c = 0;
                    if (!bs.Read(c))
                    {
                        Log("FAMILY_MENU: failed to read name byte %d", i);
                        famName[0] = '\0';
                        break;
                    }
                    famName[i] = c;
                }
                famName[len] = '\0';
            }
            else strcpy(famName, "Без семьи");

            uint32_t famMoney = 0, famRep = 0, skinId = 0;
            if (!bs.Read(famMoney)) { Log("FAMILY_MENU: failed to read money"); break; }
            if (!bs.Read(famRep))   { Log("FAMILY_MENU: failed to read reputation"); break; }
            if (!bs.Read(skinId))   { Log("FAMILY_MENU: failed to read skinId"); skinId = 0; }

            Log("FAMILY_MENU: show('%s', money=%u, rep=%u, skin=%u)",
                famName, (unsigned)famMoney, (unsigned)famRep, (unsigned)skinId);

            CFamilyMenu::show(famName, (int)famMoney, (int)famRep, (int)skinId);
            break;
        }

        case RPC_TOGGLE_SPECIALS:
        {
            uint16_t vehicleId = 0;
            uint8_t  enable    = 0;

            // Сервер шлёт: UINT16 vehicleId, UINT8 enable
            if (!bs.Read(vehicleId)) break;
            if (!bs.Read(enable))    break;

            const bool en = (enable != 0);

            // Сохраняем состояние по ID (остается даже вне стрима)
            ToggleSpecialsForVehicle(vehicleId, en);

            // Если объект в стриме — применим визуал сразу
            if (pNetGame)
            {
                if (auto* vp = pNetGame->GetVehiclePool())
                {
                    if (auto* v = vp->GetAt(vehicleId))
                    {
                        if (auto* gtaVeh = v->m_pVehicle)
                        {
                            // страховка по модели, как и раньше
                            if (gtaVeh->nModelIndex == 598)
                            {
                                ApplySpecialsFx(gtaVeh, en);

                                // если используешь сирену GTA:
                                v->SetSirenEnabled(en);
                                if (en) v->StartSirenSound();
                                else    v->StopSirenSound();
                            }
                        }
                    }
                }
            }

            break;
        }


        case RPC_STATISTIC_USER: // 205
        {
            // формат:
            // uint8 toggle;
            // if (toggle==1) LP-string json (UTF-8)
            uint8_t toggle = 0;
            if(!bs.Read(toggle)) break;

            if(toggle == 0) {
                CStatistics::hide();
                Log("STATISTICS: hide()");
                break;
            }

            // toggle==1
            char jsonBuf[2048]; // под тест хватает; если надо — увеличь
            jsonBuf[0] = 0;
            if(!ReadLPString_Generic(bs, jsonBuf, sizeof(jsonBuf))) {
                // даже если нет json — просто покажем пустое
                jsonBuf[0] = 0;
            }

            // сначала обновим данные, потом покажем
            CStatistics::updateJson(jsonBuf);
            CStatistics::show();

            Log("STATISTICS: show(), json='%s'", jsonBuf);
            break;
        }


        case RPC_BADGES_ANSWER: // 202
        {
            // BitStream уже стоит сразу после rpcID — читаем payload
            Badges_HandleAnswer(bs);
            break;
        }
            // ���-�� ������ ����� ���� #define RPC_MVD_ID 0xE17D1D (������ ��������� � ��������)

        case RPC_MVD_ID:
        {
            auto ReadLPString = [](RakNet::BitStream& _bs, char* outBuf, size_t outCap) -> bool
            {
                unsigned short len = 0;
                if(!_bs.Read(len)) return false;

                size_t toCopy = (len < outCap-1) ? len : (outCap-1);
                for(size_t i=0;i<toCopy;i++){
                    unsigned char ch=0; if(!_bs.Read(ch)) return false;
                    outBuf[i] = (char)ch;
                }
                outBuf[toCopy] = 0;

                for(size_t i=toCopy;i<len;i++){
                    unsigned char dummy=0; if(!_bs.Read(dummy)) return false;
                }
                return true;
            };

            uint8_t toggle = 0;
            if(!bs.Read(toggle)) break;

            if (toggle == 0) {
                CMvdIdOverlay::hide();
                break;
            }

            char fio[64]      = {0};
            char rankText[64] = {0};
            int32_t skinId    = -1;

            if(!ReadLPString(bs, fio,      sizeof(fio)))      break;
            if(!ReadLPString(bs, rankText, sizeof(rankText))) break;
            if(!bs.Read(skinId))                                break;

            CMvdIdOverlay::updateId(fio, rankText);
            if (skinId >= 0) CMvdIdOverlay::renderSkin(skinId);
            CMvdIdOverlay::show();
            break;
        }

        case RPC_PASSPORT:
        {
            // helper ������ LP-������ (uint16 ����� + bytes)
            auto ReadLPString = [](RakNet::BitStream& _bs, char* outBuf, size_t outCap) -> bool
            {
                unsigned short len = 0;
                if(!_bs.Read(len)) return false;

                // ������ � �������� � �����
                size_t toCopy = (len < outCap-1) ? len : (outCap-1);
                for(size_t i=0;i<toCopy;i++){
                    unsigned char ch=0; if(!_bs.Read(ch)) return false;
                    outBuf[i] = (char)ch;
                }
                outBuf[toCopy] = 0;

                // ���� ������ ������ � ���������� � �����������
                for(size_t i=toCopy;i<len;i++){
                    unsigned char dummy=0; if(!_bs.Read(dummy)) return false;
                }
                return true;
            };

            uint8_t toggle = 0;
            if(!bs.Read(toggle)) break;

            if (toggle == 0) {
                CPassportOverlay::hide();
                break;
            }

            // ������ (� ������� � \0)
            char surname[32]  ={0};
            char name_[32]    ={0};
            char military[24] ={0};
            char fraction[32] ={0};
            char sex[8]       ={0};
            char issue[16]    ={0};
            char serial[20]   ={0};

            int32_t law   = 0;
            int32_t years = 0;

            // ������� ������ ��������� � ��������
            if(!ReadLPString(bs, surname,  sizeof(surname)))  break;
            if(!ReadLPString(bs, name_,    sizeof(name_)))    break;
            if(!bs.Read(law))                                  break;
            if(!ReadLPString(bs, military, sizeof(military))) break;
            if(!ReadLPString(bs, fraction, sizeof(fraction))) break;
            if(!bs.Read(years))                                break;
            if(!ReadLPString(bs, sex,      sizeof(sex)))      break;
            if(!ReadLPString(bs, issue,    sizeof(issue)))    break;
            if(!ReadLPString(bs, serial,   sizeof(serial)))   break;

            // �������� ��������� ���� � ��������
            CPassportOverlay::updatePassport(
                    surname, name_,
                    (int)law,
                    military, fraction,
                    (int)years,
                    sex, issue, serial
            );
            CPassportOverlay::show();

            // �������������� �����: ���� ������ ������ int32 skinId � �������� �����
            if (bs.GetNumberOfUnreadBits() >= 32) {
                int32_t skinId = -1;
                if (bs.Read(skinId) && skinId >= 0) {
                    CPassportOverlay::renderSkin(skinId);
                }
            }
            break;
        }

        case RPC_BB_SHOW_TABLET: // 100 � ������ ����� ������� ������� (boombox �����)
        {
            CTabletMusic::Show();
            break;
        }
            // === ������ ���� ���� ===
                case 241: // RPC_BB_SHOW_TABLET
                {
                    BB_OnRpcShowTabletBoombox(); // ������� ������� (boombox mode)
                    break;
                }
        case 555:{
			uint8_t gg;
			bool tt;
			bs.Read(gg);
			bs.Read(tt);

			Log("%d - %d", gg, tt);
            break;
		}
		case RPC_INVENTAR_CARRYNG: {
			packetInventoryUpdateCarryng(p);
			break;
		}
		case RPC_ITEM_MATRIX: {
			packetInventoryUpdateItem(p);
			break;
		}
		case RPC_ITEM_ACTIVETOGGLE: {
            packetInventoryItemActive(p);
			break;
		}
		case RPC_SHOW_CASINO_BUY_CHIP: {
			packetCasinoChip(p);
			break;
		}
		case RPC_KILL_LIST: {
			packetKillList(p);
			break;
		}
		case RPC_TECH_INSPECT: {
			packetTechInspect(p);
			break;
		}
		case RPC_DAILY_REWARDS: {
			packetDailyRewards(p);
			break;
		}
		case RPC_SHOW_DONATE: {
			packetShowDonat(p);
			break;
		}
		case RPC_UPDATE_SATIETY: {
			packetUpdateSatiety(p);
			break;
		}
		case RPC_SHOW_ACTION_LABEL: {
			packetNotification(p);
			break;
		}
		case RPC_DUELS_SHOW_KILL_LEFT: {
			packetDuelsKillsLeft(p);
			break;
		}
		case RPC_CLEAR_KILL_LIST: {
            CDuelsGui::clearKillList();
			break;
		}
		case RPC_UPDATE_BACCARAT: {
			packetCasinoBaccarat(p);
			break;
		}
		case RPC_SET_MONEY: {
			uint32_t money;
			bs.Read(money);

			CHUD::iLocalMoney = money;

			CHUD::UpdateMoney();
			break;
		}
		case RPC_SHOW_CONTEINER_AUC: {
            packetAucContainer(p);
			break;
		}
		case RPC_INVENTAR_SHOWHIDE: {
			packetInventoryToggle(p);
			break;
		}
		case RPC_SHOW_SALARY:
		{
			packetSalary(p);
			break;
		}
		case RPC_ADMIN_RECON:
		{
			packetAdminRecon(p);
			break;
		}

		case RPC_MAFIA_WAR:
		{
            packetMafiaWar(p);
			break;
		}
		case RPC_CASINO_LUCKY_WHEEL_MENU:
		{
			uint32_t count;
			uint32_t time;

			bs.Read(count);
			bs.Read(time);

			g_pJavaWrapper->ShowCasinoLuckyWheel(count, time);
			break;
		}
		case RPC_SHOW_FACTORY_GAME:
		{
			Packet_FurnitureFactory(p);
			break;
		}
		case RPC_SEND_BUFFER:
		{
			uint16_t len;
			bs.Read(len);
			char text[len+1];
			bs.Read(text, len);
			text[len] = '\0';

			g_pJavaWrapper->SendBuffer(text);
			break;
		}
		case RPC_SHOW_DICE_TABLE:
		{
			const int MAX_PLAYERS_CASINO_DICE = 5;

			char playerName[MAX_PLAYERS_CASINO_DICE][25] = {"--", "--", "--", "--", "--"};
			uint8_t toggle;
			uint8_t tableID;
			uint32_t bet;
			uint32_t bank;
			uint16_t playerID[MAX_PLAYERS_CASINO_DICE];
			uint8_t playerStat[MAX_PLAYERS_CASINO_DICE];

			bs.Read(toggle);

			pGame->isCasinoDiceActive = toggle;
			if(toggle == 0)
			{
				g_pJavaWrapper->ShowCasinoDice(false, 0, 0, 0, 0, "--", 0, "--", 0, "--", 0, "--", 0, "--", 0);
				return;
			}
			bs.Read(tableID);
			bs.Read(bet);
			bs.Read(bank);

			CPlayerPool *pPlayerPool = GetPlayerPool();
			for(int i = 0; i < MAX_PLAYERS_CASINO_DICE; i++)
			{
				bs.Read(playerID[i]);
				bs.Read(playerStat[i]);


                if(playerID[i] == INVALID_PLAYER_ID)
                {
                    strcpy(playerName[i], "--");
				//	continue;
                }
				else if(playerID[i] == pPlayerPool->GetLocalPlayerID())
				{

					strcpy(playerName[i], pPlayerPool->GetLocalPlayerName());
				//	continue;
				}
				else
				{
					if (pPlayerPool)
					{
						if(pPlayerPool->m_pPlayers[playerID[i]])
						{
							strcpy(playerName[i], pPlayerPool->GetPlayerName(playerID[i]));
						}
						else
						{
							strcpy(playerName[i], "--");
						}
					}
				}
			}
			int money = CHUD::iLocalMoney;
			g_pJavaWrapper->ShowCasinoDice(toggle, tableID, bet, bank, money, playerName[0], playerStat[0], playerName[1], playerStat[1], playerName[2], playerStat[2], playerName[3], playerStat[3], playerName[4], playerStat[4]);
			break;
		}
		case RPC_OPEN_SETTINGS:
		{
			g_pJavaWrapper->ShowClientSettings();
			break;
		}
		case RPC_TOGGLE_HUD_ELEMENT:
		{
			uint32_t hud, toggle;
			bs.Read(hud);
			bs.Read(toggle);

            CChatWindow::AddDebugMessage("hud %d toggle %d", hud, toggle);
			pGame->ToggleHUDElement(hud, toggle);
//			pGame->HandleChangedHUDStatus();
			break;
		}
		case RPC_SHOW_TARGET_LABEL:
		{
			uint8_t value;
			char str[256];
			uint8_t len;
			bs.Read(len);
			bs.Read(&str[0], len);
			bs.Read(value);

			char text[256];
			cp1251_to_utf8(text, str, len);
			CHUD::showUpdateTargetNotify(value, (char *)text);

			break;
		}
		case RPC_SHOW_ARMY_GAME:
		{
			uint8_t toggle;
			uint8_t quantity;
			bs.Read(toggle);
			bs.Read(quantity);

			if (toggle == 1)
			{
				g_pJavaWrapper->ShowArmyGame(quantity);
			}
			else
			{
				g_pJavaWrapper->HideArmyGame();
			}
			break;
		}
		case RPC_SHOW_TD_BUS:
		{
			uint8_t toggle;
			uint32_t time;
			bs.Read(toggle);
			bs.Read(time);

			if (toggle == 1)
			{
				CHUD::showBusInfo((int)time);
			}
			else
			{
				CHUD::hideBusInfo();
			}

			break;
		}
		case RPC_SHOW_MINING_GAME:
		{
			uint8_t toggle;
			uint32_t type;

			bs.Read(toggle);
			bs.Read(type);

			if(type == 0)
			{
				g_pJavaWrapper->ShowMiningGame1(toggle);
				return;
			}
			if(type == 1)
			{
				g_pJavaWrapper->ShowMiningGame2(toggle);
				return;
			}
			if(type == 2)
			{
				g_pJavaWrapper->ShowMiningGame3(toggle);
				return;
			}
			break;
		}
		case RPC_PRE_DEATH:
		{
			packetPreDeath(p);
			break;
		}
		case RPC_MED_GAME:
		{
			packetMedGame(p);
			break;
		}
//		case RPC_CHECK_CLIENT:
//		{
//			char recievKey[17];
//			uint16_t recievKey_len;
//			bs.Read(recievKey_len);
//			bs.Read(recievKey, recievKey_len);
//
//			recievKey[recievKey_len] = '\0';
//
//			char key_with_salt[recievKey_len+ strlen(AUTH_SALT)+1];
//			strcpy(key_with_salt, recievKey);
//			strcat(key_with_salt, AUTH_SALT);
//
//			SendCheckClientPacket(sha256(key_with_salt).c_str());
//			break;
//
//		}
		case RPC_SHOW_OILGAME:
		{
			uint8_t toggle;
			bs.Read(toggle);

			if (toggle == 1)
			{
				g_pJavaWrapper->ShowOilFactoryGame();
			}
			break;
		}
		case RPC_CUSTOM_SET_LEVEL:
		{
			uint32_t current;
			uint32_t max;
			uint32_t level;
			bs.Read(level);
			bs.Read(current);
			bs.Read(max);

			CHUD::updateLevelInfo(level, current, max);
			break;
		}
		case RPC_TOGGLE_GPS_INFO:
		{
			uint8_t value;
			bs.Read(value);
			if (value == 1)
			{
				CHUD::toggleGps(true);
			}
			else if (value == 0)
			{
				CHUD::toggleGps(false);
			}
			break;
		}
		case RPC_TOGGLE_GREENZONE:
		{
			uint8_t value;
			bs.Read(value);

			if (value == 1)
			{
				CHUD::toggleGreenZone(true);
				m_GreenZoneState = true;
			}
			else if (value == 0)
			{
				CHUD::toggleGreenZone(false);
				m_GreenZoneState = false;
			}
			break;
		}
		case RPC_TOGGLE_SAMWILL_GAME:
		{
			uint8_t value;
			bs.Read(value);

			if (value == 1)
			{
				g_pJavaWrapper->ShowSamwill();
			}
			break;
		}
		case RPC_VIBRATE:
		{
			uint32_t value;
			bs.Read(value);

			g_pJavaWrapper->Vibrate(value);
			break;
		}
		case RPC_GUNSTORE_TOGGLE:
		{
			uint8_t toggle;
			bs.Read(toggle);

			if (toggle == 1)
			{
				g_pJavaWrapper->ShowGunShopManager();
			}
			else
			{
				g_pJavaWrapper->HideGunShopManager();
			}

			break;
		}
		case RPC_TOGGLE_ACCESSORIES_MENU:
		{
			uint8_t toggle;
			uint32_t price;
			bs.Read(toggle);

			if(!toggle)
			{
				g_pJavaWrapper->ToggleShopStoreManager(toggle);
			}
			bs.Read(price);

			g_pJavaWrapper->ToggleShopStoreManager(toggle, 0, price);
			break;
		}
		case RPC_TOGGLE_CLOTHING_MENU:
		{
			uint8_t toggle;
			uint32_t price;
			bs.Read(toggle);
			if(!toggle)
			{
				g_pJavaWrapper->ToggleShopStoreManager(toggle);
			}
			bs.Read(price);

			g_pJavaWrapper->ToggleShopStoreManager(toggle, 1, price);
			break;
		}
		case RPC_FUELSTATION_BUY:
		{
			uint8_t type;
			uint32_t price1;
			uint32_t price2;
			uint32_t price3;
			uint32_t price4;
			uint32_t price5;
			uint32_t maxCount;

			bs.Read(type);
			bs.Read(price1);
			bs.Read(price2);
			bs.Read(price3);
			bs.Read(price4);
			bs.Read(price5);
			bs.Read(maxCount);

			g_pJavaWrapper->ShowFuelStation(type, price1, price2, price3, price4, price5, maxCount);

			break;
		}
		case RPC_SHOW_AUTOSHOP:
		{
			uint32_t toggle;
			bs.Read(toggle);

			g_pJavaWrapper->ToggleAutoShop((bool)toggle);

			break;
		}
		case RPC_UPDATE_AUTOSHOP:
		{
			uint32_t price;
			uint32_t count;
			float maxspeed;
			float acceleration;
			uint8_t len;
			uint8_t gear;


			char name[256];

			bs.Read(len);
			bs.Read(name, len);
			name[len] = '\0';
			bs.Read(price);
			bs.Read(count);
			bs.Read(maxspeed);
			bs.Read(acceleration);
			bs.Read(gear);

			char utf8[200];
			cp1251_to_utf8(utf8, name);
			g_pJavaWrapper->UpdateAutoShop(utf8, price, count, maxspeed, acceleration, gear);
			break;
		}
		case RPC_CUSTOM_HANDLING_DEFAULTS:
		{
			break;
//			uint16_t vehId;
//			bs.Read(vehId);
//
//			if (GetVehiclePool())
//			{
//				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
//				if (pVeh)
//				{
//					pVeh->ResetVehicleHandling();
//				}
//			}
//
//			break;
		}
		case RPC_CUSTOM_VISUALS:
		{
			VEHICLEID vehicleId;
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

			bs.Read(vehicleId);
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

			bs.Read(bLen);
			bs.Read(&szText[0], bLen);

			bs.Read(bLen);
			bs.Read(&szRegion[0], bLen);


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

			bs.Read(szName, bLen);

			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehicleId);
				if (pVeh)
				{

					pVeh->SetCustomShadow(bShadowColor[0], bShadowColor[1], bShadowColor[1], (float)bShadowSizeX / 10.0f, (float)bShadowSizeY / 10.0f, szName);

					if (bLightsColor[0] != 0xFF || bLightsColor[1] != 0xFF || bLightsColor[2] != 0xFF)
					{
						pVeh->SetHeadlightsColor(bLightsColor[0], bLightsColor[1], bLightsColor[2]);
					}

					if (fWheelWidth)
					{
						pVeh->SetWheelWidth((float)fWheelWidth / 100.0f);
					}

					if (bWheelAlignX)
					{
						pVeh->SetWheelAlignment(0, (float)bWheelAlignX);
					}

					if (bWheelAlignY)
					{
						pVeh->SetWheelAlignment(1, (float)bWheelAlignY);
					}

					if (sWheelOffsetXX)
					{
						float fValueX = (float)((float)sWheelOffsetXX / 100.0f);
						pVeh->SetWheelOffset(0, fValueX);
						//pVeh->ProcessWheelsOffset();
					}
					if (sWheelOffsetXY)
					{
						float fValueX = (float)((float)sWheelOffsetXY / 100.0f);
						pVeh->SetWheelOffset(1, fValueX);
						//pVeh->ProcessWheelsOffset();
					}

					pVeh->ApplyToner(1, bToner[0]);
					pVeh->ApplyToner(2, bToner[1]);
					pVeh->ApplyToner(3, bToner[2]);
					pVeh->ApplyVinyls(bVinyls[0], bVinyls[1]);
					//CChatWindow::AddDebugMessage("%d %d %d %d %d", bToner[0], bToner[1], bToner[2], bVinyls[0], bVinyls[1]);
//					if (bPlateType)
//					{
//						CCustomPlateManager::PushPlate(vehicleId, (uint32_t)bPlateType, szText, szRegion);
//					}
				}
			}

			break;
		}
		case RPC_CUSTOM_HANDLING:
		{
			break;
//			uint16_t veh;
//			uint8_t value;
//			bs.Read(veh);
//			bs.Read(value);
//			std::vector<SHandlingData> comps;
//			for (uint8_t i = 0; i < value; i++)
//			{
//				uint8_t id;
//				float fvalue;
//				bs.Read(id);
//				bs.Read(fvalue);
//				comps.push_back(SHandlingData(id, fvalue, 0));
//			}
//			if (m_pVehiclePool)
//			{
//				if (m_pVehiclePool->GetAt(veh))
//				{
//					m_pVehiclePool->GetAt(veh)->SetHandlingData(comps);
//				}
//			}
//			break;
		}
		case RPC_CUSTOM_COMPONENT:
		{
			uint16_t veh, extra_comp;
			uint8_t comp;
			bs.Read(veh);
			CVehicle* pVehicle = nullptr;

			if (m_pVehiclePool)
			{
				pVehicle = m_pVehiclePool->GetAt(veh);
			}
			if (!pVehicle)
			{
				return;
			}
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
			str[len] = '\0';
			//CChatWindow::AddDebugMessage("%d %f %f %f %f %d %d %d %s", id, pos.x, pos.y, pos.z, fDistance, vw, interior, len, str);
		//	GetStreamPool()->AddStream(id, &pos, vw, interior, fDistance, (const char*)&str[0]);
			break;
		}
		case RPC_STREAM_INDIVIDUAL:
		{
			char str[255];
			uint8_t len;
			uint8_t repeat;

			bs.Read(len);
			bs.Read(&str[0], len);
			str[len] = '\0';
			bs.Read(repeat);
			//CChatWindow::AddDebugMessage("%s", str);
			//CChatWindow::AddDebugMessage("Playing audiostream %s", str);
			if(repeat) {
				GetStreamPool()->PlayIndividualStream(&str[0], BASS_SAMPLE_LOOP);
			}
			else {
				GetStreamPool()->PlayIndividualStream(&str[0], BASS_STREAM_AUTOFREE);
			}
			break;
		}
		case RPC_STREAM_POS:
		{
			uint16_t id;
			CVector pos;
			bs.Read(id);
			bs.Read(pos.x);
			bs.Read(pos.y);
			bs.Read(pos.z);
			if (GetStreamPool()->GetStream(id))
			{
				GetStreamPool()->GetStream(id)->SetPosition(pos);
			}
			break;
		}
		case RPC_STREAM_DESTROY:
		{
			uint32_t id;
			bs.Read(id);
			//CChatWindow::AddDebugMessage("%d", id);
			m_pStreamPool->DeleteStreamByID(id);
			break;
		}
		case RPC_STREAM_VOLUME:
		{
			uint16_t id;
			float fVolume;
			bs.Read(id);
			bs.Read(fVolume);
			//CChatWindow::AddDebugMessage("%d %f", id, fVolume);
			m_pStreamPool->SetStreamVolume(id, fVolume);
			break;
		}
		case RPC_STREAM_ISENABLED:
		{
			uint32_t isEnabled;
			bs.Read(isEnabled);
			if (isEnabled)
			{
				m_pStreamPool->Activate();
			}
			else if (isEnabled == 0)
			{
				m_pStreamPool->Deactivate();
			}
			break;
		}
		case RPC_OPEN_LINK:
		{
			Log("RPC_OPEN_LINK");
//			uint16_t len;
//			bs.Read(len);
//
//			if (len >= 0xFA)
//			{
//				break;
//			}
//
//			char link[0xFF];
//			memset(&link[0], 0, 0xFF);
//			bs.Read((char*)& link, len);
//			AND_OpenLink(&link[0]);

			break;
		}
		case RPC_TIMEOUT_CHAT:
		{
//			uint32_t timeoutStart = 0;
//			uint32_t timeoutEnd = 0;
//			bs.Read(timeoutStart);
//			bs.Read(timeoutEnd);
//
//			if (pChatWindow)
//			{
//				CChatWindow::SetChatDissappearTimeout(timeoutStart, timeoutEnd);
//			}

			break;
		}
		case RPC_CUSTOM_SET_FUEL:
		{
			float fuel;
            float mileage;
			bs.Read(fuel);
			bs.Read(mileage);

            CSpeedometr::fFuel = (int) fuel;
            CSpeedometr::iMilliage = (int) mileage;

            CSpeedometr::updateFuel();
			break;
		}

        case 0x103: // add stream
        {
            char str[255];
            uint8_t len;
            uint16_t id, vw, interior, playerid, vehicleid;
            CVector pos;
            float fDistance;
            bs.Read(id);
            bs.Read(pos.x);
            bs.Read(pos.y);
            bs.Read(pos.z);
            bs.Read(fDistance);
            bs.Read(interior);
            bs.Read(playerid);
            bs.Read(vehicleid);
            bs.Read(vw);
            bs.Read(len);
            bs.Read(&str[0], len);
            str[len] = '\0';

            Log("%d, %f, %f, %f, %d, %d, %d, %d, %f, %s", id, pos.x, pos.y, pos.z, vw, playerid, vehicleid, interior, fDistance, (const char*)&str[0]);

            GetStreamPool()->AddStream(id, pos, vw, playerid, vehicleid, interior, fDistance, (const char*)&str[0]);

            break;
        }
        case 0x108: // attach player
        {
            uint16_t attachedtoid, streamid;

            bs.Read(attachedtoid);
            bs.Read(streamid);

            Log("AttachToPlayer %d %d", streamid, attachedtoid);

            GetStreamPool()->AttachToPlayer(streamid, attachedtoid);

            break;
        }

        case 0x123: // attach vehicle
        {
            uint16_t attachedtoid, streamid;

            bs.Read(attachedtoid);
            bs.Read(streamid);

            Log("AttachToVehicle %d %d", streamid, attachedtoid);

            GetStreamPool()->AttachToVehicle(streamid, attachedtoid);

            break;
        }

        case 0x104: // destroy stream
        {
            uint16_t streamid;

            bs.Read(streamid);

            GetStreamPool()->DeleteStreamByID(streamid);

            break;
        }

        case 0x105: // DeAttachStream
        {

            uint16_t streamid;
            bs.Read(streamid);

            GetStreamPool()->DeAttachStreamByID(streamid);

            break;
        }
        case 0x106: // StreamPause
        {
            uint8_t ispause;
            uint16_t streamid;
            bs.Read(streamid);
            bs.Read(ispause);
            if(ispause)
                GetStreamPool()->PauseStream(streamid);
            else
                GetStreamPool()->PlayStream(streamid);

            break;
        }

    }

}
void CNetGame::ResetVehiclePool()
{
	Log("ResetVehiclePool");
	if(m_pVehiclePool)
		delete m_pVehiclePool;

	m_pVehiclePool = new CVehiclePool();
}

void CNetGame::ResetObjectPool()
{
	Log("ResetObjectPool");
	if(m_pObjectPool)
		delete m_pObjectPool;

	m_pObjectPool = new CObjectPool();
}

void CNetGame::ResetPickupPool()
{
	Log("ResetPickupPool");
	if(m_pPickupPool)
		delete m_pPickupPool;

	m_pPickupPool = new CPickupPool();
}

void CNetGame::ResetGangZonePool()
{
	Log("ResetGangZonePool");
	if(m_pGangZonePool)
		delete m_pGangZonePool;

	m_pGangZonePool = new CGangZonePool();
}

void CNetGame::ResetLabelPool()
{
	Log("ResetLabelPool");
	if(m_pLabelPool)
		delete m_pLabelPool;

	m_pLabelPool = new CText3DLabelsPool();
}

void CNetGame::ResetActorPool()
{
	Log("ResetActorPool");
	if (m_pActorPool)
	{
		delete m_pActorPool;
	}
	m_pActorPool = new CActorPool();
}

void CNetGame::ShutDownForGameRestart()
{
    SpeakerList::Hide();
    MicroIcon::Hide();
    Network::OnRaknetDisconnect();
	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer* pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
		{
			//pPlayer->SetTeam(NO_TEAM);
			//pPlayer->ResetAllSyncAttributes();
		}
	}

	CLocalPlayer *pLocalPlayer = m_pPlayerPool->GetLocalPlayer();
	if(pLocalPlayer)
	{
		pLocalPlayer->ResetAllSyncAttributes();
		pLocalPlayer->ToggleSpectating(false);
	}

	m_iGameState = GAMESTATE_RESTARTING;

	//m_pPlayerPool->DeactivateAll();
	m_pPlayerPool->Process();

	ResetVehiclePool();
	ResetObjectPool();
	ResetPickupPool();
	ResetGangZonePool();
	ResetLabelPool();
	ResetActorPool();
	g_pJavaWrapper->ClearScreen();

	m_bDisableEnterExits = false;
	m_fNameTagDrawDistance = 60.0f;
	m_byteWorldTime = 12;
	m_byteWorldMinute = 0;
	m_byteWeather = 0;
	m_bNameTagLOS = true;
	m_bUseCJWalk = false;
	m_fGravity = 0.008f;
	m_iDeathDropMoney = 0;

	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if(pPlayerPed)
	{
	//	pGame->RemovePlayer(pPlayerPed);
		pPlayerPed->SetInterior(0, true);
//		//pPlayerPed->SetDead();
		pPlayerPed->SetArmour(0.0f);
	}

	CHUD::iLocalMoney = 0;
	m_bZoneNames = false;
	GameResetRadarColors();
	pGame->SetGravity(m_fGravity);
}
//
//void CNetGame::SendCheckClientPacket(const char password[])
//{
//	uint8_t packet = ID_CUSTOM_RPC;
//	uint8_t RPC = RPC_CHECK_CLIENT;
//	uint16_t bytePasswordLen = strlen(password);
//	RakNet::BitStream bsSend;
//	bsSend.Write(packet);
//	bsSend.Write(RPC);
//	bsSend.Write(bytePasswordLen);
//	bsSend.Write(password, bytePasswordLen);
//	GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
//
//	//CChatWindow::AddDebugMessage("key: %s", password);
//}

void CNetGame::SendCustomPacket(uint8_t packet, uint8_t RPC, uint8_t Quantity)
{
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(Quantity);
	GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

void CNetGame::SendCustomPacketFuelData(uint8_t packet, uint8_t RPC, uint8_t fueltype, uint32_t fuel)
{
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(fueltype);
	bsSend.Write(fuel);
	GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
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

void CNetGame::SendChatCommand(const char* szCommand)
{
	if (GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsParams;
	int iStrlen = strlen(szCommand);

	bsParams.Write(iStrlen);
	bsParams.Write(szCommand, iStrlen);
	m_pRakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, int iColor, int style)
{
	if(byteIndex >= 100) return;
	if(m_dwMapIcons[byteIndex]) DisableMapIcon(byteIndex);

	m_dwMapIcons[byteIndex] = pGame->CreateRadarMarkerIcon(byteIcon, fX, fY, fZ, iColor, style);
}

void CNetGame::DisableMapIcon(uint8_t byteIndex)
{
	if(byteIndex >= 100) return;
	ScriptCommand(&disable_marker, m_dwMapIcons[byteIndex]);
	m_dwMapIcons[byteIndex] = 0;
}

void CNetGame::UpdatePlayerScoresAndPings()
{

	static uint32_t dwLastUpdateTick = 0;

	if ((GetTickCount() - dwLastUpdateTick) >= 3000) {
		dwLastUpdateTick = GetTickCount();
		RakNet::BitStream bsParams;
		m_pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void gen_auth_key(char buf[260], char* auth_in);
void CNetGame::packetAuthKey(Packet* pkt)
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
	uint8_t byteAuthKeyLen = (uint8_t)strlen(szAuthKey);

	bsKey.Write((uint8_t)ID_AUTH_KEY);
	bsKey.Write((uint8_t)byteAuthKeyLen);
	bsKey.Write(szAuthKey, byteAuthKeyLen);
	m_pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);

}

void CNetGame::Packet_DisconnectionNotification(Packet* pkt)
{
	CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::SERVER_CLOSED_CONNECTION));
	m_pRakClient->Disconnect(2000);
	g_pJavaWrapper->ClearScreen();

	//pNetGame->ShutDownForGameRestart();
}

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
	if(m_pRakClient) m_pRakClient->Disconnect(0);

	CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTION_LOST));
	ShutDownForGameRestart();

	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer *pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer) m_pPlayerPool->Delete(playerId, 0);
	}

	SetGameState(GAMESTATE_WAIT_CONNECT);
	
}
//#define SUM_MAS_ENCR	10
//int g_sumMas[SUM_MAS_ENCR] = { 290, 291, 417, 424, 477, 54+38+142+49, 51+91+91+84, 54+38+142+50, 54 + 38 + 142 + 51, 51 + 77 + 238 + 92 };

#include "..//CServerManager.h"

bool g_isValidSum(int a)
{
	for (int i = 0; i < MAX_SERVERS; i++)
	{
		if (g_sEncryptedAddresses[i].getSum() == a) return true;
	}
	return false;
}
#include <sstream>

void CNetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
	CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTED));
	SetGameState(GAMESTATE_AWAIT_JOIN);

	RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
	PLAYERID MyPlayerID;
	unsigned int uiChallenge;

	uint16_t playerid;
	bsSuccAuth.IgnoreBits(8); // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port
	bsSuccAuth.Read(playerid);
	bsSuccAuth.Read(uiChallenge);
	char ip[0x7F];
	strncpy(ip, m_szHostOrIp, sizeof(ip));

	std::vector<std::string> strings;
	std::istringstream f((const char*)&ip[0]);
	std::string s;
	int sum = 0;
	while (getline(f, s, '.'))
	{
		sum += std::atoi(s.c_str());
	}

	m_pPlayerPool->SetLocalPlayerID(playerid);

	int iVersion = NETGAME_VERSION;
	char byteMod = 0x01;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

	char byteAuthBSLen = (char)strlen(AUTH_BS);
	char byteNameLen = (char)strlen(m_pPlayerPool->GetLocalPlayerName());
	char byteClientverLen = (char)strlen(SAMP_VERSION);

	RakNet::BitStream bsSend;
	bsSend.Write(iVersion);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(m_pPlayerPool->GetLocalPlayerName(), byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(AUTH_BS, byteAuthBSLen);
	bsSend.Write(byteClientverLen);
	bsSend.Write(SAMP_VERSION, byteClientverLen);
    Network::OnRaknetRpc(RPC_ClientJoin, bsSend);

	m_pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	Log("Packet_ConnectionSucceeded");

	// auth
//	RakNet::BitStream bsSendTwo;
//	bsSendTwo.Write((uint8_t) ID_CUSTOM_RPC);
//	bsSendTwo.Write((uint8_t) PRC_AUTHORIZATION);
//
//	pNetGame->GetRakClient()->Send(&bsSendTwo, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
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

    // READ MOVESPEED VECTORS
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

	//uint8_t key = 0;

    if(m_pPlayerPool)
    {
    	pPlayer = m_pPlayerPool->GetAt(playerId);
    	if(pPlayer)
    		pPlayer->StoreOnFootFullSyncData(&ofSync, 0);
    }
}

void CNetGame::Packet_VehicleSync(Packet* pkt)
{
	CRemotePlayer *pPlayer;
	RakNet::BitStream bsSync((unsigned char *)pkt->data, pkt->length, false);
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
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
	bsSync.ReadNormQuat(
		icSync.quat.w,
		icSync.quat.x,
		icSync.quat.y,
		icSync.quat.z);

	// position
	bsSync.Read((char*)&icSync.vecPos, sizeof(CVector));

	// speed
	bsSync.ReadVector(
		icSync.vecMoveSpeed.x,
		icSync.vecMoveSpeed.y,
		icSync.vecMoveSpeed.z);

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
	if(bCheck) bsSync.Read(icSync.HydraThrustAngle);

	// triler id
	bsSync.Read(bCheck);
	if(bCheck) bsSync.Read(icSync.TrailerID);

	if(m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
		{
			pPlayer->StoreInCarFullSyncData(&icSync, 0);
		}
	}
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
	CRemotePlayer *pPlayer;
	uint8_t bytePacketID;
	PLAYERID playerId;
	PASSENGER_SYNC_DATA psSync;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsPassengerSync((unsigned char *)pkt->data, pkt->length, false);
	bsPassengerSync.Read(bytePacketID);
	bsPassengerSync.Read(playerId);
	bsPassengerSync.Read((char*)&psSync, sizeof(PASSENGER_SYNC_DATA));

	if(m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
			pPlayer->StorePassengerFullSyncData(&psSync);
	}
}

void CNetGame::Packet_MarkersSync(Packet *pkt)
{
	CRemotePlayer *pPlayer;
	int			iNumberOfPlayers = 0;
	PLAYERID	playerId;
	short		sPos[3];
	bool		bIsPlayerActive;
	uint8_t 	unk0 = 0;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

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

			if(playerId < MAX_PLAYERS && m_pPlayerPool->m_pPlayers[playerId])
			{
				pPlayer = m_pPlayerPool->GetAt(playerId);
				if(pPlayer)
				{
					if(bIsPlayerActive)
						pPlayer->ShowGlobalMarker(sPos[0], sPos[1], sPos[2]);
					else
						pPlayer->HideGlobalMarker();
				}
			}
		}
	}
}

void CNetGame::Packet_BulletSync(Packet* p)
{
	uint8_t bytePacketID;
	uint16_t PlayerID;
	BULLET_SYNC btSync;
	RakNet::BitStream bsBulletSync((unsigned char *)p->data, p->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsBulletSync.Read(bytePacketID);
	bsBulletSync.Read(PlayerID);
	bsBulletSync.Read((char *)&btSync, sizeof(BULLET_SYNC));

	CRemotePlayer *pRemotePlayer = m_pPlayerPool->GetAt(PlayerID);
	if (pRemotePlayer)
	{
		pRemotePlayer->StoreBulletSyncData(&btSync);
	}
}

void CNetGame::Packet_AimSync(Packet * p)
{
	CRemotePlayer * pPlayer;
	RakNet::BitStream bsAimSync((unsigned char*)p->data, p->length, false);
	AIM_SYNC_DATA aimSync;
	uint8_t bytePacketID = 0;
	uint16_t bytePlayerID = 0;

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	bsAimSync.Read(bytePacketID);
	bsAimSync.Read(bytePlayerID);
	bsAimSync.Read((char*)&aimSync, sizeof(AIM_SYNC_DATA));

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)  {
		pPlayer->UpdateAimFromSyncData(&aimSync);
	}
}
void CNetGame::SendLoginPacket(const char password[]) {
    uint8_t packet = PACKET_AUTH;
    uint8_t RPC = (1);
    uint8_t bytePasswordLen = strlen(password);
    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(bytePasswordLen);
    bsSend.Write(password, bytePasswordLen);
    GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);

    strcpy(CSettings::m_Settings.player_password, password);
    CSettings::save();

}

void CNetGame::SendRegisterPacket(char *password, char *mail, uint8_t sex, uint8_t age, uint32_t skinid) {
    uint8_t packet = PACKET_AUTH;
    uint8_t RPC = 4;
    uint8_t bytePasswordLen = strlen(password);
    uint8_t byteMailLen = strlen(mail);
    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(bytePasswordLen);
    bsSend.Write(byteMailLen);
    bsSend.Write(password, bytePasswordLen);
    bsSend.Write(mail, byteMailLen);
    bsSend.Write(sex);
    bsSend.Write(age);
    bsSend.Write(skinid);
    GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
void CNetGame::SendRegisterSkinPacket(uint32_t skinId) {
    uint8_t packet = PACKET_AUTH;
    uint8_t RPC = 3;
    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(skinId);
    GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}
bool SendCustomRPCToServer(unsigned char rpcId, const void* data, int len)
{
    if (!pNetGame || !pNetGame->GetRakClient())
        return false;

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint32_t)rpcId);

    if (data && len > 0) {
        uint16_t L = (uint16_t)((len > 0xFFFF) ? 0xFFFF : len);
        bsSend.Write(L);
        bsSend.Write((const char*)data, L);
    }

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
    return true;
}