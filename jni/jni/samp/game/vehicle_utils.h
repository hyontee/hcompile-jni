//
// Created by August on 12.10.2025.
//

#ifndef CRIMINAL_MOSCOW_VEHICLE_UTILS_H
#define CRIMINAL_MOSCOW_VEHICLE_UTILS_H

#endif //CRIMINAL_MOSCOW_VEHICLE_UTILS_H
// app/src/main/cpp/samp/game/vehicle_utils.h
// app/src/main/cpp/samp/game/vehicle_utils.h
// app/src/main/cpp/samp/game/vehicle_utils.h
// app/src/main/cpp/samp/game/vehicle_utils.h
// app/src/main/cpp/samp/game/vehicle_utils.h
#pragma once
#include <cstdint>

struct CVehicleGta;

void ToggleSpecialsForVehicle(uint16_t vehicleId, bool enable);
bool IsSpecialsEnabledForVehicle(uint16_t vehicleId);
void OnVehicleDestroyed(uint16_t vehicleId);

// Старые адаптеры по указателю
void ToggleSpecialsForVehicle(CVehicleGta* pVeh, bool enable);
bool IsSpecialsEnabledForVehicle(CVehicleGta* pVeh);
void OnVehicleDestroyed(CVehicleGta* pVeh);

// Получить SAMP vehicleId по GTA-указателю
uint16_t GetSampVehicleIdFromPtr(CVehicleGta* pVeh);

// NEW: применить визуал/аудио мигалки/стробов к GTA-указателю
void ApplySpecialsFx(CVehicleGta* pVeh, bool enable);
