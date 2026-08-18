//
// Created by plaka on 08.05.2023.
//

#include <string>
#include <sstream>
#include "game/RW/RenderWare.h"
#include "MaterialText.h"
#include "util/util.h"
#include "game/RW/rpworld.h"
#include "main.h"
#include "game/rgba.h"
#include "game/CRenderTarget.h"

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData *draw_data);

bool ImGui_ImplRenderWare_Init();

void ImGui_ImplRenderWare_ShutDown();

extern CGUI *pGUI;
extern uintptr_t g_libGTASA;

static uint16_t sizes[14][2] = {
        {32,  32},
        {64,  32},
        {64,  64},
        {128, 32},
        {128, 64},
        {128, 128},
        {256, 32},
        {256, 64},
        {256, 128},
        {256, 256},
        {512, 64},
        {512, 128},
        {512, 256},
        {512, 512}
};

RwTexture *CMaterialText::Generate(uint8_t matSize, const char *fontname, float fontSize, uint8_t bold,
                                   uint32_t fontcol, uint32_t backcol, uint8_t align, const char *szText) {

    matSize = (matSize / 10) - 1;

    CRenderTarget::Begin(sizes[matSize][0] * 2, sizes[matSize][1] * 2, (RwRGBA*)&backcol, false);

    SetUpMaterial(matSize, fontname, fontSize * 0.8f, bold, fontcol, backcol, align, szText);

    return CRenderTarget::End();
}

void
CMaterialText::SetUpMaterial(uint8_t matSize, const char *fontname, float fontSize, uint8_t bold,
                             const CRGBA& fontcol, const CRGBA& backcol, uint8_t align, const char *szText) {
    ImGui::NewFrame();

    char utf8[2048];
    cp1251_to_utf8(utf8, szText, 2048);

    // text align
    ImVec2 vecPos;

    std::string strText = utf8;
    std::stringstream ssLine(strText);
    std::string tmpLine;
    int newLineCount = 0;

  //  Log("TEXT: %s", szText);

    switch (align) {
        case OBJECT_MATERIAL_TEXT_ALIGN_LEFT:
            while (std::getline(ssLine, tmpLine, '\n')) {
                if (tmpLine[0] != 0) {
                    vecPos.x = 0;
                    vecPos.y = (sizes[matSize][1] - fontSize) / 2;
                }
            }
            break;
        case OBJECT_MATERIAL_TEXT_ALIGN_CENTER:
            while (std::getline(ssLine, tmpLine, '\n')) {
                if (tmpLine[0] != 0) {
                    vecPos.x = (sizes[matSize][0] / 2) -
                               (CUtil::CalcTextSizeWithoutTags((char *) tmpLine.c_str(),
                                                               fontSize).x / 2);
                    vecPos.y = (sizes[matSize][1] / 2) -
                               (CUtil::CalcTextSizeWithoutTags((char *) tmpLine.c_str(),
                                                               fontSize).y / 2);
                }
            }
            break;
        case OBJECT_MATERIAL_TEXT_ALIGN_RIGHT:
            while (std::getline(ssLine, tmpLine, '\n')) {
                if (tmpLine[0] != 0) {
                    vecPos.x = (sizes[matSize][0] -
                                CUtil::CalcTextSizeWithoutTags((char *) tmpLine.c_str(),
                                                               fontSize).x);
                    vecPos.y = (sizes[matSize][1] - fontSize);
                }
            }
            break;
    }

    ImVec2 backpos = ImVec2(vecPos.x - pGUI->GetFontSize(), vecPos.y - pGUI->GetFontSize());
    ImVec2 backsize = ImVec2(fontSize + pGUI->GetFontSize(), fontSize + pGUI->GetFontSize());
    ImGui::GetBackgroundDrawList()->AddRectFilled(backpos, backsize, backcol.ToInt());

    vecPos.x *= 2;
    vecPos.y *= 2;
    CUtil::drawText(vecPos, fontcol.ToInt(), utf8, fontSize * 2);

    ImGui::Render();
    ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}