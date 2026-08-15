#pragma once
#include "game/RW/RenderWare.h"

class CGTexture
{
public:
	CGTexture();
	~CGTexture();

	void RenderButton();
	void LoadTextureButton();
	
private:
	float ScaleX(float x) { return m_vecScale.x * x; }
	float ScaleY(float y) { return m_vecScale.y * y; }
	ImVec2		m_vecScale;
	RwTexture* p_GTexture;
};

