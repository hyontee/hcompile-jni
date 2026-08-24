//
// Created by admin on 28.12.2023.
//

#include "game_sa.h"
#include "util/armhook.h"

CCameraGta* TheCamera;
RpAtomicCallBackRender AtomicDefaultRenderCallBack;
MobileMenu* gMobileMenu;
tRadarTrace* pRadarTrace;
CWidgetGta** aWidgets = nullptr;