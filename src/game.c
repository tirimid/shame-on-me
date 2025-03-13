#include "game.h"

#include <SDL.h>

#include "choreo.h"
#include "render.h"
#include "util.h"

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define MAP \
	"S.....A..." \
	"######.###" \
	"a.....B..." \
	".........." \
	".........." \
	"b........." \
	".........." \
	"..c...d..." \
	".........." \
	".........."

void
G_Loop(void)
{
	g_Map = (struct Map)
	{
		.Data = MAP,
		.w = MAP_WIDTH,
		.h = MAP_HEIGHT
	};
	
	R_Light((vec3){7.0f, 0.0f, 8.0f}, 2.2f);
	
	C_LookWalkTo(A_PLAYER, 'A');
	C_Wait(300);
	C_LookAt(A_PLAYER, 'S');
	C_Wait(200);
	C_LookWalkTo(A_PLAYER, 'B');
	C_WalkTo(A_DUMMY, 'a');
	C_WalkTo(A_DUMMY, 'b');
	C_WalkTo(A_DUMMY, 'c');
	C_WalkTo(A_DUMMY, 'd');
	
	for (;;)
	{
		BeginTick();
		
		// handle events.
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					R_HandleResize(e.window.data1, e.window.data2);
				break;
			case SDL_QUIT:
				return;
			default:
				break;
			}
		}
		
		// update.
		C_Update();
		
		// render.
		R_BeginFrame();
		C_Render();
		R_EndFrame();
		
		EndTick();
	}
}
