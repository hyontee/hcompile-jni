#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "gui.h"
#include "interface.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;

static int m_iContentId;

CInterface::CInterface() {
    m_iCurrentHudID = 0;//pSettings->Get().iHudStyle;
    m_iWantedLevel = 0;
    m_iContentId = 0;

    m_bRenderHud = true;
    m_bRenderMenu = false;
    iMileage = 0;
    speedFont = pGUI->LoadFont("flame_speed.ttf", 30);

    engineTex0 = (RwTexture*)LoadTextureFromDB("samp","speed_engine_off");
    engineTex1 = (RwTexture*)LoadTextureFromDB("samp","speed_engine_on");

    lockTex0 = (RwTexture*)LoadTextureFromDB("samp", "speed_key_off");
    lockTex1 = (RwTexture*)LoadTextureFromDB("samp", "speed_key_on");

    lightTex0 = (RwTexture*)LoadTextureFromDB("samp", "speed_light_off");
    lightTex1 = (RwTexture*)LoadTextureFromDB("samp", "speed_light_on");

    beltTex0 = (RwTexture*)LoadTextureFromDB("samp", "speed_belt_off");
    beltTex1 = (RwTexture*)LoadTextureFromDB("samp", "speed_belt_on");

    fuelTex = (RwTexture*)LoadTextureFromDB("samp", "speed_fuel");
    healthTex = (RwTexture*)LoadTextureFromDB("samp", "speed_health");   
}

CInterface::~CInterface() = default;

