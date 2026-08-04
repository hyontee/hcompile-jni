#include <RakClient.h>
#include <RakNetworkFactory.h>
#include <PacketEnumerations.h>
#include <StringCompressor.h>

#include "types.h"
#include "RPC.h"
#include "CNetGame.h"
#include "utils.h"
#include "SAMP/SAMPRPC.h"
#include "util/CJavaWrapper.h"
#include "util/util.h"
#include "scripting.h"
#include "util/patch.h"

extern uintptr_t g_libBR;
extern uintptr_t g_libGTASA;
uint8_t g_pCurActivePlayer;

void ServerJoin(RPCParameters *rpcParams)
{
    RakNet::BitStream l_bitStream(rpcParams->input, rpcParams->numberOfBitsOfData/8 + 1, false);
    _PlayerID 		l_PlayerID;
    char			l_cIsNPC;
    unsigned char	l_ucNickLen;
    char			l_szNickName[256];
    unsigned int 	l_uiColor;
    CNetGame*		l_pNetGame;

    l_pNetGame = CNetGame::Instance();

    l_bitStream.Read(l_PlayerID);
    l_bitStream.Read(l_uiColor);
    l_bitStream.Read(l_cIsNPC);
    l_bitStream.Read(l_ucNickLen);
    l_bitStream.Read(l_szNickName, l_ucNickLen);

    //l_pNetGame->getPlayerPool()->New(l_PlayerID, l_szNickName, (bool)l_cIsNPC);

    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x07C0D0 + 1))((uintptr_t*)rpcParams);

    LOG(OBFUSCATE("ServerJoin -> %s (%d)!"), l_szNickName, l_PlayerID);
}

void ServerQuit(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("ServerQuit called!"));
}

int iNetModeNormalOnfootSendRate;

void InitGame(RPCParameters *rpcParams)
{
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsInitGame((unsigned char *)rpcParams->input,(iBitLength/8)+1,false);

    LOG(OBFUSCATE("InitGame() -> Called"));

    bool bLanMode, bStuntBonus;

    bool m_bZoneNames, m_bUseCJWalk, m_bAllowWeapons, m_bLimitGlobalChatRadius;
    float m_fGlobalChatRadius, m_fNameTagDrawDistance;
    bool m_bDisableEnterExits, m_bNameTagLOS, m_bTirePopping;
    int m_iSpawnsAvailable;
    bool m_bShowPlayerTags;
    int m_iShowPlayerMarkers;
    BYTE muint8_tWorldTime, muint8_tWeather;
    float m_fGravity;
    int m_iDeathDropMoney;
    bool m_bInstagib;
    CNetGame* l_pNetGame;

    LOG(OBFUSCATE("InitGame() -> Called"));


    l_pNetGame = CNetGame::Instance();


    bsInitGame.ReadCompressed(m_bZoneNames);
    bsInitGame.ReadCompressed(m_bUseCJWalk);
    bsInitGame.ReadCompressed(m_bAllowWeapons);
    bsInitGame.ReadCompressed(m_bLimitGlobalChatRadius);
    bsInitGame.Read(m_fGlobalChatRadius);
    bsInitGame.ReadCompressed(bStuntBonus);
    bsInitGame.Read(m_fNameTagDrawDistance);
    bsInitGame.ReadCompressed(m_bDisableEnterExits);
    bsInitGame.ReadCompressed(m_bNameTagLOS);
    bsInitGame.ReadCompressed(m_bTirePopping); //
    bsInitGame.Read(m_iSpawnsAvailable);

    _PlayerID MyPlayerID;
    bsInitGame.Read(MyPlayerID);

    bsInitGame.ReadCompressed(m_bShowPlayerTags);
    bsInitGame.Read(m_iShowPlayerMarkers);
    bsInitGame.Read(muint8_tWorldTime);
    bsInitGame.Read(muint8_tWeather);
    bsInitGame.Read(m_fGravity);
    bsInitGame.ReadCompressed(bLanMode);
    bsInitGame.Read(m_iDeathDropMoney);
    bsInitGame.ReadCompressed(m_bInstagib); //

    // Server's send rate restrictions
    bsInitGame.Read(iNetModeNormalOnfootSendRate);
    int iNetModeNormalIncarSendRate;
    bsInitGame.Read(iNetModeNormalIncarSendRate);
    int iNetModeFiringSendRate;
    bsInitGame.Read(iNetModeFiringSendRate);
    int iNetModeSendMultiplier;
    bsInitGame.Read(iNetModeSendMultiplier);

    int g_lagcompmode;
    bsInitGame.Read(g_lagcompmode);

    BYTE byteStrLen;
    bsInitGame.Read(byteStrLen);
    char g_szHostName[256];
    if(byteStrLen)
    {
        memset(g_szHostName,0,sizeof(g_szHostName));
        bsInitGame.Read(g_szHostName, byteStrLen);
    }
    g_szHostName[byteStrLen] = '\0';

    char byteVehicleModels[212];
    bsInitGame.Read(byteVehicleModels, 212);

//	l_pNetGame->getPlayerPool()->setLocalPlayerID(MyPlayerID);
    *(uintptr_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 40) = 14;

    // for br
//    uintptr_t v43 = *(uintptr_t *)(*(uintptr_t*)(g_libBR + 0x207F24));
//    if ( !(*(uintptr_t*)(g_libBR + 0x207F24)) ) {
//        v43 = (uintptr_t )operator new(0x28Au);
//        ((uintptr_t(*)(uintptr_t)) (g_libBR + 0x5212C))(v43);
////        *(uint8_t*)(g_libGTASA + 0x5D1B24) = *(uint8_t*)(v43 + 8);
////        g_pCurActivePlayer = *(uint8_t*)(v43 + 8);
////        if ( !*(uint8_t *)(*(uintptr_t*)(g_libBR + 0x207F3C)) && !*(uint8_t *)(*(uintptr_t*)(g_libBR + 0x207F38)) )
////            ((uintptr_t(*)(uintptr_t, int))(g_libBR + 0x53258))(*(uintptr_t *)(*(uintptr_t *)(*(uintptr_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 4) + 2) + 47), 1); // 0 - lock | 1 - unlock
//    }

    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x07BB04 + 1))((uintptr_t*)rpcParams);

    LOG(OBFUSCATE("Connected to %.64s"), g_szHostName);

    l_pNetGame->m_isInGame = true;


