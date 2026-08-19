#pragma once
#include <stdint.h>

class Hud
{
public:
	static RwTexture* rwTexture;

public:
	static void Create();
	static void SetTexture(RwTexture* texture);
	static void CreateUp(float x, float y, float width, float height, const char* texture);
};

