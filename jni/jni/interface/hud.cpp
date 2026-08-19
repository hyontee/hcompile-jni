//
// Created by Weikton 27.06.23
//
#include "../main.h"
#include "hud.h"
#include "../gui/gui.h"
#include "../game/game.h"
#include "../game/sprite2d.h"
#include "../game/RW/RenderWare.h"
#include "../settings.h"
#include "../util/armhook.h"
// OBFUSCATE
#include "obfuscate.h"

extern CGUI* pGUI;

RwTexture *Hud::rwTexture = nullptr;

void Hud::Create() {
    //Log(OBFUSCATE("Create BLACK RUSSIA HUD.."));
    if (rwTexture != nullptr){
        int size = sizeof(rwTexture);
        Log("[hud cpp] Size = %d", size);
        CreateUp(pGUI->ScaleX(300.0f), pGUI->ScaleY(300.0f), 620.0f, 620.0f, "radar_bg"); // верх
    }else if(rwTexture == nullptr)
        return;
}

void Hud::SetTexture(RwTexture* texture)
{
    rwTexture = texture;
}

void Hud::CreateUp(float x, float y, float width, float height, const char* texture)
{
      CSprite2d* m_pSprite = new CSprite2d();
      m_pSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", texture);
      if(rwTexture != nullptr)
          m_pSprite->m_pRwTexture = rwTexture;

      CRGBA color_white;
      color_white.A = 255;
      color_white.B = 255;
      color_white.G = 255;
      color_white.R = 255;

      m_pSprite->Draw(x, y, width, height, color_white);
}

