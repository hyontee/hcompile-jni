/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "array"
#include "CVector.h"
#include "RGBA.h"

class CSprite2d;

// Thanks to Wesser for radar-related things
enum eBlipAppearance {
    BLIP_FLAG_FRIEND, // It selects BLIP_COLOUR_BLUE. If unset together with BLIP_FLAG_THREAT, any color.
    BLIP_FLAG_THREAT,  // It selects BLIP_COLOUR_RED. If unset together with BLIP_FLAG_FRIEND, any color.
    BLIP_FLAG_UNK,

    BLIP_FLAG_NUM // Add above this
};

enum eBlipType {
    BLIP_NONE,          // 0
    BLIP_CAR,           // 1
    BLIP_CHAR,          // 2
    BLIP_OBJECT,        // 3
    BLIP_COORD,         // 4 - Checkpoint.
    BLIP_CONTACT_POINT, // 5 - Sphere.
    BLIP_SPOTLIGHT,     // 6
    BLIP_PICKUP,        // 7
    BLIP_AIRSTRIP       // 8
};

enum eBlipDisplay {
    BLIP_DISPLAY_NEITHER,    // 0
    BLIP_DISPLAY_MARKERONLY, // 1
    BLIP_DISPLAY_BLIPONLY,   // 2
    BLIP_DISPLAY_BOTH        // 3
};

// See <https://www.dropbox.com/s/oi3i4f0qsbe7z10/blip_marker_colors.html> to view these colors. (TODO: dead link, fix it.)
enum eBlipColour {
    BLIP_COLOUR_RED,        // 0
    BLIP_COLOUR_GREEN,      // 1
    BLIP_COLOUR_BLUE,       // 2
    BLIP_COLOUR_WHITE,      // 3
    BLIP_COLOUR_YELLOW,     // 4
    BLIP_COLOUR_REDCOPY,    // 5 - What? It was BLIP_COLOUR_PURPLE.
    BLIP_COLOUR_BLUECOPY,   // 6 - Why? It was BLIP_COLOUR_CYAN.
    BLIP_COLOUR_THREAT,     // 7 - If BLIP_FLAG_FRIENDLY is not set (by default) it is BLIP_COLOUR_RED, else BLIP_COLOUR_BLUE.
    BLIP_COLOUR_DESTINATION // 8 - Default color.
};

// https://wiki.multitheftauto.com/index.php?title=Radar_Blips
enum eRadarSprite {
    RADAR_SPRITE_PLAYER_INTEREST = -5,
    RADAR_SPRITE_THREAT          = -4,
    RADAR_SPRITE_FRIEND          = -3,
    RADAR_SPRITE_OBJECT          = -2,
    RADAR_SPRITE_DESTINATION     = -1,
    RADAR_SPRITE_NONE            = 0,
    RADAR_SPRITE_WHITE,         // 1
    RADAR_SPRITE_CENTRE,        // 2
    RADAR_SPRITE_MAP_HERE,      // 3
    RADAR_SPRITE_NORTH,         // 4
    RADAR_SPRITE_AIRYARD,       // 5
    RADAR_SPRITE_AMMUGUN,       // 6
    RADAR_SPRITE_BARBERS,       // 7
    RADAR_SPRITE_BIGSMOKE,      // 8
    RADAR_SPRITE_BOATYARD,      // 9
    RADAR_SPRITE_BURGERSHOT,    // 10
    RADAR_SPRITE_BULLDOZER,     // 11
    RADAR_SPRITE_CATALINAPINK,  // 12
    RADAR_SPRITE_CESARVIAPANDO, // 13 - What? R* mistype?
    RADAR_SPRITE_CHICKEN,       // 14
    RADAR_SPRITE_CJ,            // 15
    RADAR_SPRITE_CRASH1,        // 16
    RADAR_SPRITE_DINER,         // 17
    RADAR_SPRITE_EMMETGUN,      // 18
    RADAR_SPRITE_ENEMYATTACK,   // 19
    RADAR_SPRITE_FIRE,          // 20
    RADAR_SPRITE_GIRLFRIEND,    // 21
    RADAR_SPRITE_HOSTPITAL,     // 22 - Again?
    RADAR_SPRITE_LOGOSYNDICATE, // 23
    RADAR_SPRITE_MADDOG,        // 24
    RADAR_SPRITE_MAFIACASINO,   // 25
    RADAR_SPRITE_MCSTRAP,       // 26
    RADAR_SPRITE_MODGARAGE,     // 27
    RADAR_SPRITE_OGLOC,         // 28
    RADAR_SPRITE_PIZZA,         // 29
    RADAR_SPRITE_POLICE,        // 30
    RADAR_SPRITE_PROPERTYG,     // 31
    RADAR_SPRITE_PROPERTYR,     // 32
    RADAR_SPRITE_RACE,          // 33
    RADAR_SPRITE_RYDER,         // 34
    RADAR_SPRITE_SAVEGAME,      // 35
    RADAR_SPRITE_SCHOOL,        // 36
    RADAR_SPRITE_QMARK,         // 37
    RADAR_SPRITE_SWEET,         // 38
    RADAR_SPRITE_TATTOO,        // 39
    RADAR_SPRITE_THETRUTH,      // 40
    RADAR_SPRITE_WAYPOINT,      // 41
    RADAR_SPRITE_TORENORANCH,   // 42
    RADAR_SPRITE_TRIADS,        // 43
    RADAR_SPRITE_TRIADSCASINO,  // 44
    RADAR_SPRITE_TSHIRT,        // 45
    RADAR_SPRITE_WOOZIE,        // 46
    RADAR_SPRITE_ZERO,          // 47
    RADAR_SPRITE_DATEDISCO,     // 48
    RADAR_SPRITE_DATEDRINK,     // 49
    RADAR_SPRITE_DATEFOOD,      // 50
    RADAR_SPRITE_TRUCK,         // 51
    RADAR_SPRITE_CASH,          // 52
    RADAR_SPRITE_FLAG,          // 53
    RADAR_SPRITE_GYM,           // 54
    RADAR_SPRITE_IMPOUND,       // 55
    RADAR_SPRITE_LIGHT,         // 56
    RADAR_SPRITE_RUNWAY,        // 57
    RADAR_SPRITE_GANGB,         // 58
    RADAR_SPRITE_GANGP,         // 59
    RADAR_SPRITE_GANGY,         // 60
    RADAR_SPRITE_GANGN,         // 61
    RADAR_SPRITE_GANGG,         // 62
    RADAR_SPRITE_SPRAY,         // 63
    RADAR_SPRITE_TORENO         // 64
};

