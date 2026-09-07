#pragma once

enum eAppInit
{
	APP_INIT_OFFSETS,
	APP_INIT_RW,
};

class CApp
{
public:
	static void Initialise(eAppInit init_type);
};
