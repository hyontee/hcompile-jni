//
// Created by admin on 28.12.2023.
//

#pragma once

#include "../main.h"
#include "game/RW/common.h"

#include "Vector.h"
#include "Vector2D.h"
#include "Matrix.h"
#include "MatrixLink.h"
#include "MatrixLinkList.h"
#include "General.h"
#include "SimpleTransform.h"
#include "Placeable.h"
#include "CNodeAddress.h"
#include "Camera.h"
#include "eWeatherType.h"
#include "eWeatherRegion.h"
#include "CColourSet.h"
#include "TextureDatabaseRuntime.h"
#include "QueuedModel.h"
#include "MobileMenu.h"
#include "Radar.h"
#include "Rect.h"
#include "CFont.h"
#include "HIDMapping.h"
#include "CWidgets.h"
#include "Timer.h"
#include "CPathNode.h"

extern CCameraGta* TheCamera;
extern RpAtomicCallBackRender AtomicDefaultRenderCallBack;
extern MobileMenu* gMobileMenu;
extern tRadarTrace* pRadarTrace;
extern CWidgetGta** aWidgets;