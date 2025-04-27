// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INPUT_H
#define INPUT_H

typedef enum i_input
{
	I_RELEASE = 0,
	I_PRESS
} i_input;

void i_setkstate(SDL_Event const *e, i_input input);
void i_prepare(void);
bool i_kdown(SDL_Keycode k);
bool i_kpressed(SDL_Keycode k);
bool i_kreleased(SDL_Keycode k);

#endif
