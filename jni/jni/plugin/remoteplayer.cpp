#include "remoteplayer.h"
#include "xorstr.h"

#include "plugin.h"

#include "game/chat.h"

namespace
{
template <typename T>
inline T ResolveStoreFn(T& cache, const char* name)
{
    if (!cache) {
        uintptr_t addr = CGameAPI::GetBase(name);
        if (addr) {
            cache = reinterpret_cast<T>(addr);
        }
    }
    return cache;
}
}

void CRemotePlayer::StoreAimSyncData(uint8_t* data, uint32_t time)
{
    using Fn = void(*)(CRemotePlayer*, uint8_t*, uint32_t);
    static Fn fn = nullptr;
    Fn target = ResolveStoreFn(fn, xorstr("CRemotePlayer::StoreAimSyncData"));
    if (!target) {
        return;
    }
	target(this, data, time);
}

void CRemotePlayer::StoreSyncData(BROnFootSyncData* data, uint32_t time)
{
    using Fn = void(*)(CRemotePlayer*, BROnFootSyncData*, uint32_t);
    static Fn fn = nullptr;
    Fn target = ResolveStoreFn(fn, xorstr("CRemotePlayer::StoreSyncData"));
    if (!target) {
        return;
    }
	target(this, data, time);
}

void CRemotePlayer::StoreInCarSyncData(BRInCarSyncData* data, uint32_t time)
{
    using Fn = void(*)(CRemotePlayer*, BRInCarSyncData*, uint32_t);
    static Fn fn = nullptr;
    Fn target = ResolveStoreFn(fn, xorstr("CRemotePlayer::StoreInCarSyncData"));
    if (!target) {
        return;
    }
	target(this, data, time);
}

void CRemotePlayer::StorePassengerSyncData(uint8_t* data, uint32_t time)
{
    using Fn = void(*)(CRemotePlayer*, uint8_t*, uint32_t);
    static Fn fn = nullptr;
    Fn target = ResolveStoreFn(fn, xorstr("CRemotePlayer::StorePassengerSyncData"));
    if (!target) {
        return;
    }
	target(this, data, time);
}

void CRemotePlayer::StoreBulletSyncData(uint8_t* data, uint32_t time)
{
    using Fn = void(*)(CRemotePlayer*, uint8_t*, uint32_t);
    static Fn fn = nullptr;
    Fn target = ResolveStoreFn(fn, xorstr("CRemotePlayer::StoreBulletSyncData"));
    if (!target) {
        return;
    }
	target(this, data, time);
}
