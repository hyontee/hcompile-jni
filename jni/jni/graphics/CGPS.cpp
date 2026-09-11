//
// Created by admin on 30.12.2023.
//

#include "CGPS.h"
#include "util/armhook.h"
#include "../game_sa/game_sa.h"
#include "game/playerped.h"

#define STREAM_NODES    128 // def 8
#define STREAM_RADIUS   8000.0f
#define STREAM_RADIUS_LIMIT STREAM_RADIUS*1.01f
#define MAX_NODE_POINTS 65536

#define GPS_LINE_R      235
#define GPS_LINE_G      212
#define GPS_LINE_B      0
#define GPS_LINE_A      255

CNodeAddress *aPathNodes;
uint8_t aStreamablePathNodes[8 * STREAM_NODES];

int* ThePaths;
tBlipHandle TargetBlip {0};

unsigned int gpsLineColor = RWRGBALONG(GPS_LINE_R, GPS_LINE_G, GPS_LINE_B, GPS_LINE_A);
CVector2D gpsDistanceTextPos;
CRect emptyRect, radarRect;

CVector2D g_vecUnderRadar(0.0, -1.05); // 0
CVector2D g_vecAboveRadar(0.0, 1.05); // 1
CVector2D g_vecLeftRadar(-1.05, 0.0); // 2
CVector2D g_vecRightRadar(1.05, 0.0); // 3
eFontAlignment g_nTextAlignment;
float gpsDistance;
CVector2D vecTextOffset;
tRadarTrace* pTrace;

CVector2D nodePoints[MAX_NODE_POINTS];
CNodeAddress resultNodes[MAX_NODE_POINTS];
RwOpenGLVertex lineVerts[MAX_NODE_POINTS * 4] {0};

float lineWidth = 3.5f, textOffset, textScale, flMenuMapScaling;
int align = 1;

float (*FindGroundZForCoord)(float, float);
int (*DoPathSearch)(uintptr_t, unsigned char, CVector, CNodeAddress, CVector, CNodeAddress*, short*, int, float*, float, CNodeAddress*, float, bool, CNodeAddress, bool, bool);
void (*TransformRadarPointToRealWorldSpace)(CVector2D& out, CVector2D const& in);
void (*TransformRealWorldPointToRadarSpace)(CVector2D& out, CVector2D const& in);
void (*TransformRadarPointToScreenSpace)(CVector2D& out, CVector2D const& in);
void (*ClearRadarBlip)(uint32_t);
void (*LimitRadarPoint)(CVector2D& in);
void (*SetScissorRect1)(CRect&);

constexpr uint32 TOTAL_DFF_MODEL_IDS = 20000;
constexpr uint32 TOTAL_TXD_MODEL_IDS = 5000;
constexpr uint32 TOTAL_COL_MODEL_IDS = 255;
constexpr uint32 TOTAL_IPL_MODEL_IDS = 256;
constexpr uint32 TOTAL_DAT_MODEL_IDS = 64;
constexpr uint32 TOTAL_IFP_MODEL_IDS = 180;
constexpr uint32 TOTAL_RRR_MODEL_IDS = 475;
constexpr uint32 TOTAL_SCM_MODEL_IDS = 82;
constexpr uint32 TOTAL_INTERNAL_MODEL_IDS = 4; // internal use?

enum eResourceFirstID : int32 {
    // First ID of the resource
    RESOURCE_ID_DFF                = 0,                                     // default: 0
    RESOURCE_ID_TXD                = RESOURCE_ID_DFF + TOTAL_DFF_MODEL_IDS, // default: 20000
    RESOURCE_ID_COL                = RESOURCE_ID_TXD + TOTAL_TXD_MODEL_IDS, // default: 25000
    RESOURCE_ID_IPL                = RESOURCE_ID_COL + TOTAL_COL_MODEL_IDS, // default: 25255
    RESOURCE_ID_DAT                = RESOURCE_ID_IPL + TOTAL_IPL_MODEL_IDS, // default: 25511
    RESOURCE_ID_IFP                = RESOURCE_ID_DAT + TOTAL_DAT_MODEL_IDS, // default: 25575
    RESOURCE_ID_RRR                = RESOURCE_ID_IFP + TOTAL_IFP_MODEL_IDS, // default: 25755   (vehicle recordings)
    RESOURCE_ID_SCM                = RESOURCE_ID_RRR + TOTAL_RRR_MODEL_IDS, // default: 26230   (streamed scripts)

