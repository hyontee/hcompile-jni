//
// Created by gen1z on 02/03/2024.
//

#ifndef BLACK_RUSSIA_ORIGINAL_MAIN_H
#define BLACK_RUSSIA_ORIGINAL_MAIN_H


#define SA_ADDR(addr) (g_libGTASA + (addr))
#define GTA(addr) (g_libGTASA + (addr))

#define SAMP_VERSION	"0.3.7"
#define PORT_VERSION	"0.69"

#define _VERSION	"1.72"
#define _LAUNCHERPACKET	"blackrussia.online"
#define _GTAPACKET	"com.rockstargames.gtacr"

#define MAX_PLAYERS		1004
#define MAX_VEHICLES	2000
#define MAX_PLAYER_NAME	24

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969

#include <android/log.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <strstream>
#include <utility>
#include <stdio.h>
#include <stdint.h>
#include <jni.h>
#include "util/util.h"
#include "obfuscate/str_obfuscate.hpp"
#include "obfuscate_v2/obfuscator.hpp"
// raknet
#include "vendor/raknet/RakClientInterface.h"
#include "vendor/raknet/RakNetworkFactory.h"
#include "vendor/raknet/PacketEnumerations.h"
#include "vendor/raknet/StringCompressor.h"

extern  uintptr_t g_libBR;
extern  uintptr_t g_libGTASA;

extern  uintptr_t* g_CNetGame;
extern  char* g_pszStorage;
extern  uintptr_t g_iGameState;

/*#define PADDING(x,y) uint8_t x[y]

#pragma pack(1)
typedef struct _VECTOR
{
    float X,Y,Z;
} VECTOR, *PVECTOR;

#pragma pack(1)
typedef struct _MATRIX4X4
{
    VECTOR right;		// 0-12 	; r11 r12 r13
    uint32_t  flags;	// 12-16
    VECTOR up;			// 16-28	; r21 r22 r23
    float  pad_u;		// 28-32
    VECTOR at;			// 32-44	; r31 r32 r33
    float  pad_a;		// 44-48
    VECTOR pos;			// 48-60
    float  pad_p;		// 60-64
} MATRIX4X4, *PMATRIX4X4;

enum ePrimaryTasks //array indexes
{
    TASK_PRIMARY_PHYSICAL_RESPONSE = 0,
    TASK_PRIMARY_EVENT_RESPONSE_TEMP,
    TASK_PRIMARY_EVENT_RESPONSE_NONTEMP,
    TASK_PRIMARY_PRIMARY,
    TASK_PRIMARY_DEFAULT,
    TASK_PRIMARY_MAX
};

enum eSecondaryTasks //array indexes
{
    TASK_SECONDARY_ATTACK = 0,                // want duck to be after attack
    TASK_SECONDARY_DUCK,                    // because attack controls ducking movement
    TASK_SECONDARY_SAY,
    TASK_SECONDARY_FACIAL_COMPLEX,
    TASK_SECONDARY_PARTIAL_ANIM,
    TASK_SECONDARY_IK,
    TASK_SECONDARY_MAX
};

class CTaskManager
{
public:
    void* m_aPrimaryTasks[5];
    void* m_aSecondaryTasks[6];
    class CPed* m_pPed;
};

class CPedIntelligence
{
public:
    class CPed* m_pPed;
    CTaskManager   m_TaskMgr;
};

#pragma pack(1)
typedef struct _ENTITY_TYPE
{
    // ENTITY STUFF
    uint32_t vtable; 		// 0-4		;vtable
    PADDING(_pad91, 16);	// 4-20
    MATRIX4X4 *mat; 		// 20-24	;mat
    uintptr_t m_RwObject;	// 24 - 28
    PADDING(_pad92, 6);	// 28-34
    uint16_t nModelIndex; 	// 34-36	;ModelIndex
    PADDING(_pad93, 32);	// 36-68
    VECTOR vecMoveSpeed; 	// 68-80	;vecMoveSpeed
    VECTOR vecTurnSpeed; 	// 80-92	;vecTurnSpeed
    PADDING(_pad94, 88);	// 92-180
    uintptr_t dwUnkModelRel; // 180-184 ;����� ����

} ENTITY_TYPE;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _WEAPON_SLOT_TYPE
{
    uint32_t dwType;
    uint32_t dwState;
    uint32_t dwAmmoInClip;
    uint32_t dwAmmo;
    PADDING(_pwep1,12);
} WEAPON_SLOT_TYPE;  // MUST BE EXACTLY ALIGNED TO 28 bytes

#pragma pack(1)
typedef struct _PED_TYPE
{
    ENTITY_TYPE entity; 		// 0000-0184	;entity
    PADDING(_pad106, 174);		// 0184-0358
    uint32_t _pad107;			// 0358-0362	;dwPedType
    PADDING(_pad101, 722);		// 0362-1084
    CPedIntelligence* pPedIntelligence; // 1084-1088
    PADDING(_pad100, 8);		// 1088-1096
    uint32_t dwAction;			// 1096-1100	;Action
    PADDING(_pad102, 52);		// 1100-1152
    uint32_t dwStateFlags; 		// 1152-1156	;StateFlags

    uintptr_t dwInvulFlags; // 1136-1140		0x1000 = can_decap
    PADDING(_pad228, 8); // 1140-1148
    uintptr_t Tasks; // 1148-1152
    uintptr_t dwPlayerInfoOffset; // 1152-1156

    PADDING(_pad103, 168);		// 1156-1344
    float fHealth;		 		// 1344-1348	;Health
    float fMaxHealth;			// 1348-1352	;MaxHealth
    float fArmour;				// 1352-1356	;Armour
    float fAim;
    PADDING(_pad104, 8);		// 1356-1368
    float fRotation1;			// 1368-1372	;Rotation1
    float fRotation2;			// 1372-1376	;Rotation2
    PADDING(_pad105, 44);		// 1376-1420
    uint32_t pVehicle;			// 1420-1424	;pVehicle
    PADDING(_pad108, 8);		// 1424-1432
    uint32_t dwPedType;			// 1432-1436	;dwPedType
    uint32_t dwUnk1;	 // 1436-1440
    WEAPON_SLOT_TYPE WeaponSlots[13]; // 1440-1804
    PADDING(_pad270, 12); // 1804-1816
    uint8_t byteCurWeaponSlot; // 1816-1817
    PADDING(_pad280, 23); // 1817-1840
    uint32_t pFireObject;	 // 1840-1844
    PADDING(_pad281, 44); // 1844-1888
    uint32_t  dwWeaponUsed; // 1888-1892
    uintptr_t pdwDamageEntity; // 1892-1896
} PED_TYPE;
 */

#include "vendor/raknet/SAMP/samp_netencr.h"
#include "vendor/raknet/SAMP/SAMPRPC.h"

#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>
#include "game/common.h"

void Log(const char *fmt, ...);
uint32_t GetTickCount();


#endif //BLACK_RUSSIA_ORIGINAL_MAIN_H
