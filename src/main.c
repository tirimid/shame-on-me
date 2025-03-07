#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "src/game.c"
#include "src/input.c"
#include "src/render.c"
#include "src/util.c"

int
main(int Argc, char *Argv[])
{
	// initialize non-game systems.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		fprintf(stderr, "err: main: failed to init SDL2!\n");
		return 1;
	}
	atexit(SDL_Quit);
	
	if (TTF_Init())
	{
		ShowError("main: failed to init SDL2 TTF!\n");
		return 1;
	}
	atexit(TTF_Quit);
	
	if (InitRender())
		return 1;
	
	GameLoop();
	
	return 0;
}