//    // from br
//
//    int v2; // r0
//    char v3; // r2
//    int v4; // r3
//    int v5; // r1
//    int v6; // r2
//    int v7; // r1
//    int v8; // r3
//    int v9; // r3
//    int v10; // r2
//    int v11; // r0
//    int v12; // r0
//    char v13; // r2
//    int v14; // r3
//    int v15; // r1
//    int v16; // r2
//    int v17; // r3
//    int v18; // r1
//    int v19; // r0
//    char v20; // r1
//    int v21; // r0
//    int v22; // r2
//    char v23; // r0
//    int v24; // r2
//    int v25; // r5
//    int v26; // r0
//    int v27; // r5
//    int v28; // r4
//    uint16_t v29; // r6
//    int v30; // r3
//    int v31; // r3
//    int v32; // r3
//    int v33; // r3
//    int v34; // r3
//    int v35; // r3
//    int v36; // r3
//    int v37; // r3
//    int v38; // r3
//    int v39; // r3
//    int v40; // r3
//    int v41; // r3
//    int *v42; // r6
//    int v43; // r5
//    uintptr_t v44; // r0
//    uint8_t v46; // [sp+1h] [bp-1FFh] BYREF
//    uint16_t v47; // [sp+2h] [bp-1FEh] BYREF
//    char v48[212]; // [sp+4h] [bp-1FCh] BYREF
//    uintptr_t* v49; // [sp+D8h] [bp-128h] BYREF
//    int v50; // [sp+E0h] [bp-120h]
//    int v51; // [sp+E4h] [bp-11Ch]
//    int v52; // [sp+1ECh] [bp-14h]
//
//    LOG(OBFUSCATE("InitGame() -> zalupa 0"));
//
//    v49 = (*(uintptr_t**)(&bsInitGame));
//    v2 = iBitLength;
//    v3 = v50;
//    if ( v50 < iBitLength )
//    {
//        v4 = *(uint8_t *)(v51 + (v50++ >> 3));
//        v5 = v50;
//        *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 838) = ((0x80u >> (v3 & 7)) & v4) != 0;
//        if ( v5 >= v2 )
//            goto LABEL_3;
//        LABEL_6:
//        v8 = *(uint8_t *)(v51 + (v5 >> 3));
//        v6 = v5 + 1;
//        v50 = v5 + 1;
//        *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 839) = ((0x80u >> (v5 & 7)) & v8) != 0;
//        if ( v5 + 1 >= v2 )
//            goto LABEL_4;
//        goto LABEL_7;
//    }
//    LOG(OBFUSCATE("InitGame() -> zalupa 1"));
//    v5 = v50;
//    if ( v50 < iBitLength )
//        goto LABEL_6;
//    LABEL_3:
//    v6 = v5;
//    if ( v5 >= v2 )
//    {
//        LABEL_4:
//        v7 = v6;
//        goto LABEL_8;
//    }
//    LOG(OBFUSCATE("InitGame() -> zalupa 1.25"));
//    LABEL_7:
//    v9 = *(uint8_t *)(v51 + (v6 >> 3));
//    v7 = v6 + 1;
//    v50 = v6 + 1;
//    *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 840) = ((0x80u >> (v6 & 7)) & v9) != 0;
//    LOG(OBFUSCATE("InitGame() -> zalupa 1.5"));
//    LABEL_8:
//    v10 = (*(uintptr_t*)(g_libBR + 0x207F28));
//    if ( v7 < v2 )
//    {
//        v11 = *(uint8_t *)(v51 + (v7 >> 3));
//        v50 = v7 + 1;
//        *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 842) = (v11 & (0x80u >> (v7 & 7))) != 0;
//    }
//    LOG(OBFUSCATE("InitGame() -> zalupa 1.75"));
//    bsInitGame.Read((char *)(v10 + 843), 32);
//    if ( v50 < iBitLength )
//        ++v50;
//    LOG(OBFUSCATE("InitGame() -> zalupa 2"));
//    bsInitGame.Read((char *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 847), 32);
//    v12 = iBitLength;
//    v13 = v50;
//    if ( v50 >= iBitLength )
//    {
//        v15 = v50;
//        if ( v50 >= iBitLength )
//            goto LABEL_14;
//    }
//    else
//    {
//        v14 = *(uint8_t *)(v51 + (v50++ >> 3));
//        v15 = v50;
//        *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 851) = ((0x80u >> (v13 & 7)) & v14) != 0;
//        if ( v15 >= v12 )
//        {
//            LABEL_14:
//            v16 = v15;
//            goto LABEL_17;
//        }
//    }
//    LOG(OBFUSCATE("InitGame() -> zalupa 3"));
//    v17 = *(uint8_t *)(v51 + (v15 >> 3));
//    v16 = v15 + 1;
//    v50 = v15 + 1;
//    *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 852) = ((0x80u >> (v15 & 7)) & v17) != 0;
//    LABEL_17:
//    v18 = (*(uintptr_t*)(g_libBR + 0x207F28));
//    if ( v16 < v12 )
//    {
//        v19 = *(uint8_t *)(v51 + (v16 >> 3));
//        v50 = v16 + 1;
//        *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 853) = (v19 & (0x80u >> (v16 & 7))) != 0;
//    }
//    bsInitGame.Read((char *)(v18 + 854), 32);
//    bsInitGame.Read((char*)&v47, 16);
//    v20 = v50;
//    v21 = (*(uintptr_t*)(g_libBR + 0x207F28));
//    if ( v50 < iBitLength )
//    {
//        v22 = *(uint8_t *)(v51 + (v50++ >> 3));
//        *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 858) = ((0x80u >> (v20 & 7)) & v22) != 0;
//    }
//    LOG(OBFUSCATE("InitGame() -> zalupa 4"));
//    bsInitGame.Read((char *)(v21 + 859), 32);
//    bsInitGame.Read((char *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 864), 8);
//    bsInitGame.Read((char *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 866), 8);
//    bsInitGame.Read((char *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 867), 32);
//    if ( v50 < iBitLength )
//        ++v50;
//    bsInitGame.Read((char *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 871), 32);
//    v23 = v50;
//    if ( v50 < iBitLength )
//    {
//        v24 = *(uint8_t *)(v51 + (v50++ >> 3));
//        *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 875) = ((0x80u >> (v23 & 7)) & v24) != 0;
//    }
//    bsInitGame.Read(&(*(char *)(g_libBR + 0x202E6C)), 32);
//    bsInitGame.Read(&(*(char *)(g_libBR + 0x202E70)), 32);
//    bsInitGame.Read((*(char* *)(g_libBR + 0x202E74)), 32);
//    bsInitGame.Read((*(char* *)(g_libBR + 0x202E78)), 32);
//    bsInitGame.Read((char *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 876), 32);
//    bsInitGame.Read(*(char* *)v46, 8);
//    if ( v46 )
//    {
//        v25 = (*(uintptr_t*)(g_libBR + 0x207F28)) + 448;
//        memset((void *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 448), 0, 0xFFu);
//        ((uintptr_t(*)(uintptr_t*, uintptr_t))(g_libBR + 0x9F7D2 + 1))(v49, v25);
//        v26 = v46;
//    }
//    else
//    {
//        v26 = 0;
//    }
//    *(uint8_t *)(v26 + (*(uintptr_t*)(g_libBR + 0x207F28)) + 448) = 0;
//    LOG(OBFUSCATE("InitGame() -> zalupa 5"));
//    bsInitGame.Read((char*)&v48, 212);
//    bsInitGame.Read((char *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 880), 32);
//    v27 = *(uintptr_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 4);
//    if ( v27 )
//        v28 = *(uintptr_t *)(v27 + 2);
//    else
//        v28 = 0;
//    v29 = v47;
//    strcpy((char *)(v27 + 25 * v47 + 5059), (const char *)(v27 + 6));
//    *(int *)v27 = v29;
//    if ( (*(uintptr_t*)(g_libBR + 0x207F24)) )
//        ((uintptr_t(*)())(g_libBR + 0x438AC + 1))();
//    LOG(OBFUSCATE("InitGame() -> zalupa 5.25"));
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 69, 1000, v30);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 70, 1000, v31);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 71, 1000, v32);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 72, 1000, v33);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 73, 1000, v34);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 74, 1000, v35);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 75, 1000, v36);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 76, 1000, v37);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 77, 1000, v38);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 78, 1000, v39);
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E2E)), 79, 1000, v40);
//    LOG(OBFUSCATE("InitGame() -> zalupa 6"));
//    v42 = (int *)(*(uintptr_t*)(g_libBR + 0x207F24));
//    v43 = *(uintptr_t *)(*(uintptr_t*)(g_libBR + 0x207F24));
//    if ( !*(uintptr_t *)(*(uintptr_t*)(g_libBR + 0x207F24)) )
//    {
//        v43 = (uintptr_t)operator new(0x28Au);
//        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x5212C + 1))(v43);
//        *v42 = v43;
//    }
//    ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814 + 1))(&(*(uintptr_t*)(g_libBR + 0x1D6E40)), *(uintptr_t *)(v43 + 8), 2, v41);
//    v44 = (*(uintptr_t*)(g_libBR + 0x207F28));
//    if ( *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 851) )
//    {
//        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x44868 + 1))((*(uintptr_t*)(g_libBR + 0x207F24)));
//        v44 = (*(uintptr_t*)(g_libBR + 0x207F28));
//    }
//    *(uintptr_t *)(v44 + 40) = 14;
//    if ( v28 )
//    {
////        //((uintptr_t(*)(uintptr_t))(g_libBR + 0x76A84 + 1))(v28);
//        v44 = (*(uintptr_t*)(g_libBR + 0x207F28));
//    }
//
//    LOG(OBFUSCATE("InitGame() -> zalupa 7"));
////    ((uintptr_t(*)(uintptr_t, uint8_t))(g_libBR + 0x4446C + 1))((*(uintptr_t*)(g_libBR + 0x207F24)), *(uint8_t *)(v44 + 866));
////    ((uintptr_t(*)(uintptr_t, uint8_t))(g_libBR + 0x447EC + 1))((*(uintptr_t*)(g_libBR + 0x207F24)), *(uint8_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 839));
////    //((uintptr_t(*)(uintptr_t*))(g_libBR + 0x9F48A + 1))(v49);
//
//    // =======
//
//
////    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x07BB04 + 1))((uintptr_t*)rpcParams);
//
//	//LOG(OBFUSCATE("Connected to %.64s", *(char* *)(v44 + 448));
}

