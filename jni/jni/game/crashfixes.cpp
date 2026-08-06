#include "../main.h"

#include "RW/RenderWare.h"
#include "game.h"
#include "util.h"

#include "../net/netgame.h"
extern CNetGame *pNetGame;
extern CGame *pGame;

#include "../util/armhook.h"

int (*CTextureDatabaseRuntime__GetEntry)(uintptr_t thiz, const char* a2, bool* a3);
int CTextureDatabaseRuntime__GetEntry_hook(uintptr_t thiz, const char* a2, bool* a3)
{
	if (!thiz)
	{
		return -1;
	}
	return CTextureDatabaseRuntime__GetEntry(thiz, a2, a3);
}

int (*_RpMaterialDestroy)(uintptr_t);
int HOOK__RpMaterialDestroy(uintptr_t mat)
{
	if(mat) return _RpMaterialDestroy(mat);
	return 1;
}

#pragma pack(1)
typedef struct _RES_ENTRY_OBJ
{
	PADDING(_pad0, 48); 	// 0-48
	uintptr_t validate; 	//48-52
	PADDING(_pad1, 4); 		//52-56
} RES_ENTRY_OBJ;
static_assert(sizeof(_RES_ENTRY_OBJ) == 56);

int (*emu_ArraysDelete)(unsigned int a1, int a2, int a3, int a4);
int emu_ArraysDelete_hook(unsigned int a1, int a2, int a3, int a4)
{
	if(a1 > g_libGTASA) return emu_ArraysDelete(a1, a2, a3, a4);
	return 0;
}

int (*CPed__GetBonePosition)(int a1, int *a2, int a3, int a4);
int CPed__GetBonePosition_hook(int a1, int *a2, int a3, int a4)
{
	if(!a1) return 0;
	return CPed__GetBonePosition(a1, a2, a3, a4);
}

int (*RpLightDestroy)(int result);
int RpLightDestroy_hook(int result)
{
	if(!result) return 0;
	return RpLightDestroy(result);
}

char** (*CPhysical__Add)(uintptr_t thiz);
char** CPhysical__Add_hook(uintptr_t thiz)
{
	if (pNetGame)
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_aAttachedObjects[i].bState)
				{
					if(pPlayerPed->m_aAttachedObjects[i].pObject) 
					{
						if ((uintptr_t)pPlayerPed->m_aAttachedObjects[i].pObject->m_pEntity == thiz)
						{
							CObject* pObject = pPlayerPed->m_aAttachedObjects[i].pObject;
							if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
								pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
							{
								return 0;
							}
						}
					}
				}
			}
		}

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool)
		{
			//for (size_t i = 0; i < MAX_PLAYERS; i++)
			for (int i: pPlayerPool->m_PlayersCount)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_aAttachedObjects[i].bState)
								{
									if ((uintptr_t)pPlayerPed->m_aAttachedObjects[i].pObject->m_pEntity == thiz)
									{
										CObject* pObject = pPlayerPed->m_aAttachedObjects[i].pObject;
										if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
											pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
										{
											return 0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return CPhysical__Add(thiz);
}

void InstallHookFixes()
{
	SetUpHook(g_libGTASA + 0x1E3C54, (uintptr_t)HOOK__RpMaterialDestroy, (uintptr_t*)&_RpMaterialDestroy);
	SetUpHook(g_libGTASA + 0x1BDC3C, (uintptr_t)CTextureDatabaseRuntime__GetEntry_hook, (uintptr_t*)&CTextureDatabaseRuntime__GetEntry);
	SetUpHook(g_libGTASA + 0x194B04, (uintptr_t)emu_ArraysDelete_hook, (uintptr_t*)&emu_ArraysDelete);
	
	//installHook(SA_ADDR(0x3AA3C0), (uintptr_t)CPhysical__Add_hook, (uintptr_t*)&CPhysical__Add);
	//installHook(SA_ADDR(0x436590), (uintptr_t)CPed__GetBonePosition_hook, (uintptr_t*)&CPed__GetBonePosition);
	//installHook(SA_ADDR(0x1E3810), (uintptr_t)RpLightDestroy_hook, (uintptr_t*)&RpLightDestroy);
}