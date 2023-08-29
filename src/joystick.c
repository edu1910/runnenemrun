#include "joystick.h"
#include "states.h"

const JOY_HANDLER JOYSTICK_HANDLERS[] = {
    &menu_joystick,
    &records_joystick,
    &game_joystick,
};

void joystick_handler(u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
        JOYSTICK_HANDLERS[get_state()](joy, changed, state);
	}
}