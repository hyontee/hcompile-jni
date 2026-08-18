//
// Created by plaka on 03.02.2023.
//

#include "CGtaWidgets.h"
#include "main.h"
#include "game/game.h"
#include "net/netgame.h"

#include "java_systems/CHUD.h"
#include "WidgetButton.h"


extern CGame* pGame;
CWidgetGta * CGtaWidgets::m_pWidgets[190]; // int CTouchInterface::m_pWidgets[]
extern CNetGame *pNetGame;

enum eWidgetState {
    STATE_NONE,
    STATE_FIXED
};
WidgetIDs GetWidgetTypeFromWidget(CWidgetGta* pWidget)
{
    if(pWidget)
    {
        if(CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_ATTACK] && pWidget == CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_ATTACK]) return WidgetIDs::WIDGET_ATTACK;
        if(CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_SPRINT] && pWidget == CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_SPRINT]) return WidgetIDs::WIDGET_SPRINT;
        if(CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_ACCELERATE] && pWidget == CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_ACCELERATE]) return WidgetIDs::WIDGET_ACCELERATE;
        if(CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_ENTER_CAR] && pWidget == CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_ENTER_CAR]) return WidgetIDs::WIDGET_ENTER_CAR;
        if(CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_BRAKE] && pWidget == CGtaWidgets::m_pWidgets[WidgetIDs::WIDGET_BRAKE]) return WidgetIDs::WIDGET_BRAKE;
    }

    return static_cast<WidgetIDs>(-1);
}
eWidgetState ProcessFixedWidget(CWidgetGta* pWidget)
{
    WidgetIDs widgetType = GetWidgetTypeFromWidget(pWidget);

    CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
    switch(widgetType)
    {
        case -1:
            return STATE_NONE;
        case WidgetIDs::WIDGET_ATTACK:
        case WidgetIDs::WIDGET_SPRINT:
            if(pPlayerPed->IsInVehicle() )
            {
                return STATE_FIXED;
            }
            break;
        case WidgetIDs::WIDGET_ACCELERATE:
        case WidgetIDs::WIDGET_BRAKE:
            if(!pPlayerPed->IsInVehicle())
            {
                return STATE_FIXED;
            }
            break;
        case WidgetIDs::WIDGET_ENTER_CAR:
            //if(pPlayerPed->IsInJetpackMode()) return STATE_NONE;

            if(pNetGame)
            {
                CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
                if(pVehiclePool)
                {
                    CVehicle* pVehicle = pVehiclePool->FindNearestToLocalPlayerPed();
                    if(!pVehicle) return STATE_FIXED;

                    if (!pPlayerPed->IsInVehicle() &&
                        pVehicle->GetDistanceFromLocalPlayerPed() > 10.0f) {
                        return STATE_FIXED;
                    }
                }
            }
            break;
    }

    return STATE_NONE;
}

void (*CWidgetButton__Draw)(CWidgetGta* thiz);
void CWidgetButton__Draw_hook(CWidgetGta* thiz)
{
    if(thiz == CGtaWidgets::m_pWidgets[WIDGET_POSITION_HORN]) {
        return;
    }

    CWidgetButton__Draw(thiz);
}

void CGtaWidgets::setEnabled(CWidgetGta* thiz, bool bEnabled)
{
    /* if(pNetGame)
     {
         switch(ProcessFixedWidget(thiz))
         {
             case STATE_NONE: break;
             case STATE_FIXED:
                 bEnabled = false;
                 break;
         }
     }*/
    thiz->m_bEnabled = bEnabled; // this->m_bEnabled
}

//bool (*CWidget__IsTouched)(uintptr_t* thiz, CVector2D *pVecOut);
//bool CWidget__IsTouched_hook(uintptr_t* thiz, CVector2D *pVecOut)
//{
//    if(*thiz == CGtaWidgets::pWidgets[WIDGET_POSITION_HORN]) {
//        return true;
//    }
//    return CWidget__IsTouched(thiz, pVecOut);
//}

void (*CWidgetButton__Update)(CWidgetButton*);
void CWidgetButton__Update_hook(CWidgetButton* thiz)
{
    if (!thiz)
    {
        return;
    }

   // CPlayerPed *pPlayerPed;
    //if(pNetGame) pPlayerPed = pGame->FindPlayerPed();
    if(pNetGame && pNetGame->m_GreenZoneState/* || pPlayerPed->IsInVehicle()*/)
    {
        CGtaWidgets::setEnabled(CGtaWidgets::m_pWidgets[WIDGET_POSITION_CAR_SHOOT], false);
    }

    /* if(pNetGame)
     {
         switch(ProcessFixedWidget((CWidgetGta*)result))
         {
             case STATE_NONE: break;
             case STATE_FIXED: return;
         }
     }*/


    CWidgetButton__Update(thiz);
}
void (*CWidgetRegionLook__Update)(int);
void CWidgetRegionLook__Update_hook(int a1)
{
    int result; // r0

    CWidgetRegionLook__Update(a1);
    result = 0;
    if ( *(uint8_t *)(a1 + 0x90) )
    {
        *(uint8_t *)(a1 + 0x90) = 0;
        result = 1;
    }
    // CActionsPed::bPressed = result;
}

void (*CWidgetButtonEnterCar__Draw)(uintptr_t *thiz);
void CWidgetButtonEnterCar__Draw_hook(uintptr_t *thiz) {


    if(!CHUD::bIsShowEnterExitButt)
        return;

    CWidgetButtonEnterCar__Draw(thiz);
}
void CGtaWidgets::init()
{

    // CHook::InlineHook(g_libGTASA, 0x00274218, &CWidget__IsTouched_hook, &CWidget__IsTouched);
    //CHook::InlineHook(g_libGTASA, 0x002B41BC, &CWidgetButton__Update_hook, &CWidgetButton__Update);
    // CHook::HookFunc("_ZN13CWidgetButton6UpdateEv", CWidgetButton__Update_hook, &CWidgetButton__Update);
    CHook::installPLTHook(g_libGTASA + (VER_x32 ? 0x00671424 : 0x842408), (uintptr_t)CWidgetButton__Update_hook, (uintptr_t*)&CWidgetButton__Update);
    //CHook::InlineHook(g_libGTASA, 0x002B420C, &CWidgetButton__Draw_hook, &CWidgetButton__Draw);
    //CHook::HookFunc("_ZN13CWidgetButton4DrawEv", CWidgetButton__Draw_hook, &CWidgetButton__Draw);
    //CHook::HookFunc("_ZN13CWidgetButton4DrawEv", CWidgetRegionLook__Update_hook, &CWidgetRegionLook__Update);
    CHook::HookFunc("_ZN21CWidgetButtonEnterCar4DrawEv", CWidgetButtonEnterCar__Draw_hook, &CWidgetButtonEnterCar__Draw);

}

bool CGtaWidgets::IsReleased(CWidgetGta* widget, CVector2D *pVecOut) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x002B3484 + 1 : 0x372794), widget, pVecOut);
}

bool CGtaWidgets::IsTouched(CWidgetGta* widget, CVector2D *pVecOut) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x002B3324 + 1 : 0x3725D0), widget, pVecOut);
}


