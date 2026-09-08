#include "remoteplayer.h"
#include "xorstr.h"

#include "plugin.h"

#include "game/chat.h"

namespace
{
uintptr_t ResolveCallableAddress(const char* offsetName)
{
	uintptr_t addr = CGameAPI::GetBase(offsetName);
	if(!addr) {
		return 0;
	}

	// Some offsets in this project point to runtime-resolved function pointers in .bss.
	uintptr_t moduleBase = CGameAPI::GetBase();
	uintptr_t candidate = *(uintptr_t*)addr;
	if(moduleBase && candidate > moduleBase && candidate < (moduleBase + 0x10000000ULL)) {
		return candidate;
	}

	return addr;
}
}

void CRemotePlayer::StoreAimSyncData(uint8_t* data, uint32_t time)
{
	uintptr_t fn = ResolveCallableAddress(xorstr("CRemotePlayer::StoreAimSyncData"));
	if(!fn || reinterpret_cast<uintptr_t>(this) < 0x10000 || !data) return;
	reinterpret_cast<void(*)(CRemotePlayer*, uint8_t*, uint32_t)>(fn)(this, data, time);
}

void CRemotePlayer::StoreSyncData(BROnFootSyncData* data, uint32_t time)
{
	uintptr_t fn = ResolveCallableAddress(xorstr("CRemotePlayer::StoreSyncData"));
	if(!fn || reinterpret_cast<uintptr_t>(this) < 0x10000 || !data) return;
	reinterpret_cast<void(*)(CRemotePlayer*, BROnFootSyncData*, uint32_t)>(fn)(this, data, time);
}

void CRemotePlayer::StoreInCarSyncData(BRInCarSyncData* data, uint32_t time)
{
	uintptr_t fn = ResolveCallableAddress(xorstr("CRemotePlayer::StoreInCarSyncData"));
	if(!fn || reinterpret_cast<uintptr_t>(this) < 0x10000 || !data) return;
	reinterpret_cast<void(*)(CRemotePlayer*, BRInCarSyncData*, uint32_t)>(fn)(this, data, time);
}

void CRemotePlayer::StorePassengerSyncData(uint8_t* data, uint32_t time)
{
	uintptr_t fn = ResolveCallableAddress(xorstr("CRemotePlayer::StorePassengerSyncData"));
	if(!fn || reinterpret_cast<uintptr_t>(this) < 0x10000 || !data) return;
	reinterpret_cast<void(*)(CRemotePlayer*, uint8_t*, uint32_t)>(fn)(this, data, time);
}

void CRemotePlayer::StoreBulletSyncData(uint8_t* data, uint32_t time)
{
	uintptr_t fn = ResolveCallableAddress(xorstr("CRemotePlayer::StoreBulletSyncData"));
	if(!fn || reinterpret_cast<uintptr_t>(this) < 0x10000 || !data) return;
	reinterpret_cast<void(*)(CRemotePlayer*, uint8_t*, uint32_t)>(fn)(this, data, time);
}
