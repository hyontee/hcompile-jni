#pragma once


#include "game/CVehicleGta.h"
#include "game/Entity/Ped/CPedGta.h"
#include "imgui/imgui.h"
#include "game/common.h"

static class CUtil {
public:
    static bool IsValidGameVehicle(CVehicleGta *pVehicle);
    static bool IsValidGamePed(CPedGta * pPed);

    static bool IsGameEntityArePlaceable(CEntityGta *pEntity);

    static void WorldRemoveEntity(uintptr_t pEnt);

    static const char *GetWeaponName(int iWeaponID);

    static RwTexture *LoadTextureFromDB(const char *dbname, const char *texture);

    static uintptr_t FindLibrary(const char *library);

    static uintptr_t FindLib(const char *library);

    static int32_t FindPlayerSlotWithPedPointer(CPedGta *pPlayersPed);

    static CPedGta *FindPlayerPed(int32_t Player);

    static CPhysicalGta *GetPoolObj(int slot);

    static CPedGta *GetPoolPed(int slot);

    static RwTexture *GetTexture(const char *name);

    static float DegToRad(float fDegrees);

    constexpr static float RadiansToDegrees(float angleInRadians) {
        return angleInRadians * 180.0F / PI;
    }

    std::string removeColorTags(std::string line);

    static ImVec2 CalcTextSizeWithoutTags(char *szStr, float font_size = 0.0f);

    static void drawText(ImVec2 pos, char *utf8string, uint32_t dwColor);

    static void drawText(ImVec2 pos, ImColor col, const char *szStr, const float font_size);

    static void drawText(const char *fmt, ...);

    static bool ProcessInlineHexColor(const char *start, const char *end, ImVec4 &color);

    static CPhysicalGta *GamePool_Object_GetAt(int slot);
};

void cp1251_to_utf8(char *out, const char *in, unsigned int len = 0);

void AND_OpenLink(const char* szLink);
char* ConvertColorToHtml(std::string format);