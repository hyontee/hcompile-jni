#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	// 64 bit
	Add(xorstr("RwInitialised"), 0x22B6578, eArchType::ARM64); //
	Add(xorstr("RsGlobal"), 0x29EE228, eArchType::ARM64);
	Add(xorstr("CNetGame::ProcessNetwork"), 0x9ECDF0, eArchType::ARM64); //
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x518DF4, eArchType::ARM64); // эт ненад
	Add(xorstr("CNetGame::Packet_GUI"), 0x9ED748, eArchType::ARM64); //
	Add(xorstr("RegisterRPCs"), 0xA4B5A8, eArchType::ARM64); //
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x248B1B8, eArchType::ARM64); //
	Add(xorstr("CNetGame::m_iGameState"), 0x248B1C0, eArchType::ARM64); //
	Add(xorstr("CNetGame::m_pPlayerPool"), 0x248B1C8, eArchType::ARM64); //
	Add(xorstr("CNetGame::m_pVehiclePool"), 0x248B1D0, eArchType::ARM64); //
	Add(xorstr("CNetVehiclePool::New"), 0xA59C9C, eArchType::ARM64); //

	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0xA4FED0, eArchType::ARM64); //
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0xA4FF98, eArchType::ARM64); //
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0xA51B40, eArchType::ARM64); //
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0xA526D8, eArchType::ARM64); //
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0xA51DE4, eArchType::ARM64); //

	Add(xorstr("CChat::AddDebugMessage"), 0x69D730, eArchType::ARM64); //
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
