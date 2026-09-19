#include "app.h"
#include "plugin.h"

#include "game/rw/rw.h"

void CApp::Initialise(eAppInit init_type)
{
	if(init_type == eAppInit::APP_INIT_OFFSETS)
	{
		COffset::Initialise();
	}
	if(init_type == eAppInit::APP_INIT_RW)
	{
		rw::Initialise();
	}
}