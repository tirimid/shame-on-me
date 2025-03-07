#include "game.h"

#include <SDL.h>

#include "render.h"
#include "util.h"

void
GameLoop(void)
{
	for (;;)
	{
		BeginTick();
		
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
		
		SetupFrameRender();
		PresentFrame();
		
		EndTick();
	}
}
