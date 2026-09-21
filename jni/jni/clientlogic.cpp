#include "main.h"
#include "clientlogic.h"
#include "chatwindow.h"
#include "game/game.h"
#include "game/playerped.h"
#include "net/netgame.h"

extern CChatWindow *pChatWindow;
extern CNetGame *pNetGame;
extern CGame *pGame;

CObject *object = 0;
bool isSnowShowing = false;

CClientLogic::CClientLogic()
{
    Log("Client initialised");
//    if(isSnowShowing) UpdateSnow();
}

CClientLogic::~CClientLogic()
{
    Log("Client shot down");
}

void CClientLogic::ShowSnow()
{
        // 18659, SprayTag1 | 18 - 20
}

void CClientLogic::ProcessLogic()
{
        
}

void CClientLogic::UpdateSnow()
{

}
