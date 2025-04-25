// standard library.
#include <stdlib.h>
#include <time.h>

// system dependencies.
#include <cglm/cglm.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sys/time.h>

// project headers.
#include "options.h"
#include "util.h"
#include "render.h"
#include "textbox.h"
#include "durak.h"
#include "choreo.h"
#include "game.h"
#include "input.h"

// project resources.
#include "font/vcr_osd_mono_ttf.h"
#include "img/black_png.h"
#include "img/black50_png.h"
#include "img/c_back_png.h"
#include "img/c_clubs_6_png.h"
#include "img/c_clubs_7_png.h"
#include "img/c_clubs_8_png.h"
#include "img/c_clubs_9_png.h"
#include "img/c_clubs_10_png.h"
#include "img/c_clubs_j_png.h"
#include "img/c_clubs_q_png.h"
#include "img/c_clubs_k_png.h"
#include "img/c_clubs_a_png.h"
#include "img/c_diamonds_6_png.h"
#include "img/c_diamonds_7_png.h"
#include "img/c_diamonds_8_png.h"
#include "img/c_diamonds_9_png.h"
#include "img/c_diamonds_10_png.h"
#include "img/c_diamonds_j_png.h"
#include "img/c_diamonds_q_png.h"
#include "img/c_diamonds_k_png.h"
#include "img/c_diamonds_a_png.h"
#include "img/c_hearts_6_png.h"
#include "img/c_hearts_7_png.h"
#include "img/c_hearts_8_png.h"
#include "img/c_hearts_9_png.h"
#include "img/c_hearts_10_png.h"
#include "img/c_hearts_j_png.h"
#include "img/c_hearts_q_png.h"
#include "img/c_hearts_k_png.h"
#include "img/c_hearts_a_png.h"
#include "img/c_spades_6_png.h"
#include "img/c_spades_7_png.h"
#include "img/c_spades_8_png.h"
#include "img/c_spades_9_png.h"
#include "img/c_spades_10_png.h"
#include "img/c_spades_j_png.h"
#include "img/c_spades_q_png.h"
#include "img/c_spades_k_png.h"
#include "img/c_spades_a_png.h"
#include "img/ceiling_png.h"
#include "img/door_png.h"
#include "img/dummy_png.h"
#include "img/dummy_face_png.h"
#include "img/eyes_dummy_png.h"
#include "img/eyes_dummy_face_png.h"
#include "img/floor_png.h"
#include "img/glasses_dummy_png.h"
#include "img/glasses_dummy_face_png.h"
#include "img/lightbulb_png.h"
#include "img/something_png.h"
#include "img/table_png.h"
#include "img/wall_png.h"
#include "img/window_png.h"
#include "model/card_stack_obj.h"
#include "model/cube_obj.h"
#include "model/door_closed_obj.h"
#include "model/door_open_obj.h"
#include "model/lightbulb_obj.h"
#include "model/plane_obj.h"
#include "model/table_obj.h"
#include "model/window_obj.h"
#include "shader/base_frag_glsl.h"
#include "shader/base_vert_glsl.h"
#include "shader/overlay_frag_glsl.h"
#include "shader/overlay_vert_glsl.h"
#include "shader/shadow_frag_glsl.h"
#include "shader/shadow_geo_glsl.h"
#include "shader/shadow_vert_glsl.h"

// project source.
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
	
	if (r_init())
	{
		return 1;
	}
	
	g_loop();
	
	return 0;
}
