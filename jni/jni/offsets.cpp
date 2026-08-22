#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	// 64 bit
	Add(xorstr("RwInitialised"), 0x238F258, eArchType::ARM64); // +
	Add(xorstr("CNetGame::ProcessNetwork"), 0xA33B04, eArchType::ARM64); // +
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0xA35748, eArchType::ARM64);
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x2563470, eArchType::ARM64); 
	Add(xorstr("CNetGame::m_iGameState"), 0x2563478, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pPlayerPool"), 0x2563480, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pVehiclePool"), 0x2563488, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pObjectPool"), 0x2563490, eArchType::ARM64);
	Add(xorstr("CNetObjectPool::New"), 0xAB8B70, eArchType::ARM64);
	Add(xorstr("CNetVehiclePool::New"), 0xAA920C, eArchType::ARM64);
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0xA9E8D0, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0xA9E998, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0xAA05B8, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0xAA117C, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0xAA085C, eArchType::ARM64);

	Add(xorstr("CChat::AddDebugMessage"), 0x6A18F0, eArchType::ARM64); //+
	
	Add(xorstr("CNetGame::Packet_GUI"), 0xA34440, eArchType::ARM64); //
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
