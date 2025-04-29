// SPDX-License-Identifier: GPL-3.0-or-later

// standard library.
#include <stdlib.h>
#include <time.h>

// system dependencies.
#include <cglm/cglm.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sys/time.h>

// project headers.
#include "o_options.h"
#include "util.h"
#include "r_render.h"
#include "resources.h"
#include "s_sound.h"
#include "t_textbox.h"
#include "d_durak.h"
#include "c_choreo.h"
#include "g_game.h"
#include "i_input.h"

// project source.
#include "c_choreo.c"
#include "d_durak.c"
#include "g_game.c"
#include "i_input.c"
#include "r_render.c"
#include "s_sound.c"
#include "t_textbox.c"
#include "util.c"

int
main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	
	srand(time(NULL));
	
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
	
	if (Mix_Init(O_MIXFLAGS) != O_MIXFLAGS)
	{
		showerr("main: failed to init SDL2 mixer!");
		return 1;
	}
	atexit(Mix_Quit);
	
	if (r_init())
	{
		return 1;
	}
	
	if (s_init())
	{
		return 1;
	}
	
	g_loop();
	
	return 0;
}
