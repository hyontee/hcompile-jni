#pragma once

#include "../game/RW/RenderWare.h"
#include "../vendor/imgui/imgui.h"

class CButtons {

public:
    CButtons();

    void Render();
	static bool m_bIsActive;
    static bool m_bIsShow;
	static int8_t m_iKey[7];

private:
    bool m_bIsItemShow;
};