void WorldPlayerAdd(RPCParameters *rpcParams)
{
//	CNetGame::Instance()->getPlayerPool()->StreamPlayerIn(rpcParams);
    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x07C828 + 1))((uintptr_t*)rpcParams);
    LOG(OBFUSCATE("WorldPlayerAdd called!"));
}

void WorldPlayerDeath(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("WorldPlayerDeath called!"));
}

void WorldPlayerRemove(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("WorldPlayerRemove called!"));
}

void WorldVehicleAdd(RPCParameters *rpcParams)
{
    //((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x7CCDC + 1))((uintptr_t*)rpcParams);

    static int id = 1;

    unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

    NEW_VEHICLE NewVehicle;
    bsData.Read((char *)&NewVehicle,sizeof(NEW_VEHICLE));

    if(!NewVehicle.VehicleID){
        NewVehicle.vecPos.X += 5.0;
        NewVehicle.vecPos.Y += 5.0;

        NewVehicle.VehicleID = id;
        NewVehicle.aColor1 = 0;
        NewVehicle.aColor2 = 0;
        NewVehicle.cColor1 = 0;
        NewVehicle.cColor2 = 0;

        NewVehicle.fHealth = 1000.0;
        NewVehicle.fRotation = 0.0;
        NewVehicle.iVehicleType = 451;
        NewVehicle.dwPanelDamageStatus = 1;
        NewVehicle.dwDoorDamageStatus = 1;
        NewVehicle.byteTireDamageStatus = 1;
        NewVehicle.bytePaintjob = 0;
        for (int i = 0; i < 14; ++i) {
            NewVehicle.byteModSlots[i] = 0;
        }
        NewVehicle.byteAddSiren = 0;
        NewVehicle.byteLightDamageStatus = 1;
        NewVehicle.byteInterior = 0;
    }

    if(NewVehicle.VehicleID && *(uintptr_t*)((*(uintptr_t *)(g_libBR + 0x207F28)) + 8))
        ((uintptr_t(*)(uintptr_t, NEW_VEHICLE*))(g_libBR + 0x8C8B0 + 1))(*(uintptr_t*)((*(uintptr_t *)(g_libBR + 0x207F28)) + 8), &NewVehicle);

    id++;

    LOG(OBFUSCATE("WorldVehicleAdd called!"));
}

void ScrSetVehiclePos(RPCParameters *rpcParams)
{
    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x840CC + 1))((uintptr_t*)rpcParams);
    LOG(OBFUSCATE("ScrSetVehiclePos called!"));
}

void ScrSetVehicleParams(RPCParameters *rpcParams)
{
    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x83B30 + 1))((uintptr_t*)rpcParams);
    LOG(OBFUSCATE("ScrSetVehicleParams called!"));
}

void WorldVehicleRemove(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("WorldVehicleRemove called!"));
}

void ConnectionRejected(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("ConnectionRejected called!"));
}