    // Used for CStreaming lists, just search for xrefs (VS: shift f12)
    RESOURCE_ID_LOADED_LIST_START  = RESOURCE_ID_SCM + TOTAL_SCM_MODEL_IDS, // default: 26312
    RESOURCE_ID_LOADED_LIST_END    = RESOURCE_ID_LOADED_LIST_START + 1,     // default: 26313

    RESOURCE_ID_REQUEST_LIST_START = RESOURCE_ID_LOADED_LIST_END + 1,       // default: 26314
    RESOURCE_ID_REQUEST_LIST_END   = RESOURCE_ID_REQUEST_LIST_START + 1,    // default: 26315
    RESOURCE_ID_TOTAL                                               // default: 26316
};

inline int32 DATToModelId(size_t relativeId) { return (size_t)RESOURCE_ID_DAT + relativeId; }

// CPathFind::MarkRegionsForCoors(CVector,float)	002D3084
int (*CPathFind__LoadSceneForPathNodes)(uintptr_t* thiz, CVector point);
int CPathFind__LoadSceneForPathNodes_hook(uintptr_t* thiz, CVector point)
{
    CallFunction<void>(g_libGTASA+0x2D3084+1, thiz, point, STREAM_RADIUS);
    for (int i = 0; i < 8 * STREAM_NODES; i++) {
        if (aStreamablePathNodes[i]) {
            //CStreaming::RequestModel(DATToModelId((int32)i), STREAMING_DEFAULT);
            ((void (*)(int32_t, int32_t))(g_libGTASA + 0x0028EB10 + 1))(DATToModelId((int32)i), 0x0);
        }
    }
}

void CGPS::Initialise()
{
    textOffset = (8.0f * (float)RsGlobal->maximumHeight) / 448.0f;
    textScale = (0.4f * ((float)RsGlobal->maximumWidth) / 640.0f);
    flMenuMapScaling = 0.00223214285f * RsGlobal->maximumHeight;
    vecTextOffset.x = vecTextOffset.y = 0;
}

RwUInt32 CGPS::GetTraceColor(eBlipColour clr, bool friendly)
{
    switch(clr)
    {
        case BLIP_COLOUR_RED:
            return RWRGBALONG(127,0,0,255);
        case BLIP_COLOUR_GREEN:
            return RWRGBALONG(0,127,0,255);
        case BLIP_COLOUR_BLUE:
            return RWRGBALONG(0,0,127,255);
        case BLIP_COLOUR_WHITE:
            return RWRGBALONG(127,127,127,255);
        case BLIP_COLOUR_YELLOW:
            return RWRGBALONG(200,200,0,255);
        case BLIP_COLOUR_REDCOPY:
            return RWRGBALONG(127,0,127,255);
        case BLIP_COLOUR_BLUECOPY:
            return RWRGBALONG(0,127,127,255);
        case BLIP_COLOUR_THREAT:
            return friendly ? RWRGBALONG(0,0,127,255) : RWRGBALONG(127,0,0,255);
        case BLIP_COLOUR_DESTINATION:
            return RWRGBALONG(200,200,0,255);

        default:
            CRGBA a((int)clr);
            return RWRGBALONG(a.R, a.G, a.B, 255);
    }
}

CRGBA rgbclr;
CRGBA& CGPS::GetTraceTextColor(eBlipColour clr, bool friendly)
{
    switch(clr)
    {
        case BLIP_COLOUR_RED:
            return rgbclr = CRGBA(127,0,0,255);
        case BLIP_COLOUR_GREEN:
            return rgbclr = CRGBA(0,127,0,255);
        case BLIP_COLOUR_BLUE:
            return rgbclr = CRGBA(0,0,127,255);
        case BLIP_COLOUR_WHITE:
            return rgbclr = CRGBA(127,127,127,255);
        case BLIP_COLOUR_YELLOW:
            return rgbclr = CRGBA(200,200,0,255);
        case BLIP_COLOUR_REDCOPY:
            return rgbclr = CRGBA(127,0,127,255);
        case BLIP_COLOUR_BLUECOPY:
            return rgbclr = CRGBA(0,127,127,255);
        case BLIP_COLOUR_THREAT:
            return friendly ? rgbclr = CRGBA(0,0,127,255) : rgbclr = CRGBA(127,0,0,255);
        case BLIP_COLOUR_DESTINATION:
            return rgbclr = CRGBA(200,200,0,255);

        default:
            rgbclr = CRGBA((int)clr);
            rgbclr.A = 255;
            return rgbclr;
    }
}

