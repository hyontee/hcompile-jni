#include "../main.h"
#include "../CDebugInfo.h"
#include "../CSettings.h"
#include "../playertags.h"
#include "../keyboard.h"

#include "../game/game.h"
#include "../game/crosshair.h"

#include "netgame.h"

#include "../gui/gui.h"
#include "../gui/buttons.h"

extern CWidgetManager* g_pWidgetManager;
extern CCrossHair *pCrossHair;
extern CGUI *pGUI;
extern CSettings *pSettings;
extern CButtons *m_pButtons;
extern CKeyBoard *pKeyBoard;

#include "../chatwindow.h"

#include "..//CClientInfo.h"
#include "..//CLocalisation.h"

#define RPC_CUSTOM_HANDLING 			0x2
#define RPC_CUSTOM_VISUALS				0x3
#define RPC_CUSTOM_HANDLING_DEFAULTS	0x4
#define RPC_CUSTOM_STROBS				0x5
#define RPC_CUSTOM_NEON					0x6
#define RPC_CUSTOM_PLATE				0x7
#define RPC_CUSTOM_TONER				0x8
#define RPC_CUSTOM_VINYLS				0x9
#define RPC_CUSTOM_COMPONENT			0x10
#define RPC_CUSTOM_BODYWORK				0x11
#define RPC_TUNING						0x20

#define RPC_OPEN_SETTINGS				0x21
#define CUSTOM_RPC_TOGGLE_HUD_ELEMENT	0x22
#define RPC_CUSTOM_AIM					0x23
#define RPC_OPEN_SETTINGS_JAVA			0x24
#define RPC_OPEN_LINK					0x25
#define RPC_CHECK_HUD					0x26
#define RPC_SPEEDOMETR					0x27
#define RPC_HUD_GIFT					0x28
#define RPC_HUD_GIFT_HIDE				0x29

#define RPC_STREAM_CREATE				0x30
#define RPC_STREAM_POS					0x31
#define RPC_STREAM_DESTROY				0x32
#define RPC_STREAM_INDIVIDUAL			0x33
#define RPC_STREAM_VOLUME				0x34
#define RPC_STREAM_ISENABLED			0x35

#define RPC_CUSTOM_ACTOR_PUT_IN_VEH		0x40
#define RPC_CUSTOM_ACTOR_REMOVE_VEH		0x41
#define RPC_CUSTOM_ACTOR_ADD_ADDITIONAL	0x42
#define RPC_CUSTOM_ADD_PED				0x43

#define RPC_CUSTOM_VOICE_CONNECT		0x44
#define RPC_CUSTOM_VOICE_DISCONNECT		0x45
#define RPC_CUSTOM_VOICE_VOLUME			0x46
#define RPC_CUSTOM_VOICE_MUTE			0x47
#define RPC_CUSTOM_VOICE_UNMUTE			0x48
#define RPC_CUSTOM_VOICE_VOLUMETOPLAYER 0x49
#define RPC_CUSTOM_VOICE_SETTINGS		0x50

#define RPC_CHECK_CASH					0x60
#define RPC_SHOW_NOTIFICATION			0x61
#define RPC_CHATMESSAGE					0x62
#define RPC_RADIALMENU					0x63
#define RPC_RADIALMENU_HIDE				0x64

#define RPC_BATTLE_STATS 				0x70
#define RPC_BATTLE_PASS 				0x71
#define RPC_NEWBIE_BATTLE 				0x72

#define RPC_DONATE						0x90
#define RPC_TRADE 						0x100
#define RPC_LABEL_CUSTOM 				0x101

#define RPC_INVENTORY 					0x110
#define RPC_DIALOGS						0x111
#define RPC_AUTH_REG					0x112

#define RPC_FAMILY_SHOW					0x120
#define RPC_ROULETTE					0x121
#define RPC_CHAT						0x122
#define RPC_BINDERS						0x123

#define RPC_EMPLOYMENT_SHOW				0x130
#define RPC_MARKET						0x131
#define RPC_CRAFT						0x132
#define RPC_OBJECT_EDIT					0x133
#define RPC_RECON_ADMIN					0x134

#define RPC_PARKING_SHOW				0x135
#define RPC_PARKING_HIDE				0x136
#define RPC_PARKING_CAR					0x137
#define RPC_PARKING_DIALOG				0x138
#define RPC_AZS							0x139
#define RPC_FINGER_PRINT				0x140
#define RPC_PLAYER_TAG					0x141
#define RPC_ADMIN_TOOLS					0x142
#define RPC_CONTAINER					0x143
#define RPC_FISHING						0x144
#define RPC_QUESTS						0x145
#define RPC_PLATES						0x146

#define RPC_CASINO						0x150

#define RPC_CHECK_ANDROID				0x160
#define RPC_ACTOR_ACTION				0x161

#define RPC_SPAWN_PACKET				0x170
#define RPC_BUSINESS					0x171
#define RPC_ACTION						0x172

#define RPC_RENT_SHOW					0x190
#define RPC_RENT_HIDE					0x191
#define RPC_RENT_SETUP					0x192

#define RPC_SHOP						0x200

#define RPC_VALENTINE_SHOW				0x210
#define RPC_VALENTINE_HIDE				0x211
#define RPC_VALENTINE_TASKS				0x212
#define RPC_VALENTINE_SEND				0x213
#define RPC_VALENTINE_SHOP				0x214
#define RPC_VALENTINE_TOP				0x215
#define RPC_VALENTINE_DESC				0x216
#define RPC_VALENTINE_LOBBY_SHOW		0x217
#define RPC_VALENTINE_LOBBY_STATE		0x218
#define RPC_VALENTINE_LOBBY_SETNAME		0x219
#define RPC_VALENTINE_LOBBY_READY		0x220
#define RPC_VALENTINE_LOBBY_HIDE		0x221
#define RPC_VALENTINE_ADD_TOP			0x222
bool m_bValentineRPC = false;
char m_ValentineChar[255];
char m_ValentineChar2[255];
char m_ValentineChar3[255];
char m_ValentineChar4[255];
char m_ValentineChar5[255];
char m_ValentineChar6[255];

#define RPC_BOATS_SHOW					0x230
#define RPC_BOATS_HIDE					0x231
#define RPC_BOATS_COLOR					0x232
#define RPC_BOATS_UPGRADE				0x233
#define RPC_BOATS_RENT					0x234
#define RPC_BOATS_SELECT				0x235

#define RPC_QUEST_SHOW					0x260
#define RPC_QUEST_HIDE					0x261

#define RPC_E2Y_SHOW					0x270
#define RPC_E2Y_HIDE					0x271
#define RPC_E2Y_SHOPITEM				0x272
#define RPC_E2Y_ROULETTEITEM			0x273
#define RPC_E2Y_PAYMENT					0x274
#define RPC_E2Y_TOPPLAYER				0x275

#define RPC_MOBILE_CLIENT				0x280

#include "../game/CCustomPlateManager.h"

#include "../util/CJavaWrapper.h"
#include "../util/armhook.h"

#include "../graphics/CSkyBox.h"
#include "../graphics/CInterface.h"

#include "../util/interfaces/CHud.h"
#include "../util/interfaces/CNewBattlePass.h"
#include "../util/interfaces/CDonate.h"
#include "../util/interfaces/CCasino.h"
#include "../util/interfaces/CValentine.h"
#include "../util/interfaces/CInventory.h"
#include "../util/interfaces/CBoats.h"
#include "../util/interfaces/CE2y.h"
#include "../util/interfaces/CNoty.h"
#include "../util/interfaces/CMusic.h"
#include "../util/interfaces/CAction.h"
#include "../util/interfaces/CFamily.h"
#include "../util/interfaces/CShop.h"
#include "../util/interfaces/CTrade.h"
#include "../util/interfaces/CRent.h"
#include "../util/interfaces/CVoice.h"
#include "../util/interfaces/CParking.h"
#include "../util/interfaces/CMarket.h"
#include "../util/interfaces/CCraft.h"
#include "../util/interfaces/CRoulette.h"
#include "../util/interfaces/CDialogs.h"
#include "../util/interfaces/CLoading.h"

#define NETGAME_VERSION 4057

/*

	Gold Russia

*/

//#define AUTH_BS "10B3D2B1317ADD02CC1F680BC500A8BC0FD7AD42CE7"
//#define AUTH_BS "E02262CF28BC542486C558D4BE9EFB716592AFAF8B"
//#define AUTH_BS "149C15C7E69314B147D55069C245763C07DD8AB4429" // last


/*

	Crime Mobile

*/

//#define AUTH_BS "15121F6F18550C00AC4B4F8A167D0379BB0ACA99043"
#define AUTH_BS "10B3D2B1317ADD02CC1F680BC500A8BC0FD7AD42CE7"
//#define AUTH_BS "E02262CF28BC542486C558D4BE9EFB716592AFAF8B"
//#define AUTH_BS "149C15C7E69314B147D55069C245763C07DD8AB4429"

extern CGame *pGame;
extern CChatWindow *pChatWindow;

int iVehiclePoolProcessFlag = 0;
int iPickupPoolProcessFlag = 0;
int iStreamPoolProcessFlag = 0;

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

