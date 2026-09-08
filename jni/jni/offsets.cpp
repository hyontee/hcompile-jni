#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	// 32 bit
	Add(xorstr("RwInitialised"), 0x2ABBCD, eArchType::ARM);
	Add(xorstr("RsGlobal"), 0x4F38E3C, eArchType::ARM);
	Add(xorstr("CNetGame::ProcessNetwork"), 0x1C5611, eArchType::ARM);
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x1C6425, eArchType::ARM);
	
Add(xorstr("CNetGame::m_pRakClient"), 0x49527B4, eArchType::ARM);
Add(xorstr("CNetGame::m_iGameState"), 0x49527B8, eArchType::ARM);
Add(xorstr("CNetGame::m_bIsX2"), 0x49527A9, eArchType::ARM);
Add(xorstr("CNetGame::m_byteWorldTime"), 0x49527AB, eArchType::ARM);
Add(xorstr("CNetGame::m_byteWeather"), 0x49527AD, eArchType::ARM);
Add(xorstr("CNetGame::m_pPlayerPool"), 0x49527BC, eArchType::ARM);
Add(xorstr("CNetGame::m_pVehiclePool"), 0x49527C0, eArchType::ARM);
Add(xorstr("CNetGame::m_pPickupPool"), 0x49527C4, eArchType::ARM);
Add(xorstr("CNetGame::m_pTextLabelPool"), 0x49527C8, eArchType::ARM);
Add(xorstr("CNetGame::m_pTextDrawPool"), 0x49527CC, eArchType::ARM);
Add(xorstr("CNetGame::m_pGangZonePool"), 0x49527D0, eArchType::ARM);
Add(xorstr("CNetGame::m_pActorPool"), 0x49527D4, eArchType::ARM);
Add(xorstr("CNetGame::m_pObjectPool"), 0x49527D8, eArchType::ARM);
Add(xorstr("CNetGame::m_pChatBubblePool"), 0x49527DC, eArchType::ARM);

	Add(xorstr("CNetVehiclePool::New"), 0x215915, eArchType::ARM);
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x20FC45, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0x210469, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x210ED1, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x211341, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x211065, eArchType::ARM);

	Add(xorstr("CChat::AddDebugMessage"), 0x258E5D, eArchType::ARM);
	Add(xorstr("RakClient::RegisterAsRemoteProcedureCall"), 0x30AF51, eArchType::ARM);
	Add(xorstr("CNetGame::Packet_GUI"), 0x1C6E95, eArchType::ARM);
        Add(xorstr("CNetGame::Packet_Turnlights"), 0x1C6F4C, eArchType::ARM);
}

void COffset::Add(const char* name, uintptr_t addr, eArchType arch)
{
	stOffset nOffset;
	nOffset.name = name;
	nOffset.addr = addr;
	nOffset.arch = arch;
	m_offsets.push_back(nOffset);
}

uintptr_t COffset::Get(const char* name)
{
	uintptr_t result = 0;
	for(int i = 0; i < m_offsets.size(); i++)
	{
#ifdef __aarch64__
		if(m_offsets[i].arch == eArchType::ARM64)
		{
#endif
#ifdef __arm__
		if(m_offsets[i].arch == eArchType::ARM)
		{
#endif
		if(!strcasecmp(name, m_offsets[i].name.c_str()))
		{
			result = m_offsets[i].addr;
			break;
		}
		}
	}
	return result;
}
