#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "src/choreo.c"
#include "src/game.c"
#include "src/input.c"
#include "src/render.c"
#include "src/textbox.c"
#include "src/util.c"

#define SDL_INIT_FLAGS (SDL_INIT_VIDEO | SDL_INIT_AUDIO)
#define IMG_INIT_FLAGS IMG_INIT_PNG

int
main(int Argc, char *Argv[])
{
	// initialize non-game systems.
	if (SDL_Init(SDL_INIT_FLAGS))
	{
		fprintf(stderr, "err: main: failed to init SDL2!\n");
		return 1;
	}
	atexit(SDL_Quit);
	
	if (IMG_Init(IMG_INIT_FLAGS) != IMG_INIT_FLAGS)
	{
		ShowError("main: failed to init SDL2 image!");
		return 1;
	}
	atexit(IMG_Quit);
	
	if (TTF_Init())
	{
		ShowError("main: failed to init SDL2 TTF!");
		return 1;
	}
	atexit(TTF_Quit);
	
	if (R_Init())
		return 1;
	
	G_Loop();
	
	return 0;
}