void ClientMessage(RPCParameters *rpcParams)
{
    unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
    uint32_t dwStrLen;
    uint32_t dwColor;

    bsData.Read(dwColor);
    bsData.Read(dwStrLen);
    char* szMsg = (char*)malloc(dwStrLen+1);
    bsData.Read(szMsg, dwStrLen);
    szMsg[dwStrLen] = 0;

//    static float l = 0;
//    PrintChatLine("%s", szMsg);

    ((uintptr_t(*)(uintptr_t*, uintptr_t, char*))(g_libBR + 0x01DAF4 + 1))(*(uintptr_t**)(g_libBR + 0x207F2C), 0x0071BDFF, szMsg);

    free(szMsg);
    LOG(OBFUSCATE("ClientMessage called!"));
}

void Chat(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("Chat called!"));
}

void UpdateScoresPingsIPs(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("UpdateScoresPingsIPs called!"));
}

void RequestClass(RPCParameters *rpcParams)
{
    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x07C454 + 1))((uintptr_t*)rpcParams);
    LOG(OBFUSCATE("RequestClass called!"));
}

int sub_7C5BC_hook(RPCParameters *rpcParams);
void RequestSpawn(RPCParameters *rpcParams)
{
//    ScriptCommand(&lock_camera_position, 0);
//    ScriptCommand(&restore_camera_to_user);
//    ScriptCommand(&set_camera_behind_player);
//    ScriptCommand(&restore_camera_jumpcut);

//    if ( !*(uint8_t *)(*(uintptr_t*)(g_libBR + 0x207F3C)) && !*(uint8_t *)(*(uintptr_t*)(g_libBR + 0x207F38)) )
//        ((uintptr_t(*)(uintptr_t, int))(g_libBR + 0x53258 + 1))(*(uintptr_t *)(*(uintptr_t *)(*(uintptr_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 4) + 2) + 47), 1); // 0 - lock | 1 - unlock

//    sub_7C5BC_hook(rpcParams);
    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x07C5BC + 1))((uintptr_t*)rpcParams);

    LOG(OBFUSCATE("RequestSpawn called!"));
}

void ScrSetPlayerPos(RPCParameters *rpcParams)
{
    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x0825C0 + 1))((uintptr_t*)rpcParams);

    LOG(OBFUSCATE("ScrSetPlayerPos called!"));
}

void ScrInitMenu(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("ScrInitMenu called!"));
}

extern CJavaWrapper* g_pJavaWrapper;
void ScrDialogBox(RPCParameters *rpcParams)

{
    unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    uint16_t wDialogID = -1;
    uint8_t byteDialogStyle = 0;
    uint8_t len;
    char szBuff[4096+1];
    char title[64 * 3 + 1], info[4096+1], button1[20+1], button2[20+1];

    RakNet::BitStream bsData((unsigned char *)Data,(iBitLength/8)+1,false);

    bsData.Read(wDialogID);
    bsData.Read(byteDialogStyle);
    // title
    bsData.Read(len);
    bsData.Read(szBuff, len);
    szBuff[len] = '\0';
    cp1251_to_utf8(title, szBuff);
    // button1
    bsData.Read(len);
    bsData.Read(szBuff, len);
    szBuff[len] = '\0';
    cp1251_to_utf8(button1, szBuff);
    // button2
    bsData.Read(len);
    bsData.Read(szBuff, len);
    szBuff[len] = '\0';
    cp1251_to_utf8(button2, szBuff);

    // info
    stringCompressor->DecodeString(szBuff, 4096, &bsData);
    cp1251_to_utf8(info, szBuff);

    if(wDialogID < 0) return;

//    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x07D4A8 + 1))((uintptr_t*)rpcParams);

//    if(1)
//    {
//    //    pGame->FindPlayerPed()->TogglePlayerControllable(false);
    g_pJavaWrapper->MakeDialog(wDialogID, byteDialogStyle, title, info, button1, button2);
//    }
}

extern uintptr_t g_libGTASA;
void ScrGameText(RPCParameters *rpcParams)
{
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsData((unsigned char *)rpcParams->input,(iBitLength/8)+1,false);
    char szMessage[400];
    uint16_t *szGameTextMessage;
    int iType, iTime, iLength;

    bsData.Read(iType);
    bsData.Read(iTime);
    bsData.Read(iLength);

    if(iLength > 400) return; // tsk tsk, kye

    bsData.Read(szMessage, iLength);
    szMessage[iLength] = '\0';

//    ScriptCommand(&text_clear_all);
//    AsciiToGxtChar(szMessage, szGameTextMessage);
//
//    // CMessages::AddBigMesssage
//    (( void (*)(uint16_t*, int, int))(g_libGTASA+0x4D18C0+1))(szGameTextMessage, iTime, iType);

    LOG(OBFUSCATE("[GAMETEXT] %s"), szMessage);
}

void ScrPlayAudioStream(RPCParameters *rpcParams)
{
    LOG(OBFUSCATE("ScrPlayAudioStream called!"));
}

void ScrSetSpawnInfo(RPCParameters *rpcParams)
{
    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x082D9C + 1))((uintptr_t*)rpcParams);
    LOG(OBFUSCATE("ScrSetSpawnInfo called!"));
}

void ScmEvent(RPCParameters *rpcParams)
{
    ((uintptr_t(*)(uintptr_t *)) (g_libBR + 0x07E594 + 1))((uintptr_t*)rpcParams);
    LOG(OBFUSCATE("ScmEvent called!"));
}

// hooks to BR for sending info to server with raknet

int (*sub_7A8B8)(uintptr_t *a1, const char *a2);
int sub_7A8B8_hook(uintptr_t *a1, const char *a2)
{
    CNetGame::Instance()->SendChatMessage(a2);
    return 0;
}

int (*sub_7A964)(uintptr_t *a1, const char *a2);
int sub_7A964_hook(uintptr_t *a1, const char *a2)
{
    if(!strcmp(a2, OBFUSCATE("/tab")) || !strcmp(a2, OBFUSCATE("/chat")) || !strcmp(a2, OBFUSCATE("/cameditgui"))){
        return sub_7A964(a1, a2);
    }
    CNetGame::Instance()->SendChatCommand(a2);
    return 0;
}

int (*sub_7AAB8)(uintptr_t *a1, int16_t a2, char a3, int16_t a4, char *a5);
int sub_7AAB8_hook(uintptr_t *a1, int16_t a2, char a3, int16_t a4, char *a5){
    CNetGame::Instance()->SendDialogResponse(a2,a3,a4,a5);
    return 0;
}

int (*sub_76E00)(uintptr_t *a1, uintptr_t a2);
int sub_76E00_hook(uintptr_t *a1, uintptr_t a2){
    CNetGame::Instance()->RequestClass(a2);
    return 0;
}

