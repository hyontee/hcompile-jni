#include "CCustomPlateManager.h"

#include "../main.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "..//net/netgame.h"
#include "..//chatwindow.h"
#include "util/util.h"
#include "..//gui/CFontInstance.h"
#include "..//gui/CFontRenderer.h"

extern CNetGame* pNetGame;

CRenderTarget* CCustomPlateManager::m_pRenderTarget = nullptr;

struct CSprite2d* CCustomPlateManager::m_pRuSprite = nullptr;
struct CSprite2d* CCustomPlateManager::m_pUaSprite = nullptr;
struct CSprite2d* CCustomPlateManager::m_pBuSprite = nullptr;
struct CSprite2d* CCustomPlateManager::m_pKzSprite = nullptr;
struct CSprite2d* CCustomPlateManager::m_pAmSprite = nullptr;
struct CSprite2d* CCustomPlateManager::m_pNoneSprite = nullptr;
struct CSprite2d* CCustomPlateManager::m_pRuPoliceSprite = nullptr;

RwTexture* CCustomPlateManager::pNoPlateTex = nullptr;

CFontInstance* CCustomPlateManager::m_pRUFont = nullptr;
CFontInstance* CCustomPlateManager::m_pKZFont = nullptr;
CFontInstance* CCustomPlateManager::m_pUAFont = nullptr;
CFontInstance* CCustomPlateManager::m_pAMFont = nullptr;
CFontInstance* CCustomPlateManager::m_pBUFont = nullptr;

uint8_t* CCustomPlateManager::m_pBitmap = nullptr;

RwTexture* CCustomPlateManager::createTexture(ePlateType type, char* szNumber, char* szRegion)
{
//	type = NUMBERPLATE_TYPE_BY;
//    szNumber = "8682 AX-3";
//    szRegion = "01";

    switch(type) {
        case NUMBERPLATE_TYPE_UA: {
            return createUaPlate(szNumber, szRegion);
        }
        case NUMBERPLATE_TYPE_KZ: {
            return createKzPlate(szNumber, szRegion);
        }
        case NUMBERPLATE_TYPE_BY: {
            return createBuPlate(szNumber, szRegion);
        }
        case NUMBERPLATE_TYPE_RUS: {
            return createRuPlate(szNumber, szRegion);
        }
        case NUMBERPLATE_TYPE_RU_POLICE: {
            return createRuPolicePlate(szNumber, szRegion);
        }
        case NUMBERPLATE_TYPE_AM: {
            return createAmPlate(szNumber, szRegion);
        }
        default: {
            return nullptr;
        }
    }

}

void CCustomPlateManager::Initialise()
{
    char path[0xFF];
    sprintf(path, "%sSAMP/plates/ru_font.ttf", g_pszStorage);

    m_pRUFont = CFontRenderer::AddFont(path, 132);
    m_pKZFont = CFontRenderer::AddFont(path, 132);
    m_pBUFont = CFontRenderer::AddFont(path, 80);

    sprintf(path, "%sSAMP/plates/ua_font.ttf", g_pszStorage);
    m_pUAFont = CFontRenderer::AddFont(path, 68);
    m_pAMFont = CFontRenderer::AddFont(path, 73);

    m_pRenderTarget = new CRenderTarget(256, 64, true);

    m_pUaSprite = new CSprite2d();
    m_pAmSprite = new CSprite2d();
    m_pRuSprite = new CSprite2d();
    m_pBuSprite = new CSprite2d();
    m_pKzSprite = new CSprite2d();
    m_pRuPoliceSprite = new CSprite2d();
    m_pNoneSprite = new CSprite2d();


    m_pUaSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "plate_ua");
    m_pAmSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "plate_am");
    m_pRuSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "plate_ru");
    m_pBuSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "plate_bu");
    m_pKzSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "plate_kz");
    m_pNoneSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "remap_plate_c");
    m_pRuPoliceSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "plate_ru_police");

    m_pBitmap = new uint8_t[PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH];

    Log("CCustomPlateManager::Initialise...");
}

