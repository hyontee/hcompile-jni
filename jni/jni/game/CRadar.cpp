/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "CRadar.h"

int32_t CRadar::FindTraceNotTrackingBlipIndex() {
    for (int32_t i = 0; i < MAX_RADAR_TRACES; i ++) {
		auto& v = ms_RadarTrace[i];
        if (!v.m_bTrackingBlip) {
            return (int32_t)i;
        }
    }
    return -1;
}

tBlipHandle CRadar::SetCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, char* scriptName) 
{
    auto index = FindTraceNotTrackingBlipIndex();
    if (index == -1)
        return -1;

    auto& t = ms_RadarTrace[index];

    t.m_vPosition        = posn;
    t.m_nBlipDisplayFlag = blipDisplay;
    t.m_nBlipType        = type;
    t.m_nColour          = BLIP_COLOUR_DESTINATION;
    t.m_fSphereRadius    = 1.f;
    t.m_nEntityHandle    = 0;
    t.m_nBlipSize        = 1;
    t.m_nBlipSprite      = eRadarSprite::RADAR_SPRITE_NONE;
    t.m_bBright          = true;
    t.m_bTrackingBlip    = true;
    t.m_pEntryExit       = nullptr;

	Log("SetCoordBlip");
	// CRadar::SetCoordBlip
	//((tBlipHandle(*)(eBlipType, CVector, eBlipColour, eBlipDisplay, const char*))(SA_ADDR(0x3DAF84 + 1)))(type, posn, color, blipDisplay, scriptName);
	int marker = ((int(*)(eBlipType, CVector, unsigned int, eBlipDisplay, char*))(SA_ADDR(0x3DAF84 + 1)))(type, posn, color, blipDisplay, scriptName);
	//m_nTargetBlipIndex = CRadar::SetCoordBlip(BLIP_COORD, { m_vMousePos.x, m_vMousePos.y, 0.0f }, BLIP_COLOUR_REDCOPY, BLIP_DISPLAY_BOTH, nullptr);
	
	
	Log("SetCoordBlip 2");
	// CRadar::SetBlipSprite
	//((void(*)(tBlipHandle, eRadarSprite))(SA_ADDR(0x3DAF84 + 1)))(index, RADAR_SPRITE_AMMUGUN);
	((void(*)(int, int))(SA_ADDR(0x3DB2F8 + 1)))(marker, 6);
	//CRadar::SetBlipSprite(m_nTargetBlipIndex, RADAR_SPRITE_WAYPOINT);
	
	Log("SetCoordBlip 3");
    //return GetNewUniqueBlipIndex(index);
}

tBlipHandle CRadar::SetCoordBlip(int type, float X, float Y, float Z, int color, int blipDisplay, char* scriptName) 
{
	Log("SetCoordBlip");
	// CRadar::SetCoordBlip
	//((tBlipHandle(*)(eBlipType, CVector, eBlipColour, eBlipDisplay, const char*))(SA_ADDR(0x3DAF84 + 1)))(type, posn, color, blipDisplay, scriptName);
	int marker = ((int(*)(int, float, float, float, int, int, char*))(SA_ADDR(0x3DAF84 + 1)))(type, X, Y, Z, color, blipDisplay, scriptName);
	//m_nTargetBlipIndex = CRadar::SetCoordBlip(BLIP_COORD, { m_vMousePos.x, m_vMousePos.y, 0.0f }, BLIP_COLOUR_REDCOPY, BLIP_DISPLAY_BOTH, nullptr);
	
	
	Log("SetCoordBlip 2");
	// CRadar::SetBlipSprite
	//((void(*)(tBlipHandle, eRadarSprite))(SA_ADDR(0x3DAF84 + 1)))(index, RADAR_SPRITE_AMMUGUN);
	((void(*)(int, int))(SA_ADDR(0x3DB2F8 + 1)))(marker, 6);
	//CRadar::SetBlipSprite(m_nTargetBlipIndex, RADAR_SPRITE_WAYPOINT);
	
	Log("SetCoordBlip 3");
    //return GetNewUniqueBlipIndex(index);
}

tBlipHandle CRadar::GetNewUniqueBlipIndex(int32_t index) {
    auto& trace = ms_RadarTrace[index];
    if (trace.m_nCounter >= std::numeric_limits<uint16_t>::max() - 1)
        trace.m_nCounter = 1; // Wrap back to 1
    else
        trace.m_nCounter++; // Increment
    return index | (trace.m_nCounter << 16);
}