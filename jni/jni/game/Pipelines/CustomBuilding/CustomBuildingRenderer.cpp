#include "CustomBuildingRenderer.h"
#include "../../../util/patch.h"

bool CCustomBuildingRenderer::Initialise() {
    return CHook::CallFunction<bool>("_ZN23CCustomBuildingRenderer10InitialiseEv");
}

void CCustomBuildingRenderer::Update() {
    CHook::CallFunction<void>(g_libGTASA + 0x28A090 + 1);
}