int (*sub_76874)(uintptr_t *a1);
int sub_76874_hook(uintptr_t *a1)
{
    int result; // r0
    int v3; // r5
    int v4; // r2
    int v5; // r3
    RakNet::BitStream v6; // [sp+18h] [bp-128h] BYREF
    int v7; // [sp+48h] [bp-F8h]
    int v8; // [sp+4Ch] [bp-F4h]
    int v9; // [sp+50h] [bp-F0h]

    LOG(OBFUSCATE("Spawning step 0"));

    result = 0;
    if ( *(uint8_t *)(a1 + 67) )
    {
        *(uint8_t *)(*(uintptr_t*)(g_libBR + 0x207F30)) = 0;
//        if ( ((uintptr_t(*)(uintptr_t))(g_libBR + 0x52A26))(*(uintptr_t *)(a1 + 47)) == 1 )
//        {
//            LOG(OBFUSCATE("Spawning step 0.25");
//            ((uintptr_t(*)(uintptr_t, RakNet::BitStream))(g_libBR + 0x42FD0 + 1))(*(uintptr_t *)(a1 + 47), v6);
//            LOG(OBFUSCATE("Spawning step 0.5");
//            ((uintptr_t(*)(uintptr_t, uintptr_t , uintptr_t, uintptr_t))(g_libBR + 0x52994 + 1))(*(uintptr_t *)(a1 + 47), v7, v8, v9);
//            LOG(OBFUSCATE("Spawning step 0.75");
//        }
        v3 = *(uintptr_t *)((*(uintptr_t*)(g_libBR + 0x207F24)) + 5);
        LOG(OBFUSCATE("Spawning step 1"));
        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x4107C + 1))(v3);
        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x4103C + 1))(v3);
        ((uintptr_t(*)(uintptr_t, int))(g_libBR + 0x4457C + 1))((*(uintptr_t*)(g_libBR + 0x207F24)), 1);
        ((uintptr_t(*)(uintptr_t, int))(g_libBR + 0x445F4 + 1))((*(uintptr_t*)(g_libBR + 0x207F24)), 1);
//        ((uintptr_t(*)(uintptr_t, int))(g_libBR + 0x53170 + 1))(*(uintptr_t *)(a1 + 47), 1);
        LOG(OBFUSCATE("Spawning step 2"));
        if ( !(*(uint8_t *)(g_libBR + 0x201EDB)) )
            ((uintptr_t(*)(uintptr_t))(g_libBR + 0x52E04 + 1))(*(uintptr_t *)(a1 + 47));
//        ((uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t))(g_libBR + 0x44630 + 1))((*(uintptr_t*)(g_libBR + 0x207F24)), *(uintptr_t *)(a1 + 69), *(uintptr_t *)(a1 + 73));
//        ((uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t))(g_libBR + 0x53464 + 1))(*(uintptr_t *)(a1 + 47), *(uintptr_t*)a1 + 69, *(uintptr_t *)(a1 + 81));
//        ((uintptr_t(*)(uintptr_t, uintptr_t))(g_libBR + 0x52544 + 1))(*(uintptr_t *)(a1 + 47), *(uintptr_t *)(a1 + 64));
//        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x5341C + 1))(*(uintptr_t *)(a1 + 47));
//        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x53460 + 1))(*(uintptr_t *)(a1 + 47));
        LOG(OBFUSCATE("Spawning step 3"));
        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x44660 + 1))((*(uintptr_t*)(g_libBR + 0x207F24)));
        ((uintptr_t(*)(uintptr_t, const void* , int))(g_libBR + 0x1B206 + 1))((unsigned int)&(*(uintptr_t*)(g_libBR + 0x36EA2C)) + (*(uintptr_t*)(g_libBR + 0x207F0C)), (uintptr_t*)((uintptr_t(*)())(g_libBR + 0x76A5C)), 2u);
//        ((uintptr_t(*)(uintptr_t, float, uintptr_t, float))(g_libBR + 0x43010 + 1))(*(uintptr_t *)(a1 + 47), *(float *)(a1 + 69), *(uintptr_t *)(a1 + 73), (*(float *)(a1 + 77) + 0.5));
//        ((uintptr_t(*)(uintptr_t, float))(g_libBR + 0x525D8 + 1))(*(uintptr_t *)(a1 + 47), *(float *)(a1 + 81));
        *(int *)(a1 + 51) = 1;
        *(uint8_t *)a1 = 0;
        LOG(OBFUSCATE("Spawning step 4"));
        CNetGame::Instance()->m_rakClientInterface->RPC(
                &RPC_Spawn,
                &v6,
                HIGH_PRIORITY,
                RELIABLE_ORDERED,
                0,
                false,
                UNASSIGNED_NETWORK_ID,
                NULL);
        LOG(OBFUSCATE("Spawning step 5"));
//        ((uintptr_t(*)(uintptr_t*))(g_libBR + 0x8D960))((uintptr_t *)(*(uintptr_t*)(g_libBR + 0x7BEE6C)));
        if ( (*(uint8_t *)(g_libBR + 0x201EDB)) )
        {
            if ( (*(uintptr_t*)(g_libBR + 0x201EDC)) != -1 )
            {
                ((uintptr_t(*)(uintptr_t*, ...))(g_libBR + 0x55814))(&*(uintptr_t*)(g_libBR + 0x1D6D04), (*(uintptr_t*)(g_libBR + 0x201EDC)), v4, v5);
                (*(uintptr_t*)(g_libBR + 0x201EDC)) = -1;
            }
            (*(uint8_t*)(g_libBR + 0x201EDB)) = 0;
        }
        LOG(OBFUSCATE("Spawning step 6"));
//        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x9F48A))((int)v6);
        return 1;
    }
    LOG(OBFUSCATE("Spawning step finish"));
    return result;
}

void (*ScrAddGangZone)(RPCParameters *);

void (*ScrSetGravity)(RPCParameters *);

void (*ScrForceSpawnSelection)(RPCParameters *);

void (*ScrSetCameraPos)(RPCParameters *);

void (*ScrSetCameraLookAt)(RPCParameters *);

void (*ScrSetPlayerFacingAngle)(RPCParameters *);

void (*ScrSetFightingStyle)(RPCParameters *);

void (*ScrSetPlayerSkin)(RPCParameters *);

void (*ScrApplyPlayerAnimation)(RPCParameters *);

void (*ScrClearPlayerAnimations)(RPCParameters *);

void (*ScrCreateExplosion)(RPCParameters *);

void (*ScrSetPlayerHealth)(RPCParameters *);

void (*ScrSetPlayerArmour)(RPCParameters *);

void (*ScrSetPlayerColor)(RPCParameters *);

void (*ScrSetPlayerName)(RPCParameters *);

void (*ScrSetPlayerPosFindZ)(RPCParameters *);

void (*ScrSetPlayerInterior)(RPCParameters *);

void (*ScrSetMapIcon)(RPCParameters *);

void (*ScrDisableMapIcon)(RPCParameters *);

void (*ScrSetCameraBehindPlayer)(RPCParameters *);

