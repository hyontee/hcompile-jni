#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	//
	Add(xorstr("RwInitialised"), 0x482EAB1, eArchType::ARM); //
	Add(xorstr("RsGlobal"), 0x4FE38C5, eArchType::ARM); // 
	Add(xorstr("JNILib_step"), 0x4EDEA75, eArchType::ARM); // YES
	Add(xorstr("TouchEvent"), 0x48D90D, eArchType::ARM); // YES
	Add(xorstr("CNetGame::ProcessNetwork"), 0x3940D1, eArchType::ARM); // YES
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x394DF5, eArchType::ARM); // YES
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x48F8EA8, eArchType::ARM); // YESS
        uintptr_t ng_pRakClient = Get(xorstr("CNetGame::m_pRakClient"));
	Add(xorstr("CNetGame::m_iGameState"), ng_pRakClient + 4, eArchType::ARM);
	Add(xorstr("CNetGame::m_pPlayerPool"), ng_pRakClient + 8, eArchType::ARM);
	Add(xorstr("CNetGame::m_VehiclesPool"), ng_pRakClient + 12, eArchType::ARM);
	Add(xorstr("CNetGame::m_pPickupPool"), ng_pRakClient + 16, eArchType::ARM);
	Add(xorstr("CNetGame::m_pTextLabelPool"), ng_pRakClient + 20, eArchType::ARM);
	Add(xorstr("CNetGame::m_pTextDrawPool"), ng_pRakClient + 24, eArchType::ARM);
	Add(xorstr("CNetGame::m_pGangZonePool"), ng_pRakClient + 28, eArchType::ARM);
	Add(xorstr("CNetGame::m_pActorPool"), ng_pRakClient + 32, eArchType::ARM);
	Add(xorstr("CNetGame::m_pObjectPool"), ng_pRakClient + 36, eArchType::ARM);
	Add(xorstr("CNetGame::m_pChatBubblePool"), ng_pRakClient + 40, eArchType::ARM);
	Add(xorstr("CNetGame::m_pWayPointPool"), ng_pRakClient + 44, eArchType::ARM);
	// Add(xorstr("CNetGame::m_fNameTagsDrawDistance"), 0x4858CDC, eArchType::ARM);
	// Add(xorstr("CNetGame::m_byteWorldTime"), 0x4869F02, eArchType::ARM);
	Add(xorstr("CNetTextDrawPool::SetServerLogo"), 0x3EC1ED, eArchType::ARM); // YES
	Add(xorstr("CNetVehiclePool::New"), 0x37665D, eArchType::ARM); // 
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x3E5C51, eArchType::ARM); // okey
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0x535C0D, eArchType::ARM); // TEST 42DD71
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x535C0D, eArchType::ARM); // YES LOL 42F399
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x3E78A5, eArchType::ARM); // yes
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x3E75C1, eArchType::ARM); // YES
	
	Add(xorstr("RakClient::RegisterAsRemoteProcedureCall"), 0x4F9BE9, eArchType::ARM); // yes
	Add(xorstr("CChat::AddDebugMessage"), 0x43145D, eArchType::ARM); // YES
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


// tg @ida_reverse,@oelinux