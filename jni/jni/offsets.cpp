#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	// 64 bit
	Add(xorstr("RwInitialised"), 0x525ADE0, eArchType::ARM64);
	Add(xorstr("RsGlobal"), 0x53A6BC0, eArchType::ARM64);
	Add(xorstr("CNetGame::ProcessNetwork"), 0x4AB6B4, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x4AC314, eArchType::ARM64);
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x4C4D5F0, eArchType::ARM64);
	Add(xorstr("CNetGame::m_iGameState"), 0x4C4D5F8, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pPlayerPool"), 0x4C4D600, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pVehiclePool"), 0x4C4D608, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pObjectPool"), 0x4C4D638, eArchType::ARM64);
	Add(xorstr("CNetObjectPool::New"), 0x526E00, eArchType::ARM64);
	Add(xorstr("CNetVehiclePool::New"), 0x516C38, eArchType::ARM64);
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x50D2C0, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0x50D3C0, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x50EDE4, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x50F3FC, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x50F070, eArchType::ARM64);

	Add(xorstr("CChat::AddDebugMessage"), 0x569900, eArchType::ARM64);
	
	Add(xorstr("CNetGame::Packet_GUI"), 0x4AC674, eArchType::ARM64);
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