void CInterface::RenderSpeedometer()
{   
    
    if (!pGame || !pNetGame || !pGame->FindPlayerPed()->IsInVehicle()) return;
	if (pGame->FindPlayerPed()->IsAPassenger()) return;

    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Speedometer Flame", nullptr,
                 ImGuiWindowFlags_NoTitleBar
                 | ImGuiWindowFlags_NoResize
                 | ImGuiWindowFlags_NoSavedSettings
                 | ImGuiWindowFlags_NoBackground
                 | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
    );

    ImVec2 _pos(
            ImGui::GetWindowPos().x + ImGui::GetWindowSize().x / 2,
            ImGui::GetWindowPos().y + ImGui::GetWindowSize().y / 2
    );
    ImGuiWindow* win = ImGui::GetCurrentWindow();


    // ---> Render icons -- -- -- -- --;

    
 /*   // fuel
    ImVec2 sizeFuel(121 * 0.26, 145 * 0.26);
    ImVec2 _a(_pos.x - pGUI->ScaleY(200.0f + sizeFuel.x), _pos.y - pGUI->ScaleY(sizeFuel.y / 2));
    ImVec2 _b(_a.x + pGUI->ScaleY(sizeFuel.x), _a.y + pGUI->ScaleY(sizeFuel.y));
    win->DrawList->AddImage(fuelTex->raster, _a, _b);
    char buff[0x20];
    sprintf(buff, "%dл", pGUI->m_fuel);
    win->DrawList->AddText(nullptr, 
        ImGui::GetFontSize() * 0.9, 
        ImVec2(_b.x - pGUI->ScaleY(sizeFuel.x / 2) - (ImGui::CalcTextSizeByFontSize(ImGui::GetFontSize() * 0.9, buff).x / 2), _b.y), 
        ImColor(255, 255, 255, 255), 
        buff, nullptr); 

    // health
    ImVec2 sizeHealth(150 * 0.26, 150 * 0.26);
    _a = ImVec2(_pos.x + pGUI->ScaleY(200), _pos.y - pGUI->ScaleY(sizeHealth.y / 2));
    _b = ImVec2(_a.x + pGUI->ScaleY(sizeHealth.x), _a.y + pGUI->ScaleY(sizeHealth.y));
    win->DrawList->AddImage(healthTex->raster, _a, _b);
    sprintf(buff, "%0.f%%", pGame->FindPlayerPed()->GetGtaVehicle()->fHealth / 10);
    win->DrawList->AddText(nullptr, 
        ImGui::GetFontSize() * 0.9, 
        ImVec2(_b.x - pGUI->ScaleY(sizeHealth.x / 2) - (ImGui::CalcTextSizeByFontSize(ImGui::GetFontSize() * 0.9, buff).x / 2), _b.y), 
        ImColor(255, 255, 255, 255), 
        buff, nullptr); 

    win->DrawList->AddRectFilled(
        ImVec2(_pos.x - pGUI->ScaleY(60), _pos.y + pGUI->ScaleY(70)),
        ImVec2(_pos.x + pGUI->ScaleY(60), _pos.y + pGUI->ScaleY(105)),
        ImColor(0.0f, 0.0f, 0.0f, 0.65f), 10.0f
        );

    sprintf(buff, "%06d", iMileage);
    win->DrawList->AddText(nullptr, 
        ImGui::GetFontSize() * 0.9, 
        ImVec2(_pos.x - (ImGui::CalcTextSizeByFontSize(ImGui::GetFontSize() * 0.9, buff).x / 2), _pos.y + pGUI->ScaleY(75)), 
        ImColor(255, 255, 255, 255), 
        buff, nullptr); 

    win->DrawList->AddRectFilled(
        ImVec2(_pos.x - pGUI->ScaleY(110), _pos.y + pGUI->ScaleY(135)),
        ImVec2(_pos.x + pGUI->ScaleY(110), _pos.y + pGUI->ScaleY(180)),
        ImColor(0.35, 0.35, 0.35, 0.50f), 20.0f
        );
    
    _a = ImVec2(_pos.x - pGUI->ScaleY(95), _pos.y + pGUI->ScaleY(145));
    _b = ImVec2(_a.x + pGUI->ScaleY(104 * 0.3), _a.y + pGUI->ScaleY(76 * 0.3));
    win->DrawList->AddImage(bEngine ? engineTex1->raster : engineTex0->raster, _a, _b);

    _a = ImVec2(_pos.x - pGUI->ScaleY(45), _pos.y + pGUI->ScaleY(145));
    _b = ImVec2(_a.x + pGUI->ScaleY(136 * 0.3), _a.y + pGUI->ScaleY(84 * 0.3));
    win->DrawList->AddImage(bLight ? lightTex1->raster : lightTex0->raster, _a, _b);

    _a = ImVec2(_pos.x - pGUI->ScaleY(-15), _pos.y + pGUI->ScaleY(143));
    _b = ImVec2(_a.x + pGUI->ScaleY(103 * 0.3), _a.y + pGUI->ScaleY(104 * 0.3));
    win->DrawList->AddImage(bBelt ? beltTex1->raster : beltTex0->raster, _a, _b);
    
    _a = ImVec2(_pos.x - pGUI->ScaleY(-65), _pos.y + pGUI->ScaleY(143));
    _b = ImVec2(_a.x + pGUI->ScaleY(96 * 0.3), _a.y + pGUI->ScaleY(96 * 0.3));
    win->DrawList->AddImage(bLock ? lockTex1->raster : lockTex0->raster, _a, _b);
    
    // ---> Render icons -- -- -- -- --

    // arrows
    //CSprite2d* sprite = new CSprite2d();
    ImVec2 arrowSize(136 * 0.3, 108 * 0.3);
    _a = ImVec2(_pos.x - pGUI->ScaleY(125 + arrowSize.x), _pos.y + pGUI->ScaleY(142));
    _b = ImVec2(_a.x + pGUI->ScaleY(arrowSize.x), _a.y + pGUI->ScaleY(arrowSize.y));
    //sprite->m_pTexture = arrowLTex[OFF];
    //if(arrowLTex[OFF])win->DrawList->AddImage((ImTextureID)arrowLTex[OFF]->raster, _a, _b);
    //sprite->Draw(_a.x, _a.y, pGUI->ScaleY(arrowSize.x), pGUI->ScaleY(arrowSize.y), 0xFFFFFFFF);

    _a = ImVec2(_pos.x + pGUI->ScaleY(125), _pos.y + pGUI->ScaleY(142));
    _b = ImVec2(_a.x + pGUI->ScaleY(arrowSize.x), _a.y + pGUI->ScaleY(arrowSize.y));
    //if(arrowRTex[OFF])win->DrawList->AddImage((ImTextureID)arrowRTex[OFF]->raster, _a, _b);
    //sprite->m_pTexture = arrowLTex[OFF];
    //sprite->Draw(_a.x - pGUI->ScaleY(arrowSize.x), _a.y, -pGUI->ScaleY(arrowSize.x), pGUI->ScaleY(arrowSize.y), 0xFFFFFFFF);
    
    //delete sprite;
    int i_speed = 0;
    double d_speed = 0.0;

    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
    if(pPlayerPed)
    {
        CVehicle *pVehicle = nullptr;
        VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
        pVehicle = pVehiclePool->GetAt(id);
        
        if(pVehicle)
        {
            VECTOR vecMoveSpeed;
            pVehicle->GetMoveSpeedVector(&vecMoveSpeed);

            i_speed = sqrt((vecMoveSpeed.X * vecMoveSpeed.X) + (vecMoveSpeed.Y * vecMoveSpeed.Y) + (vecMoveSpeed.Z * vecMoveSpeed.Z)) * 183;
        }
    }

    d_speed = i_speed * 0.018333333333;

    char speedText[10];
    sprintf(speedText, " %i ", i_speed);

    win->DrawList->AddText(speedFont, ImGui::GetFontSize() * 2.4, ImVec2(_pos.x - (ImGui::CalcTextSizeByFontSize(ImGui::GetFontSize() * 2.4 ,speedText).x / 2), _pos.y - ImGui::GetFontSize() * 1.5), ImColor(255, 255, 255, 255), speedText, nullptr);
    win->DrawList->AddText(speedFont, ImGui::GetFontSize() * 1.0, ImVec2(_pos.x - (ImGui::CalcTextSize("км/ч").x / 2), _pos.y + ImGui::GetFontSize() * 0.8), ImColor(0x9F, 0x2B, 0xDE, 255), "км/ч", nullptr);
    {
        // ---> Render speed progress bar -- -- -- -- --

        // --> Background -- -- -- -- --
        const float a0 = (2.3) / 6.0f * 2.0f * IM_PI;
        const float a1 = (5.7 + 1.0f) / 6.0f * 2.0f * IM_PI;
        win->DrawList->PathArcTo(_pos, pGUI->ScaleY(140), a0, a1, 50);
        win->DrawList->PathStroke(ImColor(38, 38, 38, 200), false, 7.0f);

        // --> Line filling -- -- -- -- --
        const float a01 = (2.3) / 6.0f * 2.0f * IM_PI;
        const float a11 = (1.3 + d_speed + 1.0f) / 6.0f * 2.0f * IM_PI;
        win->DrawList->PathArcTo(_pos, pGUI->ScaleY(140), a01, a11, 50);
        win->DrawList->PathStroke(ImColor(201, 56, 253, 0xFF), false, 7.0f);

        const float a001 = (1.28 + d_speed + 1.0f) / 6.0f * 2.0f * IM_PI;
        const float a011 = (1.3 + d_speed + 1.0f) / 6.0f * 2.0f * IM_PI;
        win->DrawList->PathArcTo(_pos, pGUI->ScaleY(133), a001, a011, 50);
        win->DrawList->PathStroke(ImColor(0xFF, 0xFF, 0xFF, 0xFF), false, 20.0f);


        // ---> Render speed progress bar -- -- -- -- --
    }

    {
        // ---> Render line -- -- -- -- --

        const float a0 = (2.3) / 6.0f * 2.0f * IM_PI;
        const float a1 = (5.7 + 1.0f) / 6.0f * 2.0f * IM_PI;
        win->DrawList->PathArcTo(_pos, pGUI->ScaleY(117), a0, a1, 50);
        win->DrawList->PathStroke(ImColor(38, 38, 38, 200), false, ImMax(2.0f, 6.0f));

        win->DrawList->AddLine(ImVec2(_pos.x, _pos.y - pGUI->ScaleY(95)), ImVec2(_pos.x, _pos.y - pGUI->ScaleY(117)), ImColor(255, 255, 255, 255), 4.5);
        win->DrawList->AddLine(ImVec2(_pos.x - pGUI->ScaleY(78.5), _pos.y - pGUI->ScaleY(85)), ImVec2(_pos.x - pGUI->ScaleY(66), _pos.y - pGUI->ScaleY(72)), ImColor(0xDF, 0xDF, 0xDF, 255), 2.5);
        win->DrawList->AddLine(ImVec2(_pos.x + pGUI->ScaleY(78.5), _pos.y - pGUI->ScaleY(85)), ImVec2(_pos.x + pGUI->ScaleY(66), _pos.y - pGUI->ScaleY(72)), ImColor(0xDF, 0xDF, 0xDF, 255), 2.5);

        win->DrawList->AddLine(ImVec2(_pos.x - pGUI->ScaleY(115.5), _pos.y - pGUI->ScaleY(10)), ImVec2(_pos.x - pGUI->ScaleY(93), _pos.y - pGUI->ScaleY(10)), ImColor(0xDF, 0xDF, 0xDF, 255), 2.5);
        win->DrawList->AddLine(ImVec2(_pos.x + pGUI->ScaleY(115.5), _pos.y - pGUI->ScaleY(10)), ImVec2(_pos.x + pGUI->ScaleY(93), _pos.y - pGUI->ScaleY(10)), ImColor(0xDF, 0xDF, 0xDF, 255), 2.5);
        // ---> Render line -- -- -- -- --
    }

    {
        int maxFuel = 100;
        // ---> Render fuel progress bar -- -- -- -- --

        // --> Background -- -- -- -- --

        const float s0 = (-0.7854*5 - (-0.7854 / 4)) - (-0.7854*3 + (-0.7854 / 4)); // dist radians
        const float f = pGUI->m_fuel; // target
        const float aa1 = (-0.7854*5 - (-0.7854 / 4) )- ((s0 / maxFuel) * f); // filling rad to rad
    
        ImGui::GetBackgroundDrawList()->PathArcTo(_pos, pGUI->ScaleY(180), -0.7854*5 - (-0.7854 / 4), -0.7854*3 + (-0.7854 / 4), 50);
        ImGui::GetBackgroundDrawList()->PathStroke(ImColor(38, 38, 38, 255), false, ImMax(2.0f, 7.0f));
        ImGui::GetBackgroundDrawList()->PathArcTo(_pos, pGUI->ScaleY(180), -0.7854*5 - (-0.7854 / 4), aa1, 50);
        ImGui::GetBackgroundDrawList()->PathStroke(ImColor(0xFF, 0xA8, 0x26, 255), false, ImMax(2.0f, 7.0f));

        // ---> Render fuel progress bar -- -- -- -- --

        // Render Vehicle Health

        CVehicle *pVehicle = nullptr;

        VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
        pVehicle = pVehiclePool->GetAt(id);
        
        const float s = (0.7854 + (-0.7854 / 4)) - (-0.7854 + (-0.7854 / 4));
        float fHealth = pVehicle->GetHealth();
        if(fHealth > 1000.0f) fHealth = 1000.0f;
        const float a111 = (0.7854 - (-0.7854 / 4)) - ((s / 1000) * fHealth);
        ImGui::GetBackgroundDrawList()->PathArcTo(_pos, pGUI->ScaleY(180), -0.7854 - (-0.7854 / 4), 0.7854 + (-0.7854 / 4), 50);
        ImGui::GetBackgroundDrawList()->PathStroke(ImColor(38, 38, 38, 255), false, ImMax(2.0f, 7.0f));

        ImGui::GetBackgroundDrawList()->PathArcTo(_pos, pGUI->ScaleY(180), a111, 0.7854 + (-0.7854 / 4), 50);
        ImGui::GetBackgroundDrawList()->PathStroke(ImColor( 0xFF, 0x26, 0x26, 255), false, ImMax(2.0f, 7.0f));
        
        // Render Vehicle Health
    }

    ImGui::SetWindowSize(ImVec2(pGUI->ScaleY(600), pGUI->ScaleY(400)));
    ImGui::SetWindowPos(ImVec2(
            io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2,
            io.DisplaySize.y - ImGui::GetWindowHeight() * 0.97f
    ));
    ImGui::End();*/
}