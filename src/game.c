#include "game.h"

#include <SDL.h>

#include "render.h"
#include "util.h"

void
GameLoop(void)
{
	g_Camera.Pos[0] -= 10.0f;
	g_Camera.Pos[1] += 2.0f;
	g_Camera.Pos[2] += 5.0f;
	g_Camera.Yaw -= 3.1415f / 8.0f;
	g_Camera.Pitch -= 3.1415f / 10.0f;
	
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
					OnWindowResize(e.window.data1, e.window.data2);
				break;
			case SDL_QUIT:
				return;
			default:
				break;
			}
		}
		
		// update.
		//g_Camera.Yaw += 0.01f;
		
		// render.
		SetupFrameRender();
		RenderModel(
			M_CUBE,
			T_SOMETHING,
			(vec3){0},
			(vec3){0},
			(vec3){1.0f, 1.0f, 1.0f}
		);
		PresentFrame();
		
		EndTick();
	}
}