void (*ScrSetPlayerSpecialAction)(RPCParameters *);

void (*ScrTogglePlayerSpectating)(RPCParameters *);

void (*ScrSetPlayerSpectating)(RPCParameters *);

void (*ScrPlayerSpectatePlayer)(RPCParameters *);

void (*ScrPlayerSpectateVehicle)(RPCParameters *);

void (*ScrPutPlayerInVehicle)(RPCParameters *);

void (*ScrVehicleParamsEx)(RPCParameters *);

void (*ScrVehicleParams)(RPCParameters *);

void (*ScrHaveSomeMoney)(RPCParameters *);

void (*ScrLinkVehicle)(RPCParameters *);

void (*ScrResetMoney)(RPCParameters *);

void (*ScrRemovePlayerFromVehicle)(RPCParameters *);

void (*ScrSetVehicleHealth)(RPCParameters *);

void (*ScrSetVehicleVelocity)(RPCParameters *);

void (*ScrInterpolateCamera)(RPCParameters *);

void (*ScrNumberPlate)(RPCParameters *);

void (*ScrRemoveGangZone)(RPCParameters *);

void (*ScrFlashGangZone)(RPCParameters *);

void (*ScrStopFlashGangZone)(RPCParameters *);

void (*ScrCreateObject)(RPCParameters *);

void (*ScrSetObjectPos)(RPCParameters *);

void (*ScrDestroyObject)(RPCParameters *);

void (*ScrPlaySound)(RPCParameters *);

void (*ScrSetPlayerWantedLevel)(RPCParameters *);

void (*ScrGivePlayerWeapon)(RPCParameters *);

void (*ScrTogglePlayerControllable)(RPCParameters *);

void (*ScrAttachObjectToPlayer)(RPCParameters *);

void (*ScrResetPlayerWeapons)(RPCParameters *);

void (*ScrSetPlayerSkillLevel)(RPCParameters *);

void (*ScrShowTextDraw)(RPCParameters *);

void (*ScrEditTextDraw)(RPCParameters *);

void (*ScrHideTextDraw)(RPCParameters *);

void (*ScrSetPlayerAttachedObject)(RPCParameters *);

void (*ScrSetPlayerObjectMaterial)(RPCParameters *);

void (*ScrAttachTrailerToVehicle)(RPCParameters *);

void (*ScrDetachTrailerFromVehicle)(RPCParameters *);

void (*ScrSelectTextDraw)(RPCParameters *);

void (*ScrSetObjectRotation)(RPCParameters *);

void (*ScrRemoveComponent)(RPCParameters *);

int (*sub_7C5BC)(RPCParameters* a1);
int sub_7C5BC_hook(RPCParameters* a1)
{
    int v2; // r0
    const void *v3; // r1
    int v4; // r0
    int v5; // r0
    uint8_t *v6; // r4
    int v7; // r5
    bool v8; // zf
    char v10; // [sp+7h] [bp-121h] BYREF
    char v11[276]; // [sp+8h] [bp-120h] BYREF
    int v12; // [sp+11Ch] [bp-Ch]

    LOG(OBFUSCATE("RPC: RequestSpawn"));
    v2 = *(uintptr_t *)(a1 + 4);
    v3 = *(const void **)a1;
    v10 = 0;
    int iBitLength = a1->numberOfBitsOfData;
    RakNet::BitStream bsData((unsigned char *)a1->input,(iBitLength/8)+1,false);
    bsData.Read(v10);
    v4 = *(uintptr_t*)(&bsData);
    v5 = *(uintptr_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 4);
    if ( v5 )
        v6 = *(uint8_t **)(v5 + 2);
    else
        v6 = 0;
    if ( !(*(uintptr_t*)(g_libBR + 0x207F40)) )
    {
        v7 = (uintptr_t)operator new(0x360C6u);
        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x9B448+1))(v7);
        (*(uintptr_t*)(g_libBR + 0x207F40)) = v7;
        ((uintptr_t(*)(uintptr_t, uintptr_t, uint16_t))(g_libBR + 0x9C704 + 1))(v7, (*(uintptr_t*)(g_libBR + 0x207F28)) + 703, (uint16_t)(*(uintptr_t *)((*(uintptr_t*)(g_libBR + 0x207F28)) + 830) + 2));
    }
    if ( v6 )
    {
        if ( v10 == 2 )
            goto LABEL_13;
        v8 = v10 == 0;
        if ( v10 )
            v8 = *v6 == 0;
        if ( v8 )
            *v6 = 0;
        else
            LABEL_13:
            ((uintptr_t(*)(uintptr_t*))(g_libBR + 0x76874 + 1))((uintptr_t*)v6);
    }
//    ((uintptr_t(*)(uintptr_t))(g_libBR + 0x9F48A + 1))((int)v11);
    return v12;
}

int (*sub_45EA4)(int a1, float a2, float a3, int a4, int a5, int a6, char *a7);

void (*Weather)(RPCParameters *);
void (*WorldTime)(RPCParameters *);
void (*SetTimeEx)(RPCParameters *);
void (*SetCheckpoint)(RPCParameters *);
void (*DisableCheckpoint)(RPCParameters *);
void (*SetRaceCheckpoint)(RPCParameters *);
void (*DisableRaceCheckpoint)(RPCParameters *);
void (*EnterVehicle)(RPCParameters *);
void (*DialogBox)(RPCParameters *);
void (*GameModeRestart)(RPCParameters *);
void (*Pickup)(RPCParameters *);
void (*DestroyPickup)(RPCParameters *);
void (*Create3DTextLabel)(RPCParameters *);
void (*Delete3DTextLabel)(RPCParameters *);
void (*RemoveBuilding)(RPCParameters *);
void (*SetPlayerChatBubble)(RPCParameters *);
void (*DamageVehicle)(RPCParameters *);
void (*WorldActorAdd)(RPCParameters *);
void (*WorldActorRemove)(RPCParameters *);
void (*SetActorAnimation)(RPCParameters *);
void (*SetActorFacingAngle)(RPCParameters *);
void (*SetActorPos)(RPCParameters *);
void (*SetActorHealth)(RPCParameters *);
void (*ClearActorAnimations)(RPCParameters *);

void (*ExitVehicle)(RPCParameters *);

void (*ScrSetVehicleZAngle)(RPCParameters *);

void (*ScrDisplayGameText)(RPCParameters *);