void CCustomPlateManager::Shutdown()
{
    if (m_pRenderTarget)
    {
        delete m_pRenderTarget;
        m_pRenderTarget = nullptr;
    }

    if (m_pAmSprite)
    {
        delete m_pAmSprite;
        m_pAmSprite = nullptr;
    }

    if (m_pNoneSprite)
    {
        delete m_pAmSprite;
        m_pAmSprite = nullptr;
    }

    if (m_pRuSprite)
    {
        delete m_pRuSprite;
        m_pRuSprite = nullptr;
    }

    if (m_pUaSprite)
    {
        delete m_pUaSprite;
        m_pUaSprite = nullptr;
    }

    if (m_pBuSprite)
    {
        delete m_pBuSprite;
        m_pBuSprite = nullptr;
    }

    if (m_pKzSprite)
    {
        delete m_pKzSprite;
        m_pKzSprite = nullptr;
    }

    if (m_pRuPoliceSprite)
    {
        delete m_pRuPoliceSprite;
        m_pRuPoliceSprite = nullptr;
    }

    if (m_pBitmap)
    {
        delete[] m_pBitmap;
        m_pBitmap = nullptr;
    }

}

RwTexture* CCustomPlateManager::createUaPlate(char* szNumber, char* szRegion)
{
    if (!m_pUaSprite)
        return nullptr;

    if (!m_pUaSprite->m_pRwTexture)
        return nullptr;

    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);

    int x_max, y_max;
    CFontRenderer::RenderText(szNumber, m_pUAFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);

    for (int i = 0; i < PLATE_BITMAP_WIDTH * PLATE_BITMAP_HEIGHT; i++)
        m_pBitmap[i] = 255 - m_pBitmap[i];

    RwRaster* pTextRaster = GetRWRasterFromBitmap(m_pBitmap + (PLATE_BITMAP_WIDTH * 0), PLATE_BITMAP_WIDTH, 256, 64);

    if (!pTextRaster)
        return nullptr;

    auto* pText = new CSprite2d();

    pText->m_pRwTexture = ((struct RwTexture* (*)(struct RwRaster*))(g_libGTASA + 0x1B1B4C + 1))(pTextRaster); // RwTextureCreate

    CRGBA white(255, 255, 255, 255);

    m_pRenderTarget->Begin();

    m_pUaSprite->Draw(0.0f, 0.0f, 256.0f, 64.0f, white);
    pText->Draw(36.0f, 8.0f, 220.0f, 42.0f, white);

    delete pText;

    return m_pRenderTarget->End();
}

RwTexture* CCustomPlateManager::createBuPlate(char* szNumber, char* szRegion)
{
    if (!m_pBuSprite)
    {
        return nullptr;
    }
    if (!m_pBuSprite->m_pRwTexture)
    {
        return nullptr;
    }

    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);

    int x_max, y_max;
    CFontRenderer::RenderText(szNumber, m_pBUFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);

    for (int i = 0; i < PLATE_BITMAP_WIDTH * PLATE_BITMAP_HEIGHT; i++)
    {
        m_pBitmap[i] = 255 - m_pBitmap[i];
    }

    RwRaster* pTextRaster = GetRWRasterFromBitmap(m_pBitmap + (PLATE_BITMAP_WIDTH * 0), PLATE_BITMAP_WIDTH, 256, 64);

    if (!pTextRaster)
    {
        return nullptr;
    }

    CSprite2d* pText = new CSprite2d();

    if (!pText)
    {
        RwRasterDestroy(pTextRaster);
        return nullptr;
        // hz
    }

    pText->m_pRwTexture = ((struct RwTexture* (*)(struct RwRaster*))(SA_ADDR(0x1B1B4C + 1)))(pTextRaster); // RwTextureCreate

    CRGBA white;
    white.A = 255;
    white.R = 255;
    white.G = 255;
    white.B = 255;

    m_pRenderTarget->Begin();
    m_pBuSprite->Draw(0.0f, 0.0f, 256.0f, 64.0f, white);
    pText->Draw(40.0f, 4.0f, 250.0f, 42.0f, white);
    RwTexture* pTexture = m_pRenderTarget->End();

    delete pText;

    return pTexture;
}

