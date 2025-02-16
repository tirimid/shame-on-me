#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "util.h"

int
main(int Argc, char *Argv[])
{
	// initialize non-game systems.
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
		{
			fprintf(stderr, "err: main: failed to init SDL2!\n");
			return 1;
		}
		atexit(SDL_Quit);
		
		if (TTF_Init())
		{
			LogErr("main: failed to init SDL2 TTF!\n");
			return 1;
		}
		atexit(TTF_Quit);
	}
	
	return 0;
}