int sub_45EA4_hook(int a1, float a2, float a3, int a4, int a5, int a6, char *a7)
{
    float v13; // r10
    uintptr_t *v14; // r0
    float v16; // [sp+1Ch] [bp-15Ch] BYREF
    float v17; // [sp+20h] [bp-158h] BYREF
    float v18; // [sp+24h] [bp-154h] BYREF
    uintptr_t v19[69]; // [sp+28h] [bp-150h] BYREF

    if ( (*(uintptr_t*)(g_libBR + 0x207F28)) && !strncmp(a7, "CODEWAY", 7u) )
    {
        v13 = ((int (__fastcall *)(uintptr_t, uintptr_t))(g_libGTASA + 0x3C3DD8 + 1))(a2, a3);
        LOG(OBFUSCATE("OnPlayerClickMap: %f, %f, %f"));
        v14 = (uintptr_t *)((uintptr_t(*)(uintptr_t))(g_libBR + 0x9F3E8 + 1))((uintptr_t)v19);
        v16 = a2;
        ((uintptr_t(*)(uintptr_t*, uint8_t*, int, int))(g_libBR + 0x9F5D4 + 1))(v14, (uint8_t *)&v16, 32, 1);
        v17 = a3;
        ((uintptr_t(*)(uintptr_t*, uint8_t*, int, int))(g_libBR + 0x9F5D4 + 1))(v19, (uint8_t *)&v17, 32, 1);
        v18 = v13 + 1.5;
        ((uintptr_t(*)(uintptr_t*, uint8_t*, int, int))(g_libBR + 0x9F5D4 + 1))(v19, (uint8_t *)&v18, 32, 1);
        CNetGame::Instance()->m_rakClientInterface->RPC(
                &RPC_MapMarker,
                (RakNet::BitStream *)(&v19),
                HIGH_PRIORITY,
                RELIABLE,
                0,
                false,
                UNASSIGNED_NETWORK_ID,
                NULL);
//        ((uintptr_t(*)(uintptr_t))(g_libBR + 0x9F48A))((int)v19);
    }
    return ((uintptr_t(*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,char*))(g_libBR + 0x629528))(a1, a2, a3, a4, a5, a6, a7);
}

void (*sub_8D960)(uintptr_t *a1);
void sub_8D960_hook(uintptr_t *a1)
{
    return;
}

// ==================================================

template <typename Func, typename Addr>
void SetFuncAddr(Func func, Addr addr)
{
    *(void **)(func) = (void*)addr;
}

void RegisterRPCs(RakClientInterface *pRakClient)
{

    // =================
    CHook::InlineHook(g_libBR, 0x07A8B8, (uintptr_t)sub_7A8B8_hook, (uintptr_t*)&sub_7A8B8);
    CHook::InlineHook(g_libBR, 0x07A964, (uintptr_t)sub_7A964_hook, (uintptr_t*)&sub_7A964);
    CHook::InlineHook(g_libBR, 0x07AAB8, (uintptr_t)sub_7AAB8_hook, (uintptr_t*)&sub_7AAB8);
//    CHook::InlineHook(g_libBR, 0x045EA4, (uintptr_t)sub_45EA4_hook, (uintptr_t*)&sub_45EA4);
    CHook::InlineHook(g_libBR, 0x076E00, (uintptr_t)sub_76E00_hook, (uintptr_t*)&sub_76E00);
//    CHook::InlineHook(g_libBR, 0x076874, (uintptr_t)sub_76874_hook, (uintptr_t*)&sub_76874);
    CHook::InlineHook(g_libBR, 0x07C5BC, (uintptr_t)sub_7C5BC_hook, (uintptr_t*)&sub_7C5BC);
    CHook::InlineHook(g_libBR, 0x08D960, (uintptr_t)sub_8D960_hook, (uintptr_t*)&sub_8D960);
//    CHook::NOP(g_libBR + 0x07C672, 2);
//    CHook::NOP(g_libBR + 0x07C674, 4);
    // =================


    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20783C)), InitGame); // ((void (*)(RPCParameters*))(g_libBR + 0x7BB04 + 1))
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207834)), ((void (*)(RPCParameters*))(g_libBR + 0x7C0D0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207838)), ((void (*)(RPCParameters*))(g_libBR + 0x7C1D4 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078B4)), ((void (*)(RPCParameters*))(g_libBR + 0x7C294 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20785C)), ((void (*)(RPCParameters*))(g_libBR + 0x7C350 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20784C)), ((void (*)(RPCParameters*))(g_libBR + 0x7C454 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207850)), ((void (*)(RPCParameters*))(g_libBR + 0x7C5BC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078C8)), ((void (*)(RPCParameters*))(g_libBR + 0x7C518 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078B8)), ((void (*)(RPCParameters*))(g_libBR + 0x7C6D4 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078CC)), ((void (*)(RPCParameters*))(g_libBR + 0x7C768 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20787C)), ((void (*)(RPCParameters*))(g_libBR + 0x7C828 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207884)), ((void (*)(RPCParameters*))(g_libBR + 0x7C9B8 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207890)), ((void (*)(RPCParameters*))(g_libBR + 0x7CA68 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207894)), ((void (*)(RPCParameters*))(g_libBR + 0x7CB48 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20789C)), ((void (*)(RPCParameters*))(g_libBR + 0x7CB7C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078A0)), ((void (*)(RPCParameters*))(g_libBR + 0x7CCA0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207888)), WorldVehicleAdd); // ((void (*)(RPCParameters*))(g_libBR + 0x7CCDC + 1))
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20788C)), ((void (*)(RPCParameters*))(g_libBR + 0x7D1F8 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207860)), ((void (*)(RPCParameters*))(g_libBR + 0x7D2D4 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207864)), ((void (*)(RPCParameters*))(g_libBR + 0x7D3E8 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A40)), ((void (*)(RPCParameters*))(g_libBR + 0x7D4A8 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078AC)), ((void (*)(RPCParameters*))(g_libBR + 0x7DC04 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078B0)), ((void (*)(RPCParameters*))(g_libBR + 0x7DC68 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078BC)), ((void (*)(RPCParameters*))(g_libBR + 0x7DE34 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078C0)), ((void (*)(RPCParameters*))(g_libBR + 0x7DED0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A44)), ((void (*)(RPCParameters*))(g_libBR + 0x7DF68 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A48)), ((void (*)(RPCParameters*))(g_libBR + 0x7E184 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A5C)), ((void (*)(RPCParameters*))(g_libBR + 0x7E26C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078A4)), ((void (*)(RPCParameters*))(g_libBR + 0x7E380 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207874)), ((void (*)(RPCParameters*))(g_libBR + 0x7E594 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207868)), ((void (*)(RPCParameters*))(g_libBR + 0x7E654 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207880)), ((void (*)(RPCParameters*))(g_libBR + 0x7ED58 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207818)), ((void (*)(RPCParameters*))(g_libBR + 0x7E750 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20781C)), ((void (*)(RPCParameters*))(g_libBR + 0x7E870 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20782C)), ((void (*)(RPCParameters*))(g_libBR + 0x7EACC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207828)), ((void (*)(RPCParameters*))(g_libBR + 0x7EA2C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207824)), ((void (*)(RPCParameters*))(g_libBR + 0x7E98C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207820)), ((void (*)(RPCParameters*))(g_libBR + 0x7E8EC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207830)), ((void (*)(RPCParameters*))(g_libBR + 0x7EC9C + 1)));

    // script rpc

    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079F0)), ((void (*)(RPCParameters*))(g_libBR + 0x8571C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207934)), ((void (*)(RPCParameters*))(g_libBR + 0x822B8 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207990)), ((void (*)(RPCParameters*))(g_libBR + 0x824EC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079D8)), ((void (*)(RPCParameters*))(g_libBR + 0x82580 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20791C)), ((void (*)(RPCParameters*))(g_libBR + 0x825C0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20793C)), ((void (*)(RPCParameters*))(g_libBR + 0x82690 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207940)), ((void (*)(RPCParameters*))(g_libBR + 0x82750 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207964)), ((void (*)(RPCParameters*))(g_libBR + 0x8280C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A28)), ((void (*)(RPCParameters*))(g_libBR + 0x828CC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207914)), ((void (*)(RPCParameters*))(g_libBR + 0x829B4 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A10)), ((void (*)(RPCParameters*))(g_libBR + 0x82A94 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A14)), ((void (*)(RPCParameters*))(g_libBR + 0x82CBC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20790C)), ((void (*)(RPCParameters*))(g_libBR + 0x82D9C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079B0)), ((void (*)(RPCParameters*))(g_libBR + 0x82E38 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207924)), ((void (*)(RPCParameters*))(g_libBR + 0x82F28 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20797C)), ((void (*)(RPCParameters*))(g_libBR + 0x82FC8 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207930)), ((void (*)(RPCParameters*))(g_libBR + 0x83068 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207918)), ((void (*)(RPCParameters*))(g_libBR + 0x8316C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207920)), ((void (*)(RPCParameters*))(g_libBR + 0x83288 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207938)), ((void (*)(RPCParameters*))(g_libBR + 0x83380 + 1)));
