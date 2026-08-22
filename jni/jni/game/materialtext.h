#pragma once

#define OBJECT_MATERIAL_TEXT_ALIGN_LEFT		0
#define OBJECT_MATERIAL_TEXT_ALIGN_CENTER	1
#define OBJECT_MATERIAL_TEXT_ALIGN_RIGHT	2

class CMaterialText
{
public:
	CMaterialText();

	RwTexture* Generate(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char* szText);

private:
	void SetUpScene();
	void Render(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint8_t byteAlign, const char* szText);

	RwCamera*	m_camera;
	RwFrame*	m_frame;
};
