#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <genesis.h>

typedef void (*JOY_HANDLER)(u16, u16, u16);

void joystick_handler(u16, u16, u16);

#endif // _JOYSTICK_H_