void SetDistanceTextValues()
{
    CVector2D posn;
    TransformRadarPointToScreenSpace(posn, CVector2D(-1.0f, -1.0f));
    radarRect.left = posn.x + 2.0f;
    radarRect.bottom = posn.y - 2.0f;
    TransformRadarPointToScreenSpace(posn, CVector2D(1.0f, 1.0f));
    radarRect.right = posn.x - 2.0f;
    radarRect.top = posn.y + 2.0f;

    switch(align)
    {
        default:
        case 0: // Under
            g_nTextAlignment = ALIGN_CENTER;
            TransformRadarPointToScreenSpace(gpsDistanceTextPos, g_vecUnderRadar);
            gpsDistanceTextPos.x += vecTextOffset.x;
            gpsDistanceTextPos.y += vecTextOffset.y;
            gpsDistanceTextPos.y += textOffset;
            break;

        case 1: // Above
            g_nTextAlignment = ALIGN_CENTER;
            TransformRadarPointToScreenSpace(gpsDistanceTextPos, g_vecAboveRadar);
            gpsDistanceTextPos.x += vecTextOffset.x;
            gpsDistanceTextPos.y += vecTextOffset.y;
            gpsDistanceTextPos.y -= textOffset;
            break;

        case 2: // Left
            g_nTextAlignment = ALIGN_RIGHT;
            TransformRadarPointToScreenSpace(gpsDistanceTextPos, g_vecLeftRadar);
            gpsDistanceTextPos.x += vecTextOffset.x;
            gpsDistanceTextPos.y += vecTextOffset.y;
            gpsDistanceTextPos.x -= textOffset;
            break;

        case 3: // Right
            g_nTextAlignment = ALIGN_LEFT;
            TransformRadarPointToScreenSpace(gpsDistanceTextPos, g_vecRightRadar);
            gpsDistanceTextPos.x += vecTextOffset.x;
            gpsDistanceTextPos.y += vecTextOffset.y;
            gpsDistanceTextPos.x += textOffset;
            break;

        case 4: // Custom
            gpsDistanceTextPos = vecTextOffset;
            break;
    }
}

void CGPS::Setup2DVertex(RwOpenGLVertex &vertex, float x, float y, RwUInt32 color)
{
    const RwReal nearScreenZ = 		*(RwReal*)(SA_ADDR(0x9DAA60));	// CSprite2d::NearScreenZ 009DAA60
    const RwReal recipNearClip = 	*(RwReal*)(SA_ADDR(0x9DAA64));	// CSprite2d::RecipNearClip 009DAA64

    vertex.x = x;
    vertex.y = y;
    //vertex.texCoord.u = vertex.texCoord.v = 0.0f;
    vertex.z = nearScreenZ + 0.0001f;
    vertex.rhw = recipNearClip;
    vertex.emissiveColor = color;
}

inline bool IsBMXNaviAllowed()
{
    return false;
}

inline bool IsInSupportedVehicle(CPlayerPed* player)
{
    return (player &&
            player->GetGtaVehicle() &&
            player->IsInVehicle() &&
            player->GetGtaVehicle()->entity.vtable != g_libGTASA+0x5CD0B0 &&
            player->GetGtaVehicle()->entity.vtable != g_libGTASA+0x5CCE60);
}

inline bool LaneDirectionRespected()
{
    return false;
}

inline bool IsBoatNaviAllowed()
{
    return false;
}
bool *m_UserPause, *m_CodePause;
inline bool IsRadarVisible()
{
    CWidgetGta* radar = aWidgets[161];
    Log("IsRadarVisible %d", radar->m_bEnabled);
    return (radar != NULL && radar->m_bEnabled);
}
inline bool IsGamePaused() { return m_CodePause || m_UserPause; };
inline bool IsRGBValue(int value) { return value >= 0 && value <= 255; }

