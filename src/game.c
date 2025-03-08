#include "game.h"

#include <SDL.h>

#include "choreo.h"
#include "render.h"
#include "util.h"

void
GameLoop(void)
{
	ActorWalk(A_PLAYER, (vec2){12.0f, 0.0f});
	ActorLook(A_PLAYER, 0.0f, GLM_PI / 2.0f);
	ActorWalk(A_PLAYER, (vec2){12.0f, 20.0f});
	ActorLook(A_PLAYER, 0.0f, GLM_PI);
	ActorWalk(A_PLAYER, (vec2){0.0f, 20.0f});
	ActorLook(A_PLAYER, 0.0f, 3.0f / 2.0f * GLM_PI);
	ActorWalk(A_PLAYER, (vec2){0.0f, 0.0f});
	ActorLook(A_PLAYER, 0.0f, 0.0f);
	
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
		UpdateChoreo();
		
		// render.
		SetupFrameRender();
		RenderChoreo();
		PresentFrame();
		
		EndTick();
	}
}
