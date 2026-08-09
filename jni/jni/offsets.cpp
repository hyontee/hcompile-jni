#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	// 32 bit
	Add(xorstr("RwInitialised"), 0x4849729, eArchType::ARM);
	Add(xorstr("RsGlobal"), 0x4E4723C, eArchType::ARM);
	Add(xorstr("CNetGame::ProcessNetwork"), 0x2EB941, eArchType::ARM);
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x2EC701, eArchType::ARM);
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x486F30C, eArchType::ARM);
	uintptr_t ng_pRakClient = Get(xorstr("CNetGame::m_pRakClient"));
	Add(xorstr("CNetGame::m_iGameState"), ng_pRakClient + 4, eArchType::ARM);
	Add(xorstr("CNetGame::m_pPlayerPool"), ng_pRakClient + 8, eArchType::ARM);
	Add(xorstr("CNetGame::m_pVehiclePool"), ng_pRakClient + 12, eArchType::ARM);
	Add(xorstr("CNetVehiclePool::New"), 0x346AC1, eArchType::ARM);
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x33F015, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0x33F0F5, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x340691, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x340B8D, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x3408A5, eArchType::ARM);
	
	Add(xorstr("RakClient::RegisterAsRemoteProcedureCall"), 0x451FED, eArchType::ARM);
	Add(xorstr("CChat::AddDebugMessage"), 0x38B1B1, eArchType::ARM);

	// 64 bit
	Add(xorstr("RwInitialised"), 0x2217CD0, eArchType::ARM64); //
	Add(xorstr("RsGlobal"), 0x29EE228, eArchType::ARM64);
	Add(xorstr("CNetGame::ProcessNetwork"), 0x9C1338, eArchType::ARM64); //
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x518DF4, eArchType::ARM64); // эт ненад
	Add(xorstr("CNetGame::Packet_GUI"), 0x9C1DAC, eArchType::ARM64); // 
	Add(xorstr("CNetGame::SendChatCommand"), 0x9C5E08, eArchType::ARM64); //
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x23EC828, eArchType::ARM64); //
	Add(xorstr("CNetGame::m_iGameState"), 0x23EC830, eArchType::ARM64); //
	Add(xorstr("CNetGame::m_pPlayerPool"), 0x23EC838, eArchType::ARM64); //
	Add(xorstr("CNetGame::m_pVehiclePool"), 0x23EC840, eArchType::ARM64); //
	Add(xorstr("CNetObjectPool::New"), 0xA3DB6C, eArchType::ARM64); //
	Add(xorstr("CNetVehiclePool::New"), 0xA2E194, eArchType::ARM64); //

	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0xA243B4, eArchType::ARM64); //
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0xA2447C, eArchType::ARM64); //
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0xA26024, eArchType::ARM64); //
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0xA26BBC, eArchType::ARM64); //
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0xA262C8, eArchType::ARM64); //

	//Add(xorstr("RakClient::RegisterAsRemoteProcedureCall"), 0x9DFC08, eArchType::ARM64);
	Add(xorstr("CChat::AddDebugMessage"), 0x67B2C4, eArchType::ARM64); //
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