char text[24];
unsigned short* textGxt = new unsigned short[0xFF];
// CDebug::DebugDisplayTextBuffer(void)	00390028
void (*CDebug_DebugDisplayTextBuffer)();
void CDebug_DebugDisplayTextBuffer_hook()
{
    CDebug_DebugDisplayTextBuffer();
    if(gpsDistance > 0.0f && !IsGamePaused() && IsRadarVisible())
    {
        static bool bInit = false;
        if(!bInit)
        {
            bInit = true;
            SetDistanceTextValues();
        }

        CRGBA rgbaWhite;
        rgbaWhite.R = 255;rgbaWhite.G = 255;
        rgbaWhite.B = 255;rgbaWhite.A = 255;


        if(gpsDistance == 100000.0f) sprintf(text, "Far from the road!");
        else if (gpsDistance > 1609.344f) sprintf(text, "%.2fmil", 0.000621371192237334f * gpsDistance);
        else sprintf(text, "%dyrd", (int)(gpsDistance * 1.094f));
        CFont::AsciiToGxtChar(text, textGxt);

        CFont::SetOrientation(g_nTextAlignment);
        if(!TargetBlip.arrayIndex && pTrace) CFont::SetColor((CRGBA*)&CGPS::GetTraceTextColor(pTrace->m_nColour, pTrace->m_bFriendly));
        else CFont::SetColor((CRGBA*)&rgbaWhite);
        CFont::SetBackground(false, false);
        CFont::SetWrapX(500.0f);
        CFont::SetScale(textScale);
        CFont::SetFontStyle(FONT_SUBTITLES);
        CFont::SetProportional(true);
        CFont::SetDropShadowPosition(1);
        CFont::PrintString(gpsDistanceTextPos.x, gpsDistanceTextPos.y, textGxt);
        ((void (*)())(SA_ADDR(0x53411C + 1)))();
    }
    gpsDistance = 0;
}

