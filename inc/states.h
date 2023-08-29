#ifndef _STATES_H_
#define _STATES_H_

#include <genesis.h>

#define STATE_NONE     -1
#define STATE_MENU      0
#define STATE_RECORDS   1
#define STATE_GAME      2

typedef void (*t_create_handler)();
typedef void (*t_update_handler)();
typedef void (*t_destroy_handler)();

int  get_state();
void change_state(int);
void update_state();

void menu_joystick(u16 joy, u16 changed, u16 state);
void menu_create();
void menu_update();
void menu_destroy();

void records_joystick(u16 joy, u16 changed, u16 state);
void records_create();

void game_joystick(u16 joy, u16 changed, u16 state);
void game_create();
void game_update();
void game_destroy();

#endif // _STATES_H_