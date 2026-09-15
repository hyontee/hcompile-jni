#pragma once

#include <stdint.h>

enum E_MSG : uint32_t
{
	CONNECTING,
	CONNECTED,
	SERVER_CLOSED_CONNECTION,
	MODIFIED_FILES,
	NOT_FROM_LAUNCHER,
	BANNED,
	CONNECTION_LOST,
	CONNECTION_ATTEMPT_FAILED,
	FULL_SERVER,
	
	REJECTED_1,
	REJECTED_2,
	REJECTED_3,
	REJECTED_4,
	
	KICKED_WEAPON_DAT,
	KICKED_WEAPON_DAT_1,
	
	KICKED_WATER_TEXTURE,
	KICKED_VIRTUAL,
	
	VOICE_MUTED,

	MSG_COUNT
};

#define MAX_LOCALISATION_LENGTH	256

class CLocalisation
{
	static char m_szMessages[E_MSG::MSG_COUNT][MAX_LOCALISATION_LENGTH];
public:
	static void Initialise(const char* szFile);

	static char* GetMsg(E_MSG msg);
};