extern CGame* pGame;
void DoPathDraw(CVector to, RwUInt32 color, bool isTargetBlip = false, float* dist = NULL)
{
    CPlayerPed* player = pGame->FindPlayerPed();

    short nodesCount = 0;
    float trashVar;
    bool isGamePaused = IsGamePaused(), bScissors = !isGamePaused || !gMobileMenu->DisplayingMap;

    DoPathSearch((uintptr_t)ThePaths, LaneDirectionRespected(), player->m_pPed->entity.mat->pos,
                 CNodeAddress(), to, resultNodes, &nodesCount, MAX_NODE_POINTS, dist ? dist : &trashVar, 1000000.0f, NULL, 1000000.0f, false,
                 CNodeAddress(), false, IsBoatNaviAllowed());

    if(nodesCount > 0)
    {
        if(isTargetBlip && bScissors &&
           gpsDistance < 50.0f)
        {
            ClearRadarBlip(TargetBlip.arrayIndex);
            gMobileMenu->waypoint_blip.arrayIndex = 0;
            TargetBlip.arrayIndex = 0;
            return;
        }

        CPathNode* node;
        CVector2D nodePos;
        if (isGamePaused)
        {
            for (int i = 0; i < nodesCount; ++i)
            {
                node = (CPathNode*)(ThePaths[513 + resultNodes[i].m_wAreaId] + 28 * resultNodes[i].m_wNodeId);
                nodePos = CVector2D(node->GetNodeCoors().x, node->GetNodeCoors().y);
                TransformRealWorldPointToRadarSpace(nodePos, nodePos);
                LimitRadarPoint(nodePos);
                TransformRadarPointToScreenSpace(nodePoints[i], nodePos);
                nodePoints[i].x *= flMenuMapScaling;
                nodePoints[i].y *= flMenuMapScaling;
            }
        }
        else
        {
            for (int i = 0; i < nodesCount; ++i)
            {
                node = (CPathNode*)(ThePaths[513 + resultNodes[i].m_wAreaId] + 28 * resultNodes[i].m_wNodeId);
                nodePos = CVector2D(node->GetNodeCoors().x, node->GetNodeCoors().y);
                TransformRealWorldPointToRadarSpace(nodePos, nodePos);
                TransformRadarPointToScreenSpace(nodePoints[i], nodePos);
            }
        }

        if (bScissors) SetScissorRect1(radarRect); // Scissor
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);

        unsigned int vertIndex = 0;
        --nodesCount;

        CVector2D point[4], shift[2], dir;
        float angle;
        if (isGamePaused)
        {
            float mp = gMobileMenu->NEW_MAP_SCALE - 140.0f;
            if (mp < 140.0f) mp = 140.0f;
            else if (mp > 960.0f) mp = 960.0f;
            mp = mp / 960.0f + 0.4f;
            mp *= lineWidth;

            for (int i = 0; i < nodesCount; i++)
            {
                dir = {nodePoints[i + 1].x - nodePoints[i].x, nodePoints[i + 1].y - nodePoints[i].y};
                angle = atan2(dir.y, dir.x);

                sincosf(angle - 1.5707963f, &shift[0].y, &shift[0].x); shift[0].x *= mp; shift[0].y *= mp;
                sincosf(angle + 1.5707963f, &shift[1].y, &shift[1].x); shift[1].x *= mp; shift[1].y *= mp;

                CGPS::Setup2DVertex(lineVerts[vertIndex], nodePoints[i].x + shift[0].x, nodePoints[i].y + shift[0].y, color);
                CGPS::Setup2DVertex(lineVerts[++vertIndex], nodePoints[i + 1].x + shift[0].x, nodePoints[i + 1].y + shift[0].y, color);
                CGPS::Setup2DVertex(lineVerts[++vertIndex], nodePoints[i].x + shift[1].x, nodePoints[i].y + shift[1].y, color);
                CGPS::Setup2DVertex(lineVerts[++vertIndex], nodePoints[i + 1].x + shift[1].x, nodePoints[i + 1].y + shift[1].y, color);
                ++vertIndex;
            }
        }
        else
        {
            for (int i = 0; i < nodesCount; i++)
            {
                dir = {nodePoints[i + 1].x - nodePoints[i].x, nodePoints[i + 1].y - nodePoints[i].y};
                angle = atan2(dir.y, dir.x);

                sincosf(angle - 1.5707963f, &shift[0].y, &shift[0].x); shift[0].x *= lineWidth; shift[0].y *= lineWidth;
                sincosf(angle + 1.5707963f, &shift[1].y, &shift[1].x); shift[1].x *= lineWidth; shift[1].y *= lineWidth;

                CGPS::Setup2DVertex(lineVerts[vertIndex], nodePoints[i].x + shift[0].x, nodePoints[i].y + shift[0].y, color);
                CGPS::Setup2DVertex(lineVerts[++vertIndex], nodePoints[i + 1].x + shift[0].x, nodePoints[i + 1].y + shift[0].y, color);
                CGPS::Setup2DVertex(lineVerts[++vertIndex], nodePoints[i].x + shift[1].x, nodePoints[i].y + shift[1].y, color);
                CGPS::Setup2DVertex(lineVerts[++vertIndex], nodePoints[i + 1].x + shift[1].x, nodePoints[i + 1].y + shift[1].y, color);
                ++vertIndex;
            }
        }
        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, lineVerts, 4 * nodesCount);
        if (bScissors) SetScissorRect1(emptyRect); // Scissor
    }
}

/*uint8_t bGZ = 0;
#include "../net/netgame.h"
extern CNetGame* pNetGame;
void (*CRadar__DrawRadarGangOverlay1)(uint8_t v1);
void CRadar__DrawRadarGangOverlay1_hook(uint8_t v1)
{
    bGZ = v1;
    if (pNetGame && pNetGame->GetGangZonePool())
        pNetGame->GetGangZonePool()->Draw();

    Log("waypoint: %d", gMobileMenu->waypoint_blip.arrayIndex);
    if(gMobileMenu->waypoint_blip.arrayIndex)
    {
        bool isGamePaused = IsGamePaused();
        if(TargetBlip.arrayIndex != gMobileMenu->waypoint_blip.arrayIndex && !isGamePaused && IsRadarVisible())
        {
            TargetBlip = gMobileMenu->waypoint_blip;
        }

        CVector& bpos = pRadarTrace[gMobileMenu->waypoint_blip.number].m_vPosition;
        if(bpos.z == 0) bpos.z = FindGroundZForCoord(bpos.x, bpos.y) + 5.0f;
        DoPathDraw(bpos, gpsLineColor, true, &gpsDistance);
    }
    else
    {
        TargetBlip.arrayIndex = 0;
    }
}*/

