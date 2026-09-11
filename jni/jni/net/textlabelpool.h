#pragma once

#define MAX_TEXT_LABELS			1024
#define MAX_PLAYER_TEXT_LABELS	1024
#define INVALID_TEXT_LABEL		1025

#define MAX_LABELS_LENGTH		2048

#pragma pack(push, 1)
typedef struct _TEXT_LABELS
{
	char text[2048+1];
	char textWithoutColors[2048+1];
	uint32_t color;
	CVector pos;
	float drawDistance;
	int virtualWorld;
	bool useLineOfSight;
	PLAYERID attachedToPlayerID;
	VEHICLEID attachedToVehicleID;
	CVector offsetCoords;
	float m_fTrueX;
} TEXT_LABELS;
#pragma pack(pop)

class CText3DLabelsPool
{
private:
	/*TEXT_LABELS			*m_pTextLabels[MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2];
	bool				m_bSlotState[MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2];*/
	static inline std::unordered_map<int, TEXT_LABELS*> list;

public:
	static void Free();

	static void CreateTextLabel(int labelID, char* text, uint32_t color,
		float posX, float posY, float posZ, float drawDistance,
		 bool useLOS, PLAYERID attachedToPlayerID, VEHICLEID attachedToVehicleID);
	static void Delete(int labelID);
	static void AttachToPlayer(int labelID, PLAYERID playerID, CVector pos);
	static void AttachToVehicle(int labelID, VEHICLEID vehicleID, CVector pos);
	static void Update3DLabel(int labelID, uint32_t color, char* text);
	static void Draw();

	static TEXT_LABELS* GetAt(int labelID)
	{
		auto it = list.find(labelID);
		if(it != list.end())
			return list[labelID];

		return nullptr;
	}
};