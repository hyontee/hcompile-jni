#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	// 64 bit
	Add(xorstr("RwInitialised"), 0x5B9E60, eArchType::ARM64);
	Add(xorstr("RsGlobal"), 0x5B9E64, eArchType::ARM64);
	Add(xorstr("CNetGame::ProcessNetwork"), 0x2A37B8, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_PlayerSync_207"), 0x2A3F64, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_VehicleSync_200"), 0x2A42E8, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_PassengerSync_211"), 0x2A467C, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_PlayerSync_209"), 0x2A4754, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_BulletSync_206"), 0x2A4840, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_253"), 0x2A4A58, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_VehicleSync_210"), 0x2A4B64, eArchType::ARM64);
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x2A4A58, eArchType::ARM64);
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x4C28E88, eArchType::ARM64);
	Add(xorstr("CNetGame::m_iGameState"), 0x4C28E90, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pPlayerPool"), 0x4C28E98, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pVehiclePool"), 0x4C28EA0, eArchType::ARM64);
	Add(xorstr("CNetGame::m_pObjectPool"), 0x4C28EA8, eArchType::ARM64);
	Add(xorstr("CNetObjectPool::New"), 0x2AC158, eArchType::ARM64);
	Add(xorstr("CNetVehiclePool::New"), 0x2B8E9C, eArchType::ARM64);
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x50D2C0, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0x50D3C0, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x50EDE4, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x50F3FC, eArchType::ARM64);
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x50F070, eArchType::ARM64);

	Add(xorstr("CChat::AddDebugMessage"), 0x273AA4, eArchType::ARM64);
	
	Add(xorstr("CNetGame::Packet_GUI"), 0x2A4940, eArchType::ARM64);
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
