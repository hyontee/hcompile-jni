//
// Created by Error on 18.10.2024.
//

#ifndef BRSELL_CSNAPSHOTS_H
#define BRSELL_CSNAPSHOTS_H
#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"

#include "util/CJavaWrapper.h"
class CSnapShots {
    //static CRenderTarget* m_pRenderTarget;
public:
#pragma pack(1)
    enum eAxis
    {
        X = 0,
        Y,
        Z
    };
    static void Initialise();
    static RwTexture* CreatePedSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom);
    static RwTexture* CreateVehicleSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom, int dwColor1, int dwColor2);
    static RwTexture* CreateObjectSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, CVector* offset);
    static RwTexture* CreateTextureSnapShot(const char* tex);
    static RwTexture* CreatePlayerSnapShot(CVector* vecRot, float fZoom);
};


#endif //BRSELL_CSNAPSHOTS_H