CNetGame::CNetGame(const char* szHostOrIp, int iPort, const char* szPlayerName, const char* szPass)
{
	strcpy(m_szHostName, OBFUSCATE("San Andreas Multiplayer"));
	strncpy(m_szHostOrIp, szHostOrIp, sizeof(m_szHostOrIp));
	m_iPort = iPort;

	m_pPlayerPool = new CPlayerPool();
	m_pPlayerPool->SetLocalPlayerName(szPlayerName);
	
	Log("VehiclePool installing...");
	m_pVehiclePool = new CVehiclePool();
	Log("ObjectPool installing...");
	m_pObjectPool = new CObjectPool();
	Log("PickupPool installing...");
	m_pPickupPool = new CPickupPool();
	Log("GangZonePool installing...");
	m_pGangZonePool = new CGangZonePool();
	Log("3DTextPool installing...");
	m_pLabelPool = new CText3DLabelsPool();
	Log("TextDrawPool installing...");
	m_pTextDrawPool = new CTextDrawPool();
	Log("WidgetManagerPool installing...");
	g_pWidgetManager = new CWidgetManager();
	Log("StreamPool installing...");
	m_pStreamPool = new CStreamPool();
	Log("ActorPool installing...");
	m_pActorPool = new CActorPool();

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

	for (int i = 0; i < 100; i++)
	{
		m_dwMapIcons[i] = -1;
	}
	
	pGame->EnableClock(false);
	pGame->EnableZoneNames(false);

	//if(pChatWindow)
	//	pChatWindow->AddDebugMessage(OBFUSCATE("{FFFFFF}SA-MP {B9C9BF}" SAMP_VERSION " {FFFFFF}Started"));
}

#include "..//voice/CVoiceChatClient.h"
extern CVoiceChatClient* pVoice;
CNetGame::~CNetGame()
{
	m_pRakClient->Disconnect(0);

	UnRegisterRPCs(m_pRakClient);
	UnRegisterScriptRPCs(m_pRakClient);

	RakNetworkFactory::DestroyRakClientInterface(m_pRakClient);

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
}

extern uint8_t m_SelectedServer;

bool g_IsVoiceServer();
static bool once = false;
static uint32_t timeNetwork = GetTickCount();
void CNetGame::Process()
{
	UpdateNetwork();

	// server checkpoints update
	if (m_pPlayerPool)
	{
		if (m_pPlayerPool->GetLocalPlayer())
		{
			if (m_pPlayerPool->GetLocalPlayer()->m_bIsActive && m_pPlayerPool->GetLocalPlayer()->GetPlayerPed())
				pGame->UpdateCheckpoints();
		}
	}

	if(m_bHoldTime)
		pGame->SetWorldTime(m_byteWorldTime, m_byteWorldMinute);

	if(GetGameState() == GAMESTATE_CONNECTED)
	{
		// pool process
		if (m_pPlayerPool) m_pPlayerPool->Process();
		if (m_pObjectPool) m_pObjectPool->Process();
		if (m_pVehiclePool && iVehiclePoolProcessFlag > 5)
		{
			m_pVehiclePool->Process();
			iVehiclePoolProcessFlag = 0;
		}
		else
		{
			++iVehiclePoolProcessFlag;
		}

		if (m_pPickupPool && iPickupPoolProcessFlag > 5)
		{
			m_pPickupPool->Process();
			iPickupPoolProcessFlag = 0;
		}
		else
		{
			++iPickupPoolProcessFlag;
		}
		
		if (m_pStreamPool && iStreamPoolProcessFlag > 5)
		{
			m_pStreamPool->Process();
			iStreamPoolProcessFlag = 0;
		}
		else
		{
			++iStreamPoolProcessFlag;
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
			pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMsg(E_MSG::CONNECTING));

		CLoading::SetLoadingStatus("Подключение к серверу...");
		m_pRakClient->Connect(m_szHostOrIp, m_iPort, 0, 0, 5);
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
			CLoading::SetLoadingStatus("Проблемы с сетью, переподключение...");
			SetGameState(GAMESTATE_WAIT_CONNECT);
			break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
			pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMsg(E_MSG::FULL_SERVER));
			CLoading::SetLoadingStatus("Сервер полон, переподключение...");
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
			CLoading::SetLoadingStatus("Не правильный пароль от сервера");
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

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)
		pPlayer->StoreTrailerFullSyncData(&trSync);
}

