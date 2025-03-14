#ifndef INPUT_H
#define INPUT_H

enum InputType
{
	IT_RELEASE = 0,
	IT_PRESS
};

void I_SetKeybdState(SDL_Event const *e, enum InputType It);
void I_Prepare(void);
bool I_KeyDown(SDL_Keycode k);
bool I_KeyPressed(SDL_Keycode k);
bool I_KeyReleased(SDL_Keycode k);

#endif
