#include <stdlib.h>

#include <cglm/cglm.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sys/time.h>

#include "options.h"
#include "util.h"
#include "render.h"
#include "textbox.h"
#include "choreo.h"
#include "game.h"
#include "input.h"

#include "choreo.c"
#include "game.c"
#include "input.c"
#include "render.c"
#include "textbox.c"
#include "util.c"

int
main(int Argc, char *Argv[])
{
	(void)Argc;
	(void)Argv;
	
	// initialize non-game systems.
	if (SDL_Init(O_SDL_INIT_FLAGS))
	{
		fprintf(stderr, "err: main: failed to init SDL2!\n");
		return 1;
	}
	atexit(SDL_Quit);
	
	if (IMG_Init(O_IMG_INIT_FLAGS) != O_IMG_INIT_FLAGS)
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
