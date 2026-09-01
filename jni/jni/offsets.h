#ifndef OFFSETS_H
#define OFFSETS_H

#define OFFSET_LOCAL_PLAYER     0x7F3A2C8  // il2cpp: PlayerManager::localPlayer
#define OFFSET_ENTITY_LIST      0x7F3A2D0  // List<PlayerController*>
#define OFFSET_ENTITY_COUNT     0x7F3A2D8  // int

// PlayerController (размер 0x4A0)
#define OFF_POSITION            0x00C0     // Vector3 (x,y,z)
#define OFF_HEAD                0x00D0     // Vector3 (голова)
#define OFF_VIEW_ANGLES         0x0140     // Vector2 (pitch, yaw)
#define OFF_HEALTH              0x01A0     // float
#define OFF_TEAM                0x01A4     // int (0=спецназ, 1=террор)
#define OFF_VISIBLE             0x0284     // byte (0/1)
#define OFF_RECOIL_X            0x0320     // float
#define OFF_RECOIL_Y            0x0324     // float
#define OFF_SPREAD              0x0328     // float
#define OFF_FIRE_RATE           0x0330     // float (интервал между выстрелами)
#define OFF_WEAPON_ID           0x0380     // int

// Camera (главный рендер)
#define OFFSET_CAMERA           0x7F2B1A0  // Camera.main
#define OFF_CAMERA_POS          0x0040     // Vector3
#define OFF_CAMERA_FOV          0x0060     // float

// Unity PlayerLoop (для ESP)
#define OFFSET_MATRIX_VP        0x7F2C000  // матрица View-Projection

// AntiBan скрытые адреса
#define OFF_ANTICHEAT_FLAG      0x7F4E000  // byte (0=чисто, 1=бан)
#define OFF_PACKET_CHECK        0x7F4E008  // uint32 (контрольная сумма)

#endif