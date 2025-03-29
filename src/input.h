#ifndef INPUT_H
#define INPUT_H

typedef enum I_InputType
{
	I_IT_RELEASE = 0,
	I_IT_PRESS
} I_InputType;

void I_SetKeybdState(SDL_Event const *e, I_InputType IT);
void I_Prepare(void);
bool I_KeyDown(SDL_Keycode k);
bool I_KeyPressed(SDL_Keycode k);
bool I_KeyReleased(SDL_Keycode k);

#endif