RwTexture* CCustomPlateManager::createKzPlate(char* szNumber, char* szRegion)
{
    if (!m_pKzSprite)
        return nullptr;

    if (!m_pKzSprite->m_pRwTexture)
        return nullptr;

    // process text
    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);

    int x_max, y_max;
    CFontRenderer::RenderText(szNumber, m_pKZFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);

    for (int i = 0; i < PLATE_BITMAP_WIDTH * PLATE_BITMAP_HEIGHT; i++)
        m_pBitmap[i] = 255 - m_pBitmap[i];

    RwRaster* pTextRaster = GetRWRasterFromBitmap(m_pBitmap + (PLATE_BITMAP_WIDTH * 30), PLATE_BITMAP_WIDTH, 256, 64);

    // process region code
    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);
    CFontRenderer::RenderText(szRegion, m_pKZFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);

    for (int i = 0; i < PLATE_BITMAP_WIDTH * PLATE_BITMAP_HEIGHT; i++)
        m_pBitmap[i] = 255 - m_pBitmap[i];

    RwRaster* pRegionRaster = GetRWRasterFromBitmap(m_pBitmap + (PLATE_BITMAP_WIDTH * 30), PLATE_BITMAP_WIDTH, 128, 64);

    if (!pRegionRaster || !pTextRaster)
    {
        if (pRegionRaster)
            RwRasterDestroy(pRegionRaster);

        if (pTextRaster)
            RwRasterDestroy(pTextRaster);

        return nullptr;
    }

    auto* pText = new CSprite2d();
    auto* pRegion = new CSprite2d();

    pText->m_pRwTexture = ((struct RwTexture* (*)(struct RwRaster*))(g_libGTASA + 0x1B1B4C + 1))(pTextRaster); // RwTextureCreate
    pRegion->m_pRwTexture = ((struct RwTexture* (*)(struct RwRaster*))(g_libGTASA + 0x1B1B4C + 1))(pRegionRaster); // RwTextureCreate

    CRGBA white(255, 255, 255, 255);

    m_pRenderTarget->Begin();

    m_pKzSprite->Draw(0.0f, 0.0f, 256.0f, 64.0f, white);
    pText->Draw(35.0f, 12.0f, 150.0f, 33.0f, white);
    pRegion->Draw(210.0f, 14.0f, 42.0f, 30.0f, white);

    delete pText;
    delete pRegion;

    return m_pRenderTarget->End();
}

RwTexture* CCustomPlateManager::createRuPolicePlate(char* szNumber, char* szRegion)
{
    // process text
    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);

    int x_max, y_max;
    CFontRenderer::RenderText(szNumber, m_pRUFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);

    RwRaster* pTextRaster = GetRWRasterFromBitmapPalette(m_pBitmap + (PLATE_BITMAP_WIDTH * 30), PLATE_BITMAP_WIDTH, 256, 64, 0, 85, 185, 0);

    // process region code
    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);
    CFontRenderer::RenderText(szRegion, m_pRUFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);

    RwRaster* pRegionRaster = GetRWRasterFromBitmapPalette(m_pBitmap + (PLATE_BITMAP_WIDTH * 30), PLATE_BITMAP_WIDTH, 128, 64, 0, 85, 185, 0);

    if (!pRegionRaster || !pTextRaster)
    {
        if (pRegionRaster)
        {
            RwRasterDestroy(pRegionRaster);
        }
        if (pTextRaster)
        {
            RwRasterDestroy(pTextRaster);
        }
        return nullptr;
    }

    auto pText = new CSprite2d();
    auto pRegion = new CSprite2d();


    pText->m_pRwTexture = RwTextureCreate(pTextRaster); // RwTextureCreate
    pRegion->m_pRwTexture = RwTextureCreate(pRegionRaster); // RwTextureCreate

    CRGBA white {255, 255, 255, 255};

    m_pRenderTarget->Begin();
    m_pRuPoliceSprite->Draw(0.0f, 0.0f, 256.0f, 64.0f, white);
    pText->Draw(22.0f, 12.0f, 160.0f, 44.0f, white);
    pRegion->Draw(206.0f, 14.0f, 38.0f, 24.0f, white);
    RwTexture* pTexture = m_pRenderTarget->End();

    delete pText;
    delete pRegion;

    return pTexture;
}

