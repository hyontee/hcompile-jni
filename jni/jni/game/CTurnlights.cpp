#include "CTurnlights.h"
#include "game_sa/game_sa.h"
#include "game_sa/CBaseModelInfo.h"

enum eCoronaType {
    CORONATYPE_SHINYSTAR,
    CORONATYPE_HEADLIGHT,
    CORONATYPE_MOON,
    CORONATYPE_REFLECTION,
    CORONATYPE_HEADLIGHTLINE,
    CORONATYPE_HEX,
    CORONATYPE_CIRCLE,
    CORONATYPE_RING,
    CORONATYPE_STREAK,
    CORONATYPE_TORUS,
    CORONATYPE_NONE
};

enum eCoronaFlareType : uint8 { FLARETYPE_NONE, FLARETYPE_SUN, FLARETYPE_HEADLIGHTS };

CBaseModelInfo* GetModelInfoByID1(int iModelID)
{
    if (iModelID < 0 || iModelID > 20000) {
        return nullptr;
    }

    CBaseModelInfo** dwModelArray = (CBaseModelInfo**)(g_libGTASA + 0x87BF48);
    return dwModelArray[iModelID];
}

void CTurnlights::SetEnabledRight(CVehicle* pVehicle, bool toggle)
{
    /*if (toggle) {
        DrawTurnLights(pVehicle, 0, false);
        DrawTurnLights(pVehicle, 1, false);
    }*/
    if(GetAtRight(pVehicle))
    {
        listRight.erase(pVehicle);
    }
    listRight[pVehicle] = toggle;
}
void CTurnlights::SetEnabledLeft(CVehicle* pVehicle, bool toggle) {
    /*if (toggle) {
        DrawTurnLights(pVehicle, 0, true);
        DrawTurnLights(pVehicle, 1, true);
    }*/
    if(GetAtLeft(pVehicle))
    {
        listLeft.erase(pVehicle);
    }
    listLeft[pVehicle] = toggle;
}

#include "game_sa/CVehicleModelInfo.h"
void CTurnlights::DrawTurnLightsLeft(int dummy) {
    for(auto & pair : listLeft) {
        auto pVehicle = pair.first;
        auto bToggled = pair.second;
        if(pVehicle && bToggled) {
            auto pModelInfoStart = static_cast<CVehicleModelInfo *>(GetModelInfoByID1(
                    pVehicle->m_pEntity->nModelIndex));

            CVector *m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

            CVector vecRight1, vecRight2;

            vecRight1.x = -(m_avDummyPos[dummy].x);
            vecRight1.y = m_avDummyPos[dummy].y;
            vecRight1.z = m_avDummyPos[dummy].z;

            static uint32_t time = GetTickCount();
            if (GetTickCount() % 1000 < 500) {
                ((void (*)(unsigned int, ENTITY_TYPE *, unsigned char, unsigned char, unsigned char,
                           unsigned char, CVector const &, float, float, int, int, bool, bool, int,
                           float,
                           bool, float, unsigned char, float, bool, bool)) (g_libGTASA + 0x52EDD0 +
                                                                            1))(
                        reinterpret_cast<unsigned int>(pVehicle->m_pVehicle) + 50 + dummy +
                        0,
                        (ENTITY_TYPE *) pVehicle->m_pVehicle, 255, 160, 0, 255, vecRight1,
                        1.5f, 10.0f, CORONATYPE_HEADLIGHT, FLARETYPE_NONE, false, false, 0, 0.0f,
                        false,
                        0.5f, 0, 50.0f, false, true);
                time = GetTickCount();
            }
        }
    }
}

void CTurnlights::DrawTurnLightsRight(int dummy) {
    for(auto & pair : listRight) {
        auto pVehicle = pair.first;
        auto bToggled = pair.second;
        if(pVehicle && bToggled) {
            auto pModelInfoStart = static_cast<CVehicleModelInfo *>(GetModelInfoByID1(
                    pVehicle->m_pEntity->nModelIndex));

            CVector *m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

            CVector vecRight1, vecRight2;

            vecRight1.x = m_avDummyPos[dummy].x;
            vecRight1.y = m_avDummyPos[dummy].y;
            vecRight1.z = m_avDummyPos[dummy].z;

            /*RwFrame *frame_front;
            RwFrame *frame_rear;

            frame_front = ((RwFrame * (*)(uintptr_t,
            const char*))(g_libGTASA + 0x00335CEC + 1))(pVehicle->m_pVehicle->entity.m_pRpClump, OBFUSCATE(
                    "turnlights_front")); // GetFrameFromname
            frame_rear = ((RwFrame * (*)(uintptr_t,
            const char*))(g_libGTASA + 0x00335CEC + 1))(pVehicle->m_pVehicle->entity.m_pRpClump, OBFUSCATE(
                    "turnlights_rear")); // GetFrameFromname

            if (!frame_front || !frame_rear) {
                Log("no frame");
                return;
            }
            RwV3d p1 = frame_front->modelling.pos;
            RwV3d p2 = frame_rear->modelling.pos;
            Log("turnlight %f %f %f %f %f %f", p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);

            if (left) {
                p1.x *= -1.0f;
                p2.x *= -1.0f;
            }*/
            bool left = false;

            static uint32_t time = GetTickCount();
            if (GetTickCount() % 1000 < 500) {
                ((void (*)(unsigned int, ENTITY_TYPE *, unsigned char, unsigned char, unsigned char,
                           unsigned char, CVector const &, float, float, int, int, bool, bool, int, float,
                           bool, float, unsigned char, float, bool, bool)) (g_libGTASA + 0x52EDD0 + 1))(
                        reinterpret_cast<unsigned int>(pVehicle->m_pVehicle) + 50 + dummy + 2,
                        (ENTITY_TYPE *) pVehicle->m_pVehicle, 255, 160, 0, 255, vecRight1,
                        1.5f, 10.0f, CORONATYPE_HEADLIGHT, FLARETYPE_NONE, false, false, 0, 0.0f, false,
                        0.5f, 0, 50.0f, false, true);
                time = GetTickCount();
            }
        }
    }
}