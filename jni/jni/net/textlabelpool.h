#pragma once

#define MAX_TEXT_LABELS			1024
#define MAX_PLAYER_TEXT_LABELS	1024
#define INVALID_TEXT_LABEL		1025

#define MAX_LABELS_LENGTH		2048

#pragma pack(1)
typedef struct _TEXT_LABELS
{
	char text[2048+1];
	char textWithoutColors[2048+1];
	uint32_t color;
	VECTOR pos;
	float drawDistance;
	int virtualWorld;
	bool useLineOfSight;
	PLAYERID attachedToPlayerID;
	VEHICLEID attachedToVehicleID;
	VECTOR offsetCoords;
	float m_fTrueX;
} TEXT_LABELS;

class CText3DLabelsPool
{
private:
	bool m_bSlotState[MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2];
    std::array<bool, MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2> m_bShowBackground;
    std::array<bool, MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2> m_bShowIcon;
    std::array<char*, MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2> m_szDatabase;
    std::array<char*, MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2> m_szTextures;
    std::array<RwTexture*, MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2> m_pTexture;

public:
	CText3DLabelsPool();
	~CText3DLabelsPool();
	
	/**
	 * Set the "Show Background" flag for a specific label.
	 *
	 * @param labelID The identifier of the label for which to set the flag.
	 * @param showBackground A boolean value indicating whether to show the background.
	 *
	 * @note The labelID should be within a valid range.
	 */
	void SetShowBackground(int labelID, bool showBackground)
	{
	    if (labelID >= 0 && labelID < m_bShowBackground.size())
	    {
	        m_bShowBackground[labelID] = showBackground;
	    }
	}

	/**
	 * Check if the "Show Background" flag is set for a specific label.
	 *
	 * @param labelID The identifier of the label to check.
	 *
	 * @return True if the background should be shown, false otherwise. If the labelID is out of bounds, it returns false.
	 */
	bool IsShowBackground(int labelID)
	{
	    if (labelID >= 0 && labelID < m_bShowBackground.size())
	    {
	        return m_bShowBackground[labelID];
	    }
	    return false;
	}

	/**
	 * Set the "Show Icon" flag for a specific label.
	 *
	 * @param labelID The identifier of the label for which to set the flag.
	 * @param showIcon A boolean value indicating whether to show an icon.
	 *
	 * @note The labelID should be within a valid range.
	 */
	void SetShowIcon(int labelID, bool showIcon)
	{
	    if (labelID >= 0 && labelID < m_bShowIcon.size())
	    {
	        m_bShowIcon[labelID] = showIcon;
	    }
	}

	/**
	 * Check if the "Show Icon" flag is set for a specific label.
	 *
	 * @param labelID The identifier of the label to check.
	 *
	 * @return True if an icon should be shown, false otherwise. If the labelID is out of bounds, it returns false.
	 */
	bool IsShowIcon(int labelID)
	{
	    if (labelID >= 0 && labelID < m_bShowIcon.size())
	    {
	        return m_bShowIcon[labelID];
	    }
	    return false;
	}

	/**
	 * Set the icon texture for a specific label.
	 *
	 * @param labelID The identifier of the label for which to set the texture.
	 * @param texture A pointer to the null-terminated string specifying the texture.
	 *
	 * @note The labelID should be within a valid range.
	 */
	void SetIconTexture(int labelID, const char* database, const char* texture)
	{
	    if (labelID >= 0 && labelID < m_szTextures.size())
	    {
			m_szDatabase[labelID] = strdup(database);
			m_szTextures[labelID] = strdup(texture);
	    }
	}

	/**
	 * Get the icon texture for a specific label.
	 *
	 * @param labelID The identifier of the label to retrieve the texture for.
	 *
	 * @return A pointer to the texture string if found, or nullptr if the labelID is out of bounds.
	 */
	const char* GetIconTexture(int labelID)
	{
	    if (labelID >= 0 && labelID < m_szTextures.size())
	    {
	        return m_szTextures[labelID];
	    }
	    return nullptr;
	}

	void CreateTextLabel(int labelID, char* text, uint32_t color, 
		float posX, float posY, float posZ, float drawDistance,
		 bool useLOS, PLAYERID attachedToPlayerID, VEHICLEID attachedToVehicleID);
	void Delete(int labelID);
	void AttachToPlayer(int labelID, PLAYERID playerID, VECTOR pos);
	void AttachToVehicle(int labelID, VEHICLEID vehicleID, VECTOR pos);
	void Update3DLabel(int labelID, uint32_t color, char* text);
	void Draw();
	
	TEXT_LABELS *m_pTextLabels[MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2];
	std::list<int> m_TextLabelCount;
};