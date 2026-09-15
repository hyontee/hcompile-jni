#include "../main.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "../gui/gui.h"
#include "../util/interfaces/CLoading.h"

extern CGUI* pGUI;

RwTexture* splashTexture = nullptr;
RwTexture* splashTexture_1 = nullptr;

#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_BLACK 	0xFF000000
#define COLOR_ORANGE 	0xFF00A5FA
#define COLOR_PURPLE	0xFFCE39DF
#define COLOR_ROSE		0xFFFF99FF
#define COLOR_GREY		0xFF5d5d5d
#define COLOR_BRED		0xFF9933FF
#define COLOR_BLUE		0xFF6C2713
#define COLOR_CYAN		0xFFCE6816
#define COLOR_1			0xFFB58891
#define COLOR_2			0xFF673F40
#define COLOR_PIZDEC	0xFFff008b

struct stRect
{
	int x1;	// left
	int y1;	// top
	int x2;	// right
	int y2;	// bottom
};

struct stfRect
{
	float x1;
	float y1;
	float x2;
	float y2;
};

#define MAX_SCHEMAS 4
uint32_t colors[MAX_SCHEMAS][2] = {
	{ COLOR_BLACK,	COLOR_WHITE },
	{ COLOR_BLACK, 	COLOR_ORANGE },
	{ COLOR_BLACK,	COLOR_ORANGE },
	{ COLOR_BLACK,	COLOR_BLUE }
};
unsigned int color_scheme = 1;

extern uint8_t m_SelectedServer;
void Draw(stRect* rect, uint32_t color, RwRaster* raster = nullptr, stfRect* uv = nullptr)
{
	static RwIm2DVertex vert[4];
	const RwReal nearScreenZ = *(RwReal*)(g_libGTASA + 0x9DAA60);	// CSprite2d::NearScreenZ
	const RwReal recipNearClip = *(RwReal*)(g_libGTASA + 0x9DAA64);	// CSprite2d::RecipNearClip

	RwIm2DVertexSetScreenX(&vert[0], rect->x1);
	RwIm2DVertexSetScreenY(&vert[0], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[0], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[0], recipNearClip);
	vert[0].emissiveColor = color;
	RwIm2DVertexSetU(&vert[0], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[0], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[1], rect->x2);
	RwIm2DVertexSetScreenY(&vert[1], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[1], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[1], recipNearClip);
	vert[1].emissiveColor = color;
	RwIm2DVertexSetU(&vert[1], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[1], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[2], rect->x1);
	RwIm2DVertexSetScreenY(&vert[2], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[2], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[2], recipNearClip);
	vert[2].emissiveColor = color;
	RwIm2DVertexSetU(&vert[2], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[2], uv ? uv->y1 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[3], rect->x2);
	RwIm2DVertexSetScreenY(&vert[3], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[3], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[3], recipNearClip);
	vert[3].emissiveColor = color;
	RwIm2DVertexSetU(&vert[3], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[3], uv ? uv->y1 : 0.0f, recipNearClip);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)raster);
	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vert, 4);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);
}

static uint32_t SplashTickCount = GetTickCount();
bool SplashTextureUsed = false;

void RenderSplash()
{
	if (!splashTexture) return;
	
	stRect rect;
	stfRect uv;

	// background
	rect.x1 = 0;
	rect.y1 = 0;
	rect.x2 = RsGlobal->maximumWidth;
	rect.y2 = RsGlobal->maximumHeight;
	Draw(&rect, colors[color_scheme][0]);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	
	// splash
	rect.x1 = 0;
    rect.y1 = 0;
    rect.x2 = RsGlobal->maximumWidth;
    rect.y2 = RsGlobal->maximumWidth / 16 * 9;
    uv.x1 = 0.0f;
    uv.y1 = 0.0f;
    uv.x2 = 1.0f;
    uv.y2 = 1.0f;
	
	if (splashTexture_1 != nullptr)
	{
		if (GetTickCount() - SplashTickCount >= 750)
		{
			if (GetTickCount() - SplashTickCount >= 1500) SplashTickCount = GetTickCount();
			SplashTextureUsed = true;
		}
		else 
		{
			SplashTextureUsed = false;
		}
		
		if (SplashTextureUsed == true) Draw(&rect, COLOR_WHITE, splashTexture_1->raster, &uv);
		else Draw(&rect, COLOR_WHITE, splashTexture->raster, &uv);
	}
	else Draw(&rect, COLOR_WHITE, splashTexture->raster, &uv);
}