extern bool openSpeedometr;
extern char MusicText[300];
extern float VehicleFuel;
extern float TimeFuel;
extern float floatVehicleMillage;
extern int VehicleMilleage;
void CNetGame::Packet_CustomRPC(Packet* p)
{
	RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
	uint8_t packetID;
	uint32_t rpcID;

	bs.Read(packetID);
	bs.Read(rpcID);

	switch (rpcID)
	{
		case RPC_OPEN_SETTINGS_JAVA:
		{
			Log("RPC_OPEN_SETTINGS_JAVA");
			g_pJavaWrapper->ShowClientSettings();
			break;
		}
		
		/*case RPC_SET_SKY_SPEED:
		{
			uint8_t speed;
			bs.Read(speed);

			CSkyBox::SetRotSpeed((float)speed / 1000.0f);
		}*/

		case RPC_CUSTOM_HANDLING_DEFAULTS:
		{
			Log("RPC_CUSTOM_HANDLING_DEFAULTS");
			uint16_t vehId;
			bs.Read(vehId);

			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
					pVeh->ResetVehicleHandling();
			}

			break;
		}

		case RPC_CUSTOM_VISUALS:
		{
			Log("RPC_CUSTOM_VISUALS");
			uint16_t vehId;
			uint8_t bLightsColor[3];
			uint16_t fWheelWidth;
			int8_t bWheelAlignX;
			int8_t bWheelAlignY;
			int16_t sWheelOffsetXX;
			int16_t sWheelOffsetXY;

			bs.Read(vehId);
			bs.Read(bLightsColor[0]);
			bs.Read(bLightsColor[1]);
			bs.Read(bLightsColor[2]);
			bs.Read(fWheelWidth);
			bs.Read(bWheelAlignX);
			bs.Read(bWheelAlignY);
			bs.Read(sWheelOffsetXX);
			bs.Read(sWheelOffsetXY);
			
			uint8_t bPlateType, bLen, bLenRegion;
			char szText[32];
			char szRegion[10];
			
			memset(szText, 0, sizeof(szText));
			memset(szRegion, 0, sizeof(szRegion));

			bs.Read(bPlateType);

			if (bPlateType == 2 || bPlateType == 5 || bPlateType == 6)
			{
				bs.Read(bLen);
				if (bLen >= sizeof(szText) - 1)
				{
					return;
				}
				bs.Read(&szText[0], bLen);

				bs.Read(bLenRegion);
				if (bLenRegion >= sizeof(szRegion) - 1)
				{
					return;
				}
				bs.Read(&szRegion[0], bLenRegion);
			}
			else if (bPlateType == 3 || bPlateType == 4)
			{
				bs.Read(bLen);
				if (bLen >= sizeof(szText) - 1)
				{
					return;
				}
				bs.Read(&szText[0], bLen);
			}

			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
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
					}
					if (sWheelOffsetXY)
					{
						float fValueY = (float)((float)sWheelOffsetXY / 100.0f);
						pVeh->SetWheelOffset(1, fValueY);
					}
					
					if (bPlateType)
					{
						CCustomPlateManager::PushPlate(vehId, (uint32_t)bPlateType, szText, szRegion);
					}
				}
			}
			break;
		}
		
		case RPC_CUSTOM_STROBS:
		{
			Log("RPC_CUSTOM_STROBS");
			uint16_t vehId;
			uint8_t bStrobsType;
			uint8_t bStrobsPolice;

			bs.Read(vehId);
			bs.Read(bStrobsType);
			bs.Read(bStrobsPolice);

			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
					pVeh->SetStrobs(bStrobsType, bStrobsPolice);
				}
			}
			break;
		}
		
		case RPC_CUSTOM_BODYWORK:
		{
			Log("RPC_CUSTOM_BODYWORK");
			uint16_t vehId;
			uint8_t bShadowColor[3];
			uint8_t bShadowSizeX, bShadowSizeY;
			uint8_t bLen;
			char szName[32], szStorage[32];
			
			memset(szName, 0, sizeof(szName));
			memset(szStorage, 0, sizeof(szStorage));

			bs.Read(vehId);
			bs.Read(bShadowColor[0]);
			bs.Read(bShadowColor[1]);
			bs.Read(bShadowColor[2]);
			bs.Read(bShadowSizeX);
			bs.Read(bShadowSizeY);
			bs.Read(bLen);
			bs.Read(&szName[0], bLen);
			bs.Read(bLen);
			bs.Read(&szStorage[0], bLen);
			
			uint8_t bToner[4];
			char szNameToner[32];
			
			memset(szNameToner, 0, sizeof(szNameToner));

			bs.Read(bToner[0]);
			bs.Read(bToner[1]);
			bs.Read(bToner[2]);
			bs.Read(bToner[3]);
			bs.Read(bLen);
			bs.Read(&szNameToner[0], bLen);
			
			uint8_t bVinyls[2];
			char szNameVinyls[32];
			
			memset(szNameVinyls, 0, sizeof(szNameVinyls));

			bs.Read(bVinyls[0]);
			bs.Read(bVinyls[1]);
			bs.Read(bLen);
			bs.Read(&szNameVinyls[0], bLen);
			
			uint8_t bStrobsType;
			uint8_t bStrobsPolice;

			bs.Read(bStrobsType);
			bs.Read(bStrobsPolice);
			
			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
					if (bShadowColor[0] != 0 || bShadowColor[1] != 0 || bShadowColor[2] != 0) pVeh->SetCustomShadow(bShadowColor[0], bShadowColor[1], bShadowColor[2], (float)bShadowSizeX / 10.0f, (float)bShadowSizeY / 10.0f, szName, szStorage);
					else pVeh->SetCustomShadow(bShadowColor[0], bShadowColor[1], bShadowColor[2], 0.0f, 0.0f, szName, szStorage);
					
					if (bToner[0] != 0 || bToner[1] != 0 || bToner[2] != 0)
					{
						uint32_t result = bToner[2] + (bToner[1] << 8) + (bToner[0] << 16) + (bToner[3] << 24);
						pVeh->ApplyTonerGenerate(1, 1, szNameToner, result);
						pVeh->ApplyTonerGenerate(2, 2, szNameToner, result);
						pVeh->ApplyTonerGenerate(3, 3, szNameToner, result);
					}
					else
					{
						uint32_t result = (255 << 24) + (255 << 16) + (255 << 8) + 255;
						pVeh->ApplyTonerGenerate(1, 0, szNameToner, result);
						pVeh->ApplyTonerGenerate(2, 0, szNameToner, result);
						pVeh->ApplyTonerGenerate(3, 0, szNameToner, result);
					}
					
					if (bVinyls[0] && bVinyls[1]) pVeh->ApplyVinyls(bVinyls[0], bVinyls[1], szNameVinyls);
					if (bStrobsType) pVeh->SetStrobs(bStrobsType, bStrobsPolice);
				}
			}
			break;
		}
		
		case RPC_CUSTOM_NEON:
		{
			Log("RPC_CUSTOM_NEON");
			uint16_t vehId;
			uint8_t bShadowColor[3];
			uint8_t bShadowSizeX, bShadowSizeY;
			uint8_t bLen;
			char szName[32], szStorage[32];
			
			memset(szName, 0, sizeof(szName));
			memset(szStorage, 0, sizeof(szStorage));

			bs.Read(vehId);
			bs.Read(bShadowColor[0]);
			bs.Read(bShadowColor[1]);
			bs.Read(bShadowColor[2]);
			bs.Read(bShadowSizeX);
			bs.Read(bShadowSizeY);
			bs.Read(bLen);

			if (bLen >= sizeof(szName) - 1)
				return;

			bs.Read(&szName[0], bLen);
			
			bs.Read(bLen);

			if (bLen >= sizeof(szStorage) - 1)
				return;
			
			bs.Read(&szStorage[0], bLen);
			
			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
					pVeh->SetCustomShadow(bShadowColor[0], bShadowColor[1], bShadowColor[2], (float)bShadowSizeX / 10.0f, (float)bShadowSizeY / 10.0f, szName, szStorage);
				}
			}
			break;
		}
		
		case RPC_CUSTOM_PLATE:
		{
			Log("RPC_CUSTOM_PLATE");
			uint16_t vehId;
			uint8_t bPlateType, bLen, bLenRegion;
			char szText[32];
			char szRegion[10];
			
			memset(szText, 0, sizeof(szText));
			memset(szRegion, 0, sizeof(szRegion));

			bs.Read(vehId);
			bs.Read(bPlateType);

			if (bPlateType == 2 || bPlateType == 5 || bPlateType == 6)
			{
				bs.Read(bLen);
				if (bLen >= sizeof(szText) - 1)
				{
					return;
				}
				bs.Read(&szText[0], bLen);

				bs.Read(bLenRegion);
				if (bLenRegion >= sizeof(szRegion) - 1)
				{
					return;
				}
				bs.Read(&szRegion[0], bLenRegion);
			}
			else if (bPlateType == 3 || bPlateType == 4)
			{
				bs.Read(bLen);
				if (bLen >= sizeof(szText) - 1)
				{
					return;
				}
				bs.Read(&szText[0], bLen);
			}
			
			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
					if (bPlateType)
					{
						CCustomPlateManager::PushPlate(vehId, (uint32_t)bPlateType, szText, szRegion);
					}
				}
			}
			break;
		}
		
		case RPC_CUSTOM_TONER:
		{
			Log("RPC_CUSTOM_TONER");
			uint16_t vehId;
			uint8_t bToner[3], bLen;
			char szName[32];
			
			memset(szName, 0, sizeof(szName));

			bs.Read(vehId);
			bs.Read(bToner[0]);
			bs.Read(bToner[1]);
			bs.Read(bToner[2]);

			bs.Read(bLen);

			if (bLen >= sizeof(szName) - 1)
				return;

			bs.Read(&szName[0], bLen);
			
			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
					pVeh->ApplyToner(1, bToner[0], szName);
					pVeh->ApplyToner(2, bToner[1], szName);
					pVeh->ApplyToner(3, bToner[2], szName);
				}
			}
			break;
		}
		
		case RPC_CUSTOM_VINYLS:
		{
			Log("RPC_CUSTOM_VINYLS");
			uint16_t vehId;
			uint8_t bVinyls[2], bLen;
			char szName[32];
			
			memset(szName, 0, sizeof(szName));

			bs.Read(vehId);
			bs.Read(bVinyls[0]);
			bs.Read(bVinyls[1]);

			bs.Read(bLen);

			if (bLen >= sizeof(szName) - 1)
				return;

			bs.Read(&szName[0], bLen);
			
			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
					pVeh->ApplyVinyls(bVinyls[0], bVinyls[1], szName);
				}
			}
			break;
		}
		
		case RPC_TUNING:
		{
			Log("RPC_TUNING");
			
			packetTuning(p);
			break;
		}

		case RPC_CUSTOM_ACTOR_PUT_IN_VEH:
		{
			Log("RPC_CUSTOM_ACTOR_PUT_IN_VEH");
			uint16_t actorId;
			VEHICLEID vehicleId;
			uint8_t seat;

			bs.Read(actorId);
			bs.Read(vehicleId);
			bs.Read(seat);

			CActorPool* pActorPool = GetActorPool();
			if (pActorPool && GetVehiclePool())
			{
				if (pActorPool->GetAt(actorId) && GetVehiclePool()->GetAt(vehicleId))
				{
					int iCarID = GetVehiclePool()->FindGtaIDFromID((int)vehicleId);

					pActorPool->GetAt(actorId)->PutDirectlyInVehicle(iCarID, (int)seat);
				}
			}
			break;
		}

		case RPC_CUSTOM_ACTOR_REMOVE_VEH:
		{
			Log("RPC_CUSTOM_ACTOR_REMOVE_VEH");
			uint16_t actorId;
			bs.Read(actorId);

			CActorPool* pActorPool = GetActorPool();
			if (pActorPool)
			{
				if (pActorPool->GetAt(actorId))
					pActorPool->GetAt(actorId)->RemoveFromVehicle();
			}
			break;
		}

		case RPC_CUSTOM_ACTOR_ADD_ADDITIONAL:
		{
			Log("RPC_CUSTOM_ACTOR_ADD_ADDITIONAL");
			uint16_t actorId;
			VEHICLEID vehicleId;
			uint8_t seat;

			bs.Read(actorId);
			bs.Read(vehicleId);
			bs.Read(seat);

			if (GetActorPool() && GetVehiclePool())
			{
				if (GetActorPool()->GetAt(actorId) && GetVehiclePool()->GetAt(vehicleId))
				{
					int iCarID = GetVehiclePool()->FindGtaIDFromID((int)vehicleId);

					GetActorPool()->GetAt(actorId)->PutDirectlyInVehicle(iCarID, (int)seat);
				}
			}
			break;
		}

		case RPC_CUSTOM_ADD_PED:
		{
			Log("RPC_CUSTOM_ADD_PED");
			uint16_t player;
			uint8_t moveAnim;
			bs.Read(player);
			bs.Read(moveAnim);
			
			if (m_pPlayerPool)
			{
				if (player == m_pPlayerPool->GetLocalPlayerID())
				{
					if (m_pPlayerPool->GetLocalPlayer())
					{
						if (m_pPlayerPool->GetLocalPlayer()->GetPlayerPed())
							m_pPlayerPool->GetLocalPlayer()->GetPlayerPed()->SetMoveAnim((int)moveAnim);
					}
				}

				if (m_pPlayerPool->GetAt(player))
				{
					if (m_pPlayerPool->GetAt(player)->GetPlayerPed())
						m_pPlayerPool->GetAt(player)->GetPlayerPed()->SetMoveAnim((int)moveAnim);
				}
			}
			break;
		}

		case RPC_CUSTOM_HANDLING:
		{
			Log("RPC_CUSTOM_HANDLING");
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
				
				if (fvalue == 0.0) continue;
				comps.push_back(SHandlingData(id, fvalue, 0));
			}
			if (m_pVehiclePool)
			{
				if (m_pVehiclePool->GetAt(veh))
				{
					m_pVehiclePool->GetAt(veh)->SetHandlingData(comps);
				}
			}
			break;
		}

		case RPC_CUSTOM_COMPONENT:
		{
			Log("RPC_CUSTOM_COMPONENT");
			uint16_t veh, extra_comp;
			uint8_t comp;
			bool comp_status;
			bs.Read(veh);
			
			CVehicle* pVehicle = nullptr;

			if (m_pVehiclePool) pVehicle = m_pVehiclePool->GetAt(veh);
			if (!pVehicle) return;

			bs.Read(comp);
			bs.Read(extra_comp);
			bs.Read(comp_status);
			pVehicle->BoolComponentVisible(comp, (uint16_t)extra_comp, comp_status);
			break;
		}

		/*case RPC_CUSTOM_COMPONENT:
		{
			uint16_t veh, extra_comp;
			uint8_t comp;
			bs.Read(veh);
			CVehicle* pVehicle = nullptr;

			if (m_pVehiclePool)
				pVehicle = m_pVehiclePool->GetAt(veh);

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
		}*/

		case CUSTOM_RPC_TOGGLE_HUD_ELEMENT:
		{
			Log("CUSTOM_RPC_TOGGLE_HUD_ELEMENT");
			uint8_t hud;
			bool toggle;
			bs.Read(hud);
			bs.Read(toggle);

			pGame->ToggleHUDElement(hud, toggle);
			pGame->HandleChangedHUDStatus();
			break;
		}
		
		case RPC_STREAM_CREATE:
		{
			Log("RPC_STREAM_CREATE");
			char str[255];
			
			uint8_t len, type;
			uint16_t id, vw, interior, playerId;

			VECTOR pos;

			float fDistance;
			bs.Read(id);
			bs.Read(type);
			bs.Read(playerId);
			bs.Read(pos.X);
			bs.Read(pos.Y);
			bs.Read(pos.Z);
			bs.Read(fDistance);
			bs.Read(vw);
			bs.Read(interior);
			bs.Read(len);
			bs.Read(&str[0], len);
			str[len] = 0;

			if (m_pStreamPool) GetStreamPool()->AddStream(id, type, playerId, &pos, vw, interior, fDistance, (const char*)&str[0]);
			break;
		}

		case RPC_STREAM_INDIVIDUAL:
		{
			Log("RPC_STREAM_INDIVIDUAL");
			char str[255];
			uint8_t len;

			bs.Read(len);
			bs.Read(&str[0], len);
			str[len] = 0;

			if (m_pStreamPool) GetStreamPool()->PlayIndividualStream(&str[0]);
			break;
		}

		case RPC_STREAM_POS:
		{
			Log("RPC_STREAM_POS");
			uint8_t iButton1, iButton2, iRecorder;

			bs.Read(iButton1);
			bs.Read(iButton2);
			bs.Read(iRecorder);

			bool _status = (pSettings->GetWrite().iVolumeActivate == 1) ? (true) : (false);
			CMusic::ShowAudioMenu(iRecorder, iButton1, iButton2, pSettings->GetWrite().iVolumeStream, _status, MusicText);
			break;
		}

		case RPC_STREAM_DESTROY:
		{
			Log("RPC_STREAM_DESTROY");
			uint32_t id;
			bs.Read(id);

			if (m_pStreamPool) m_pStreamPool->DeleteStreamByID(id);
			break;
		}

		case RPC_STREAM_VOLUME:
		{
			Log("RPC_STREAM_VOLUME");
			uint16_t id;
			float fVolume;
			bs.Read(id);
			bs.Read(fVolume);

			if (m_pStreamPool) m_pStreamPool->SetStreamVolume(id, fVolume);
			break;
		}

		case RPC_STREAM_ISENABLED:
		{
			Log("RPC_STREAM_ISENABLED");
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
			Log("RPC_OPEN_LINK");
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
		
		case RPC_CHECK_HUD:
		{
			Log("RPC_CHECK_HUD");
			
			uint8_t hudType;
			bs.Read(hudType);
			
			if (hudType == 1)
			{
				uint8_t bSatiety;
				bs.Read(bSatiety);
				
				uint32_t mDonate;
				bs.Read(mDonate);
				
				uint32_t mEventCoins;
				bs.Read(mEventCoins);
				
				bool bUsedEvent;
				bs.Read(bUsedEvent);
				
				bool bGZ;
				bs.Read(bGZ);
				
				bool bE2y;
				bs.Read(bE2y);

				CInterface::m_iSatiety = bSatiety;
				CInterface::m_iEventCoins = mEventCoins;
				CInterface::m_iDonate = mDonate;
				CInterface::m_bUsedEvent = bUsedEvent;
				CInterface::m_bGZ = bGZ;
				CInterface::m_bE2y = bE2y;
			}
			else if (hudType == 2)
			{
				int8_t iKey;
				bs.Read(iKey);
				CButtons::m_iKey[0] = iKey;
				
				int8_t iKey2;
				bs.Read(iKey2);
				CButtons::m_iKey[1] = iKey2;
				
				int8_t iKey3;
				bs.Read(iKey3);
				CButtons::m_iKey[2] = iKey3;
				
				int8_t iKey4;
				bs.Read(iKey4);
				CButtons::m_iKey[3] = iKey4;
				
				int8_t iKey5;
				bs.Read(iKey5);
				CButtons::m_iKey[4] = iKey5;
				
				int8_t iKey6;
				bs.Read(iKey6);
				CButtons::m_iKey[5] = iKey6;
				
				int8_t iKey7;
				bs.Read(iKey7);
				CButtons::m_iKey[6] = iKey7;
				
				if (CButtons::m_iKey[0] == -1 || pSettings->GetReadOnly().iHUD != 1)
				{
					CButtons::m_bIsShow = false;
					CButtons::m_bIsActive = false;
				}
				else
				{
					CButtons::m_bIsShow = false;
					CButtons::m_bIsActive = true;
				}
			}
			else if (hudType == 3)
			{
				bool bVisible;
				bs.Read(bVisible);
				
				bool bExtra;
				bs.Read(bExtra);
			
				char szText[128 + 1];
				memset(szText, 0, sizeof(szText) - 1);
				
				uint8_t bLen;
				bs.Read(bLen);
				
				bs.Read(szText, bLen);
				szText[bLen] = '\0';
			
				uint8_t iconID;
				bs.Read(iconID);
				
				CHud::buttonExtra(bVisible, bExtra, szText, iconID);
			}
			break;
		}
		
		case RPC_HUD_GIFT:
		{
			Log("RPC_HUD_GIFT");
			
			uint8_t iconID;
			bs.Read(iconID);
			
			char szText[128 + 1];
			memset(szText, 0, sizeof(szText) - 1);
			
			uint8_t bLen;
			bs.Read(bLen);
			
			bs.Read(szText, bLen);
			szText[bLen] = '\0';
			
			CHud::giftLayout(iconID, szText);
			break;
		}
		
		case RPC_HUD_GIFT_HIDE:
		{
			Log("RPC_HUD_GIFT_HIDE");
			
			CHud::giftHide();
			break;
		}
		
		case RPC_SPEEDOMETR:
		{
			Log("RPC_SPEEDOMETR");
			
			float vFuel;
			bs.Read(vFuel);
			
			float tFuel;
			bs.Read(tFuel);
			
			uint32_t vMillage;
			bs.Read(vMillage);
			
			bool bOpen;
			bs.Read(bOpen);
			
			VehicleFuel = vFuel;
			TimeFuel = tFuel;
			VehicleMilleage = vMillage;
			floatVehicleMillage = (float)vMillage;
			openSpeedometr = bOpen;
			break;
		}
		
		case RPC_CUSTOM_AIM:
		{
			Log("RPC_CUSTOM_AIM");
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
		
		case RPC_CUSTOM_VOICE_CONNECT:
		{
			Log("RPC_CUSTOM_VOICE_CONNECT");
			if (pVoice) pVoice->SetNetworkState(VOICECHAT_WAIT_CONNECT);
			break;
		}
		
		case RPC_CUSTOM_VOICE_DISCONNECT:
		{
			Log("RPC_CUSTOM_VOICE_DISCONNECT");
			if (pVoice) pVoice->FullDisconnect();
			break;
		}
		
		case RPC_CUSTOM_VOICE_VOLUME:
		{
			Log("RPC_CUSTOM_VOICE_VOLUME");
			uint8_t volume;
			bs.Read(volume);
			if (pVoice) pVoice->SetVolume(volume);
			break;
		}
		
		case RPC_CUSTOM_VOICE_MUTE:
		{
			Log("RPC_CUSTOM_VOICE_MUTE");
			uint16_t playermuted;
			bs.Read(playermuted);
			if (pVoice) pVoice->MutePlayer(playermuted);
			break;
		}
		
		case RPC_CUSTOM_VOICE_UNMUTE:
		{
			Log("RPC_CUSTOM_VOICE_UNMUTE");
			uint16_t playermuted;
			bs.Read(playermuted);
			if (pVoice) pVoice->UnMutePlayer(playermuted);
			break;
		}
		
		case RPC_CUSTOM_VOICE_VOLUMETOPLAYER:
		{
			Log("RPC_CUSTOM_VOICE_VOLUMETOPLAYER");
			uint16_t playervolume;
			uint8_t volume;
			
			bs.Read(playervolume);
			bs.Read(volume);
			
			if (pVoice) pVoice->SetVolumePlayer(playervolume, volume);
			break;
		}
		
		case RPC_CUSTOM_VOICE_SETTINGS:
		{
			Log("RPC_CUSTOM_VOICE_SETTINGS");
			CVoice::ShowVoice();
			break;
		}
		
		case RPC_CHECK_CASH:
		{
			Log("RPC_CHECK_CASH");
			uint8_t bLen, bLen1;
			uint16_t bVersion;
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
			
			uint32_t iVersion;
			if (pCashTexture) 
			{
				iVersion = bVersion;
				RwTextureDestroy(pCashTexture);
			}
			else iVersion = 0;

			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_CustomHash);
				
			bsParams.Write(iVersion);
			m_pRakClient->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
			break;
		}
		
		case RPC_SHOW_NOTIFICATION:
		{
			Log("RPC_SHOW_NOTIFICATION");
			
			packetNoty(p);
			break;
		}
		
		case RPC_DONATE:
		{
			Log("RPC_DONATE");
			
			packetDonate(p);
			break;
		}
		
		case RPC_BATTLE_STATS:
		{
			Log("RPC_BATTLE_STATS");
			
			packetNewBattlePass(p);
			break;
		}
		
		case RPC_BATTLE_PASS:
		{
			Log("RPC_BATTLE_PASS");
			
			packetBattlePass(p);
			break;
		}
		
		case RPC_NEWBIE_BATTLE:
		{
			Log("RPC_NEWBIE_BATTLE");
			
			packetNewBieBattle(p);
			break;
		}
		
		case RPC_TRADE:
		{
			Log("RPC_TRADE");

			packetTrade(p);
			break;
		}
		
		case RPC_LABEL_CUSTOM:
		{
			Log("RPC_LABEL_CUSTOM");
			
			CText3DLabelsPool* pLabelPool = GetLabelPool();
			if(!pLabelPool) {
				return;
			}
	
			uint16_t labelId;
			bool bShowBg = false;
			bool bShowIcon = false;

			bs.Read(labelId);
			bs.Read(bShowBg);
			bs.Read(bShowIcon);

			pLabelPool->SetShowBackground(labelId, bShowBg);

			if(bShowIcon)
			{
				char szBase[128];
				memset(szBase, 0, 128);

				uint16_t bLen;
				bs.Read(bLen);
				bs.Read(szBase, bLen);
				szBase[bLen] = '\0';
				
				char szTex[2048];
				memset(szTex, 0, 2048);

				bs.Read(bLen);
				bs.Read(szTex, bLen);
				szTex[bLen] = '\0';
			
				pLabelPool->SetIconTexture(labelId, szBase, szTex);
				pLabelPool->SetShowIcon(labelId, true);
			}
			else
			{
				pLabelPool->SetShowIcon(labelId, false);
			}
			break;
		}
		
		case RPC_INVENTORY:
		{
			Log("RPC_INVENTORY");
		
			packetInventory(p);
			break;
		}
		
		case RPC_DIALOGS:
		{
			Log("RPC_DIALOGS");
		
			packetDialogs(p);
			break;
		}
		
		case RPC_AUTH_REG:
		{
			Log("RPC_AUTH_REG");
			
			packetAuthReg(p);
			break;
		}
		
		case RPC_FAMILY_SHOW:
		{
			Log("RPC_FAMILY_SHOW");
			
			packetFamily(p);
			break;
		}
		
		case RPC_ROULETTE:
		{
			Log("RPC_ROULETTE");
			
			packetRoulette(p);
			break;
		}
		
		case RPC_CHAT:
		{
			Log("RPC_CHAT");
			
			packetChat(p);
			break;
		}
		
		case RPC_BINDERS:
		{
			Log("RPC_BINDERS");
			
			packetBinders(p);
			break;
		}
		
		case RPC_EMPLOYMENT_SHOW:
		{
			Log("RPC_EMPLOYMENT_SHOW");
			
			packetEmployment(p);
			break;
		}
		
		case RPC_MARKET:
		{
			Log("RPC_MARKET");
			
			packetMarket(p);
			break;
		}
		
		case RPC_CRAFT:
		{
			Log("RPC_CRAFT");
			
			packetCraft(p);
			break;
		}
		
		case RPC_OBJECT_EDIT:
		{
			Log("RPC_OBJECT_EDIT");
			
			packetEdit(p);
			break;
		}
		
		case RPC_RECON_ADMIN:
		{
			Log("RPC_RECON_ADMIN");
			
			packetRecon(p);
			break;
		}
		
		case RPC_PARKING_SHOW:
		{
			Log("RPC_PARKING_SHOW");
			
			CParking::showParking();
			break;
		}
		
		case RPC_PARKING_HIDE:
		{
			Log("RPC_PARKING_HIDE");
			
			CParking::hideParking();
			break;
		}
		
		case RPC_PARKING_CAR:
		{
			Log("RPC_PARKING_CAR");
			
			uint8_t max_items;
			bs.Read(max_items);
			for (int i = 0; i < max_items; i ++)
			{
				uint32_t ID;
				bs.Read(ID);
				
				uint8_t Type;
				bs.Read(Type);
				
				uint32_t Model;
				bs.Read(Model);
				
				char Name[128];
				memset(Name, 0, 128);
						
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(Name, bLen);
				Name[bLen] = '\0';
				
				uint8_t Color1;
				bs.Read(Color1);
				
				uint8_t Color2;
				bs.Read(Color2);
				
				float rotX;
				bs.Read(rotX);
				
				float rotY;
				bs.Read(rotY);
				
				float rotZ;
				bs.Read(rotZ);
				
				float Zoom;
				bs.Read(Zoom);
				
				uint32_t Status;
				bs.Read(Status);
				
				uint8_t Fuel;
				bs.Read(Fuel);
				
				uint8_t FuelStatus;
				bs.Read(FuelStatus);
				
				char TimeText[128];
				memset(TimeText, 0, 128);
						
				bs.Read(bLen);
				bs.Read(TimeText, bLen);
				TimeText[bLen] = '\0';
				
				CParking::addParking(ID, Type, Model, Name, Color1, Color2, rotX, rotY, rotZ, Zoom, Status, Fuel, FuelStatus, TimeText);
			}
			break;
		}
		
		case RPC_PARKING_DIALOG:
		{
			Log("RPC_PARKING_DIALOG");
			
			char caption[128];
			memset(caption, 0, 128);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(caption, bLen);
			caption[bLen] = '\0';

			char text[128];
			memset(text, 0, 128);
					
			bs.Read(bLen);
			bs.Read(text, bLen);
			text[bLen] = '\0';

			char buttonNo[128];
			memset(buttonNo, 0, 128);
					
			bs.Read(bLen);
			bs.Read(buttonNo, bLen);
			buttonNo[bLen] = '\0';

			char buttonYes[128];
			memset(buttonYes, 0, 128);
					
			bs.Read(bLen);
			bs.Read(buttonYes, bLen);
			buttonYes[bLen] = '\0';
			
			CParking::parkingDialog(caption, text, buttonNo, buttonYes);
			break;
		}
		
		case RPC_AZS:
		{
			Log("RPC_AZS");
			
			packetAzs(p);
			break;
		}

		case RPC_FINGER_PRINT:
		{
			Log("RPC_FINGER_PRINT");
			
			uint8_t _id;
			bs.Read(_id);
			
			char FingerText[128];
			char FingerText1[128];
			memset(FingerText, 0, 128);
			memset(FingerText1, 0, 128);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(FingerText, bLen);
			FingerText[bLen] = '\0';
			cp1251_to_utf8(FingerText1, FingerText, 128);
			
			g_pJavaWrapper->showFingerPrintAuth(_id, FingerText1);
			break;
		}
		
		case RPC_PLAYER_TAG:
		{
			Log("RPC_PLAYER_TAG");
			
			PLAYERID playerId;
			bs.Read(playerId);
			
			bool status;
			bs.Read(status);
			
			char tagText[128];
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(tagText, bLen);
			tagText[bLen] = '\0';
			
			uint8_t tagType;
			bs.Read(tagType);
			
			uint32_t tagColor;
			bs.Read(tagColor);
			
			uint32_t tagColorBox;
			bs.Read(tagColorBox);
			
			CPlayerTags::UpdatePlayerTags(playerId, status, tagText, tagType, tagColor, tagColorBox);
			break;
		}
		
		case RPC_ADMIN_TOOLS:
		{
			Log("RPC_ADMIN_TOOLS");
			
			packetAdminTools(p);
			break;
		}
		
		case RPC_CONTAINER:
		{
			Log("RPC_CONTAINER");
			
			packetContainer(p);
			break;
		}
		
		case RPC_FISHING:
		{
			Log("RPC_FISHING");
			
			packetFishing(p);
			break;
		}
		
		case RPC_QUESTS:
		{
			Log("RPC_QUESTS");
			
			packetQuests(p);
			break;
		}
		
		case RPC_PLATES:
		{
			Log("RPC_PLATES");
			
			packetPlates(p);
			break;
		}
		
		case RPC_CASINO:
		{
			Log("RPC_CASINO");
			
			packetCasino(p);
			break;
		}
		
		case RPC_CHECK_ANDROID:
		{
			Log("RPC_CHECK_ANDROID");
			
			int8_t id;
			bs.Read(id);
			
			g_pJavaWrapper->CheckAndroidID(id);
			break;
		}
		
		case RPC_ACTOR_ACTION:
		{
			packetActor(p);
			break;
		}
		
		case RPC_SPAWN_PACKET:
		{
			Log("RPC_SPAWN_PACKET");
			
			packetSpawn(p);
			break;
		}
		
		case RPC_BUSINESS:
		{
			Log("RPC_BUSINESS");
			
			packetBusiness(p);
			break;
		}
		
		case RPC_ACTION:
		{
			Log("RPC_ACTION");
			
			packetAction(p);
			break;
		}
		
		case RPC_CHATMESSAGE:
		{
			Log("RPC_CHATMESSAGE");
			
			uint32_t dwStrLen;
			uint32_t dwColor;

			bs.Read(dwColor);
			bs.Read(dwStrLen);
			
			char* szMsg = (char*)malloc(dwStrLen+1);
			memset(szMsg, 0, sizeof(szMsg) - 1);
			bs.Read(szMsg, dwStrLen);
			szMsg[dwStrLen] = 0;
			
			uint8_t dwType;
			uint32_t dwNumber;
			bs.Read(dwType);
			bs.Read(dwNumber);
			
			pChatWindow->FilterInvalidChars(szMsg);
			pChatWindow->AddToChatWindowBuffer(CHAT_TYPE_INFO, szMsg, nullptr, dwColor, 0, dwType, dwNumber);
			//g_pJavaWrapper->PushChatMessage(dwColor, szMsg, ' ', dwType, dwNumber);
			break;
		}
		
		case RPC_RADIALMENU:
		{
			Log("RPC_RADIALMENU");
			
			int8_t i;
			bs.Read(i);
			
			int8_t i2;
			bs.Read(i2);
			
			int8_t i3;
			bs.Read(i3);
			
			int8_t i4;
			bs.Read(i4);
			
			int8_t i5;
			bs.Read(i5);
			
			int8_t i6;
			bs.Read(i6);
			
			int8_t i7;
			bs.Read(i7);
			
			int8_t i8;
			bs.Read(i8);
			
			int8_t i9;
			bs.Read(i9);
			
			int8_t i10;
			bs.Read(i10);
			
			g_pJavaWrapper->ShowRadialMenu(i, i2, i3, i4, i5, i6, i7, i8, i9, i10);
			break;
		}
		
		case RPC_RADIALMENU_HIDE:
		{
			Log("RPC_RADIALMENU_HIDE");
			
			g_pJavaWrapper->HideRadialMenu();
			break;
		}
		
		case RPC_RENT_SHOW:
		{
			Log("RPC_RENT_SHOW");
			
			CRent::SetRentShow();
			break;
		}
		
		case RPC_RENT_HIDE:
		{
			Log("RPC_RENT_HIDE");
			
			CRent::SetRentHide();
			break;
		}
		
		case RPC_RENT_SETUP:
		{
			Log("RPC_RENT_SETUP");
			
			uint16_t modelId;
			bs.Read(modelId);
			
			uint16_t maxSpeed;
			bs.Read(maxSpeed);
			
			uint32_t rentPrice;
			bs.Read(rentPrice);
			
			CRent::SetRentUp(modelId, maxSpeed, rentPrice);
			break;
		}
		
		case RPC_SHOP:
		{
			Log("RPC_SHOP");
			
			packetShop(p);
			break;
		}
		
		case RPC_VALENTINE_SHOW:
		{
			Log("RPC_VALENTINE_SHOW");
			
			uint32_t valentineBox;
			bs.Read(valentineBox);
			
			uint8_t valentineQuest2;
			bs.Read(valentineQuest2);
			
			uint8_t valentineQuest3;
			bs.Read(valentineQuest3);
			
			uint32_t valentineGift;
			bs.Read(valentineGift);
			
			char valentineDate[32];
			char valentineDate1[32];
			memset(valentineDate, 0, 32);
			memset(valentineDate1, 0, 32);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(valentineDate, bLen);
			valentineDate[bLen] = '\0';
			cp1251_to_utf8(valentineDate1, valentineDate, 32);
			
			uint32_t playerValentine;
			bs.Read(playerValentine);
			
			if (m_bValentineRPC == false)
			{
				char s[255];
				memset(s, 0, 255);
				memset(m_ValentineChar, 0, 255);
						
				bs.Read(bLen);
				bs.Read(s, bLen);
				s[bLen] = '\0';
				cp1251_to_utf8(m_ValentineChar, s, 255);
				
				char s2[255];
				memset(s2, 0, 255);
				memset(m_ValentineChar2, 0, 255);
						
				bs.Read(bLen);
				bs.Read(s2, bLen);
				s2[bLen] = '\0';
				cp1251_to_utf8(m_ValentineChar2, s2, 255);
				
				char s3[255];
				memset(s3, 0, 255);
				memset(m_ValentineChar3, 0, 255);
						
				bs.Read(bLen);
				bs.Read(s3, bLen);
				s3[bLen] = '\0';
				cp1251_to_utf8(m_ValentineChar3, s3, 255);
				
				char s4[255];
				memset(s4, 0, 255);
				memset(m_ValentineChar4, 0, 255);
						
				bs.Read(bLen);
				bs.Read(s4, bLen);
				s4[bLen] = '\0';
				cp1251_to_utf8(m_ValentineChar4, s4, 255);
				
				char s5[255];
				memset(s5, 0, 255);
				memset(m_ValentineChar5, 0, 255);
						
				bs.Read(bLen);
				bs.Read(s5, bLen);
				s5[bLen] = '\0';
				cp1251_to_utf8(m_ValentineChar5, s5, 255);
				
				char s6[255];
				memset(s6, 0, 255);
				memset(m_ValentineChar6, 0, 255);
						
				bs.Read(bLen);
				bs.Read(s6, bLen);
				s6[bLen] = '\0';
				cp1251_to_utf8(m_ValentineChar6, s6, 255);
				
				m_bValentineRPC = true;
			}
			
			CValentine::valentineShowMainPage(valentineBox, valentineQuest2, valentineQuest3, valentineGift, valentineDate1, playerValentine, 
													m_ValentineChar, m_ValentineChar2, m_ValentineChar3, m_ValentineChar4, m_ValentineChar5, m_ValentineChar6);
			break;
		}

		case RPC_VALENTINE_HIDE:
		{
			Log("RPC_VALENTINE_HIDE");
			
			CValentine::valentineHide();
			break;
		}
		
		case RPC_VALENTINE_TASKS:
		{
			Log("RPC_VALENTINE_TASKS");
			
			uint8_t max_items;
			bs.Read(max_items);
			
			uint8_t playerQuest[max_items];
			for (int i = 0; i < max_items; i ++)
			{
				bs.Read(playerQuest[i]);
			}
			
			uint8_t playerStatus[max_items];
			for (int i = 0; i < max_items; i ++)
			{
				bs.Read(playerStatus[i]);
			}
			
			CValentine::valentineShowTaskPage(playerQuest[0], playerQuest[1], playerQuest[2], playerQuest[3],
													playerStatus[0], playerStatus[1], playerStatus[2], playerStatus[3]);
			break;
		}
		
		case RPC_VALENTINE_SEND:
		{
			Log("RPC_VALENTINE_SEND");
			
			uint8_t max_items;
			bs.Read(max_items);
			
			uint16_t playerBox[max_items];
			for (int i = 0; i < max_items; i ++)
			{
				bs.Read(playerBox[i]);
			}
			
			CValentine::valentineShowSendPage(playerBox[0], playerBox[1], playerBox[2], playerBox[3], playerBox[4]);
			break;
		}
		
		case RPC_VALENTINE_SHOP:
		{
			Log("RPC_VALENTINE_SHOP");
			
			uint8_t max_items;
			bs.Read(max_items);
			
			uint32_t valentinePrice[max_items];
			for (int i = 0; i < max_items; i ++)
			{
				bs.Read(valentinePrice[i]);
			}
			
			bs.Read(max_items);
			uint32_t valentineModel[max_items];
			for (int i = 0; i < max_items; i ++)
			{
				bs.Read(valentineModel[i]);
			}
			
			CValentine::valentineShowShopPage(valentinePrice[0], valentinePrice[1], valentinePrice[2], valentinePrice[3], valentinePrice[4],
													valentinePrice[5], valentinePrice[6], valentinePrice[7], valentinePrice[8], valentinePrice[9], valentinePrice[10],
													valentineModel[0], valentineModel[1], valentineModel[2]);
			break;
		}
		
		case RPC_VALENTINE_TOP:
		{
			Log("RPC_VALENTINE_TOP");
			
			CValentine::valentineShowTopPage();
			break;
		}
		
		case RPC_VALENTINE_DESC:
		{
			Log("RPC_VALENTINE_DESC");
			
			CValentine::valentineShowDescPage();
			break;
		}
		
		case RPC_VALENTINE_LOBBY_SHOW:
		{
			Log("RPC_VALENTINE_LOBBY_SHOW");
			
			uint8_t lobbyId;
			bs.Read(lobbyId);
			
			CValentine::valentineShowLobbyPage(lobbyId);
			break;
		}
		
		case RPC_VALENTINE_LOBBY_STATE:
		{
			Log("RPC_VALENTINE_LOBBY_STATE");
			
			uint8_t stateId;
			bs.Read(stateId);
			
			CValentine::valentineLobbySetState(stateId);
			break;
		}
		
		case RPC_VALENTINE_LOBBY_SETNAME:
		{
			Log("RPC_VALENTINE_LOBBY_SETNAME");
			
			uint8_t position;
			bs.Read(position);
			
			char playerName[MAX_PLAYER_NAME];
			char playerName1[MAX_PLAYER_NAME];
			memset(playerName, 0, MAX_PLAYER_NAME);
			memset(playerName1, 0, MAX_PLAYER_NAME);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(playerName, bLen);
			playerName[bLen] = '\0';
			cp1251_to_utf8(playerName1, playerName, MAX_PLAYER_NAME);
			
			CValentine::valentineLobbySetName(position, playerName1);
			break;
		}
		
		case RPC_VALENTINE_LOBBY_READY:
		{
			Log("RPC_VALENTINE_LOBBY_READY");
			
			uint8_t position;
			bs.Read(position);
			
			uint8_t ready;
			bs.Read(ready);
			
			CValentine::valentineLobbySetReady(position, ready);
			break;
		}
		
		case RPC_VALENTINE_LOBBY_HIDE:
		{
			Log("RPC_VALENTINE_LOBBY_HIDE");
			
			CValentine::valentineLobbyHide();
			break;
		}
		
		case RPC_VALENTINE_ADD_TOP:
		{
			Log("RPC_VALENTINE_ADD_TOP");
			
			uint8_t max_items;
			bs.Read(max_items);
			
			for (int q = 0; q < 4; q ++)
			{
				for (int i = 0; i < max_items; i ++)
				{
					uint8_t topId;
					bs.Read(topId);
					
					uint8_t position;
					bs.Read(position);
					
					char playerName[MAX_PLAYER_NAME];
					char playerName1[MAX_PLAYER_NAME];
					memset(playerName, 0, MAX_PLAYER_NAME);
					memset(playerName1, 0, MAX_PLAYER_NAME);
					
					uint8_t bLen;
					bs.Read(bLen);
					bs.Read(playerName, bLen);
					playerName[bLen] = '\0';
					cp1251_to_utf8(playerName1, playerName, MAX_PLAYER_NAME);
					
					uint32_t result;
					bs.Read(result);
					
					CValentine::valentineAddTopItem(topId, position, playerName, result);
				}
			}
			break;
		}
		
		case RPC_BOATS_SHOW:
		{
			Log("RPC_BOATS_SHOW");
			
			uint8_t fragmentId;
			bs.Read(fragmentId);
			
			CBoats::boatsShow(fragmentId);
			break;
		}
		
		case RPC_BOATS_HIDE:
		{
			Log("RPC_BOATS_HIDE");
			
			CBoats::boatsHide();
			break;
		}
		
		case RPC_BOATS_COLOR:
		{
			Log("RPC_BOATS_COLOR");
			
			uint8_t oldcolor;
			bs.Read(oldcolor);
			
			uint32_t cost;
			bs.Read(cost);
			
			uint8_t type;
			bs.Read(type);
			
			uint16_t modelId;
			bs.Read(modelId);
			
			CBoats::boatsColor(oldcolor, cost, type, modelId);
			break;
		}
		
		case RPC_BOATS_UPGRADE:
		{
			Log("RPC_BOATS_UPGRADE");
			
			uint16_t modelId;
			bs.Read(modelId);
			
			uint8_t type;
			bs.Read(type);
			
			uint8_t max_items;
			bs.Read(max_items);
			
			uint8_t lvl[max_items];
			int32_t cost[max_items];
			for (int i = 0; i < max_items; i ++)
			{
				bs.Read(lvl[i]);
				bs.Read(cost[i]);
			}
			
			bs.Read(max_items);
			int16_t stats[max_items];
			for (int i = 0; i < max_items; i ++)
			{
				bs.Read(stats[i]);
			}
			
			CBoats::boatsUpgrade(modelId, type, 
								lvl[0], lvl[1], lvl[2], lvl[3], lvl[4], lvl[5],
								cost[0], cost[1], cost[2], cost[3], cost[4], cost[5],
								stats[0], stats[1], stats[2], stats[3]);
			break;
		}
		
		case RPC_BOATS_RENT:
		{
			Log("RPC_BOATS_RENT");
			
			uint8_t max_items;
			bs.Read(max_items);
			for (int i = 0; i < max_items; i ++)
			{
				uint8_t id;
				bs.Read(id);
				
				uint8_t _typeVehicle;
				bs.Read(_typeVehicle);
				
				uint16_t speed;
				bs.Read(speed);
				
				uint32_t cost;
				bs.Read(cost);
				
				uint8_t type;
				bs.Read(type);
				
				uint16_t modelId;
				bs.Read(modelId);
				
				CBoats::boatsRent(id, _typeVehicle, speed, cost, type, modelId);
			}
			break;
		}
		
		case RPC_BOATS_SELECT:
		{
			Log("RPC_BOATS_SELECT");
			
			uint8_t max_items;
			bs.Read(max_items);
			for (int i = 0; i < max_items; i ++)
			{
				uint32_t id;
				bs.Read(id);
				
				char vehicleName[MAX_PLAYER_NAME];
				char vehicleName1[MAX_PLAYER_NAME];
				memset(vehicleName, 0, MAX_PLAYER_NAME);
				memset(vehicleName1, 0, MAX_PLAYER_NAME);
				
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(vehicleName, bLen);
				vehicleName[bLen] = '\0';
				cp1251_to_utf8(vehicleName1, vehicleName, MAX_PLAYER_NAME);
				
				uint16_t speed;
				bs.Read(speed);
				
				bool isLoaded;
				bs.Read(isLoaded);
				
				uint8_t type;
				bs.Read(type);
				
				uint16_t modelId;
				bs.Read(modelId);
				
				CBoats::boatsSelect(id, vehicleName1, speed, isLoaded, type, modelId);
			}
			break;
		}
		
		case RPC_QUEST_SHOW:
		{
			Log("RPC_QUEST_SHOW");
			
			char questCaption[128];
			char questCaption1[128];
			memset(questCaption, 0, 128);
			memset(questCaption1, 0, 128);
					
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(questCaption, bLen);
			questCaption[bLen] = '\0';
			cp1251_to_utf8(questCaption1, questCaption, 128);
			
			char questText[128];
			char questText1[128];
			memset(questText, 0, 128);
			memset(questText1, 0, 128);
					
			bs.Read(bLen);
			bs.Read(questText, bLen);
			questText[bLen] = '\0';
			cp1251_to_utf8(questText1, questText, 128);
			
			char questButton[128];
			char questButton1[128];
			memset(questButton, 0, 128);
			memset(questButton1, 0, 128);
					
			bs.Read(bLen);
			bs.Read(questButton, bLen);
			questButton[bLen] = '\0';
			cp1251_to_utf8(questButton1, questButton, 128);
			
			bool questCancel;
			bs.Read(questCancel);
			
			bool showQuest;
			bs.Read(showQuest);
			
			CHud::questShow(questCaption1, questText1, questButton1, questCancel, showQuest);
			break;
		}
		
		case RPC_QUEST_HIDE:
		{
			Log("RPC_QUEST_HIDE");
			
			CHud::questHide();
			break;
		}
		
		case RPC_E2Y_SHOW:
		{
			Log("RPC_E2Y_SHOW");
			
			uint8_t max_items;
			bs.Read(max_items);
			for (int i = 0; i < max_items; i ++)
			{
				char questName[128];
				memset(questName, 0, 128);
				
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(questName, bLen);
				questName[bLen] = '\0';
				
				char questProgress[128];
				memset(questProgress, 0, 128);
				
				bs.Read(bLen);
				bs.Read(questProgress, bLen);
				questProgress[bLen] = '\0';
				
				CE2y::e2yQuest(0, i, questName, questProgress, "");
			}
			
			bs.Read(max_items);
			for (int i = 0; i < max_items; i ++)
			{
				char questName[128];
				memset(questName, 0, 128);
				
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(questName, bLen);
				questName[bLen] = '\0';
				
				char questProgress[128];
				memset(questProgress, 0, 128);
				
				bs.Read(bLen);
				bs.Read(questProgress, bLen);
				questProgress[bLen] = '\0';
				
				CE2y::e2yQuest(1, i, questName, questProgress, "");
			}
			
			bs.Read(max_items);
			for (int i = 0; i < max_items; i ++)
			{
				char infoString[512];
				memset(infoString, 0, 512);
				
				uint16_t bLen;
				bs.Read(bLen);
				bs.Read(infoString, bLen);
				infoString[bLen] = '\0';
				
				char prizeString[128];
				memset(prizeString, 0, 128);
				
				uint8_t bLen2;
				bs.Read(bLen2);
				bs.Read(prizeString, bLen2);
				prizeString[bLen2] = '\0';
				
				char buttonString[128];
				memset(buttonString, 0, 128);
				
				bs.Read(bLen2);
				bs.Read(buttonString, bLen2);
				buttonString[bLen2] = '\0';
				
				CE2y::e2yQuest(2, i, infoString, prizeString, buttonString);
			}
			
			int32_t iBalance;
			bs.Read(iBalance);
			
			char dateString[128];
			memset(dateString, 0, 128);
			
			uint8_t bLen;
			bs.Read(bLen);
			bs.Read(dateString, bLen);
			dateString[bLen] = '\0';
			
			CE2y::e2yShow(iBalance, dateString);
			break;
		}
		
		case RPC_E2Y_HIDE:
		{
			Log("RPC_E2Y_HIDE");
			
			CE2y::e2yHide();
			break;
		}
		
		case RPC_E2Y_SHOPITEM:
		{
			Log("RPC_E2Y_SHOPITEM");
			
			uint8_t max_items;
			bs.Read(max_items);
			
			for (int i = 0; i < max_items; i ++)
			{
				uint32_t modelId;
				bs.Read(modelId);
				
				uint8_t renderType;
				bs.Read(renderType);
				
				uint8_t color1;
				bs.Read(color1);
				
				uint8_t color2;
				bs.Read(color2);
				
				uint8_t rareType;
				bs.Read(rareType);
				
				uint32_t price;
				bs.Read(price);
				
				char itemName[128];
				memset(itemName, 0, 128);
				
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(itemName, bLen);
				itemName[bLen] = '\0';
				
				CE2y::e2yShopItem(modelId, renderType, color1, color2, rareType, price, itemName);
			}
			break;
		}
		
		case RPC_E2Y_ROULETTEITEM:
		{
			Log("RPC_E2Y_ROULETTEITEM");
			
			uint8_t max_items;
			bs.Read(max_items);
			
			for (int i = 0; i < max_items; i ++)
			{
				uint32_t modelId;
				bs.Read(modelId);
				
				uint8_t renderType;
				bs.Read(renderType);
				
				uint8_t color1;
				bs.Read(color1);
				
				uint8_t color2;
				bs.Read(color2);
				
				uint8_t rareType;
				bs.Read(rareType);
				
				uint32_t price;
				bs.Read(price);
				
				char itemName[128];
				memset(itemName, 0, 128);
				
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(itemName, bLen);
				itemName[bLen] = '\0';
				
				CE2y::e2yRouletteItem(modelId, renderType, color1, color2, rareType, price, itemName);
			}
			break;
		}
		
		case RPC_E2Y_PAYMENT:
		{
			Log("RPC_E2Y_PAYMENT");
			
			uint32_t modelId;
			bs.Read(modelId);
			
			CE2y::e2yPaymentOkay(modelId);
			break;
		}
		
		case RPC_E2Y_TOPPLAYER:
		{
			Log("RPC_E2Y_TOPPLAYER");
			
			uint8_t max_items;
			bs.Read(max_items);
			
			for (int i = 0; i < max_items; i ++)
			{
				uint8_t topID;
				bs.Read(topID);
				
				char playerName[MAX_PLAYER_NAME];
				memset(playerName, 0, MAX_PLAYER_NAME);
				
				uint8_t bLen;
				bs.Read(bLen);
				bs.Read(playerName, bLen);
				playerName[bLen] = '\0';
				
				bool prem_status;
				bs.Read(prem_status);
				
				uint32_t count;
				bs.Read(count);
				
				CE2y::e2yTopPlayer(topID, playerName, prem_status, count);
			}
			break;
		}
		
		case RPC_MOBILE_CLIENT:
		{
			Log("[IsModelMobile] so good.");

			char byteAuthBSLen = (char)strlen(AUTH_BS);

			RakNet::BitStream bsParams;
			bsParams.Write((uint8_t)ID_CUSTOM_RPC);
			bsParams.Write((uint32_t)RPC_MobileClient);

			bsParams.Write(byteAuthBSLen);
			bsParams.Write(AUTH_BS, byteAuthBSLen);
			m_pRakClient->Send(&bsParams, HIGH_PRIORITY, RELIABLE, 0);
			break;
		}
	}
}

