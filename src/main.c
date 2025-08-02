// SPDX-License-Identifier: GPL-3.0-or-later

// standard library.
#include <errno.h>
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
#include "util.h"
#include "o_options.h"
#include "r_render.h"
#include "resources.h"
#include "s_sound.h"
#include "t_textbox.h"
#include "d_durak.h"
#include "c_choreo.h"
#include "g_game.h"
#include "i_input.h"
#include "u_ui.h"
#include "m_menus.h"

// project source.
#include "c_choreo.c"
#include "d_durak.c"
#include "g_game.c"
#include "i_input.c"
#include "m_menus.c"
#include "o_options.c"
#include "r_render.c"
#include "s_sound.c"
#include "t_textbox.c"
#include "u_ui.c"
#include "util.c"

int
main(int argc, char *argv[])
{
	NEWTIMER(largetimer);
	NEWTIMER(stagetimer);
	
	(void)argc;
	(void)argv;
	
	// initialize non-game systems.
	BEGINTIMER(&largetimer);
	
	srand(time(NULL));
	
	BEGINTIMER(&stagetimer);
	if (SDL_Init(O_SDLFLAGS))
	{
		fprintf(stderr, "err: main: failed to init SDL2!\n");
		return 1;
	}
	atexit(SDL_Quit);
	ENDTIMER(stagetimer, "main: init SDL2");
	
	BEGINTIMER(&stagetimer);
	if (IMG_Init(O_IMGFLAGS) != O_IMGFLAGS)
	{
		showerr("main: failed to init SDL2 image!");
		return 1;
	}
	atexit(IMG_Quit);
	ENDTIMER(stagetimer, "main: init SDL2 image");
	
	BEGINTIMER(&stagetimer);
	if (TTF_Init())
	{
		showerr("main: failed to init SDL2 TTF!");
		return 1;
	}
	atexit(TTF_Quit);
	ENDTIMER(stagetimer, "main: init SDL2 TTF");
	
	BEGINTIMER(&stagetimer);
	if (Mix_Init(O_MIXFLAGS) != O_MIXFLAGS)
	{
		showerr("main: failed to init SDL2 mixer!");
		return 1;
	}
	atexit(Mix_Quit);
	ENDTIMER(stagetimer, "main: init SDL2 mixer");
	
	ENDTIMER(largetimer, "main: init non-game systems");
	
	// initialize game systems.
	BEGINTIMER(&largetimer);
	
	BEGINTIMER(&stagetimer);
	if (o_dynread())
	{
		o_dyndefault();
		if (o_dynwrite())
		{
			return 1;
		}
	}
	ENDTIMER(stagetimer, "main: init dynamic options");
	
	BEGINTIMER(&stagetimer);
	if (r_init())
	{
		return 1;
	}
	ENDTIMER(stagetimer, "main: init render");
	
	BEGINTIMER(&stagetimer);
	if (s_init())
	{
		return 1;
	}
	ENDTIMER(stagetimer, "main: init sound");
	
	ENDTIMER(largetimer, "main: init game systems");
	
	s_playmusic(S_BGOMINOUS);
	for (;;)
	{
		m_main();
	}
}
