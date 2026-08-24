#pragma once

#include "CRenderTarget.h"
#include "gui/CFontInstance.h"
#include <list>


#define PLATE_BITMAP_WIDTH	512
#define PLATE_BITMAP_HEIGHT 128

enum ePlateType
{
    NUMBERPLATE_TYPE_NONE,
    NUMBERPLATE_TYPE_RUS,		//1
    NUMBERPLATE_TYPE_UA,		//2
    NUMBERPLATE_TYPE_BY,		//3
    NUMBERPLATE_TYPE_KZ,		//4
    NUMBERPLATE_TYPE_RU_POLICE,	//5
    NUMBERPLATE_TYPE_AM,		//6
    NUMBERPLATE_TYPE_TAX,		//7
    NUMBERPLATE_TYPE_GOS		//8
};

class CCustomPlateManager
{
    static CRenderTarget* m_pRenderTarget;

    static struct CSprite2d* m_pRuSprite;
    static struct CSprite2d* m_pUaSprite;
    static struct CSprite2d* m_pBuSprite;
    static struct CSprite2d* m_pKzSprite;
    static struct CSprite2d* m_pAmSprite;
    static struct CSprite2d* m_pNoneSprite;
    static struct CSprite2d* m_pRuPoliceSprite;

    static CFontInstance* m_pRUFont;
    static CFontInstance* m_pKZFont;
    static CFontInstance* m_pUAFont;
    static CFontInstance* m_pAMFont;
    static CFontInstance* m_pBUFont;

    static uint8_t* m_pBitmap;

public:
    static RwTexture* pNoPlateTex;
    static void Initialise();	// call this once in CGame__Process
    static void Shutdown();

    static RwTexture *createTexture(ePlateType type, char *szNumber, char *szRegion);

    static RwTexture *createRuPlate(char *szNumber, char *szRegion);

    static RwTexture *createRuPolicePlate(char *szNumber, char *szRegion);

    static RwTexture *createKzPlate(char *szNumber, char *szRegion);

    static RwTexture *createUaPlate(char *szNumber, char *szRegion);

    static RwTexture *createBuPlate(char* szNumber, char* szRegion);

    static RwTexture *createAmPlate(char* szNumber, char* szRegion);

    static RwTexture* createNonePlate(char* szNumber, char* szRegion);
};