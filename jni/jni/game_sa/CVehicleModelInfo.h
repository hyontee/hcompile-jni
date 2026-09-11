#pragma once

#include "CClumpModelInfo.h"
#include "../game/quaternion.h"
#include "../game/RW/rpworld.h"

#pragma pack(push, 1)
enum eVehicleClass : int8_t {
    VEHICLE_CLASS_IGNORE = -1,
    VEHICLE_CLASS_NORMAL = 0,
    VEHICLE_CLASS_POORFAMILY,
    VEHICLE_CLASS_RICHFAMILY,
    VEHICLE_CLASS_EXECUTIVE,
    VEHICLE_CLASS_WORKER,
    VEHICLE_CLASS_BIG,
    VEHICLE_CLASS_TAXI,
    VEHICLE_CLASS_MOPED,
    VEHICLE_CLASS_MOTORBIKE,
    VEHICLE_CLASS_LEISUREBOAT,
    VEHICLE_CLASS_WORKERBOAT,
    VEHICLE_CLASS_BICYCLE,
};

enum eVehicleDummy {
    DUMMY_LIGHT_FRONT_MAIN      = 0,
    DUMMY_LIGHT_REAR_MAIN       = 1,

    DUMMY_LIGHT_FRONT_SECONDARY = 2,
    DUMMY_LIGHT_REAR_SECONDARY  = 3,

    DUMMY_SEAT_FRONT            = 4,
    DUMMY_SEAT_REAR             = 5,

    DUMMY_EXHAUST               = 6,
    DUMMY_ENGINE                = 7,
    DUMMY_GAS_CAP               = 8,
    DUMMY_TRAILER_ATTACH        = 9,
    DUMMY_HAND_REST             = 10,
    DUMMY_EXHAUST_SECONDARY     = 11,
    DUMMY_WING_AIR_TRAIL        = 12,
    DUMMY_VEHICLE_GUN           = 13,
};

union tVehicleCompsUnion {
    uint32_t m_nComps;
    struct {
        uint32_t nExtraA_comp1 : 4;
        uint32_t nExtraA_comp2 : 4;
        uint32_t nExtraA_comp3 : 4;
        uint32_t : 4;

        uint32_t nExtraB_comp1 : 4;
        uint32_t nExtraB_comp2 : 4;
        uint32_t nExtraB_comp3 : 4;
        uint32_t : 4;
    };
    struct {
        uint32_t nExtraAComp : 12;
        uint32_t nExtraARule : 4;

        uint32_t nExtraBComp : 12;
        uint32_t nExtraBRule : 4;
    };

    struct {
        uint32_t nExtraA : 16;
        uint32_t nExtraB : 16;
    };
};

struct UpgradePosnDesc {
    CVector     m_vPosition;
    CQuaternion m_qRotation;
    int32_t       m_nParentComponentId;
};

struct CVehicleModelInfo : public CClumpModelInfo {
    RpMaterial*        m_pPlateMaterial; // 3C
    char               m_szPlateText[9]; // 40
    unsigned char      m_nPlateType;    //  49
    char               m_szGameName[8]; //  4a
    char               _pad3A[2];       //  52
    unsigned int       m_nVehicleType;  //  54
    float              m_fWheelSizeFront;// 58
    float              m_fWheelSizeRear;    //5C
    short              m_nWheelModelIndex;  //60
    short              m_nHandlingId;       // 62
    // int8_t             field_4B;
    unsigned char      m_nNumDoors;     //64
    eVehicleClass      m_nVehicleClass; //65
    unsigned char      m_nFlags;    //66
    unsigned char      m_nWheelUpgradeClass;//67
    unsigned char      m_nTimesUsed;    //68
    char               field_51;    // 69
    unsigned short     m_nFrq;      // 6A
    tVehicleCompsUnion m_extraComps;    //6c
    float              m_fBikeSteerAngle; // 70

    struct {
        CVector m_avDummyPos[15];
        UpgradePosnDesc m_aUpgrades[18];
        RpAtomic* m_apExtras[6];
        unsigned char m_nNumExtras;
        unsigned int m_nMaskComponentsDamagable;
    } * m_pVehicleStruct;   // 74
    uint32_t m_firstColour[49];
    uint32_t m_secondColour[33];
    uint32_t m_thirdColour[17];
    uint32_t m_fourthColour[17];
    uintptr_t* m_apDirtMaterials[64];
    uint8_t m_possibleColours[32];
    uint8_t m_numPossibleColours;
    uint8_t m_lastColUsed;
    uintptr_t *m_lastCol;
    int16_t   m_anUpgrades[18];
    uint8_t skip_0[2];
    uint32_t m_anRemapTxds[4];

    union {
        uintptr_t* m_pAnimBlock;
        char* m_animBlockFileName;
        unsigned int m_dwAnimBlockIndex;
    };

};
#pragma pack(pop)