void CGPS::InjectHooks() {
    SET_TO(m_UserPause, getSym(hGTASA, "_ZN6CTimer11m_UserPauseE"));
    SET_TO(m_CodePause, getSym(hGTASA, "_ZN6CTimer11m_CodePauseE"));

    SET_TO(TransformRadarPointToRealWorldSpace, getSym(hGTASA, "_ZN6CRadar35TransformRadarPointToRealWorldSpaceER9CVector2DRKS0_"));
    SET_TO(TransformRealWorldPointToRadarSpace, getSym(hGTASA, "_ZN6CRadar35TransformRealWorldPointToRadarSpaceER9CVector2DRKS0_"));
    SET_TO(TransformRadarPointToScreenSpace, getSym(hGTASA, "_ZN6CRadar32TransformRadarPointToScreenSpaceER9CVector2DRKS0_"));
    SET_TO(LimitRadarPoint, getSym(hGTASA, "_ZN6CRadar15LimitRadarPointER9CVector2D"));
    SET_TO(ClearRadarBlip, getSym(hGTASA, "_ZN6CRadar9ClearBlipEi"));
    SET_TO(DoPathSearch, getSym(hGTASA, "_ZN9CPathFind12DoPathSearchEh7CVector12CNodeAddressS0_PS1_PsiPffS2_fbS1_bb"));
    SET_TO(FindGroundZForCoord, getSym(hGTASA, "_ZN6CWorld19FindGroundZForCoordEff"));
    SET_TO(ThePaths, getSym(hGTASA, "ThePaths"));
    SET_TO(SetScissorRect1, getSym(hGTASA, "_ZN7CWidget10SetScissorER5CRect"));

    for(int i = 0; i < 8 * MAX_NODE_POINTS; ++i) aStreamablePathNodes[i] = 1;
    aPathNodes = new CNodeAddress[MAX_NODE_POINTS];
    for(int i = 0; i < MAX_NODE_POINTS; ++i) *(uint32_t*)(&aPathNodes[i]) = 0xFFFF;

    //InlineHook(g_libGTASA, 0x3DE9A8, &CRadar__DrawRadarGangOverlay1_hook, &CRadar__DrawRadarGangOverlay1);
    InlineHook(g_libGTASA, 0x390028, &CDebug_DebugDisplayTextBuffer_hook, &CDebug_DebugDisplayTextBuffer);
    //InlineHook(g_libGTASA, 0x2D3084, &CPathFind__LoadSceneForPathNodes_hook, &CPathFind__LoadSceneForPathNodes);

    WriteMemory(g_libGTASA + 0x2D1DCE, (uintptr)"\x4C\xF2\x50\x32", 4); // 4999 -> 50000
    WriteMemory(g_libGTASA + 0x2D1EC0, (uintptr)"\x4C\xF2\x1E\x32", 4); // 4949 -> 49950
    unProtect(g_libGTASA + 0x2D209C, sizeof(void*)); *(uintptr_t*)(g_libGTASA + 0x2D209C) = (uintptr_t)aPathNodes - 0x2D1D3E - g_libGTASA;
    unProtect(g_libGTASA + 0x2D20A0, sizeof(void*)); *(uintptr_t*)(g_libGTASA + 0x2D20A0) = (uintptr_t)aPathNodes - 0x2D1D4A - g_libGTASA;
    unProtect(g_libGTASA + 0x2D20A4, sizeof(void*)); *(uintptr_t*)(g_libGTASA + 0x2D20A4) = (uintptr_t)aPathNodes - 0x2D2042 - g_libGTASA;
    unProtect(g_libGTASA + 0x2D20A8, sizeof(void*)); *(uintptr_t*)(g_libGTASA + 0x2D20A8) = (uintptr_t)aPathNodes - 0x2D2078 - g_libGTASA;

    //005D0854
    unProtect(g_libGTASA + 0x5D0854, sizeof(void*)); *(uintptr_t*)(g_libGTASA + 0x5D0854) = (uintptr_t)aStreamablePathNodes;
    //Write(g_libGTASA+0x002D3988, "\x40", 1);
    //Write(g_libGTASA+0x2D3A1A, "\x04\xF1\x40\x04", 4);

    //Write(g_libGTASA+0x2D3922, "\x31\xEF\x08", 3);*/
}