//    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207984)), ((void (*)(RPCParameters*))(g_libBR + 0x83440 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207988)), ((void (*)(RPCParameters*))(g_libBR + 0x83544 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207950)), ((void (*)(RPCParameters*))(g_libBR + 0x835E0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A20)), ((void (*)(RPCParameters*))(g_libBR + 0x83624 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079C4)), ((void (*)(RPCParameters*))(g_libBR + 0x836D0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079C8)), ((void (*)(RPCParameters*))(g_libBR + 0x83794 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079CC)), ((void (*)(RPCParameters*))(g_libBR + 0x83860 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079D0)), ((void (*)(RPCParameters*))(g_libBR + 0x83934 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207928)), ((void (*)(RPCParameters*))(g_libBR + 0x83A08 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20794C)), ((void (*)(RPCParameters*))(g_libBR + 0x83B30 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A4C)), ((void (*)(RPCParameters*))(g_libBR + 0x83BF4 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207960)), ((void (*)(RPCParameters*))(g_libBR + 0x83DF0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207968)), ((void (*)(RPCParameters*))(g_libBR + 0x83E88 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207978)), ((void (*)(RPCParameters*))(g_libBR + 0x83EBC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20792C)), ((void (*)(RPCParameters*))(g_libBR + 0x83F68 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207994)), ((void (*)(RPCParameters*))(g_libBR + 0x84004 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207944)), ((void (*)(RPCParameters*))(g_libBR + 0x840CC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A30)), ((void (*)(RPCParameters*))(g_libBR + 0x841B4 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A2C)), ((void (*)(RPCParameters*))(g_libBR + 0x84324 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079C0)), ((void (*)(RPCParameters*))(g_libBR + 0x8452C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A50)), ((void (*)(RPCParameters*))(g_libBR + 0x84670 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A00)), ((void (*)(RPCParameters*))(g_libBR + 0x847BC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A04)), ((void (*)(RPCParameters*))(g_libBR + 0x848B0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A08)), ((void (*)(RPCParameters*))(g_libBR + 0x84954 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A0C)), ((void (*)(RPCParameters*))(g_libBR + 0x84A04 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079A0)), ((void (*)(RPCParameters*))(g_libBR + 0x84AAC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079A4)), ((void (*)(RPCParameters*))(g_libBR + 0x84EA4 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079AC)), ((void (*)(RPCParameters*))(g_libBR + 0x84DE4 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207958)), ((void (*)(RPCParameters*))(g_libBR + 0x851A0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079EC)), ((void (*)(RPCParameters*))(g_libBR + 0x85264 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207970)), ((void (*)(RPCParameters*))(g_libBR + 0x854BC + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207954)), ((void (*)(RPCParameters*))(g_libBR + 0x85624 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079DC)), ((void (*)(RPCParameters*))(g_libBR + 0x84F90 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20796C)), ((void (*)(RPCParameters*))(g_libBR + 0x85478 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079B8)), ((void (*)(RPCParameters*))(g_libBR + 0x85318 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207898)), ((void (*)(RPCParameters*))(g_libBR + 0x85878 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A54)), ((void (*)(RPCParameters*))(g_libBR + 0x85930 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A58)), ((void (*)(RPCParameters*))(g_libBR + 0x85994 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078F8)), ((void (*)(RPCParameters*))(g_libBR + 0x85C18 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079F4)), ((void (*)(RPCParameters*))(g_libBR + 0x85CD0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079F8)), ((void (*)(RPCParameters*))(g_libBR + 0x85F0C + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079FC)), ((void (*)(RPCParameters*))(g_libBR + 0x85F90 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20798C)), ((void (*)(RPCParameters*))(g_libBR + 0x85570 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078E8)), ((void (*)(RPCParameters*))(g_libBR + 0x86478 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2078EC)), ((void (*)(RPCParameters*))(g_libBR + 0x86748 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x2079D4)), ((void (*)(RPCParameters*))(g_libBR + 0x869C8 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207948)), ((void (*)(RPCParameters*))(g_libBR + 0x86A98 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A60)), ((void (*)(RPCParameters*))(g_libBR + 0x86B74 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A64)), ((void (*)(RPCParameters*))(g_libBR + 0x872C0 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207A3C)), ((void (*)(RPCParameters*))(g_libBR + 0x88E00 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207998)), ((void (*)(RPCParameters*))(g_libBR + 0x88E74 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x20799C)), ((void (*)(RPCParameters*))(g_libBR + 0x88F98 + 1)));
    pRakClient->RegisterAsRemoteProcedureCall(&(*(int *)(g_libBR + 0x207910)), ((void (*)(RPCParameters*))(g_libBR + 0x89070 + 1)));
}