extern bool m_bStartedJava;
extern bool m_bStopLoadingScreen;
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();
void RenderSplashScreen()
{
	if (!pGUI)
		return;
	
	if (m_SelectedServer == ServerCrime || m_SelectedServer == ServerGoldRussia)
	{
		if (m_bStartedJava == false && m_bStopLoadingScreen == false)
		{
			CLoading::SetLoadingShow();
			CLoading::SetLoadingStatus("Загрузка игрового режима...");
			m_bStartedJava = true;
		}
	}
	else
	{
		RenderSplash();
		
		ImGui_ImplRenderWare_NewFrame();
		ImGui::NewFrame();
		
		ImGuiIO& io = ImGui::GetIO();
		
		// texture
		float percent = *(float*)(g_libGTASA + 0x8F08C0);
		if (percent <= 0.0f) percent = 0.0f;
		if (percent >= 100.0f) percent = 100.0f;
		if (!pGUI) return;

		float mult = 100 / 340.0f;
		// offset
		float newX = pGUI->ScaleX(1520.0f) + ((percent * 20) * mult);
		if (newX >= pGUI->ScaleX(1520.0f) + pGUI->ScaleX(340.0f)) newX = pGUI->ScaleX(1520.0f) + pGUI->ScaleX(340.0f);

		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(pGUI->ScaleX(1520.0f), io.DisplaySize.y * (1 - 0.025)), ImVec2(pGUI->ScaleX(1520.0f) + pGUI->ScaleX(340.0f), io.DisplaySize.y - pGUI->ScaleY(50.0f)), ImColor(0x4A, 0x4A, 0x4A, 0xFF - 30)); // серый цвет полоска

		ImVec2 pos = ImVec2(pGUI->ScaleX(60), io.DisplaySize.y * (1 - 0.055));
		ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), pGUI->ScaleY(40.0f), pos, 0xFFFFFFFF, "Идёт загрузка, пожалуйста, подождите...");

		// нижняя полоса
		//ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(pGUI->ScaleX(120.0f), io.DisplaySize.y * (1 - 0.025)), ImVec2(newX, io.DisplaySize.y - pGUI->ScaleY(60.0f)), ImColor(0x99, 0x32, 0xCC, 0xFF));
		switch (m_SelectedServer)
		{
			case ServerJust:
			{
				ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(pGUI->ScaleX(1520.0f), io.DisplaySize.y * (1 - 0.025)), ImVec2(newX, io.DisplaySize.y - pGUI->ScaleY(50.0f)), ImColor(0x45, 0x82, 0xA1, 0xFF), ImColor(0x45, 0x82, 0xA1, 0xFF), ImColor(0x45, 0x82, 0xA1, 0xFF), ImColor(0x45, 0x82, 0xA1, 0xFF));
				break;
			}
			default:
			{
				ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(pGUI->ScaleX(1520.0f), io.DisplaySize.y * (1 - 0.025)), ImVec2(newX, io.DisplaySize.y - pGUI->ScaleY(50.0f)), ImColor(0xFF, 0x8C, 0x00, 0xFF), ImColor(0xFF, 0x8C, 0x00, 0xFF), ImColor(0xFF, 0xA5, 0x00, 0xFF), ImColor(0xFF, 0xD7, 0x00, 0xFF));
				break;
			}
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
	}
}