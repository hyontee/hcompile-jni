// app/src/main/cpp/samp/game/vehicle_utils.cpp

#include <unordered_map>
#include <cstdint>

#include "game/game.h"          // CVehicleGta и пр.
#include "net/netgame.h"        // CNetGame, CVehiclePool
#include "game/vehicle_utils.h" // наши декларации
#include "game/vehicle.h"       // CVehicle (нужен для SetSirenEnabled/StartSirenSound/StopSirenSound)

// Экспортируется из клиента SA:MP
extern CNetGame* pNetGame;

// Удобство
namespace {
    constexpr uint16_t INVALID_VEH_ID = 0xFFFF;

    // Получить CVehicle* по SAMP vehicleId (если в стриме)
    inline CVehicle* GetVehicleById(uint16_t id) {
        if (!pNetGame) return nullptr;
        auto* vp = pNetGame->GetVehiclePool();
        if (!vp) return nullptr;
        return vp->GetAt(id); // может вернуть nullptr, если ТС вне стрима
    }
}

// Храним состояние спецсигналов по vehicleId
static std::unordered_map<uint16_t, bool> g_Specials;

// ---------- Публичные функции по vehicleId ----------

void ToggleSpecialsForVehicle(uint16_t vehicleId, bool enable)
{
    g_Specials[vehicleId] = enable;

    // Если объект в стриме — применим визуал сразу
    if (auto* v = GetVehicleById(vehicleId))
    {
        if (auto* gtaVeh = v->m_pVehicle)
        {
            ApplySpecialsFx(gtaVeh, enable);
        }
    }
}

bool IsSpecialsEnabledForVehicle(uint16_t vehicleId)
{
    const auto it = g_Specials.find(vehicleId);
    return (it != g_Specials.end()) && it->second;
}

void OnVehicleDestroyed(uint16_t vehicleId)
{
    g_Specials.erase(vehicleId);

    // На всякий случай дёрнем визуал "выкл", если объект ещё доступен
    if (auto* v = GetVehicleById(vehicleId))
    {
        if (auto* gtaVeh = v->m_pVehicle)
        {
            ApplySpecialsFx(gtaVeh, false);
        }
    }
}

// ---------- Адаптеры по указателю (для старого кода) ----------

uint16_t GetSampVehicleIdFromPtr(CVehicleGta* pVeh)
{
    if (!pVeh || !pNetGame) return INVALID_VEH_ID;
    auto* vp = pNetGame->GetVehiclePool();
    if (!vp) return INVALID_VEH_ID;

    // Если у вашего пула есть быстрый метод — лучше использовать его:
    // return vp->GetIdFromGtaPtr(pVeh);

    // Фолбэк: линейный поиск (обычно MAX_VEHICLES = 2000)
#ifndef MAX_VEHICLES
#define MAX_VEHICLES 2000
#endif
    for (uint16_t id = 0; id < MAX_VEHICLES; ++id)
    {
        if (auto* v = vp->GetAt(id))
        {
            if (v->m_pVehicle == pVeh)
                return id;
        }
    }
    return INVALID_VEH_ID;
}

void ToggleSpecialsForVehicle(CVehicleGta* pVeh, bool enable)
{
    const uint16_t id = GetSampVehicleIdFromPtr(pVeh);
    if (id != INVALID_VEH_ID)
        ToggleSpecialsForVehicle(id, enable);
}

bool IsSpecialsEnabledForVehicle(CVehicleGta* pVeh)
{
    const uint16_t id = GetSampVehicleIdFromPtr(pVeh);
    return (id != INVALID_VEH_ID) && IsSpecialsEnabledForVehicle(id);
}

void OnVehicleDestroyed(CVehicleGta* pVeh)
{
    const uint16_t id = GetSampVehicleIdFromPtr(pVeh);
    if (id != INVALID_VEH_ID)
        OnVehicleDestroyed(id);
}

// ---------- Реальный визуал/аудио спецсигналов ----------
// Включает/выключает флаг сирены на CVehicle + 3D-звук.
// Мерцание балки и стробов рисуется в рендере при активной сирене.
void ApplySpecialsFx(CVehicleGta* pVeh, bool enable)
{
    if (!pVeh || !pNetGame) return;

    // (Опционально) ограничим на модель 598, как у тебя в RPC-хэндлере
    if (pVeh->nModelIndex != 598) {
        // Если хочешь — просто игнорируем не-598, либо снимем флаг.
        enable = false;
    }

    const uint16_t id = GetSampVehicleIdFromPtr(pVeh);
    if (id == INVALID_VEH_ID) return;

    if (auto* vp = pNetGame->GetVehiclePool())
    {
        if (CVehicle* v = vp->GetAt(id))
        {
            // Задаём флаг, чтобы процессор рендера знал, что надо мигать
            v->SetSirenEnabled(enable);

            // 3D-звук сирены
            if (enable) v->StartSirenSound();
            else        v->StopSirenSound();

            // При желании можно синхронизировать штатную gta-сирену на самом GTA-объекте:
            // if (pVeh) {
            //     // например, если есть подходящее поле/метод в твоём SDK
            //     // pVeh->m_nVehicleFlags.bSirenOrAlarm = enable;
            // }
        }
    }
}
