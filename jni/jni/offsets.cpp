#include "offsets.h"
#include "xorstr.h"

std::vector<COffset::stOffset> COffset::m_offsets;

void COffset::Initialise()
{
    Add(xorstr("RwInitialised"), 0x5A98B0, eArchType::ARM64); // TODO: заменить Ч новый оффсет не найден
    Add(xorstr("RsGlobal"), 0x5321E98, eArchType::ARM64); // yes - struct base; +0x8 is width
    Add(xorstr("JNILib_step"), 0x6CCDC8, eArchType::ARM64); // yes - JNI/шаг главного цикла
    Add(xorstr("TouchEvent"), 0x6CD160, eArchType::ARM64); // yes - JNI обработчик multi-touch
    Add(xorstr("CNetGame::ProcessNetwork"), 0x5A98B0, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::Packet_ConnectionLost"), 0x5AA3BC, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::Packet_TurnLightsSync"), 0x5AA55C, eArchType::ARM64); // yes

    // CNetGame globals below are explicit absolute RVAs (подтверждены кластером обращений в ProcessNetwork/RPC).
    Add(xorstr("CNetGame::m_pRakClient"), 0x4D150A8, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_iGameState"), 0x4D150B0, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pPlayerPool"), 0x4D150B8, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pVehiclePool"), 0x4D150C0, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pPickupPool"), 0x4D150C8, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pTextLabelPool"), 0x4D150D0, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pTextDrawPool"), 0x4D150D8, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pGangZonePool"), 0x4D150E0, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pActorPool"), 0x4D150E8, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pObjectPool"), 0x4D150F0, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pChatBubblePool"), 0x4D150F8, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::m_pWayPointPool"), 0x4D15100, eArchType::ARM64); // yes

    Add(xorstr("CNetTextDrawPool::SetServerLogo"), 0x60FCF8, eArchType::ARM64); // yes
    Add(xorstr("CNetVehiclePool::New"), 0x611A58, eArchType::ARM64); // yes
    Add(xorstr("CNetGame::Packet_GUI"), 0x5AA640, eArchType::ARM64); // yes

Add(xorstr("CRemotePlayer::StoreAimSyncData"), 0x60872C, eArchType::ARM64);
Add(xorstr("CRemotePlayer::StoreSyncData"), 0x60882C, eArchType::ARM64);
Add(xorstr("CRemotePlayer::StoreInCarSyncData"), 0x60A314, eArchType::ARM64);
Add(xorstr("CRemotePlayer::StorePassengerSyncData"), 0x60A940, eArchType::ARM64);
Add(xorstr("CRemotePlayer::StoreBulletSyncData"), 0x60A5B4, eArchType::ARM64);

    // Extra transport thunks already used by the existing ARM64 source logic.

    Add(xorstr("CChat::AddDebugMessage"), 0x6616A8, eArchType::ARM64); // yes
    Add(xorstr("CVoiceChatClient::OnPacketIncoming"), 0x737B88, eArchType::ARM64); // yes

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