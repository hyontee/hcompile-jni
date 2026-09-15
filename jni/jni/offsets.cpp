#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
	// tg @ida_reverse,@oelinux
	Add(xorstr("RwInitialised"), 0x4849729, eArchType::ARM); // tg @ida_reverse,@oelinux
	Add(xorstr("RsGlobal"), 0x4E4723C, eArchType::ARM); // tg @ida_reverse,@oelinux
	Add(xorstr("JNILib_step"), 0x4FE0BA8, eArchType::ARM); // tg @ida_reverse,@oelinux
	Add(xorstr("TouchEvent"), 0x4CD8D5, eArchType::ARM); // tg @ida_reverse 
	Add(xorstr("CNetGame::ProcessNetwork"), 0x3DC841, eArchType::ARM); //  tg @ida_reverse,@oelinux
	Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x3DD53C, eArchType::ARM);
	
	Add(xorstr("CNetGame::m_pRakClient"), 0x49FB5E8, eArchType::ARM); // tg @ida_reverse 
        uintptr_t ng_pRakClient = Get(xorstr("CNetGame::m_pRakClient"));
	Add(xorstr("CNetGame::m_iGameState"), ng_pRakClient + 4, eArchType::ARM);
	Add(xorstr("CNetGame::m_pPlayerPool"), ng_pRakClient + 8, eArchType::ARM);
	Add(xorstr("CNetGame::m_pVehiclePool"), ng_pRakClient + 12, eArchType::ARM);
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
	Add(xorstr("CNetTextDrawPool::SetServerLogo"), 0x4341DC, eArchType::ARM); // YES
	Add(xorstr("CNetVehiclePool::New"), 0x3DC289, eArchType::ARM); // tg @ida_reverse
	
	Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x42DC91, eArchType::ARM); // tg @ida_reverse
	Add(xorstr("CRemotePlayer::StoreSyncData"), 0x33F0F5, eArchType::ARM);
	Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x42F399, eArchType::ARM); // tg @ida_reverse
	Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x42F8A5, eArchType::ARM); // tg @ida_reverse
	Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x42F5C1, eArchType::ARM); // tg @ida_reverse
	
	Add(xorstr("RakClient::RegisterAsRemoteProcedureCall"), 0x536DF9, eArchType::ARM); // tg @ida_reverse
	Add(xorstr("CChat::AddDebugMessage"), 0x476AE1, eArchType::ARM); // tg @ida_reverse
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