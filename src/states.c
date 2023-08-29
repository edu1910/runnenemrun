#include "states.h"

const t_create_handler CREATE_HANDLERS[] = {
    &menu_create,
    &records_create,
    &game_create,
    NULL,
    NULL,
};

const t_update_handler UPDATE_HANDLERS[] = {
    &menu_update,
    NULL,
    &game_update,
    NULL,
    NULL,
};

const t_destroy_handler DESTROY_HANDLERS[] = {
    &menu_destroy,
    NULL,
    &game_destroy,
    NULL,
    NULL,
};

static int _state = STATE_NONE;

int get_state()
{
    return _state;
}

void change_state(int state)
{
    if (_state != STATE_NONE && DESTROY_HANDLERS[_state])
    {
        DESTROY_HANDLERS[_state]();
    }

    _state = state;
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearTextArea(0, 0, 40, 40);
    if (CREATE_HANDLERS[_state])  CREATE_HANDLERS[_state]();
}

void update_state()
{
    if (UPDATE_HANDLERS[_state])  UPDATE_HANDLERS[_state]();
}