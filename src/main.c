#include <genesis.h>
#include "states.h"
#include "joystick.h"

int main() 
{
	VDP_setScreenWidth320();
	VDP_setPlaneSize(64, 64, TRUE);
	SPR_init();

	change_state(STATE_MENU);

	JOY_init();
	JOY_setEventHandler(&joystick_handler);
	
	while (TRUE)
	{
		update_state();
		SYS_doVBlankProcess();
	}
	SPR_end();
	return 0;
}