enum eRadarTraceHeight {
    RADAR_TRACE_LOW,   // 0 Up-pointing Triangle △
    RADAR_TRACE_HIGH,  // 1 Down-pointing Triangle ▽
    RADAR_TRACE_NORMAL // 2 Box □
};

/*
struct tBlipHandle {
    uint16 arrayIndex;
    uint16 number;
};
VALIDATE_SIZE(tBlipHandle, 0x4);
*/
using tBlipHandle = uint32_t; // TODO: Use struct above

struct airstrip_info {
    CVector2D position;
    float     direction; // angle
    float     radius; // not sure
};

// NOTSA
enum eAirstripLocation {
    AIRSTRIP_LS_AIRPORT,
    AIRSTRIP_SF_AIRPORT,
    AIRSTRIP_LV_AIRPORT,
    AIRSTRIP_VERDANT_MEADOWS,

    NUM_AIRSTRIPS
};

class CEntryExit;

struct tRadarTrace {
    eBlipColour  m_nColour;
    uint32_t       m_nEntityHandle;
    CVector      m_vPosition;
    uint16_t       m_nCounter;
    float        m_fSphereRadius;
    uint16_t       m_nBlipSize;
    CEntryExit*  m_pEntryExit;
    eRadarSprite m_nBlipSprite;

    bool         m_bBright : 1;              // It makes use of bright colors. Always set.
    bool         m_bTrackingBlip : 1;        // It is available.
    bool         m_bShortRange : 1;          // It doesn't show permanently on the radar.
    bool         m_bFriendly : 1;            // It is affected by BLIP_COLOUR_THREAT.
    bool         m_bBlipRemain : 1;          // It has the priority over the entity (it will still appear after the entity's deletion).
    bool         m_bBlipFade : 1;            // Possibly a leftover. Always unset (unused).
    uint8_t        m_nCoordBlipAppearance : 2; // see eBlipAppearance

    eBlipDisplay    m_nBlipDisplayFlag : 2;
    eBlipType       m_nBlipType : 4;
    eBlipAppearance m_nAppearance : 2;

    [[nodiscard]] auto HasSprite() const { return m_nBlipSprite != eRadarSprite::RADAR_SPRITE_NONE; }
    [[nodiscard]] CRGBA GetStaticColour() const;
    [[nodiscard]] CVector GetWorldPos() const;

    std::pair<CVector2D, CVector2D> GetRadarAndScreenPos(float* radarPointDist) const;
};

static constexpr uint32_t MAX_RADAR_WIDTH_TILES = 12;
static constexpr uint32_t MAX_RADAR_HEIGHT_TILES = 12;

class CRadar {
public:
    static constexpr uint32_t MAX_RADAR_SPRITES = 64;
    static constexpr uint32_t MAX_RADAR_TRACES = 175;

    static inline std::array<tRadarTrace, MAX_RADAR_TRACES> ms_RadarTrace;
	
	static int32_t FindTraceNotTrackingBlipIndex(); // Return the index of the first trace with the `TrackingBlip` flag NOT set
	static tBlipHandle SetCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, char* scriptName = nullptr);
	static tBlipHandle SetCoordBlip(int type, float X, float Y, float Z, int color, int blipDisplay, char* scriptName = "");
    static tBlipHandle GetNewUniqueBlipIndex(int32_t blipIndex);
};