void CNetGame::ResetVehiclePool()
{
	Log(OBFUSCATE("ResetVehiclePool"));
	if(m_pVehiclePool)
		delete m_pVehiclePool;

	m_pVehiclePool = new CVehiclePool();
}

void CNetGame::ResetObjectPool()
{
	Log(OBFUSCATE("ResetObjectPool"));
	if(m_pObjectPool)
		delete m_pObjectPool;

	m_pObjectPool = new CObjectPool();
}

void CNetGame::ResetPickupPool()
{
	Log(OBFUSCATE("ResetPickupPool"));
	if(m_pPickupPool)
		delete m_pPickupPool;

	m_pPickupPool = new CPickupPool();
}

void CNetGame::ResetGangZonePool()
{
	Log(OBFUSCATE("ResetGangZonePool"));
	if(m_pGangZonePool)
		delete m_pGangZonePool;

	m_pGangZonePool = new CGangZonePool();
}

void CNetGame::ResetLabelPool()
{
	Log(OBFUSCATE("ResetLabelPool"));
	if(m_pLabelPool)
		delete m_pLabelPool;

	m_pLabelPool = new CText3DLabelsPool();
}

void CNetGame::ResetActorPool()
{
	Log(OBFUSCATE("ResetActorPool"));
	if (m_pActorPool)
		delete m_pActorPool;

	m_pActorPool = new CActorPool();
}

