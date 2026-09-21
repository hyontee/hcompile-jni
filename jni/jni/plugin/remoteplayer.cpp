#include "remoteplayer.h"
#include "xorstr.h"

#include "plugin.h"

#include "game/chat.h"

void CRemotePlayer::StoreAimSyncData(uint8_t* data, uint32_t time)
{
	const uintptr_t fn = CGameAPI::GetBase(xorstr("CRemotePlayer::StoreAimSyncData"));
	if (fn && data) reinterpret_cast<void(*)(CRemotePlayer*, uint8_t*, uint32_t)>(fn)(this, data, time);
}

void CRemotePlayer::StoreSyncData(BROnFootSyncData* data, uint32_t time)
{
	const uintptr_t fn = CGameAPI::GetBase(xorstr("CRemotePlayer::StoreSyncData"));
	if (fn && data) reinterpret_cast<void(*)(CRemotePlayer*, BROnFootSyncData*, uint32_t)>(fn)(this, data, time);
}

void CRemotePlayer::StoreInCarSyncData(BRInCarSyncData* data, uint32_t time)
{
	const uintptr_t fn = CGameAPI::GetBase(xorstr("CRemotePlayer::StoreInCarSyncData"));
	if (fn && data) reinterpret_cast<void(*)(CRemotePlayer*, BRInCarSyncData*, uint32_t)>(fn)(this, data, time);
}

void CRemotePlayer::StorePassengerSyncData(uint8_t* data, uint32_t time)
{
	const uintptr_t fn = CGameAPI::GetBase(xorstr("CRemotePlayer::StorePassengerSyncData"));
	if (fn && data) reinterpret_cast<void(*)(CRemotePlayer*, uint8_t*, uint32_t)>(fn)(this, data, time);
}

void CRemotePlayer::StoreBulletSyncData(uint8_t* data, uint32_t time)
{
	const uintptr_t fn = CGameAPI::GetBase(xorstr("CRemotePlayer::StoreBulletSyncData"));
	if (fn && data) reinterpret_cast<void(*)(CRemotePlayer*, uint8_t*, uint32_t)>(fn)(this, data, time);
}
