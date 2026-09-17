#pragma once

class WaterShader{
public:
    static int BuildShadersSource1(uintptr_t*);
	static int BuildShadersSource2(uintptr_t*);
    static int EmuShader__Select3(uintptr_t*);
    static void GetTexture_waterDUDV();
    static int sub_ResetEnvMap(int);
};