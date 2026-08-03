#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	// 32 bit
	Add(xorstr("RwInitialised"), 0x4925ED8, eArchType::ARM);
	Add(xorstr("RsGlobal"), 0x4F225A8, eArchType::ARM);
	Add(xorstr("CNetGame::ProcessNetwork"), 0x3DCDD1, eArchType::ARM);
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x2B45D1, eArchType::ARM);
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x493B238, eArchType::ARM);
	Add(xorstr("CNetGame::m_iGameState"), 0x493B23C, eArchType::ARM);
	Add(xorstr("CNetGame::m_pPlayerPool"), 0x493B240, eArchType::ARM);
	Add(xorstr("CNetGame::m_pVehiclePool"), 0x493B244, eArchType::ARM);
	Add(xorstr("CNetVehiclePool::New"), 0x436CA1, eArchType::ARM);
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x42F301, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0x42F3E1, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x430A59, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x430F6D, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x430C89, eArchType::ARM);

	Add(xorstr("CChat::AddDebugMessage"), 0x47B2D1, eArchType::ARM);

	// 64 bit
	Add(xorstr("RwInitialised"), 0x4D81364, eArchType::ARM64);
	Add(xorstr("RsGlobal"), 0x53A6BC0, eArchType::ARM64);
	Add(xorstr("CNetGame::ProcessNetwork"), 0x626A0C, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x4A799C, eArchType::ARM64);
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x4D9B0B8, eArchType::ARM64);
	Add(xorstr("CNetGame::m_iGameState"), 0x4D9B0C0, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pPlayerPool"), 0x4D9B0C8, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pVehiclePool"), 0x4D9B0D0, eArchType::ARM64);
	Add(xorstr("CNetVehiclePool::New"), 0x68F8CC, eArchType::ARM64);
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x686478, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0x686578, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x6880C4, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x6886F4, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x688368, eArchType::ARM64);

	Add(xorstr("CChat::AddDebugMessage"), 0x6E23D4, eArchType::ARM64);
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
