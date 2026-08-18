#include "../main.h"
#include "game.h"

#if VER_x32
CAMERA_AIM* GameGetInternalAim()
{
    return (CAMERA_AIM*)(VER_x32 ? g_libGTASA + 0x00951FA8 + 528 * *((unsigned char*)(g_libGTASA + 0x951FFF)) + 728 : 0xBBABB0);
}
#else

//v_2.1
CAMERA_AIM* GameGetInternalAim()
{
    //return (CAMERA_AIM*)(VER_x32 ? g_libGTASA + 0x00951FA8 + 528 * *((unsigned char*)(g_libGTASA + 0x951FFF)) + 728 : 0xBBABB0);


    CCameraGta& origCam = *reinterpret_cast<CCameraGta*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));
   return reinterpret_cast<CAMERA_AIM *>(&origCam.m_aCams[0].Front);
}
#endif
CAMERA_AIM caLocalPlayerAim;
CAMERA_AIM caRemotePlayerAim[MAX_PLAYERS];
uint16_t * pbyteCameraMode = nullptr;
uint16_t* wCameraMode2 = nullptr;

CAMERA_AIM * pcaInternalAim = nullptr;
float * pfCameraExtZoom = nullptr;
float* pfAspectRatio = nullptr;

float fCameraExtZoom[MAX_PLAYERS];
float fLocalCameraExtZoom;

float fCameraAspectRatio[MAX_PLAYERS];
float fLocalAspectRatio;

uint16_t byteCameraMode[MAX_PLAYERS];

//----------------------------------------------------------

void GameStoreLocalPlayerCameraExtZoom()
{
    fLocalCameraExtZoom = *pfCameraExtZoom;
    fLocalAspectRatio = *pfAspectRatio;
}

//----------------------------------------------------------

void GameSetLocalPlayerCameraExtZoom()
{
    *pfCameraExtZoom = fLocalCameraExtZoom;
    *pfAspectRatio = fLocalAspectRatio;
}

//----------------------------------------------------------

void GameSetPlayerCameraExtZoom(int bytePlayerID, float fZoom)
{
    fCameraExtZoom[bytePlayerID] = fZoom;
}

void GameSetPlayerCameraExtZoom(int bytePlayerID, float fZoom, float fAspectRatio)
{
    fCameraExtZoom[bytePlayerID] = fZoom;
    fCameraAspectRatio[bytePlayerID] = fAspectRatio;
}

//----------------------------------------------------------

float GameGetPlayerCameraExtZoom(int bytePlayerID)
{
    return fCameraExtZoom[bytePlayerID];
}

//----------------------------------------------------------

float GameGetLocalPlayerCameraExtZoom()
{
    float value = ((*pfCameraExtZoom) - 35.0f) / 35.0f;	// normalize for 35.0 to 70.0
    return value;
}

//----------------------------------------------------------

void GameSetRemotePlayerCameraExtZoom(int bytePlayerID)
{
    float fZoom = fCameraExtZoom[bytePlayerID];
    float fValue = fZoom * 35.0f + 35.0f; // unnormalize for 35.0 to 70.0
    *pfCameraExtZoom = fValue;
}

//----------------------------------------------------------

void GameSetPlayerCameraMode(uint16_t byteMode, int bytePlayerID)
{
    byteCameraMode[bytePlayerID] = byteMode;
}

//----------------------------------------------------------

uint16_t GameGetPlayerCameraMode(int bytePlayerID)
{
    return byteCameraMode[bytePlayerID];
}

//----------------------------------------------------------

uint16_t GameGetLocalPlayerCameraMode()
{
    if (!pbyteCameraMode) return 0;
    return *pbyteCameraMode;
}

void GameSetLocalPlayerCameraMode(uint16_t mode)
{
    if (!pbyteCameraMode) return;
    *pbyteCameraMode = mode;
}

//----------------------------------------------------------

void GameAimSyncInit()
{
    memset(&caLocalPlayerAim, 0, sizeof(caLocalPlayerAim));
    memset(&caRemotePlayerAim, 0, sizeof(caRemotePlayerAim));
    memset(byteCameraMode, 4, sizeof(byteCameraMode));

    pbyteCameraMode = (uint16_t*)(g_libGTASA + (VER_x32 ? 0x00951FA8 + 528 * *((unsigned char*)(g_libGTASA + 0x951FFF)) + 382 : 0xBBAA56));
    pcaInternalAim = GameGetInternalAim();
    pfCameraExtZoom = (float*)(g_libGTASA + (VER_x32 ? 0x00951FA8 + 528 * *((unsigned char*)(g_libGTASA + 0x951FFF)) + 508 : 0xBBAAD4));
    pfAspectRatio = (float*)(g_libGTASA + (VER_x32 ? 0x00A26A90 : 0xCC7F00));
    wCameraMode2 = (uint16_t*)(g_libGTASA + (VER_x32 ? 0x00951FA8 + 0x7B4 : 0xBBB0DC));


    for (int i = 0; i < MAX_PLAYERS; i++)
        fCameraExtZoom[i] = 1.0f;

}

//----------------------------------------------------------

void GameStoreLocalPlayerAim()
{
    if (!pcaInternalAim) return;
    memcpy(&caLocalPlayerAim, pcaInternalAim, sizeof(CAMERA_AIM));
}

//----------------------------------------------------------

void GameSetLocalPlayerAim()
{
    //if (!pcaInternalAim) return;
    memcpy(pcaInternalAim, &caLocalPlayerAim, sizeof(CAMERA_AIM));
    //memcpy(pInternalCamera,&SavedCam,sizeof(RwMatrix));
}

//----------------------------------------------------------

//----------------------------------------------------------

void GameStoreRemotePlayerAim(int iPlayer, CAMERA_AIM * caAim)
{
    memcpy(&caRemotePlayerAim[iPlayer], caAim, sizeof(CAMERA_AIM));
}

//----------------------------------------------------------

void GameSetRemotePlayerAim(int iPlayer)
{
    if (!pcaInternalAim) return;
    memcpy(pcaInternalAim, &caRemotePlayerAim[iPlayer], sizeof(CAMERA_AIM));
}

//----------------------------------------------------------

CAMERA_AIM * GameGetRemotePlayerAim(int iPlayer)
{
    return &caRemotePlayerAim[iPlayer];
}

float GameGetAspectRatio()
{
    return *pfAspectRatio;
}