void CNetGame::ResetTextDrawPool()
{
	Log(OBFUSCATE("ResetTextDrawPool"));
	if (m_pTextDrawPool)
		delete m_pTextDrawPool;

	m_pTextDrawPool = new CTextDrawPool();
}

extern int RemoveModelIDs[1200];
extern VECTOR RemovePos[1200];
extern float RemoveRad[1200];
extern int iTotalRemovedObjects;

void CNetGame::ShutDownForGameRestart()
{
	iTotalRemovedObjects = 0;
	for (int & RemoveModelID : RemoveModelIDs)
		RemoveModelID = -1;

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
	ResetTextDrawPool();
	ResetActorPool();

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

	m_pRakClient->RPC(&RPC_Chat, &bsSend, HIGH_PRIORITY,RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
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

void CNetGame::SendDialogResponse(uint16_t wDialogID, uint8_t byteButtonID, uint16_t wListBoxItem, char* szInput)
{
	uint8_t respLen = strlen(szInput);

	RakNet::BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(byteButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(respLen);
	bsSend.Write(szInput, respLen);
	m_pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);

    pGUI->m_imWindow = nullptr;
}

void CNetGame::SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, int iColor, int style)
{
	if (byteIndex >= 100)
		return;

	if(m_dwMapIcons[byteIndex] != -1) DisableMapIcon(byteIndex);

	m_dwMapIcons[byteIndex] = pGame->CreateRadarMarkerIcon(byteIcon, fX, fY, fZ, iColor, style);
}

void CNetGame::DisableMapIcon(uint8_t byteIndex)
{
	if (byteIndex >= 100)
		return;
	
	if (m_dwMapIcons[byteIndex] == -1)
		return;

	((void(*)(int))(SA_ADDR(0x3DC27C + 1)))(m_dwMapIcons[byteIndex]);
	//ScriptCommand(&disable_marker, m_dwMapIcons[byteIndex]);
	m_dwMapIcons[byteIndex] = -1;
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
		CRemotePlayer *pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer) m_pPlayerPool->Delete(playerId, 0);
	}

	CLoading::SetLoadingStatus("Потеряно соеденение с сервером, переподключение...");
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
	
	CLoading::SetLoadingStatus("Вход в игру");
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

	m_pPlayerPool->SetLocalPlayerID(MyPlayerID);

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

	//CClientInfo::WriteClientInfoToBitStream(bsSend);

	m_pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);

	RakNet::BitStream bsSends;
	bsSends.Write((uint8_t)ID_CUSTOM_RPC);
	bsSends.Write((uint32_t)RPC_MobileClient);
	
	bsSends.Write(byteAuthBSLen);
	bsSends.Write(AUTH_BS, byteAuthBSLen);

    m_pRakClient->Send(&bsSends, HIGH_PRIORITY, RELIABLE, 0);
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

	// VECTOR POS
	bsPlayerSync.Read((char*)&ofSync.vecPos,sizeof(VECTOR));

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
    ofSync.vecMoveSpeed.X = tx;
    ofSync.vecMoveSpeed.Y = ty;
    ofSync.vecMoveSpeed.Z = tz;

    bsPlayerSync.Read(bHasVehicleSurfingInfo);
    if (bHasVehicleSurfingInfo)
    {
        bsPlayerSync.Read(ofSync.wSurfInfo);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.X);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.Y);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.Z);
    }
    else
        ofSync.wSurfInfo = INVALID_VEHICLE_ID;

	bool bHasAnimInfo;
	bsPlayerSync.Read(bHasAnimInfo);

	if (bHasAnimInfo)
		bsPlayerSync.Read(ofSync.dwAnimation);
	else ofSync.dwAnimation = 0b10000000000000000000000000000000;

	uint8_t key = 0;

    if(m_pPlayerPool)
    {
    	pPlayer = m_pPlayerPool->GetAt(playerId);
    	if(pPlayer)
    		pPlayer->StoreOnFootFullSyncData(&ofSync, 0, key);
    }
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
	bsSync.Read((char*)&icSync.vecPos, sizeof(VECTOR));

	// speed
	bsSync.ReadVector(icSync.vecMoveSpeed.X, icSync.vecMoveSpeed.Y, icSync.vecMoveSpeed.Z);

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

	if(m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
			pPlayer->StoreInCarFullSyncData(&icSync, 0);
	}
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

			if(playerId < MAX_PLAYERS && m_pPlayerPool->GetSlotState(playerId))
			{
				pPlayer = m_pPlayerPool->GetAt(playerId);
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

	CRemotePlayer* pRemotePlayer = m_pPlayerPool->GetAt(PlayerID);
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

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)
		pPlayer->UpdateAimFromSyncData(&aimSync);
}