RwTexture* CCustomPlateManager::createRuPlate(char* szNumber, char* szRegion)
{
    // process text
    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);

    int x_max, y_max;
    CFontRenderer::RenderText(szNumber, m_pRUFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);

    for (int i = 0; i < PLATE_BITMAP_WIDTH * PLATE_BITMAP_HEIGHT; i++)
    {
        m_pBitmap[i] = 255 - m_pBitmap[i];
    }

    RwRaster* pTextRaster = GetRWRasterFromBitmap(m_pBitmap + (PLATE_BITMAP_WIDTH * 30), PLATE_BITMAP_WIDTH, 256, 64);

    // process region code
    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);
    CFontRenderer::RenderText(szRegion, m_pRUFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);
    for (int i = 0; i < PLATE_BITMAP_WIDTH * PLATE_BITMAP_HEIGHT; i++)
    {
        m_pBitmap[i] = 255 - m_pBitmap[i];
    }

    RwRaster* pRegionRaster = GetRWRasterFromBitmap(m_pBitmap + (PLATE_BITMAP_WIDTH * 30), PLATE_BITMAP_WIDTH, 128, 64);

    if (!pRegionRaster || !pTextRaster)
    {
        if (pRegionRaster)
        {
            RwRasterDestroy(pRegionRaster);
        }
        if (pTextRaster)
        {
            RwRasterDestroy(pTextRaster);
        }
        return nullptr;
    }

    CSprite2d* pText = new CSprite2d();
    CSprite2d* pRegion = new CSprite2d();

    pText->m_pRwTexture = RwTextureCreate(pTextRaster); // RwTextureCreate
    pRegion->m_pRwTexture = RwTextureCreate(pRegionRaster); // RwTextureCreate

    CRGBA white {255, 255, 255, 255};

    m_pRenderTarget->Begin();
    m_pRuSprite->Draw(0.0f, 0.0f, 256.0f, 64.0f, white);
    pText->Draw(22.0f, 8.0f, 160.0f, 38.0f, white); // x y width height �����
    pRegion->Draw(212.0f, 11.0f, 33.0f, 20.0f, white); // x y width height ������
    RwTexture* pTexture = m_pRenderTarget->End();

    delete pText;
    delete pRegion;

    return pTexture;
}

RwTexture* CCustomPlateManager::createAmPlate(char* szNumber, char* szRegion)
{
    if (!m_pAmSprite)
    {
        return nullptr;
    }
    if (!m_pAmSprite->m_pRwTexture)
    {
        return nullptr;
    }

    memset(m_pBitmap, 0, PLATE_BITMAP_HEIGHT * PLATE_BITMAP_WIDTH);

    int x_max, y_max;
    CFontRenderer::RenderText(szNumber, m_pAMFont, m_pBitmap, PLATE_BITMAP_WIDTH, x_max, y_max);

    for (int i = 0; i < PLATE_BITMAP_WIDTH * PLATE_BITMAP_HEIGHT; i++)
    {
        m_pBitmap[i] = 255 - m_pBitmap[i];
    }

    RwRaster* pTextRaster = GetRWRasterFromBitmap(m_pBitmap + (PLATE_BITMAP_WIDTH * 0), PLATE_BITMAP_WIDTH, 256, 64);

    if (!pTextRaster)
    {
        return nullptr;
    }

    CSprite2d* pText = new CSprite2d();

    if (!pText)
    {
        RwRasterDestroy(pTextRaster);
        return nullptr;
        // hz
    }

    pText->m_pRwTexture = ((struct RwTexture* (*)(struct RwRaster*))(SA_ADDR(0x1B1B4C + 1)))(pTextRaster); // RwTextureCreate

    CRGBA white;
    white.A = 255;
    white.R = 255;
    white.G = 255;
    white.B = 255;

    m_pRenderTarget->Begin();
    m_pAmSprite->Draw(0.0f, 0.0f, 256.0f, 64.0f, white);
    pText->Draw(50.0f, 6.0f, 200.0f, 42.0f, white);
    RwTexture* pTexture = m_pRenderTarget->End();

    delete pText;

    return pTexture;
}