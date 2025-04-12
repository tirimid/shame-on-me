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
#include "durak.h"
#include "choreo.h"
#include "game.h"
#include "input.h"

#include "choreo.c"
#include "durak.c"
#include "game.c"
#include "input.c"
#include "render.c"
#include "textbox.c"
#include "util.c"

int
main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	
	// initialize non-game systems.
	if (SDL_Init(O_SDLFLAGS))
	{
		fprintf(stderr, "err: main: failed to init SDL2!\n");
		return 1;
	}
	atexit(SDL_Quit);
	
	if (IMG_Init(O_IMGFLAGS) != O_IMGFLAGS)
	{
		showerr("main: failed to init SDL2 image!");
		return 1;
	}
	atexit(IMG_Quit);
	
	if (TTF_Init())
	{
		showerr("main: failed to init SDL2 TTF!");
		return 1;
	}
	atexit(TTF_Quit);
	
	if (r_init())
	{
		return 1;
	}
	
	g_loop();
	
	return 0;
}
